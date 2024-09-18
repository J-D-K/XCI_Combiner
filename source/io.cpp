#include "io.hpp"

#define BUFFER_SIZE 0x800000

uint64_t getFileSize(const std::filesystem::path &filePath)
{
    std::ifstream testFile(filePath, std::ios::binary);
    testFile.seekg(0, std::ios::end);
    return testFile.tellg();
}

void readThread(std::ifstream &source, sharedThreadStruct sharedStruct)
{
    // Keep track of how much of file has been read
    uint64_t bytesRead = 0;
    // Local vector/buffer to read too.
    std::vector<char> localBuffer(BUFFER_SIZE);

    // Loop through file
    while (bytesRead < sharedStruct->fileSize)
    {
        // Read to vector
        source.read(localBuffer.data(), BUFFER_SIZE);

        // Wait for mutex to be unlocked
        std::unique_lock sharedBufferLock(sharedStruct->bufferLock);
        sharedStruct->bufferIsReady.wait(sharedBufferLock, [sharedStruct] { return sharedStruct->bufferIsFull == false; });

        // Copy localBuffer to shared buffer
        sharedStruct->sharedBuffer.assign(localBuffer.begin(), localBuffer.begin() + source.gcount());
        // Set condition for other thread
        sharedStruct->bufferIsFull = true;

        // Unlock
        sharedBufferLock.unlock();

        // Notify
        sharedStruct->bufferIsReady.notify_one();

        // Update bytes read
        bytesRead += source.gcount();
    }
}

void writeThread(std::ofstream &destination, sharedThreadStruct sharedStruct)
{
    // Keep track of bytes written
    uint64_t bytesWritten = 0;
    // Local buffer to store data to be written
    std::vector<char> localBuffer;

    // Keep looping and writing when sharedBuffer is full
    while (bytesWritten < sharedStruct->fileSize)
    {
        // Wait for signal sharedBuffer is ready
        std::unique_lock sharedBufferLock(sharedStruct->bufferLock);
        sharedStruct->bufferIsReady.wait(sharedBufferLock, [sharedStruct] { return sharedStruct->bufferIsFull == true; });

        // Copy sharedBuffer to localBuffer so read thread can continue while this one writes
        localBuffer.assign(sharedStruct->sharedBuffer.begin(), sharedStruct->sharedBuffer.end());

        // Make sure read thread can see shared buffer is now ready to be overwritten
        sharedStruct->bufferIsFull = false;

        // Unlock
        sharedBufferLock.unlock();

        // Notify read thread
        sharedStruct->bufferIsReady.notify_one();

        // Write to file
        destination.write(localBuffer.data(), localBuffer.size());

        // Update bytes written
        bytesWritten += localBuffer.size();
    }
}

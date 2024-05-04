#include "io.hpp"

#define BUFFER_SIZE 0x800000

uint64_t getFileSize(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);
    file.seekg(0, std::ios::end);
    return file.tellg();
}

void readThread(std::ifstream &source, std::shared_ptr<threadStruct> structIn)
{
    // Keep track of how much of file has been read
    uint64_t bytesRead = 0;
    // Local vector/buffer to read too.
    std::vector<char> localBuffer(BUFFER_SIZE);

    // Loop through file
    while(bytesRead < structIn->fileSize)
    {
        // Read to vector
        source.read(localBuffer.data(), BUFFER_SIZE);

        // Wait for mutex to be unlocked
        std::unique_lock sharedBufferLock(structIn->bufferLock);
        structIn->bufferIsReady.wait(sharedBufferLock, [structIn] { return structIn->bufferIsFull == false; });

        // Copy localBuffer to shared buffer
        structIn->sharedBuffer.assign(localBuffer.begin(), localBuffer.begin() + source.gcount());

        // Set condition for other thread
        structIn->bufferIsFull = true;

        // Unlock
        sharedBufferLock.unlock();

        // Notify
        structIn->bufferIsReady.notify_one();

        // Update bytes read
        bytesRead += source.gcount();
    }
}

void writeThread(std::ofstream &destination, std::shared_ptr<threadStruct> structIn)
{
    // Keep track of bytes written
    uint64_t bytesWritten = 0;
    // Local buffer to store data to be written
    std::vector<char> localBuffer;
    
    // Keep looping and writing when sharedBuffer is full
    while(bytesWritten < structIn->fileSize)
    {
        // Wait for signal sharedBuffer is ready
        std::unique_lock sharedBufferLock(structIn->bufferLock);
        structIn->bufferIsReady.wait(sharedBufferLock, [structIn] { return structIn->bufferIsFull == true; } );

        // Copy sharedBuffer to localBuffer so read thread can continue while this one writes
        localBuffer.assign(structIn->sharedBuffer.begin(), structIn->sharedBuffer.end());

        // Make sure read thread can see shared buffer is now ready to be overwritten
        structIn->bufferIsFull = false;

        // Unlock
        sharedBufferLock.unlock();

        // Notify read thread
        structIn->bufferIsReady.notify_one();

        // Write to file
        destination.write(localBuffer.data(), localBuffer.size());
        
        // Update bytes written
        bytesWritten += localBuffer.size();
    }
}
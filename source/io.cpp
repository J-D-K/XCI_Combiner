#include "io.hpp"

#include <iostream>

namespace
{
    constexpr size_t BUFFER_SIZE = 0x200000;
}

void read_thread_function(std::ifstream &source, SharedThreadStruct sharedStruct)
{
    auto &queueMutex        = sharedStruct->queueMutex;
    auto &bufferQueue       = sharedStruct->bufferQueue;
    const uint64_t fileSize = sharedStruct->fileSize;

    for (uint64_t i = 0; i < fileSize;)
    {
        std::lock_guard queueGuard{queueMutex};

        std::vector<char> readBuffer(BUFFER_SIZE);
        source.read(readBuffer.data(), BUFFER_SIZE);
        size_t readCount = source.gcount();
        bufferQueue.push(std::move(readBuffer));

        i += readCount;
    }
}

void write_function(std::ofstream &destination, SharedThreadStruct sharedStruct)
{
    auto &queueMutex        = sharedStruct->queueMutex;
    auto &bufferQueue       = sharedStruct->bufferQueue;
    const uint64_t fileSize = sharedStruct->fileSize;

    for (uint64_t i = 0; i < fileSize;)
    {
        std::vector<char> writeBuffer{};
        {
            std::lock_guard queueGuard{queueMutex};
            if (bufferQueue.empty()) { continue; }

            writeBuffer = std::move(bufferQueue.front());
            bufferQueue.pop();
        }

        destination.write(writeBuffer.data(), writeBuffer.size());

        i += writeBuffer.size();
    }
}

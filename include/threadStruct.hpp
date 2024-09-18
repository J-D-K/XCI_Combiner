#pragma once
#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>

// Struct that is shared and coordinates threads
typedef struct
{
    // Mutex
    std::mutex bufferLock;
    // Conditional to make sure buffer is ready
    std::condition_variable bufferIsReady;
    // Bool to check if buffer is full. False by default
    bool bufferIsFull = false;
    // Bool for when operation is finished
    bool finished = false;
    // Shared buffer
    std::vector<char> sharedBuffer;
    // Size of the file being read
    uint64_t fileSize;
} threadStruct;

using sharedThreadStruct = std::shared_ptr<threadStruct>;

inline sharedThreadStruct createSharedThreadStruct(uint64_t fileSize)
{
    sharedThreadStruct newThreadStruct = std::make_shared<threadStruct>();
    newThreadStruct->fileSize = fileSize;
    return newThreadStruct;
}

#pragma once
#include <mutex>
#include <condition_variable>
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
#pragma once
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

// clang-format off
struct ThreadStruct
{
    std::mutex queueMutex{};
    std::queue<std::vector<char>> bufferQueue{};
    uint64_t fileSize{};
};
// clang-format on

using SharedThreadStruct = std::shared_ptr<ThreadStruct>;

inline SharedThreadStruct create_shared_thread_struct(uint64_t fileSize)
{
    auto returnStruct      = std::make_shared<ThreadStruct>();
    returnStruct->fileSize = fileSize;
    return returnStruct;
}

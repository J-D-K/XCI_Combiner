#pragma once
#include "ThreadStruct.hpp"

#include <filesystem>
#include <fstream>
#include <memory>

/// @brief Function executed on the read thread.
void read_thread_function(std::ifstream &source, SharedThreadStruct sharedStruct);

/// @brief Function called in the main thread to handle writing.
void write_function(std::ofstream &destination, SharedThreadStruct sharedStruct);

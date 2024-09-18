#pragma once
#include "threadStruct.hpp"
#include <filesystem>
#include <fstream>
#include <memory>

// File related functions
uint64_t getFileSize(const std::filesystem::path &filePath);
// These are the thread functions for reading and writing from files.
void readThread(std::ifstream &source, sharedThreadStruct sharedStruct);
void writeThread(std::ofstream &destination, sharedThreadStruct sharedStruct);

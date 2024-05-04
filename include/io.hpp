#pragma once
#include <fstream>
#include <memory>
#include "threadStruct.hpp"

// File related functions
uint64_t getFileSize(const std::string &path);
// These are the thread functions for reading and writing from files.
void readThread(std::ifstream &source, std::shared_ptr<threadStruct> structIn);
void writeThread(std::ofstream &destination, std::shared_ptr<threadStruct> structIn);
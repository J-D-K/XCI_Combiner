#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <memory>

#include "io.hpp"

std::string getXciNameFromPath(const std::string &path)
{
    size_t lastSlash = path.find_last_of("/\\");
    return path.substr(lastSlash + 1, path.npos);
}

int main(int argc, const char *argv[])
{
    // Bail if not enough args supplied
    if(argc < 3)
    {
        std::cout << "Usage: \nxciCombiner \"[source directory]\" \"[destination directory]\"";
        return -1;
    }

    // Store source and destination as C++ strings for easier manipulation
    std::string sourceDirectory = argv[1];
    std::string destinationDirectory = argv[2];

    // Iterate through source directory
    for(const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(sourceDirectory))
    {
        if(entry.is_directory())
        {
            // Make the full path to final XCI
            std::string xciPath = destinationDirectory + getXciNameFromPath(entry.path().string());

            std::cout << entry.path().string() << " -> " << xciPath << std::endl;

            // Open it for write thread
            std::ofstream destinationXci(xciPath, std::ios::binary);

            // This should be the folder containing the split xci. Have to iterate again
            for(const std::filesystem::directory_entry &xciEntry : std::filesystem::directory_iterator(entry.path()))
            {
                // Prepare struct for threads
                std::shared_ptr<threadStruct> sendStruct = std::make_shared<threadStruct>();
                sendStruct->fileSize = getFileSize(xciEntry.path().string());

                // Open source
                std::ifstream source(xciEntry.path().string(), std::ios::binary);

                // Start read & write threads
                std::thread read(readThread, std::ref(source), sendStruct);
                std::thread write(writeThread, std::ref(destinationXci), sendStruct);

                // Wait for finish
                read.join();
                write.join();
            }
        }
        else
        {
            // Just copy XCI to destination. Final path
            std::string xciPath = destinationDirectory + getXciNameFromPath(entry.path().string());

            std::cout << entry.path().string() << " -> " << xciPath << std::endl;

            // Prepare struct
            std::shared_ptr<threadStruct> sendStruct = std::make_shared<threadStruct>();
            sendStruct->fileSize = getFileSize(entry.path().string());

            // Files
            std::ifstream source(entry.path().string(), std::ios::binary);
            std::ofstream destination(xciPath, std::ios::binary);

            // Threads
            std::thread read(readThread, std::ref(source), sendStruct);
            std::thread write(writeThread, std::ref(destination), sendStruct);

            // Wait
            read.join();
            write.join();
        }
    }
    return 0;
}
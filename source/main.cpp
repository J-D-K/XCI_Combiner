#include "io.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

int main(int argc, const char *argv[])
{
    // Bail if not enough args supplied
    if (argc < 3)
    {
        std::cout << "Usage: xci_combiner \"[source directory]\" \"[destination directory]\"";
        return -1;
    }

    std::filesystem::path sourceDirectory = argv[1];
    std::filesystem::path destinationDirectory = argv[2];

    // Iterate through source directory
    for (const std::filesystem::directory_entry &currentEntry : std::filesystem::directory_iterator(sourceDirectory))
    {
        if (currentEntry.is_directory() == true)
        {
            // Path to output XCI File
            std::filesystem::path xciPath = destinationDirectory / currentEntry.path().filename();

            // Just print this so I know it's working...
            std::cout << currentEntry.path() << " -> " << xciPath << std::endl;

            // Open output file
            std::ofstream xciDestinationFile(xciPath, std::ios::binary);

            // Open the inner folder containing the xci parts.
            for (const std::filesystem::directory_entry &xciPart : std::filesystem::directory_iterator(currentEntry))
            {
                // Struct shared by threads.
                sharedThreadStruct sharedStruct = createSharedThreadStruct(getFileSize(xciPart.path()));

                // Open source for reading.
                std::ifstream xciSourceFile(xciPart.path(), std::ios::binary);

                // Spawn read and write threads.
                std::thread read(readThread, std::ref(xciSourceFile), sharedStruct);
                std::thread write(writeThread, std::ref(xciDestinationFile), sharedStruct);

                // Wait for them to finish.
                read.join();
                write.join();
            }
        }
        else
        {
            // Just copy since it's probably less than 4GB
            std::filesystem::path xciPath = destinationDirectory / currentEntry.path().filename();

            std::cout << currentEntry.path() << " -> " << xciPath << std::endl;

            sharedThreadStruct sharedStruct = createSharedThreadStruct(getFileSize(currentEntry.path()));

            std::ifstream xciSource(currentEntry.path(), std::ios::binary);
            std::ofstream xciDestination(xciPath, std::ios::binary);

            std::thread read(readThread, std::ref(xciSource), sharedStruct);
            std::thread write(writeThread, std::ref(xciDestination), sharedStruct);

            read.join();
            write.join();
        }
    }
    return 0;
}

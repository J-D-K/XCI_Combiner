#include "io.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

int main(int argc, const char *argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: xci_combiner \"[source directory]\" \"[destination directory]\"";
        return -1;
    }

    const std::filesystem::path sourceDir{argv[1]};
    const std::filesystem::path destDir{argv[2]};

    std::cout << sourceDir << ", " << destDir << std::endl;

    for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(sourceDir))
    {
        if (entry.is_directory())
        {
            const std::filesystem::path xciPath{destDir / entry.path().filename()};

            std::cout << entry.path() << " -> " << xciPath.string() << std::endl;

            std::ofstream xciFile{xciPath, std::ios::binary};

            for (const std::filesystem::directory_entry &subEntry : std::filesystem::directory_iterator(entry))
            {
                std::ifstream xciPart{subEntry.path(), std::ios::binary};
                SharedThreadStruct ioStruct = create_shared_thread_struct(std::filesystem::file_size(subEntry));

                std::thread readThread(read_thread_function, std::ref(xciPart), ioStruct);
                write_function(xciFile, ioStruct);

                readThread.join();
            }
        }
    }

    return 0;
}

// int main(int argc, const char *argv[])
// {
//     // Bail if not enough args supplied
//     if (argc < 3)
//     {
//         std::cout << "Usage: xci_combiner \"[source directory]\" \"[destination directory]\"";
//         return -1;
//     }

//     std::filesystem::path sourceDirectory = argv[1];
//     std::filesystem::path destinationDirectory = argv[2];

//     // Iterate through source directory
//     for (const std::filesystem::directory_entry &currentEntry : std::filesystem::directory_iterator(sourceDirectory))
//     {
//         if (currentEntry.is_directory() == true)
//         {
//             // Path to output XCI File
//             std::filesystem::path xciPath = destinationDirectory / currentEntry.path().filename();

//             // Just print this so I know it's working...
//             std::cout << currentEntry.path() << " -> " << xciPath << std::endl;

//             // Open output file
//             std::ofstream xciDestinationFile(xciPath, std::ios::binary);

//             // Open the inner folder containing the xci parts.
//             for (const std::filesystem::directory_entry &xciPart : std::filesystem::directory_iterator(currentEntry))
//             {
//                 // Struct shared by threads.
//                 sharedThreadStruct sharedStruct = createSharedThreadStruct(getFileSize(xciPart.path()));

//                 // Open source for reading.
//                 std::ifstream xciSourceFile(xciPart.path(), std::ios::binary);

//                 // Spawn read and write threads.
//                 std::thread read(readThread, std::ref(xciSourceFile), sharedStruct);
//                 std::thread write(writeThread, std::ref(xciDestinationFile), sharedStruct);

//                 // Wait for them to finish.
//                 read.join();
//                 write.join();
//             }
//         }
//         else
//         {
//             // Just copy since it's probably less than 4GB
//             std::filesystem::path xciPath = destinationDirectory / currentEntry.path().filename();

//             std::cout << currentEntry.path() << " -> " << xciPath << std::endl;

//             sharedThreadStruct sharedStruct = createSharedThreadStruct(getFileSize(currentEntry.path()));

//             std::ifstream xciSource(currentEntry.path(), std::ios::binary);
//             std::ofstream xciDestination(xciPath, std::ios::binary);

//             std::thread read(readThread, std::ref(xciSource), sharedStruct);
//             std::thread write(writeThread, std::ref(xciDestination), sharedStruct);

//             read.join();
//             write.join();
//         }
//     }
//     return 0;
// }

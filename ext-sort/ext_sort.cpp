// #define OLD_GCC

#include <fstream>
#include <stdexcept>

#ifdef OLD_GCC
#include <tr1/memory>
#endif
#include <vector>
#include <tclap/CmdLine.h>
#include "binary_heap.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <cstdio>

const int DEFAULT_BLOCK_SIZE = 1024 * 1024 * 1024; // 1 GB
const int DEFAULT_BRANCHING_DEGREE = 8;

// Structure to hold command line arguments
struct CliArguments {
    std::string input_file;
    std::string output_file;
    long long block_size;
    int branching_degree;

    void CheckOrDie() const {
        const long long GB32 = 34359738368LU;
        if (block_size % 4 != 0 ||
            block_size < 4 ||
            block_size > GB32) {
            throw std::runtime_error("Block size must be in range [4 B; 32 GB]");
        }
    }
};
// Parses command line arguments from input
// Returns structure with extracted arguments
CliArguments ParseCliArguments(int argc, char **argv);
// Sorts file with name input_file and writes result into file with name output_file
// Assumes input file and output file are binary
// Block size is maximum file size which can be loaded into RAM
// Branching is maximum number of splits per file
void ExternalMergeSort(std::string input_file, std::string output_file, long long block_size, int branching);
// Splits a file into a sequence of sorted files
// Sorting is performed sequentially (not parallel),
// because it's assumed that only block of size block_size fits into memory
// Returns vector with filenames, that store sorted files
std::vector<std::string> SplitFileIntoSortedFiles(std::string input_file_name, std::string temp_file_name_mask, long long block_size, int branching);
// Merges files into one big file
// Input is in input_file_names, output is in output_file_name
void MergeFiles(const std::vector<std::string> &input_file_names, std::string output_file_name);
// Utility function that returns file size
long long GetFileSize(std::string filename);

// Helper structure to merge files
// Stores a pointer to file and last value read from that file
struct MergeElement;

int main(int argc, char **argv) {
    try {
        CliArguments arguments = ParseCliArguments(argc, argv);
        ExternalMergeSort(arguments.input_file, arguments.output_file, arguments.block_size, arguments.branching_degree);
    } catch (TCLAP::ArgException &arg) {
        std::cout << "Invalid arguments: " << arg.error() << "for arg " << arg.argId() << std::endl;
        return 1;
    } catch (std::runtime_error& err) {
        std::cout << "Runtime error: " << err.what() << std::endl;
        return 1;
    }

    return 0;
}


// A pointer to std::ifstream object (needed to store them in a container)
#ifdef OLD_GCC
typedef std::tr1::shared_ptr<std::ifstream> FilePointer;
#else
typedef std::shared_ptr<std::ifstream> FilePointer;
#endif


struct MergeElement {
    std::vector<FilePointer>::iterator file_iterator_;
    uint64_t value_;

public:
    MergeElement(std::vector<FilePointer>::iterator file_iterator) :
        file_iterator_(file_iterator)
    {
        ReadNextValue();
    }

    bool operator < (const MergeElement &other) {
        return (value_ < other.value_);
    }

    bool operator > (const MergeElement &other) const {
        return (value_ > other.value_);
    }

    void ReadNextValue() {
        (*file_iterator_)->read((char *) &value_, sizeof(value_));
    }

    uint64_t GetValue() const {
        return value_;
    }

    bool Stop() const {
        return !(**file_iterator_);
    }
};


void MergeFiles(const std::vector<std::string> &input_file_names, std::string output_file_name) {
    algorithms::BinaryHeap<MergeElement, std::greater<MergeElement>> merge_elements;

    std::vector<FilePointer> in_files;
    for (std::string file_name: input_file_names) {
        in_files.push_back(FilePointer(new std::ifstream(file_name, std::ios_base::in | std::ios_base::binary)));
    }
    std::ofstream out_file(output_file_name, std::ios_base::out | std::ios_base::binary);

    for (auto file_iter = in_files.begin(); file_iter != in_files.end(); ++file_iter) {
        merge_elements.Insert(MergeElement(file_iter));
    }

    while (merge_elements.GetSize() > 0) {
        MergeElement minimum = merge_elements.GetTop();
        merge_elements.Pop();

        uint64_t value = minimum.GetValue();
        out_file.write((char *) &value, sizeof(value));

        minimum.ReadNextValue();
        if (!minimum.Stop()) {
            merge_elements.Insert(minimum);
        }
    }

    out_file.flush();
}


long long GetFileSize(std::string filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}


std::vector<std::string> SplitFileIntoSortedFiles(std::string input_file_name, std::string temp_file_name_mask, long long block_size, int branching_degree) {
    long long file_size = GetFileSize(input_file_name);

    int chunk_size = ceil(double(file_size) / branching_degree) - (int(ceil(double(file_size) / branching_degree)) % 4);

    std::vector<std::string> sorted_file_names;
    std::ifstream in_file(input_file_name, std::ios_base::in | std::ios_base::binary);
    std::vector<uint64_t> buffer(block_size / sizeof(uint64_t), 0);
    int file_name_number = 0;

    if (chunk_size <= block_size) {
        // can do in one pass

        while (in_file) {
            in_file.read((char *) &buffer[0], block_size);
            size_t bytes_read = in_file.gcount();
            if (bytes_read < sizeof(uint64_t)) {
                break;
            }
            std::sort(buffer.begin(), buffer.begin() + (bytes_read / sizeof(uint64_t)));

            std::string temp_file_name = temp_file_name_mask + std::to_string(file_name_number++);
            sorted_file_names.push_back(temp_file_name);

            std::ofstream out_file(temp_file_name, std::ios_base::out | std::ios_base::binary);
            out_file.write((char *) &buffer[0], bytes_read);
            out_file.flush();
        }
    } else {
        // need multiple passes
        std::vector<std::string> temp_file_names;
        // split input file into chunks
        while (in_file) {
            std::string temp_file_name = temp_file_name_mask + std::to_string(file_name_number++);
            std::ofstream chunk_file(temp_file_name, std::ios_base::out | std::ios_base::binary);

            int chunk_filled = 0;
            while (chunk_size - chunk_filled > block_size) {
                in_file.read((char *) &buffer[0], block_size);
                size_t bytes_read = in_file.gcount();
                if (bytes_read < sizeof(uint64_t)) {
                    break;
                }

                chunk_file.write((char *) &buffer[0], bytes_read);
                chunk_filled += bytes_read;
            }

            if (chunk_filled > 0) {
                temp_file_names.push_back(temp_file_name);
                sorted_file_names.push_back(temp_file_name + "_s");
                chunk_file.flush();
            }
        }

        for (size_t file_name_idx = 0; file_name_idx < temp_file_names.size(); ++file_name_idx) {
            ExternalMergeSort(temp_file_names[file_name_idx], sorted_file_names[file_name_idx], block_size, branching_degree);
        }

        for (std::string temp_file: temp_file_names) {
            ::remove(temp_file.c_str());
        }
    }
    return sorted_file_names;
}


CliArguments ParseCliArguments(int argc, char **argv) {
    TCLAP::CmdLine cmd("Sorting in external memory", ' ', "1.0");
    TCLAP::ValueArg<int> block_size_arg("b", "block_size", "Size of one block to use (in bytes)", false, DEFAULT_BLOCK_SIZE, "integer");
    TCLAP::ValueArg<int> branching_degree_arg("d", "branching", "Branching degree", false, DEFAULT_BRANCHING_DEGREE, "integer");
    TCLAP::UnlabeledValueArg<std::string> input_file_arg( "input_file", "Input file name", true, "", "nameString");
    TCLAP::UnlabeledValueArg<std::string> output_file_arg( "output_file", "Output file name", true, "", "nameString");
    cmd.add(input_file_arg);
    cmd.add(output_file_arg);
    cmd.add(block_size_arg);
    cmd.add(branching_degree_arg);

    cmd.parse(argc, argv);

    CliArguments arguments {
        input_file_arg.getValue(),
        output_file_arg.getValue(),
        block_size_arg.getValue(),
        branching_degree_arg.getValue()
    };
    arguments.CheckOrDie();

    return arguments;
}


void ExternalMergeSort(std::string input_file, std::string output_file, long long block_size, int branching_degree) {
    std::vector<std::string> temp_file_names = SplitFileIntoSortedFiles(input_file, input_file + "_tmp", block_size, branching_degree);
    MergeFiles(temp_file_names, output_file);

    // remove unnecessary files
    for (std::string temp_file: temp_file_names) {
        ::remove(temp_file.c_str());
    }
}

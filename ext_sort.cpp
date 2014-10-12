#include <fstream>
#include <vector>
#include <tclap/CmdLine.h>
#include "binary_heap.hpp"

// Structure to hold command line arguments
struct CliArguments {
    std::string input_file;
    std::string output_file;
    int block_size;
    int branching_degree;
};
// Parses command line arguments from input
// Returns structure with extracted arguments
CliArguments ParseCliArguments(int argc, char **argv);
// Sorts file with name input_file and writes result into file with name output_file
// Assumes input file and output file are binary
// Block size is maximum file size which can be loaded into RAM
// Branching is maximum number of splits per file
void ExternalMergeSort(std::string input_file, std::string output_file, int block_size, int branching);
// Splits a file into a sequence of sorted files
// Sorting is performed sequentially (not parallel),
// because it's assumed that only block of size block_size fits into memory
// Returns vector with filenames, that store sorted files
std::vector<std::string> SplitFileIntoSortedFiles(std::string input_file, int block_size, int branching);
// Merges files into one big file
// Input is in input_file_names, output is in output_file_name
void MergeFiles(const std::vector<std::string> &input_file_names, std::string output_file_name);

// Helper structure to merge files
// Stores a pointer to file and last value read from that file
struct MergeElement;

int main(int argc, char **argv) {
    CliArguments arguments = ParseCliArguments(argc, argv);
    ExternalMergeSort(arguments.input_file, arguments.output_file, arguments.block_size, arguments.branching_degree);
    return 0;
}


struct MergeElement {
    std::vector<std::ifstream>::iterator file_iterator_;
    int value_;

public:
    MergeElement(std::vector<std::ifstream>::iterator file_iterator) :
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
        file_iterator_->read((char *) &value_, sizeof(value_));
    }

    int GetValue() const {
        return value_;
    }

    bool Stop() const {
        return !(*file_iterator_);
    }
};

void MergeFiles(const std::vector<std::string> &input_file_names, std::string output_file_name) {
    algorithms::BinaryHeap<MergeElement, std::greater<MergeElement>> merge_elements;

    std::vector<std::ifstream> in_files;
    for (std::string file_name: input_file_names) {
        in_files.push_back(std::ifstream(file_name, std::ios_base::in | std::ios_base::binary));
    }
    std::ofstream out_file(output_file_name, std::ios_base::out | std::ios_base::binary);

    // std::vector<int> merge;
    for (auto file_iter = in_files.begin(); file_iter != in_files.end(); ++file_iter) {
        merge_elements.Insert(MergeElement(file_iter));
    }

    while (merge_elements.GetSize() > 0) {
        MergeElement minimum = merge_elements.GetTop();
        merge_elements.Pop();

        int value = minimum.GetValue();
        std::cout << "v " << value << std::endl;
        out_file.write((char *) &value, sizeof(value));

        minimum.ReadNextValue();
        if (!minimum.Stop()) {
            merge_elements.Insert(minimum);
        }
    }
}


CliArguments ParseCliArguments(int argc, char **argv) {
    try {
        TCLAP::CmdLine cmd("Sorting in external memory", ' ', "1.0");
        TCLAP::ValueArg<int> block_size_arg("b", "block-size", "Size of one block to use (in Mb)", false, 128, "integer");
        TCLAP::ValueArg<int> branching_degree_arg("d", "branching", "Branching degree", false, 4, "integer");
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

        return arguments;
    } catch (TCLAP::ArgException &arg) {
        std::cout << "Invalid arguments: " << arg.error() << "for arg " << arg.argId() << std::endl;
    }

    return CliArguments();
}


void ExternalMergeSort(std::string input_file, std::string output_file, int block_size, int branching) {
    std::vector<std::string> temp_file_names = SplitFileIntoSortedFiles(input_file, block_size, branching);
    MergeFiles(temp_file_names, output_file);
}

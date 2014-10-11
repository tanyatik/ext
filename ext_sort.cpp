#include <fstream>
#include <vector>
#include <tclap/CmdLine.h>

void parse_cli_arguments(int argc, char **argv) {
    try {
        TCLAP::CmdLine cmd("Sorting in external memory", ' ', "1.0");
        TCLAP::ValueArg<int> block_size_arg("b", "block-size", "Size of one block to use (in Mb)", false, 128, "integer");
        TCLAP::ValueArg<int> branching_degree_arg("d", "degree", "Brancking degree", false, 4, "integer");
        TCLAP::UnlabeledValueArg<std::string> input_file_arg( "input_file", "Input file name", true, "", "nameString");
        TCLAP::UnlabeledValueArg<std::string> output_file_arg( "output_file", "Output file name", true, "", "nameString");
        cmd.add(input_file_arg);
        cmd.add(output_file_arg);
        cmd.add(block_size_arg);
        cmd.add(branching_degree_arg);

        cmd.parse(argc, argv);

        int block_size = block_size_arg.getValue();
        int branching_degree = branching_degree_arg.getValue();
        std::string input_file = input_file_arg.getValue();
        std::string output_file = output_file_arg.getValue();

        std::cout << "Parsed arguments " << block_size
             << ", " << branching_degree
             << ", " << input_file
             << ", " << output_file
             << std::endl;
    } catch (TCLAP::ArgException &arg) {
        std::cout << "Invalid arguments: " << arg.error() << "for arg " << arg.argId() << std::endl;
    }
}

int main(int argc, char **argv) {
    parse_cli_arguments(argc, argv);
    return 0;
}

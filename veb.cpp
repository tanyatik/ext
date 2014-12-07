#include <iostream>
#include <vector>
#include <tclap/CmdLine.h>
#include <assert.h>
#include <cmath>


const int DEFAULT_MEMORY_SIZE = 1024 * 1024 * 1024; // 1 GB

typedef long long Data;

struct CliArguments {
    std::string input_file;
    std::string output_file;
    long long memory_size; // in bytes

    void CheckOrDie() const {
        const long long GB4 = 1024 * 1024 * 1024 * 4;
        if (memory_size % 4 != 0 ||
                memory_size < 4 ||
                memory_size > GB4) {
             throw std::runtime_error("Memory size must be in range [4 B; 4 GB]");
        }
    }
};

struct SearchEvent {
    size_t values_number;
    long long time_sec;
};


template<typename T>
struct VEBNode {
    T key;
    size_t left_index;
    size_t right_index;

    VEBNode(T key = 0, size_t left_index = 0, size_t right_index = 0) :
        key(key), left_index(left_index), right_index(right_index) {}
};

template<typename T>
class VEBLayout {
public:
    typedef VEBNode<T> TNode;
    typedef VEBLayout<T> TLayout;

private:
    std::vector<TNode> layout_;

    static bool IsPowerOf2(size_t x) {
        return (x & (x - 1)) == 0;
    }

public:
    void DebugPrint() const {
        for (size_t i = 0; i < layout_.size(); ++i) {
            std::cerr << layout_[i].key << " [" << layout_[i].left_index << ", " <<
                    layout_[i].right_index << "] " << std::endl;
        }
    }


    template<typename TInputIter>
    std::vector<T> Create(TInputIter begin, TInputIter end,
            size_t out_begin) {
        size_t size = std::distance(begin, end);
        std::cerr << "Size: " << size << std::endl;
        assert(IsPowerOf2(size + 1));

        if (size == 1) {
            layout_[out_begin] = TNode(*begin, -1, -1);
            return { (int) out_begin };
        }

        size_t height = ceil(log2(size));
        size_t top_height = height >> 1;
        size_t bottom_height = height - top_height;
        size_t top_size = (1 << top_height) - 1;
        size_t top_total_leaves_number = (top_size + 1) >> 1;
        size_t bottom_size = (1 << bottom_height) - 1;
        size_t bottom_leaves_number = (bottom_size + 1) >> 1;
        size_t bottom_total_leaves_number = (size + 1) >> 1;

        size_t bottom_number = top_size + 1;
        std::cerr << "out_begin " << out_begin
                << " height " << height
                << " top height " << top_height
                << " top_total_leaves_number " << top_total_leaves_number
                << " bottom height " << bottom_height
                << " top size " << top_size
                << " bottom size " << bottom_size
                << " bottom number " << bottom_number
                << " bottom leaves number " << bottom_leaves_number
                << " bottom total leaves number " << bottom_total_leaves_number << std::endl;

        size_t out = out_begin;

        // create top tree
        auto current_end = std::next(begin, top_size);
        std::vector<T> top_leaves_idxs = Create(begin, current_end, out_begin);

        begin = std::next(begin, top_size);
        out_begin += top_size;

        size_t top_leaves_number = bottom_number >> 1;
        size_t top_leaves_begin = out + top_size - top_leaves_number;
        size_t bottom_begin = top_leaves_begin + top_leaves_number;

        // create bottom trees
        std::vector<T> bottom_total_leaves_idxs;
        for (size_t i = 0, t_idx = 0; i < bottom_number; ++i) {
            end = std::next(begin, bottom_size);
            std::vector<T> bottom_leaves_idxs = Create(begin, end, out_begin);
            bottom_total_leaves_idxs.insert(bottom_total_leaves_idxs.end(), bottom_leaves_idxs.begin(), bottom_leaves_idxs.end());
            if (i % 2 == 0) {
                layout_[top_leaves_idxs[t_idx]].left_index = out_begin;
            } else {
                layout_[top_leaves_idxs[t_idx]].right_index = out_begin;
                ++t_idx;
            }
            begin = std::next(begin, top_size);
            out_begin += bottom_size;
        }

        return bottom_total_leaves_idxs;
    }

    void Create(const std::vector<T>& keys) {
        layout_.resize(keys.size());

        std::vector<T> temp = Create(keys.begin(), keys.end(), 0);

        for (int i = 0; i < temp.size(); ++i) {
            std::cerr << temp[i] << std::endl;
        }
    }
};


CliArguments ParseCliArguments(int argc, char **argv) {
    TCLAP::CmdLine cmd("Searching in external memory with Van-Emde-Boas algorithm", ' ', "1.0");
    TCLAP::ValueArg<int> memory_size_arg("m", "memory_size", "Maximum memory amount to use", false, DEFAULT_MEMORY_SIZE, "integer");
    TCLAP::UnlabeledValueArg<std::string> input_file_arg( "input_file", "Input file name", true, "", "nameString");
    TCLAP::UnlabeledValueArg<std::string> output_file_arg( "output_file", "Output file name", true, "", "nameString");
    cmd.add(input_file_arg);
    cmd.add(output_file_arg);
    cmd.add(memory_size_arg);

    cmd.parse(argc, argv);

    CliArguments arguments {
        input_file_arg.getValue(),
        output_file_arg.getValue(),
        memory_size_arg.getValue(),
    };
    arguments.CheckOrDie();

    return arguments;
}

void Test() {
/*
    {
        std::cerr << "TEST CASE #1" << std::endl;
        VEBLayout<int> layout;
        auto v = {1};
        layout.Create(v);
        layout.DebugPrint();
    }

    {
        std::cerr << "TEST CASE #2" << std::endl;
        VEBLayout<int> layout;
        auto v = {2, 1, 3};
        layout.Create(v);
        layout.DebugPrint();
    }
*/
/*
    {
        std::cerr << "TEST CASE #3" << std::endl;
        VEBLayout<int> layout;
        auto v = {3, 2, 7, 1, 5, 6, 8};
        layout.Create(v);
        layout.DebugPrint();
    }
*/
    {
        std::cerr << "TEST CASE #4" << std::endl;
        VEBLayout<int> layout;
        auto v = {8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15};
        layout.Create(v);
        layout.DebugPrint();
    }
}


int main(int argc, char **argv) {
    try {
        // CliArguments arguments = ParseCliArguments(argc, argv);
        Test();
    } catch (TCLAP::ArgException &arg) {
        std::cout << "Invalid arguments: " << arg.error() << "for arg " << arg.argId() << std::endl;
        return 1;
    } catch (std::runtime_error& err) {
        std::cout << "Runtime error: " << err.what() << std::endl;
        return 1;
    }

    return 0;
}

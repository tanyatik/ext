#include <iostream>
#include <fstream>
#include <vector>
#include <tclap/CmdLine.h>
#include <assert.h>
#include <cmath>
#include <queue>


static bool IsPowerOf2(size_t x) {
    return (x & (x - 1)) == 0;
}

static size_t GetCeilPowerOf2(size_t x) {
    if (IsPowerOf2(x)) {
        return x;
    }
    size_t shifts = 0;
    while (x) {
        x >>= 1;
        ++shifts;
    }
    x = 1LU << (shifts);

    return x;
}


struct SearchEvent {
    size_t values_number;
    long long time_sec;
};


template<typename T>
std::vector<T> BfsFromInorder(const std::vector<T>& keys) {
    std::vector<T> bfs;
    std::queue<std::pair<size_t, size_t>> bfs_ranges;
    bfs_ranges.push(std::make_pair(0, keys.size()));

    while (!bfs_ranges.empty()) {
        std::pair<size_t, size_t> range = bfs_ranges.front();
        bfs_ranges.pop();

        if (range.first == range.second) {
            continue;
        }

        if (range.first + 1 == range.second) {
            bfs.push_back(keys[range.first]);
            continue;
        }

        size_t middle = (range.first + range.second) >> 1;

        bfs.push_back(keys[middle]);
        bfs_ranges.push(std::make_pair(range.first, middle));
        bfs_ranges.push(std::make_pair(middle + 1, range.second));
    }

    return bfs;
}


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


public:
    void DebugPrint() const {
        for (size_t i = 0; i < layout_.size(); ++i) {
            std::cerr << layout_[i].key << " [" << layout_[i].left_index << ", " <<
                    layout_[i].right_index << "] " << std::endl;
        }
    }

    template<typename TInputIter>
    std::vector<size_t> Create(TInputIter begin, TInputIter end,
            size_t out_begin) {
        size_t size = std::distance(begin, end);
        size_t ceil_size = GetCeilPowerOf2(size) - 1;

        if (size == 1) {
            layout_[out_begin] = TNode(*begin, -1, -1);
            return { out_begin };
        }

        size_t height = ceil(log2(ceil_size));
        size_t top_height = height >> 1;
        size_t bottom_height = height - top_height;
        size_t top_size = (1 << top_height) - 1;
        size_t bottom_size = (1 << bottom_height) - 1;

        // create top tree
        auto current_begin = begin;
        size_t current_out_begin = out_begin;

        auto current_end = std::next(current_begin, top_size);
        std::vector<size_t> top_leaves_idxs = Create(current_begin, current_end, current_out_begin);

        current_begin = current_end;
        current_out_begin += top_size;

        // create bottom trees
        std::vector<size_t> bottom_total_leaves_idxs;
        bool place_to_left = true;
        size_t t_idx = 0;

        // need to output exactly size
        while (current_out_begin < out_begin + size) {
            size_t current_bottom_size = std::min(bottom_size, size + out_begin - current_out_begin);
            current_end = std::next(current_begin, current_bottom_size);

            std::vector<size_t> bottom_leaves_idxs = Create(current_begin, current_end, current_out_begin);
            current_begin = current_end;

            if (place_to_left) {
                layout_[top_leaves_idxs[t_idx]].left_index = current_out_begin;
            } else {
                layout_[top_leaves_idxs[t_idx++]].right_index = current_out_begin;
            }
            current_out_begin += current_bottom_size;

            place_to_left = !place_to_left;
            bottom_total_leaves_idxs.insert(bottom_total_leaves_idxs.end(), bottom_leaves_idxs.begin(), bottom_leaves_idxs.end());
        }

        return bottom_total_leaves_idxs;
    }


    // Assume keys are in inorder format
    void Create(const std::vector<T>& keys) {
        layout_.resize(keys.size());

        Create(keys.begin(), keys.end(), 0);
    }

    bool Find(T key) {
        size_t idx = 0;

        while (idx != -1) {
            if (layout_[idx].key == key) {
                return true;
            } else if (layout_[idx].key < key) {
                idx = layout_[idx].right_child;
            } else {
                idx = layout_[idx].left_child;
            }
        }

        return false;
    }
};


std::vector<int> ReadDataArray() {
    int n;
    std::cin >> n;

    std::vector<int> v(n);
    for (auto& i : v) {
        std::cin >> i;
    }

    return v;
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
    {
        std::cerr << "TEST CASE #3" << std::endl;
        VEBLayout<int> layout;
        auto v = {1, 2, 3, 4, 5, 6, 7};
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


void TestBFS() {
    {
        std::cerr << "TEST CASE #1" << std::endl;
        std::vector<int> v = {1};
        auto b = BfsFromInorder(v);
        for (auto i : b) {
            std::cerr << i << " ";
        }
        std::cerr << std::endl;
    }
    {
        std::cerr << "TEST CASE #2" << std::endl;
        std::vector<int> v = {1, 2};
        auto b = BfsFromInorder(v);
        for (auto i : b) {
            std::cerr << i << " ";
        }
        std::cerr << std::endl;
    }
    {
        std::cerr << "TEST CASE #3" << std::endl;
        std::vector<int> v = {1, 2, 3};
        auto b = BfsFromInorder(v);
        for (auto i : b) {
            std::cerr << i << " ";
        }
        std::cerr << std::endl;
    }
    {
        std::cerr << "TEST CASE #4" << std::endl;
        std::vector<int> v = {1, 2, 3, 4};
        auto b = BfsFromInorder(v);
        for (auto i : b) {
            std::cerr << i << " ";
        }
        std::cerr << std::endl;
    }
    {
        std::cerr << "TEST CASE #5" << std::endl;
        std::vector<int> v = {1, 2, 3, 4, 5};
        auto b = BfsFromInorder(v);
        for (auto i : b) {
            std::cerr << i << " ";
        }
        std::cerr << std::endl;
    }
    {
        std::cerr << "TEST CASE #6" << std::endl;
        std::vector<int> v = {1, 2, 3, 4, 5, 6};
        auto b = BfsFromInorder(v);
        for (auto i : b) {
            std::cerr << i << " ";
        }
        std::cerr << std::endl;
    }
    {
        std::cerr << "TEST CASE #7" << std::endl;
        std::vector<int> v = {1, 2, 3, 4, 5, 6, 7};
        auto b = BfsFromInorder(v);
        for (auto i : b) {
            std::cerr << i << " ";
        }
        std::cerr << std::endl;
    }
    {
        std::cerr << "TEST CASE #8" << std::endl;
        std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
        auto b = BfsFromInorder(v);
        for (auto i : b) {
            std::cerr << i << " ";
        }
        std::cerr << std::endl;
    }
}


void TestCeilPowerOf2() {
    for (int i = 0; i < 100; ++i) {
        std::cerr << i << "\t" << GetCeilPowerOf2(i) << std::endl;
    }
}


int main() {
    try {
        VEBLayout<int> layout;
        std::vector<int> data = ReadDataArray();
        layout.Create(BfsFromInorder(data));
        layout.DebugPrint();

        Test();
        // TestBFS();
        // TestCeilPowerOf2();

    } catch (std::runtime_error& err) {
        std::cout << "Runtime error: " << err.what() << std::endl;
        return 1;
    }

    return 0;
}

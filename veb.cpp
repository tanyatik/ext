#include <iostream>
#include <fstream>
#include <vector>
#include <tclap/CmdLine.h>
#include <assert.h>
#include <cmath>
#include <queue>


const size_t UNEXISTING_INDEX = 18446744073709551615LU;


static bool IsPowerOf2(size_t x) {
    return (x & (x - 1)) == 0;
}

static size_t GetNextPowerOf2(size_t x) {
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
    assert(IsPowerOf2(keys.size() + 1));
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
struct TreeNode {
    T key;
    size_t left_index;
    size_t right_index;

    TreeNode(T key = 0, size_t left_index = 0, size_t right_index = 0) :
        key(key), left_index(left_index), right_index(right_index) {}

    bool operator == (const TreeNode<T>& other) const {
        return key == other.key && left_index == other.left_index && right_index == other.right_index;
    }
};

template<typename T>
class VEBLayout {
public:
    typedef TreeNode<T> TNode;
    typedef VEBLayout<T> TLayout;

private:
    std::vector<TNode> layout_;

public:
    VEBLayout(const std::vector<int>& keys) :
        layout_(keys.size()) {
        Create(keys.begin(), keys.end(), 0);
    }


    void DebugPrint() const {
        for (size_t i = 0; i < layout_.size(); ++i) {
            std::cerr << layout_[i].key << " [" << layout_[i].left_index << ", " <<
                    layout_[i].right_index << "] " << std::endl;
        }
    }

    std::vector<TNode> layout() const { return layout_; }

    template<typename TInputIter>
    std::vector<size_t> Create(TInputIter begin, TInputIter end,
            size_t out) {
        using std::cerr;
        using std::endl;

        size_t size = std::distance(begin, end);
        assert(IsPowerOf2(size + 1));

        assert(size != 0);
        if (size == 1) {
            layout_[out] = TNode(*begin, UNEXISTING_INDEX, UNEXISTING_INDEX);
            return { out };
        }

        size_t height = ceil(log2(size));
        size_t top_height = height >> 1;
        size_t bottom_height = height - top_height;
        size_t top_size = (1 << top_height) - 1;
        size_t bottom_size = (1 << bottom_height) - 1;

        // create top tree
        size_t out_end  = out + size;

        end = std::next(begin, top_size);
        std::vector<size_t> top_leaves = Create(begin, end, out);

        begin = end;
        out += top_size;

        // create bottom trees
        std::vector<size_t> leaves;
        bool place_to_left = true;
        size_t t_idx = 0;

        // need to output exactly size
        while (out < out_end) {
            size_t current_bottom_size = std::min(bottom_size, out_end - out);
            end = std::next(begin, current_bottom_size);

            std::vector<size_t> bottom_leaves = Create(begin, end, out);
            begin = end;

            if (place_to_left) {
                layout_[top_leaves[t_idx]].left_index = out;
            } else {
                layout_[top_leaves[t_idx++]].right_index = out;
            }
            out += current_bottom_size;

            place_to_left = !place_to_left;
            leaves.insert(leaves.end(), bottom_leaves.begin(), bottom_leaves.end());
        }

        return leaves;
    }


    bool Find(T key) {
        size_t idx = 0;

        while (idx != UNEXISTING_INDEX) {
            if (layout_[idx].key == key) {
                return true;
            } else if (layout_[idx].key < key) {
                idx = layout_[idx].right_index;
            } else {
                idx = layout_[idx].left_index;
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


void TestCreateVEB() {
    const size_t E = UNEXISTING_INDEX;
    typedef TreeNode<int> T;
    using std::vector;
    typedef VEBLayout<int> L;

    std::vector<TreeNode<int>> pre = {TreeNode<int>(1, -1, -1)};
    assert(L({1}).layout() == pre);

    pre = { T(2, 1, 2), T(1, E, E), T(3, E, E) };
    assert(L({2, 1, 3}).layout() == pre);

    pre = { T(1, 1, 4), T(2, 2, 3), T(3, E, E), T(4, E, E),
            T(5, 5, 6), T(6, E, E), T(7, E, E)};
    L({0, 1, 2, 3, 4, 5, 6}).DebugPrint();
    std::cerr << std::endl;
    assert(L({1, 2, 3, 4, 5, 6, 7}).layout() == pre);

    L({4, 2, 6, 1, 3, 5, 7}).DebugPrint();
    std::cerr << std::endl;


    pre = { T(8, 1, 2), T(4, 3, 6), T(12, 9, 12),
            T(2, 4, 5), T(6, E, E), T(10, E, E),
            T(14, 7, 8), T(1, E, E), T(3, E, E),
            T(5, 10, 11), T(7, E, E), T(9, E, E),
            T(11, 13, 14), T(13, E, E), T(15, E, E) };
    assert(L({8, 4, 12, 2, 6, 10, 14, 1, 3, 5, 7, 9, 11, 13, 15}).layout() == pre);
}


void TestBFS() {
    assert(BfsFromInorder(std::vector<int>({1})) == std::vector<int>({1}));
    assert(BfsFromInorder(std::vector<int>({1, 2, 3})) == std::vector<int>({2, 1, 3}));
    assert(BfsFromInorder(std::vector<int>({1, 2, 3, 4, 5, 6, 7})) == std::vector<int>({4, 2, 6, 1, 3, 5, 7}));
    assert(BfsFromInorder(std::vector<int>({1, 2, 3, 5, 8, 10, 17})) == std::vector<int>({5, 2, 10, 1, 3, 8, 17}));
}


void TestFindVEB() {
    typedef TreeNode<int> T;
    using std::vector;
    typedef VEBLayout<int> L;

    L layout(BfsFromInorder(std::vector<int>({1, 2, 3, 5, 8, 10, 17})));
    layout.DebugPrint();
    assert(layout.Find(1));
    assert(layout.Find(2));
    assert(layout.Find(3));
    assert(layout.Find(5));
    assert(layout.Find(8));
    assert(layout.Find(10));
    assert(layout.Find(17));

    assert(!layout.Find(4));
    assert(!layout.Find(6));
    assert(!layout.Find(7));
    assert(!layout.Find(9));
    assert(!layout.Find(11));
    assert(!layout.Find(12));
    assert(!layout.Find(13));
    assert(!layout.Find(14));
    assert(!layout.Find(15));
    assert(!layout.Find(16));
    assert(!layout.Find(18));
    assert(!layout.Find(19));
    assert(!layout.Find(20));
}


int main() {
    try {
        // std::vector<int> data = ReadDataArray();
        // VEBLayout<int> layout(BfsFromInorder(data));
        // layout.DebugPrint();

        // TestCeilPowerOf2();
        TestCreateVEB();
        TestBFS();
        TestFindVEB();

    } catch (std::runtime_error& err) {
        std::cout << "Runtime error: " << err.what() << std::endl;
        return 1;
    }

    return 0;
}

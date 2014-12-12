#include <iostream>
#include <fstream>
#include <vector>
#include <tclap/CmdLine.h>
#include <assert.h>
#include <cmath>
#include <queue>
#include <algorithm>


const size_t UNEXISTING_INDEX = 18446744073709551615LU;

/*
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
*/


struct SearchEvent {
    size_t values_number;
    long long time_sec;
};



template<typename T>
struct TreeNode {
    T key;
    size_t left_child;
    size_t right_child;

    TreeNode(T key = 0, size_t left_child = UNEXISTING_INDEX, size_t right_child = UNEXISTING_INDEX) :
        key(key), left_child(left_child), right_child(right_child) {}

    bool operator == (const TreeNode<T>& other) const {
        return key == other.key && left_child == other.left_child && right_child == other.right_child;
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

        std::vector<TNode> temp_layout(keys.size());
        for (size_t idx = 0; idx < keys.size(); ++idx) {
            temp_layout[idx].key = keys[idx];
        }

        Create(keys.begin(), keys.end(), 0);
    }


    void DebugPrint() const {
        for (size_t i = 0; i < layout_.size(); ++i) {
            std::cerr << "i = " << i << " : " << layout_[i].key << " [";
            if (layout_[i].left_child != UNEXISTING_INDEX) {
                std::cerr << layout_[layout_[i].left_child].key << " ";
            }
            std::cerr << " at " << layout_[i].left_child << ", ";


            if (layout_[i].right_child != UNEXISTING_INDEX) {
                std::cerr << layout_[layout_[i].right_child].key << " ";
            }
            std::cerr << " at " << layout_[i].right_child << "] " << std::endl;
        }
    }

    std::vector<TNode> layout() const { return layout_; }


    template<typename TInputIter>
    void Create(TInputIter begin, TInputIter end,
            size_t out_begin) {
        using std::cerr;
        using std::endl;

        size_t size = std::distance(begin, end);
        // std::cerr << "size " << size << std::endl;
        // assert(IsPowerOf2(size + 1));

        if (size == 0) {
            return;
        }
        if (size == 1) {
            layout_[out_begin] = *begin;
            return;
        }

        size_t height = floor(log2(size)) + 1;
        // std::cerr << "height " << height << std::endl;
        size_t bottom_height = height >> 1;
        size_t top_height = height - bottom_height;
        size_t top_size = (1 << top_height) - 1;
        // std::cerr << "top size " << top_size << std::endl;
        size_t bottom_size = (1 << bottom_height) - 1;
        // std::cerr << "bottom size " << bottom_size << std::endl;
        size_t top_leaves_number = (top_size + 1) >> 1;
        // std::cerr << "top leaves number " << top_leaves_number << std::endl;


        // leave place for top tree
        size_t out = out_begin + top_size;
        size_t out_top_leaves = out_begin + (top_size - top_leaves_number);
        size_t out_end = out_begin + size;
        // out += top_size;

        std::vector<TNode> top_data;

        bool set_left_child = true;
        // create subtrees
        while (out != out_end) {
            // std::cerr << "out " << out << " end " << out_end << std::endl;
            // std::cerr << "dist " << std::distance(begin, end) << std::endl;
            size_t current_bottom_size = std::min(bottom_size, out_end - out);
            // std::cerr << "current bottom size " << current_bottom_size << std::endl;

            Create(begin, begin + current_bottom_size, out);
            begin += current_bottom_size;

            // link bottom subtrees to top subtree

            if (set_left_child) {
                // if (size == 5) std::cerr << "set left child of " << out_top_leaves << " to " << out << std::endl;
                top_data.push_back(*begin);
                top_data.back().left_child = out;
                begin++;
                // layout_[out_top_leaves].left_child = out;
            } else {
                // if (size == 5) std::cerr << "set right child of " << out_top_leaves << " to " << out << std::endl;
                top_data.back().right_child = out;
                if (begin != end) {
                    top_data.push_back(*begin);
                }
                begin++;
                // layout_[out_top_leaves++].right_child = out;
            }
            set_left_child = !set_left_child;
            // children[begin_distance] = out;

//                if (size == 5) {
//                    std::cerr << "add top data " << begin->key << " ["
//                            << begin->left_child << ", " << begin->right_child << "]" << std::endl;
//                }
            // bottom subtrees keys alternate with keys from top subtree
            out += current_bottom_size;
        }

        top_data.insert(top_data.end(), begin, end);
        // top subtree (all links are prepared)
//        if (size == 5) {
//            std::cerr << "create top subtree " << top_data.size() << std::endl;
//            for (auto t : top_data) { std::cerr << t.key << " [" << t.left_child << " " << t.right_child << "]"; }
//            std::cerr << std::endl;
//        }

        Create(top_data.begin(), top_data.end(), out_begin);
    }


    bool Find(T key) {
        size_t idx = 0;

        while (idx != UNEXISTING_INDEX) {
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



bool TestCreateFindVEB(size_t size, size_t max_element) {
    std::vector<int> v(size);
    std::vector<bool> ex(max_element, false);
    for (int i = 0; i < size; ++i) {
        v[i] = rand() % max_element;
        ex[v[i]] = true;
    }
    std::sort(v.begin(), v.end());

    // std::cerr << "TEST " << size << std::endl;
    auto l = VEBLayout<int>(std::vector<int>(v.begin(), std::unique(v.begin(), v.end())));

    for (int i = 0; i < max_element; ++i) {
        bool found = l.Find(i);
        bool exists = ex[i];
        if (!((found && exists) || (!found && !exists))) {
            std::cerr << "found " << found << " exists " << exists << std::endl;
            std::cerr << "Not found: " << i <<
                " in VEB with keys: ";
            for (auto k : v) { std::cerr << k << ","; }
            std::cerr << std::endl;
            l.DebugPrint();
            return false;
        }
    }
    return true;
}


void TestCreateFindVEB() {
    for (int size = 1; size < 1000; ++size) {
        assert(TestCreateFindVEB(size, 1.5 * size));
        assert(TestCreateFindVEB(size, 2 * size));
        assert(TestCreateFindVEB(size, 4 * size));
    }
    std::cerr << "All tests passed" << std::endl;
}


int main() {
    try {
        // std::vector<int> data = ReadDataArray();
        // VEBLayout<int> layout(BfsFromInorder(data));
        // layout.DebugPrint();

        // TestCeilPowerOf2();
        // TestCreateFindVEB(6, 12);
        TestCreateFindVEB();
        // TestFindVEB();

    } catch (std::runtime_error& err) {
        std::cout << "Runtime error: " << err.what() << std::endl;
        return 1;
    }

    return 0;
}

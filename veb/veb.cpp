#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <queue>
#include <algorithm>
#include <sys/time.h>


const size_t UNEXISTING_INDEX = 18446744073709551615LU;


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
        size_t size = std::distance(begin, end);

        if (size == 0) {
            return;
        }
        if (size == 1) {
            layout_[out_begin] = *begin;
            return;
        }

        size_t height = floor(log2(size)) + 1;
        size_t bottom_height = height >> 1;
        size_t top_height = height - bottom_height;
        size_t top_size = (1 << top_height) - 1;
        size_t bottom_size = (1 << bottom_height) - 1;

        // size_t top_leaves_number = (top_size + 1) >> 1;

        // leave place for top tree
        size_t out = out_begin + top_size;
        // size_t out_top_leaves = out_begin + (top_size - top_leaves_number);
        size_t out_end = out_begin + size;

        std::vector<TNode> top_data;

        bool set_left_child = true;
        // create subtrees
        while (out != out_end) {
            size_t current_bottom_size = std::min(bottom_size, out_end - out);

            Create(begin, begin + current_bottom_size, out);
            begin += current_bottom_size;

            // link bottom subtrees to top subtree
            // bottom subtrees keys alternate with keys from top subtree
            if (set_left_child) {
                top_data.push_back(*begin);
                top_data.back().left_child = out;
                begin++;
            } else {
                top_data.back().right_child = out;
                if (begin != end) {
                    top_data.push_back(*begin);
                }
                begin++;
            }
            set_left_child = !set_left_child;
            out += current_bottom_size;
        }

        top_data.insert(top_data.end(), begin, end);

        // create top subtree (all links are prepared)
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


void WriteDataArray(const std::vector<int>& data) {
    for (auto d : data) {
        std::cout << d << " ";
    }
    std::cout << std::endl;
}


bool TestCreateFindVEB(size_t size, size_t max_element) {
    std::vector<int> v(size);
    std::vector<bool> ex(max_element, false);
    for (size_t i = 0; i < size; ++i) {
        v[i] = rand() % max_element;
        ex[v[i]] = true;
    }
    std::sort(v.begin(), v.end());

    // std::cerr << "TEST " << size << std::endl;
    // auto l = VEBLayout<int>(std::vector<int>(v.begin(), v.end()));
    auto l = VEBLayout<int>(v);

    for (size_t i = 0; i < max_element; ++i) {
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


std::vector<int> BenchmarkVEB(const std::vector<int>& data, const std::vector<int>& queries) {
    timeval seconds, elapsed;
    gettimeofday(&seconds, nullptr);
    auto l = VEBLayout<int>(data);
    gettimeofday(&elapsed, nullptr);
    std::cout << "Van Emde Boas layout for " << data.size() << " keys created in "
            << 1000 * (elapsed.tv_sec - seconds.tv_sec) + (elapsed.tv_usec - seconds.tv_usec) / 1000
            << " milliseconds" << std::endl;

    // benchmark binary search
    auto bs_answer = std::vector<int>(queries.size());
    gettimeofday(&seconds, nullptr);
    for (size_t idx = 0; idx < queries.size(); ++idx) {
        bs_answer[idx] = (std::binary_search(data.begin(), data.end(), queries[idx]) ? 1 : 0);
    }
    gettimeofday(&elapsed, nullptr);
    std::cout << "Number of queries: " << queries.size() << std::endl;
    std::cout << "std::binary_search: \t\t"
            << 1000 * (elapsed.tv_sec - seconds.tv_sec) + (elapsed.tv_usec - seconds.tv_usec) / 1000
            << " milliseconds" << std::endl;

    // benchmark VEB binary search
    auto veb_answer = std::vector<int>(queries.size());
    gettimeofday(&seconds, nullptr);
    for (size_t idx = 0; idx < queries.size(); ++idx) {
        veb_answer[idx] = (l.Find(queries[idx]) ? 1 : 0);
    }
    gettimeofday(&elapsed, nullptr);
    std::cout << "Van Emde Boas search: \t\t"
            << 1000 * (elapsed.tv_sec - seconds.tv_sec) + (elapsed.tv_usec - seconds.tv_usec) / 1000
            << " milliseconds" << std::endl;

    return veb_answer;
}

int main(int argc, char **) {
    try {
        std::vector<int> keys = ReadDataArray();
        std::vector<int> queries = ReadDataArray();

        std::vector<int> answers = BenchmarkVEB(keys, queries);

        if (argc == 1) {
            WriteDataArray(answers);
        }

        return 0;
    } catch (std::runtime_error& err) {
        std::cout << "Runtime error: " << err.what() << std::endl;
        return 1;
    }
}

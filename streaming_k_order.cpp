#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <assert.h>


using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::sort;
using std::min;
using std::max;


const size_t MAX_MEMORY = 2 * 1024 * 1024 * 1024;
const size_t MAX_CHUNK = 75 * 1024 * 1024;
const size_t MAX_HEIGHT = 30;


struct Filter {
    int left_range;
    int right_range;

    Filter(int l, int r) : left_range(l), right_range(r) {}

    bool operator() (int value) {
        return left_range <= value && value <= right_range;
    }
};


size_t GetChunkSize() {
    return 4;
    // return MAX_CHUNK;
}


vector<int> MergeSubtrees(const vector<int>& left, const vector<int>& right, int thinning_coeff) {
    auto begin_first = left.begin();
    auto begin_second = right.begin();
    vector<int> result;

    // While there are elements in the left or right runs
    int num = 0;
    while (num < left.size() + right.size()) {
        // If left run head exists and is <= existing right run head.
        if (begin_first < left.end() &&
                (begin_second >= right.end() || !(*begin_second < *begin_first))) {
            if ((num + 1) % thinning_coeff == 0) {
                result.push_back(*begin_first);  // Increment begin_first after using it as an index.
            }
            ++begin_first;
        }
        else {
            if ((num + 1) % thinning_coeff == 0) {
                result.push_back(*begin_second);  // Increment begin_first after using it as an index.
            }
            begin_second++;  // Increment begin_second after using it as an index.
        }
        ++num;
    }

    return result;
}


vector<int> ReadTreeChunk(Filter filter, int chunk_size, bool* stop) {
    vector<int> chunk;
    while (!*stop && chunk.size() < chunk_size) {  //    int idx = 0; idx < chunk_size; ++idx) {
        int value;
        cin >> value;
        if (value == 0) {
            *stop = true;
        } else if (filter(value)) {
            chunk.push_back(value);
        }
    }
    sort(chunk.begin(), chunk.end());
    return chunk;
}


vector<int> BuildTree(Filter filter, size_t chunk_size, bool* stop, size_t* height = nullptr) {
    size_t left_height = 1;
    vector<int> left_subtree = ReadTreeChunk(filter, chunk_size, stop);

    while (!*stop && (!height || (height && left_height < *height))) {
        // get right subtree of the same height
        size_t right_height = left_height;
        vector<int> right_subtree = BuildTree(filter, chunk_size, stop, &right_height);
        if (right_height < left_height) {
            *stop = true;
        }

        if (right_subtree.size() > 0) {
            left_subtree = MergeSubtrees(left_subtree, right_subtree, 2);
            left_height += 1;
            cout << "left height " << left_height << endl;
        }
    }

    if (height) {
        *height = left_height;
    }
    return left_subtree;
}


int MaximumGreater(size_t level, int pow2, size_t j) {
    cout << "M" << j << " = " << (level + j - 1) * pow2 << endl;
    return (level + j - 1) * pow2;
}

int MinimumGreater(size_t level, int pow2, size_t j) {
    cout << "L" << j << " = " << (j * pow2 - 1) << endl;
    return (j * pow2 - 1);
}


Filter GetNewFilter(size_t height, int k_order, const vector<int>& tree_root) {
    size_t level = height - 1;
    int pow2 = int(pow(2, level));

    int alpha = 0;
    int beta = tree_root.size() - 1;

    int j1 = int(ceil(1.0 * k_order / pow2)) - int(level);
    if (1 <= j1 && j1 <= tree_root.size()) {
        beta = tree_root.size() - j1;
    }

    int j2 = int(ceil(1.0 * k_order / pow2));
    if (1 <= j2 && j2 <= tree_root.size()) {
        alpha = tree_root.size() - j2;
    }
    return Filter(tree_root.at(alpha), tree_root.at(beta));
}


Filter FirstPass(int k_order) {
    size_t chunk_size = GetChunkSize();

    bool stop = false;
    size_t height = INT_MAX;
    vector<int> tree_root = BuildTree(Filter(INT_MIN, INT_MAX), chunk_size, &stop, &height);

    return GetNewFilter(height, k_order, tree_root);
}


int KOrderStatistics(int k_order) {
    Filter first_pass_result = FirstPass(k_order);

    cout << "[" << first_pass_result.left_range << ", " << first_pass_result.right_range << "]" << endl;
    // TODO
    return 0;
    // return SecondPass(first_pass_result);
}


void PrintStatistics(int k_order, int st) {
    cout << k_order << "-statictics of input stream is " << st << endl;
}


// TESTS
void TestMergeSubtrees() {
    cout << "TEST" << endl;
    vector<int> a = {11, 31, 41, 51, 71, 81};
    vector<int> b = {2, 3, 4, 5, 8, 15, 24};
    vector<int> res = MergeSubtrees(a, b, 2);
    for (int r : res) {
        cout << r << " ";
    }
    cout << endl;
}


void TestBuildTree() {
    bool stop = false;
    vector<int> tree = BuildTree(Filter(INT_MIN, INT_MAX), 4, &stop);
    for (int r : tree) {
        cout << r << " ";
    }
    cout << endl;
}


void PrintUsage() {
    cout << "Implementation of Munro-Paterson algorithm "
        << " of finding a k-order maximum in streaming data. " << endl
        << "Usage: " << endl
        << "./munpat k" << endl
        << "k should be in range [0..N-1], where N stands for input size." << endl;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        PrintUsage();
        return 0;
    }
    int k_order = atoi(argv[1]);

    int st = KOrderStatistics(k_order);
    PrintStatistics(k_order, st);

    // TestMergeSubtrees();
    // TestBuildTree();

    return 0;
}

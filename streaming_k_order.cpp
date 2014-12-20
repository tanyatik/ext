#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <assert.h>


using std::vector;
using std::cin;
using std::cerr;
using std::cout;
using std::endl;
using std::sort;
using std::min;
using std::max;


const size_t MAX_MEMORY = 2 * 1024 * 1024 * 1024;
const size_t MAX_MEMORY_COUNT = MAX_MEMORY / sizeof(int);
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


vector<int> ReadTreeChunk(Filter filter, size_t chunk_size, bool* stop, int *count_right) {
    vector<int> chunk;
    while (!(*stop) && chunk.size() < chunk_size) {
        int value;
        cin >> value;
        if (value == 0) {
            *stop = true;
        } else if (filter(value)) {
            chunk.push_back(value);
        } else if (value > filter.right_range) {
            ++*count_right;
        }
    }
    sort(chunk.begin(), chunk.end());
    return chunk;
}


vector<int> BuildTree(Filter filter, size_t chunk_size, bool* stop, size_t* height, int *count_right) {
    size_t left_height = 1;
    vector<int> left_subtree = ReadTreeChunk(filter, chunk_size, stop, count_right);

    while (!*stop && left_height < *height) {
        // get right subtree of the same height
        size_t right_height = left_height;
        vector<int> right_subtree = BuildTree(filter, chunk_size, stop, &right_height, count_right);
        if (right_height < left_height) {
            *stop = true;
        }

        if (right_subtree.size() > 0) {
            left_subtree = MergeSubtrees(left_subtree, right_subtree, 2);
            left_height += 1;
        }
    }

    if (height) {
        *height = left_height;
    }
    return left_subtree;
}


Filter GetNewFilter(Filter filter, size_t height, int k_order, const vector<int>& tree_root) {
    size_t level = height - 1;
    int pow2 = int(pow(2, level));
    cerr << "pow2 " << pow2 << endl;

    int j1 = int(ceil(1.0 * k_order / pow2)) - int(level);
    if (1 <= j1 && j1 <= tree_root.size()) {
        cerr << "j1 " << j1 << endl;
        int beta = tree_root.size() - j1;
        filter.right_range = tree_root.at(beta);
    }

    int j2 = int(ceil(1.0 * k_order / pow2)) + 2;
    if (1 <= j2 && j2 <= tree_root.size()) {
        cerr << "j2 " << j2 << endl;
        int alpha = tree_root.size() - j2;
        filter.left_range = tree_root.at(alpha);
    }


    assert(filter.left_range == INT_MIN || filter.right_range == INT_MAX ||
            filter.right_range - filter.left_range < pow2 * (pow2 - 1));
    return filter;
}


Filter FirstPass(Filter filter, int k_order) {
    size_t chunk_size = GetChunkSize();

    bool stop = false;
    size_t height = INT_MAX;
    int count_before = 0;
    vector<int> tree_root = BuildTree(filter, chunk_size, &stop, &height, &count_before);

    return GetNewFilter(filter, height, k_order, tree_root);
}


int SecondPass(Filter filter, int k_order) {
    int count_before = 0;

    // assert(filter.right_range - filter.left_range <= MAX_MEMORY_COUNT);

    bool stop = false;
    int count_right = 0;
    cerr << "2nd pass " << endl;
    vector<int> data = ReadTreeChunk(filter, MAX_MEMORY_COUNT, &stop, &count_right);
    cerr << "data size " << data.size() << endl;
    cerr << "count right " << count_right << endl;

    assert(data.size() <= MAX_MEMORY_COUNT);
    assert(k_order >= count_right);
    k_order -= count_right;
    assert(k_order <= data.size());
    cerr << "k order " << k_order << endl;

    cerr << "index in sorted " << std::distance(data.begin(), data.end() - k_order) << endl;
    std::nth_element(data.begin(), data.end() - k_order, data.end());
    return data[data.size() - k_order];
}


int KOrderStatistics(int k_order) {
    Filter filter = FirstPass(Filter(INT_MIN, INT_MAX), k_order);

    cerr << "[" << filter.left_range << ", " << filter.right_range << "]" << endl;
    return SecondPass(filter, k_order);
}


void PrintStatistics(int k_order, int st) {
    cout << k_order << "-statictics of input stream is " << st << endl;
}


// TESTS
void TestMergeSubtrees() {
    cerr << "TEST" << endl;
    vector<int> a = {11, 31, 41, 51, 71, 81};
    vector<int> b = {2, 3, 4, 5, 8, 15, 24};
    vector<int> res = MergeSubtrees(a, b, 2);
    for (int r : res) {
        cerr << r << " ";
    }
    cerr << endl;
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

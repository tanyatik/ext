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


struct Filter {
    int left_range;
    int right_range;

    Filter(int l, int r) : left_range(l), right_range(r) {}

    bool operator() (int value) {
        return left_range <= value && value <= right_range;
    }
};


size_t GetChunkSize() {
    return MAX_CHUNK;
}


vector<int> MergeSubtrees(const vector<int>& left, const vector<int>& right, int thinning_coeff) {
    auto begin_first = left.begin();
    auto begin_second = right.begin();
    vector<int> result;

    // While there are elements in the left or right runs
    size_t num = 0;
    while (num < left.size() + right.size()) {
        // If left run head exists and is <= existing right run head.
        if (begin_first < left.end() &&
                (begin_second >= right.end() || !(*begin_second < *begin_first))) {
            if ((num) % thinning_coeff == 0) {
                result.push_back(*begin_first);  // Increment begin_first before using it as an index.
            }
            ++begin_first;
        }
        else {
            if ((num) % thinning_coeff == 0) {
                result.push_back(*begin_second);  // Increment begin_first before using it as an index.
            }
            begin_second++;  // Increment begin_second before using it as an index.
        }
        ++num;
    }

    return result;
}


vector<int> ReadTreeChunk(Filter filter, size_t chunk_size, bool* stop, int *count_before) {
    vector<int> chunk;
    while (!(*stop) && chunk.size() < chunk_size) {
        int value;
        cin >> value;
        if (value == 0) {
            *stop = true;
        } else if (filter(value)) {
            chunk.push_back(value);
        } else if (value < filter.left_range) {
            ++*count_before;
        }
    }
    sort(chunk.begin(), chunk.end());
    return chunk;
}


vector<int> BuildTree(Filter filter, size_t chunk_size, bool* stop, size_t* height, int *count_before) {
    size_t left_height = 1;
    vector<int> left_subtree = ReadTreeChunk(filter, chunk_size, stop, count_before);

    while (!*stop && left_height < *height) {
        // get right subtree of the same height
        size_t right_height = left_height;
        vector<int> right_subtree = BuildTree(filter, chunk_size, stop, &right_height, count_before);
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

    int j1 = int(ceil(1.0 * k_order / pow2)) - int(level) - 1;
    if (0 <= j1 && j1 < int(tree_root.size())) {
        filter.left_range = tree_root.at(j1);
    }

    int j2 = int(ceil(1.0 * k_order / pow2));
    if (0 <= j2 && j2 < int(tree_root.size())) {
        filter.right_range = tree_root.at(j2);
    }

    int len = max(1, pow2 * (pow2 - 1));
    assert(filter.left_range <= filter.right_range);
    assert(filter.left_range == INT_MIN || filter.right_range == INT_MAX ||
            filter.right_range - filter.left_range <= len);
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
    bool stop = false;
    int count_before = 0;
    vector<int> data = ReadTreeChunk(filter, MAX_MEMORY_COUNT, &stop, &count_before);

    assert(0 < data.size() && data.size() <= MAX_MEMORY_COUNT);
    assert(k_order >= count_before);
    k_order = k_order - count_before - 1;
    assert(0 <= k_order && k_order < int(data.size()));

    std::nth_element(data.begin(), data.begin() + k_order, data.end());
    return data[k_order];
}


int KOrderStatistics(int k_order) {
    Filter filter = FirstPass(Filter(INT_MIN, INT_MAX), k_order);

    return SecondPass(filter, k_order);
}


void PrintStatistics(int /*k_order*/, int st) {
    cout << st << endl;
}


void PrintUsage() {
    cout << "Implementation of Munro-Paterson algorithm "
        << " of finding a k-order minimum in streaming data. " << endl
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

    return 0;
}

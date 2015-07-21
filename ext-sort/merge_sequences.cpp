// #define TEST

#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>

#include "algorithms/binary_heap.hpp"

std::vector<std::vector<int>> ReadSequences(std::istream &stream = std::cin);
void WriteSequence(const std::vector<int> &sequence, std::ostream &stream = std::cout);

std::vector<int> MergeSequences(const std::vector<std::vector<int>> &sequences);

std::vector<std::vector<int>> ReadSequences(std::istream &stream) {
    int sequences_count, sequence_length;
    stream >> sequences_count >> sequence_length;
    std::vector<std::vector<int>> sequences(sequences_count, std::vector<int>(sequence_length));

    for (auto &sequence : sequences) {
        for (auto &element : sequence) {
            stream >> element;
        }
    }
    return sequences;
}

void WriteSequence(const std::vector<int> &sequence, std::ostream &stream) {
    for (auto element : sequence) {
        stream << element << " ";
    }
    stream << std::endl;
}

template<typename T, typename C>
typename C::iterator BinarySearch(T element, C *container) {
    if (container->empty()) {
        return container->begin();
    }

    auto begin = container->begin();
    auto end = container->end() - 1;
    while (begin != end) {
       int distance = std::distance(begin, end) / 2;
       auto middle = begin + distance;
       if (element < *middle) {
            end = middle;
       } else {
            begin = middle + 1;
       }
    }

    auto insert_position = begin;
    if (*insert_position < element) {
        insert_position = container->end();
    }

    return insert_position;
}

template<typename T, typename C>
void SortedInsert(T element, C *container) {
    auto insert_position = BinarySearch(element, container);
    container->insert(insert_position, element);
}

template<typename TIter, typename F>
TIter MedianPivot(TIter begin, TIter end, F comp) {
    int idx = (static_cast<int>(end - begin)) >> 1;
    TIter middle = begin + idx;

    if (comp(*begin, *end)) {
        if (comp(*end, *middle)) {
            return end;
        } else if (comp(*begin, *middle)) {
            return middle;
        } else {
            return begin;
        }
    } else if (comp(*middle, *end)) {
        return end;
    } else if (comp(*begin, *middle)) {
        return begin;
    } else {
        return middle;
    }
}

template<typename TIter, typename F>
void QuickSort(TIter input_begin, TIter input_end, F comp) {
    TIter lhs = input_begin, rhs = input_end - 1;
    if (lhs >= rhs) return;
    if (lhs + 1 == rhs) {
        if (comp(*rhs, *lhs)) std::swap(*lhs, *rhs);
        return;
    }
    TIter pivot = MedianPivot(lhs, rhs - 1, comp);
    TIter store_index = lhs;
    auto pivot_value = *pivot;
    std::swap(*pivot, *rhs);

    for (TIter it = lhs; it != rhs; ++it) {
        if (!comp(pivot_value, *it)) {
            std::swap(*it, *store_index);
            ++store_index;
        }
    }
    std::swap(*store_index, *rhs);

    QuickSort(input_begin, store_index, comp);
    QuickSort(store_index + 1, input_end, comp);
    // assert(std::is_sorted(input_begin, input_end));
}

template<typename TIter>
void Merge(TIter begin_iter,
        TIter middle_iter,
        TIter end_iter,
        TIter begin_output,
        TIter end_output) {
    TIter begin_first = begin_iter, begin_second = middle_iter;

    // While there are elements in the left or right runs
    for (TIter j = begin_output; j < end_output; ++j) {
        // If left run head exists and is <= existing right run head.
        if (begin_first < middle_iter &&
                (begin_second >= end_iter || !(*begin_second < *begin_first)))
            *j = *begin_first++;  // Increment begin_first after using it as an index.
        else
            *j = *begin_second++;  // Increment begin_second after using it as an index.
    }
}

template<typename F, typename TIter>
void MergeSort(TIter input_begin,
        TIter input_end,
        TIter temp_begin,
        TIter temp_end,
        F comp) {
    if (input_end <= input_begin + 1) {
        return;
    }
    size_t middle_idx = (static_cast<size_t> (input_end - input_begin)) >> 1;
    TIter middle = input_begin + middle_idx;
    TIter temp_middle = temp_begin + middle_idx;

    MergeSort(input_begin, middle, temp_begin, temp_middle, comp);
    MergeSort(middle, input_end, temp_middle, temp_end, comp);
    Merge(input_begin, middle, input_end, temp_begin, temp_end);

    std::copy(temp_begin, temp_end, input_begin);
}

template<typename I, typename F>
void Sort(I begin, I end, F comparator) {
    // QuickSort(begin, end, comparator);
    auto temp_container = std::vector<typename I::value_type>(begin, end);
    MergeSort(begin, end, temp_container.begin(), temp_container.end(), comparator);
}


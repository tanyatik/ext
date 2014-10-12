#pragma once

#include <vector>
#include <assert.h>
#include <iostream>
#include <functional>

namespace algorithms {

struct EmptyCallback {
    template<typename TElement, typename TIndex>
    void operator () (TElement &, TIndex) {}
};

struct EmptyDeleteCallback {
    template<typename TElement>
    void operator () (TElement &) {}
};

template <typename TElement,
          typename TComparator = std::less<TElement>,
          typename TMoveCallback = EmptyCallback,
          typename TInsertCallback = EmptyCallback,
          typename TDeleteCallback = EmptyDeleteCallback>
class BinaryHeap {
    public:
        typedef long long TIndex;
        typedef TElement TElementType;

        // Removes element on the top of the heap
        void Pop() {
            DeleteElement(0);
            HeapifyDown(0);
        }

        const TElement& GetTop() const {
            return elements_.front();
        }
        void Insert(const TElement& element);
        // Removes element which is located by index, keeping all heap properties
        void RemoveElementByIndex(TIndex index) {
            DeleteElement(index);
            
            if (index < GetSize()) {
                index = HeapifyUp(index);
                HeapifyDown(index);
            }
        }
        
        // Number of elements kept in the heap
        unsigned int GetSize() const { return elements_.size(); }

    private:
        bool CompareElements(const TElement &one, const TElement &other) const { 
            return TComparator() (one, other); 
        }

        void HeapifyDown(TIndex index);
        TIndex HeapifyUp(TIndex index);

        void SwapElements(TIndex a, TIndex b) {
            using std::swap;
            swap(elements_[a], elements_[b]);
            TMoveCallback()(elements_[a], a);
            TMoveCallback()(elements_[b], b);
        }

        TIndex GetParentIndex(TIndex index) {
            if (index <= 0) return -1;
            return (index - 1) >> 1;
        }
        TIndex GetLeftIndex(TIndex index) { return (index << 1) + 1; }
        TIndex GetRightIndex(TIndex index) { return (index << 1) + 2; }

        // Helper function -- swaps element with index 'index' 
        // with last element of the heap,
        // and then removes last element
        void DeleteElement(TIndex index) {
            SwapElements(index, GetSize() - 1);
            TDeleteCallback()(elements_[GetSize() - 1]);
            elements_.erase(elements_.end() - 1);
        }

        std::vector<TElement> elements_;
};

template<typename TElement, 
    typename TComparator, 
    typename TMoveCallback, 
    typename TInsertCallback, 
    typename TDeleteCallback>
void BinaryHeap<TElement, TComparator, TMoveCallback, TInsertCallback, TDeleteCallback>::HeapifyDown
        (TIndex index) {
    TIndex left_index = GetLeftIndex(index);
    TIndex right_index = GetRightIndex(index);
    TIndex max_index = index;

    if (left_index < GetSize() && CompareElements(elements_[max_index], elements_[left_index])) {
        max_index = left_index;
    }
    if (right_index < GetSize() && CompareElements(elements_[max_index], elements_[right_index])) {
        max_index = right_index;
    }
    if (max_index != index) {
        SwapElements(index, max_index);
        HeapifyDown(max_index);
    }
}

template<typename TElement, 
    typename TComparator, 
    typename TMoveCallback, 
    typename TInsertCallback, 
    typename TDeleteCallback>
void BinaryHeap<TElement, TComparator, TMoveCallback, TInsertCallback, TDeleteCallback>::Insert
        (const TElement &element) {
    elements_.push_back(element);

    int heap_size = elements_.size() - 1;
    elements_[heap_size] = element;
    TInsertCallback()(elements_[heap_size], heap_size);

    HeapifyUp(heap_size);
}

template<typename TElement, 
    typename TComparator, 
    typename TMoveCallback, 
    typename TInsertCallback, 
    typename TDeleteCallback>
typename BinaryHeap<TElement, TComparator, TMoveCallback, TInsertCallback, TDeleteCallback>::TIndex 
BinaryHeap<TElement, TComparator, TMoveCallback, TInsertCallback, TDeleteCallback>::HeapifyUp
        (TIndex index) {
    TIndex parent_index = GetParentIndex(index);

    while (index > 0 && CompareElements(elements_[parent_index], elements_[index])) {
        SwapElements(index, parent_index);
        index = parent_index;
        parent_index = GetParentIndex(index);
    }
    return index;
}

} // namespace algorithms

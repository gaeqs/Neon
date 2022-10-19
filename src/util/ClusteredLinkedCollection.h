//
// Created by gaelr on 19/10/2022.
//

#ifndef RVTRACKING_CLUSTEREDLINKEDCOLLECTION_H
#define RVTRACKING_CLUSTEREDLINKEDCOLLECTION_H

#include <array>
#include <bitset>

template<class T, int Size>
class ClusteredLinkedCollection;

template<class T, int Size>
class ClusteredLinkedCollectorIterator {

    ClusteredLinkedCollection<T, Size>* _collection;
    size_t _index;

public:

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    ClusteredLinkedCollectorIterator(ClusteredLinkedCollection<T, Size>* collection, size_t index)
            : _collection(collection), _index(index) {
        while (_collection != nullptr && !_collection->_occupied[_index]) {
            ++_index;
            if (_index >= Size) {
                _index = 0;
                _collection = _collection->_next;
            }
        }
    }


    reference operator*() const {
        return _collection->_data[_index];
    }

    pointer operator->() { return _collection->_data + _index; }

    // Prefix increment
    ClusteredLinkedCollectorIterator<T, Size>& operator++() {
        do {
            ++_index;
            if (_index >= Size) {
                _index = 0;
                _collection = _collection->_next;
            }
        } while (_collection != nullptr && !_collection->_occupied[_index]);

        return *this;
    }

    // Postfix increment
    ClusteredLinkedCollectorIterator<T, Size> operator++(int) {
        ClusteredLinkedCollectorIterator<T, Size> tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const ClusteredLinkedCollectorIterator<T, Size>& b) {
        return _collection == b._collection && _index == b._index;
    };

    bool operator!=(const ClusteredLinkedCollectorIterator<T, Size>& b) {
        return _collection != b._collection || _index != b._index;
    };

};

template<class T, int Size>
class ClusteredLinkedCollection {

    friend class ClusteredLinkedCollectorIterator<T, Size>;

    T* _data;
    std::bitset<Size> _occupied;
    size_t _localSize;

    ClusteredLinkedCollection<T, Size>* _next;

    void expand() {
        if (_next == nullptr) {
            _next = new ClusteredLinkedCollection<T, Size>();
        }
    }

public:

    ClusteredLinkedCollection() : _data(), _occupied(), _localSize(0), _next(nullptr) {
        _data = reinterpret_cast<T*>(malloc(sizeof(T) * Size));
    }

    ~ClusteredLinkedCollection() {
        free(_data);
    }

    size_t size() {
        return _localSize + (_next == nullptr ? 0 : _next->size());
    }

    ClusteredLinkedCollectorIterator<T, Size> begin() {
        return ClusteredLinkedCollectorIterator<T, Size>(this, 0);
    }

    ClusteredLinkedCollectorIterator<T, Size> end() {
        return ClusteredLinkedCollectorIterator<T, Size>(nullptr, 0);
    }

    template<class... Args>
    T* emplace(Args&& ... values) {
        if (_localSize == Size) {
            expand();
            _next->emplace(values...);
        }


        size_t index = 0;
        while (_occupied[index]) {
            ++index;
        }
        _occupied.flip(index);
        ++_localSize;

        return std::construct_at(_data + index, values...);
    }


};


#endif //RVTRACKING_CLUSTEREDLINKEDCOLLECTION_H

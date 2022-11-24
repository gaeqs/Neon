//
// Created by gaelr on 19/10/2022.
//

#ifndef RVTRACKING_CLUSTEREDLINKEDCOLLECTION_H
#define RVTRACKING_CLUSTEREDLINKEDCOLLECTION_H

#include <bitset>
#include <functional>
#include <iostream>
#include <memory>
#include <cstring>

constexpr size_t DEFAULT_CLUSTER_SIZE = 256;

class AbstractClusteredLinkedCollection {

public:

    virtual size_t size() const = 0;

    virtual size_t capacity() const = 0;

    virtual void forEachRaw(std::function<void(void*)> function) = 0;

};


template<class Collection>
class ClusteredLinkedCollectorIterator {

    Collection* _collection;
    size_t _index;

public:

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = typename Collection::ValueType;

    ClusteredLinkedCollectorIterator(Collection* collection, size_t index)
            : _collection(collection), _index(index) {
        while (_collection != nullptr && !_collection->_occupied[_index]) {
            ++_index;
            if (_index >= _collection->capacity()) {
                _index = 0;
                _collection = _collection->_next;
            }
        }
    }


    value_type& operator*() const {
        return _collection->rawPointer(_index)[0];
    }

    value_type* operator->() const { return _collection->rawPointer(_index); }

    value_type* raw() const {
        return _collection->rawPointer(_index);
    }

    // Prefix increment
    ClusteredLinkedCollectorIterator<Collection>& operator++() {
        do {
            ++_index;
            if (_index >= _collection->capacity()) {
                _index = 0;
                _collection = _collection->_next;
            }
        } while (_collection != nullptr && !_collection->_occupied[_index]);

        return *this;
    }

    // Postfix increment
    ClusteredLinkedCollectorIterator<Collection> operator++(int) {
        ClusteredLinkedCollectorIterator<Collection> tmp = *this;
        ++(*this);
        return tmp;
    }

    bool
    operator==(const ClusteredLinkedCollectorIterator<Collection>& b) const {
        return _collection == b._collection && _index == b._index;
    };

    bool
    operator!=(const ClusteredLinkedCollectorIterator<Collection>& b) const {
        return _collection != b._collection || _index != b._index;
    };

};

template<class Collection>
class ConstClusteredLinkedCollectorIterator {

    const Collection* _collection;
    size_t _index;

public:

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = typename Collection::ValueType;

    ConstClusteredLinkedCollectorIterator(const Collection* collection,
                                          size_t index)
            : _collection(collection), _index(index) {
        while (_collection != nullptr && !_collection->_occupied[_index]) {
            ++_index;
            if (_index >= _collection->capacity()) {
                _index = 0;
                _collection = _collection->_next;
            }
        }
    }


    const value_type& operator*() const {
        return _collection->rawPointer(_index)[0];
    }

    const value_type* operator->() const {
        return _collection->rawPointer(_index);
    }

    const value_type* raw() const {
        return _collection->rawPointer(_index);
    }

    // Prefix increment
    ConstClusteredLinkedCollectorIterator<Collection>& operator++() {
        do {
            ++_index;
            if (_index >= _collection->capacity()) {
                _index = 0;
                _collection = _collection->_next;
            }
        } while (_collection != nullptr && !_collection->_occupied[_index]);

        return *this;
    }

    // Postfix increment
    ConstClusteredLinkedCollectorIterator<Collection> operator++(int) {
        ConstClusteredLinkedCollectorIterator<Collection> tmp = *this;
        ++(*this);
        return tmp;
    }

    bool
    operator==(const ConstClusteredLinkedCollectorIterator<Collection>& b) const {
        return _collection == b._collection && _index == b._index;
    };

    bool
    operator!=(const ConstClusteredLinkedCollectorIterator<Collection>& b) const {
        return _collection != b._collection || _index != b._index;
    };

};

template<class T, size_t Size = DEFAULT_CLUSTER_SIZE>
class ClusteredLinkedCollection : public AbstractClusteredLinkedCollection {

    using ValueType = T;

    friend class ClusteredLinkedCollectorIterator<ClusteredLinkedCollection<T, Size>>;

    friend class ConstClusteredLinkedCollectorIterator<ClusteredLinkedCollection<T, Size>>;

    size_t _objectSize;

    char* _data;
    std::bitset<Size> _occupied;
    size_t _localSize;

    ClusteredLinkedCollection<T, Size>* _next;

    void expand() {
        if (_next == nullptr) {
            _next = new ClusteredLinkedCollection<T, Size>();
        }
    }

    T* rawPointer(size_t index) const {
        return reinterpret_cast<T*>(_data + index * _objectSize);
    }

public:

    ClusteredLinkedCollection() :
            _objectSize(sizeof(T)),
            _data(),
            _occupied(),
            _localSize(0),
            _next(nullptr) {
        _data = reinterpret_cast<char*>(malloc(sizeof(T) * Size));
    }

    ~ClusteredLinkedCollection() {

        for (size_t i = 0; i < Size; i += _objectSize) {
            if (_occupied[i]) {
                std::destroy_at(_data + i);
            }
        }

        free(_data);
        delete _next;
    }

    size_t size() const override {
        return _localSize + (_next == nullptr ? 0 : _next->size());
    }

    size_t capacity() const override {
        return Size;
    }

    ClusteredLinkedCollectorIterator<ClusteredLinkedCollection<T, Size>>
    begin() {
        return ClusteredLinkedCollectorIterator<
                ClusteredLinkedCollection<T, Size>>(this, 0);
    }

    ConstClusteredLinkedCollectorIterator<ClusteredLinkedCollection<T, Size>>
    cbegin() const {
        return ConstClusteredLinkedCollectorIterator<
                ClusteredLinkedCollection<T, Size>>(this, 0);
    }

    ClusteredLinkedCollectorIterator<ClusteredLinkedCollection<T, Size>>
    end() {
        return ClusteredLinkedCollectorIterator<
                ClusteredLinkedCollection<T, Size>>(nullptr, 0);
    }

    ConstClusteredLinkedCollectorIterator<ClusteredLinkedCollection<T, Size>>
    cend() const {
        return ConstClusteredLinkedCollectorIterator<
                ClusteredLinkedCollection<T, Size>>(nullptr, 0);
    }

    void forEachRaw(std::function<void(void*)> function) override {
        auto it = begin();
        auto itEnd = end();

        while (it != itEnd) {
            function(it.raw());
            ++it;
        }
    }

    void forEach(std::function<void(T*)> function) {
        auto it = begin();
        auto itEnd = end();

        while (it != itEnd) {
            function(it.raw());
            ++it;
        }
    }

    void forEach(std::function<void(const T*)> function) const {
        auto it = cbegin();
        auto itEnd = cend();

        while (it != itEnd) {
            function(it.raw());
            ++it;
        }
    }


    template<class... Args>
    T* emplace(Args&& ... values) {
        if (_localSize == Size) {
            expand();
            return _next->emplace(values...);
        }


        size_t index = 0;
        while (_occupied[index]) {
            ++index;
        }
        _occupied.flip(index);
        ++_localSize;

        auto* ptr = reinterpret_cast<T*>(_data + index * _objectSize);
        return std::construct_at(ptr, values...);
    }

    bool remove(const T* value) {
        size_t position = 0;
        bool found = false;

        for (size_t i = 0; i < Size; ++i) {
            if (_occupied[i] && rawPointer(i) == value) {
                position = i;
                found = true;
                break;
            }
        }

        if (found) {
            auto* ptr = reinterpret_cast<T*>(_data + position * _objectSize);
            _occupied.flip(position);
            --_localSize;
            std::destroy_at(ptr);
            memset(ptr, 0, _objectSize);
            return true;
        } else if (_next != nullptr) {
            return _next->remove(value);
        }

        return false;
    }


};

#endif //RVTRACKING_CLUSTEREDLINKEDCOLLECTION_H

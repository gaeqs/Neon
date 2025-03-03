//
// Created by gaelr on 19/10/2022.
//

#ifndef RVTRACKING_CLUSTEREDLINKEDCOLLECTION_H
#define RVTRACKING_CLUSTEREDLINKEDCOLLECTION_H

#include <bitset>
#include <functional>

namespace neon {
    constexpr size_t DEFAULT_CLUSTER_SIZE = 256;

    class AbstractClusteredLinkedCollection {
    public:
        virtual ~AbstractClusteredLinkedCollection() = default;

        [[nodiscard]] virtual size_t size() const = 0;

        [[nodiscard]] virtual bool empty() const = 0;

        [[nodiscard]] virtual size_t capacity() const = 0;

        virtual void forEachRaw(std::function<void(void*)> function) = 0;

        virtual bool erase(const void* value) = 0;
    };


    template<class CollectionPtr>
    class ClusteredLinkedCollectorIterator {
        CollectionPtr _collection;
        size_t _index;

    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = typename std::remove_pointer_t<CollectionPtr>::ValueType;

        ClusteredLinkedCollectorIterator(CollectionPtr collection, size_t index)
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

        value_type* operator->() const {
            return _collection->rawPointer(_index);
        }

        value_type* raw() const {
            return _collection->rawPointer(_index);
        }

        CollectionPtr getCollection() const {
            return _collection;
        }

        size_t getIndex() const {
            return _index;
        }

        operator ClusteredLinkedCollectorIterator<const std::remove_pointer_t<CollectionPtr>*>() {
            return ClusteredLinkedCollectorIterator<const std::remove_pointer_t<CollectionPtr>*>(_collection, _index);
        }

        // Prefix increment
        ClusteredLinkedCollectorIterator<CollectionPtr>& operator++() {
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
        ClusteredLinkedCollectorIterator<CollectionPtr> operator++(int) {
            ClusteredLinkedCollectorIterator<CollectionPtr> tmp = *this;
            ++(*this);
            return tmp;
        }

        bool
        operator==(
            const ClusteredLinkedCollectorIterator<CollectionPtr>& b) const {
            return _collection == b._collection && _index == b._index;
        };

        bool
        operator!=(
            const ClusteredLinkedCollectorIterator<CollectionPtr>& b) const {
            return _collection != b._collection || _index != b._index;
        };
    };

    /**
    * This class implements a specialized collection that ensures elements remain
    * at fixed memory locations once inserted.
    * This eliminates any risk of element movement,
    * making it ideal for scenarios where you need to maintain stable
    * pointers to elements without concern for relocation.
    * <p>
    * Features:
    * <p>
    * Stable Pointers: the memory addresses of elements remain consistent,
    * providing safety and reliability when storing references or pointers.
    * <p>
    * Spatial Locality: the collection strives to store elements in contiguous memory blocks,
    * optimizing cache usage and improving performance by leveraging spatial locality.
    */
    template<class T, size_t Size = DEFAULT_CLUSTER_SIZE>
    class ClusteredLinkedCollection : public AbstractClusteredLinkedCollection {
    public:
        friend class ClusteredLinkedCollectorIterator<ClusteredLinkedCollection<T, Size>*>;
        friend class ClusteredLinkedCollectorIterator<const ClusteredLinkedCollection<T, Size>*>;

        using ValueType = T;
        using Iterator = ClusteredLinkedCollectorIterator<ClusteredLinkedCollection<T, Size>*>;
        using ConstIterator = ClusteredLinkedCollectorIterator<const ClusteredLinkedCollection<T, Size>*>;

    private:
        size_t _objectSize;

        T* _data;
        std::bitset<Size> _occupied;
        size_t _localSize;

        ClusteredLinkedCollection* _next;

        void expand() {
            if (_next == nullptr) {
                _next = new ClusteredLinkedCollection();
            }
        }

        T* rawPointer(size_t index) const {
            return static_cast<T*>(_data + index);
        }

    public:
        /**
        * Creates a new empty ClusteredLinkedCollection.
        */
        ClusteredLinkedCollection() :
            _objectSize(sizeof(T)),
            _data(static_cast<T*>(malloc(sizeof(T) * Size))),
            _occupied(),
            _localSize(0),
            _next(nullptr) {}

        ~ClusteredLinkedCollection() override {
            for (size_t i = 0; i < Size; ++i) {
                if (_occupied[i]) {
                    T* element = _data + i;
                    std::destroy_at(element);
                }
            }

            free(_data);
            delete _next;
        }

        /**
        * Creates a copy of the given collection.
        */
        ClusteredLinkedCollection(const ClusteredLinkedCollection& other) :
            _objectSize(other._objectSize),
            _data(static_cast<T*>(malloc(other._objectSize * Size))),
            _occupied(),
            _localSize(0),
            _next(nullptr) {
            for (auto& t: other) {
                push(t);
            }
        }

        /**
        * Creates a copy of the given collection.
        */
        ClusteredLinkedCollection& operator=(const ClusteredLinkedCollection& other) {
            if (&other == this) return *this;
            clear();
            for (auto& t: other) {
                push(t);
            }
            return *this;
        }

        /**
        * Moves the given collection.
        */
        ClusteredLinkedCollection(ClusteredLinkedCollection&& other) noexcept :
            _objectSize(other._objectSize),
            _data(other._data),
            _occupied(other._occupied),
            _localSize(other._localSize),
            _next(other._next) {
            other._occupied.reset();
            other._data = static_cast<T*>(malloc(other._objectSize * Size));
            other._localSize = 0;
            other._next = nullptr;
        }

        /**
        * Moves the given collection.
        */
        ClusteredLinkedCollection& operator=(ClusteredLinkedCollection&& other) noexcept {
            if (&other == this) return *this;

            _objectSize = other._objectSize;
            _data = other._data;
            _occupied = other._occupied;
            _localSize = other._localSize;
            _next = other._next;

            other._occupied.reset();
            other._data = static_cast<T*>(malloc(other._objectSize * Size));
            other._localSize = 0;
            other._next = nullptr;

            return *this;
        }

        /**
        * @returns the amount of elements inside this collection.
        */
        [[nodiscard]] size_t size() const override {
            return _localSize + (_next == nullptr ? 0 : _next->size());
        }

        /**
        * @returns whether this collection is empty.
        */
        [[nodiscard]] bool empty() const override {
            return _localSize == 0 && (_next == nullptr || _next->empty());
        }

        /**
        * @params returns the amount of elements this local cluster can hold without expanding.
        */
        [[nodiscard]] size_t capacity() const override {
            return Size;
        }

        /**
        * Calls the given function for each element inside this collection.
        */
        void forEachRaw(std::function<void(void*)> function) override {
            auto it = begin();
            auto itEnd = end();

            while (it != itEnd) {
                function(it.raw());
                ++it;
            }
        }

        /**
        * Calls the given function for each element inside this collection.
        */
        void forEach(std::function<void(T*)> function) {
            auto it = begin();
            auto itEnd = end();

            while (it != itEnd) {
                function(it.raw());
                ++it;
            }
        }

        /**
        * Calls the given function for each element inside this collection.
        */
        void forEach(std::function<void(const T*)> function) const {
            auto it = begin();
            auto itEnd = end();

            while (it != itEnd) {
                function(it.raw());
                ++it;
            }
        }

        /**
        * Inserts the given elements into this collection, creating a copy.
        * @returns the pointer to the new copy.
        */
        T* push(const T& t) {
            if (_localSize == Size) {
                expand();
                return _next->push(t);
            }

            size_t index = 0;
            while (_occupied[index]) {
                ++index;
            }
            _occupied.flip(index);
            ++_localSize;

            auto* ptr = _data + index;
            return std::construct_at<T>(ptr, t);
        }

        /**
        * Inserts the given elements into this collection, moving it.
        * @returns the pointer to the moved element.
        */
        T* push(T&& t) {
            if (_localSize == Size) {
                expand();
                return _next->push(std::move(t));
            }

            size_t index = 0;
            while (_occupied[index]) {
                ++index;
            }
            _occupied.flip(index);
            ++_localSize;

            auto* ptr = _data + index;
            return std::construct_at<T>(ptr, std::move(t));
        }

        /**
        * Creates a new element into this collection using the given parameters.
        * @returns the pointer to the new element.
        */
        template<class... Args>
        T* emplace(Args&&... values) {
            if (_localSize == Size) {
                expand();
                return _next->emplace(std::forward<Args>(values)...);
            }


            size_t index = 0;
            while (_occupied[index]) {
                ++index;
            }
            _occupied.flip(index);
            ++_localSize;

            auto* ptr = _data + index;
            return std::construct_at<T>(ptr, std::forward<Args>(values)...);
        }

        /**
        * Erases the element at the given position.
        * @returns whether the element has been erased.
        */
        bool erase(ConstIterator it) {
            if (it.getCollection() == this) {
                size_t position = it.getIndex();
                if (!_occupied[position]) return false;
                auto* ptr = _data + position;
                _occupied.flip(position);
                --_localSize;
                std::destroy_at(ptr);
                return true;
            }

            if (_next != nullptr) {
                return _next->erase(it);
            }

            return false;
        }

        /**
        * Erases the element at the given pointer.
        * @returns whether the element has been erased.
        */
        bool erase(const void* value) override {
            int64_t position = static_cast<const T*>(value) - _data;
            if (position < 0 || position >= Size) {
                if (_next != nullptr) {
                    return _next->erase(value);
                }
                return false;
            }

            if (!_occupied[position]) {
                return false;
            }

            _occupied.flip(position);
            std::destroy_at(_data + position);

            --_localSize;
            return true;
        }

        /**
        * Clears all the contents of this collection.
        */
        void clear() {
            for (size_t i = 0; i < Size; ++i) {
                if (_occupied[i]) {
                    std::destroy_at(_data + i);
                }
            }

            _occupied.reset();
            _localSize = 0;

            delete _next;
            _next = nullptr;
        }


        Iterator begin() {
            return Iterator(this, 0);
        }

        ConstIterator begin() const {
            return ConstIterator(this, 0);
        }

        Iterator end() {
            return Iterator(nullptr, 0);
        }

        ConstIterator end() const {
            return ConstIterator(nullptr, 0);
        }
    };
}

#endif //RVTRACKING_CLUSTEREDLINKEDCOLLECTION_H

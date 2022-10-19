//
// Created by grial on 10/10/22.
//

#include "Resource.h"

Resource::Resource(const char* start, const char* end) : _data(start), _size(end - start) {
}

const char* Resource::data() const {
    return _data;
}

size_t Resource::size() const {
    return _size;
}

const char* Resource::begin() const {
    return _data;
}

const char* Resource::end() const {
    return _data + _size;
}

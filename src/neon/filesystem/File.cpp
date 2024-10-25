//
// Created by gaeqs on 24/10/24.
//

#include "File.h"

namespace neon {
    File::File() : _data(nullptr), _size(0), _autoFree(false) {}

    File::File(const char* data, size_t size, bool autoFree)
        : _data(data),
          _size(size),
          _autoFree(autoFree) {}

    File::~File() {
        if (_autoFree && _data != nullptr) {
            delete _data;
        }
    }

    const char* File::getData() const {
        return _data;
    }

    size_t File::getSize() const {
        return _size;
    }
}

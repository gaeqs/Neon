//
// Created by gaeqs on 24/10/24.
//

#include "File.h"

namespace neon
{
    File::File() :
        _data(nullptr),
        _size(0),
        _autoFree(false)
    {
    }

    File::File(File&& other) noexcept :
        _data(other._data),
        _size(other._size),
        _autoFree(other._autoFree)
    {
        other._data = nullptr;
        other._size = 0;
        other._autoFree = false;
    }

    File& File::operator=(File&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }
        _data = other._data;
        _size = other._size;
        _autoFree = other._autoFree;
        other._data = nullptr;
        other._size = 0;
        other._autoFree = false;
        return *this;
    }

    File::File(const char* data, size_t size, bool autoFree) :
        _data(data),
        _size(size),
        _autoFree(autoFree)
    {
    }

    File::~File()
    {
        if (_autoFree && _data != nullptr) {
            delete _data;
        }
    }

    bool File::isValid() const
    {
        return _data != nullptr;
    }

    const char* File::getData() const
    {
        return _data;
    }

    size_t File::getSize() const
    {
        return _size;
    }

    std::optional<nlohmann::json> File::toJson() const
    {
        auto json = nlohmann::json::parse(_data, _data + _size, nullptr, false);
        if (json.is_discarded()) {
            return {};
        }
        return json;
    }

    std::string File::toString() const
    {
        return std::string(_data, _size);
    }

    std::vector<std::string> File::readLines() const
    {
        std::vector<std::string> lines;
        std::istringstream f(toString());
        std::string line;
        while (std::getline(f, line)) {
            lines.push_back(line);
        }
        return lines;
    }
} // namespace neon

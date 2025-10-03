//
// Created by gaeqs on 24/10/24.
//

#include "File.h"

namespace neon
{
    File::File() :
        _data(nullptr),
        _size(0),
        _destructor(nullptr)
    {
    }

    File::File(File&& other) noexcept :
        _data(other._data),
        _size(other._size),
        _destructor(other._destructor)
    {
        other._data = nullptr;
        other._size = 0;
        other._destructor = nullptr;
    }

    File& File::operator=(File&& other) noexcept
    {
        if (this == &other) {
            return *this;
        }

        if (_destructor != nullptr && _data != nullptr) {
            _destructor(_data);
        }

        _data = other._data;
        _size = other._size;
        _destructor = other._destructor;
        other._data = nullptr;
        other._size = 0;
        other._destructor = nullptr;
        return *this;
    }

    File::File(const std::byte* data, size_t size, std::function<void(const std::byte*)> destructor) :
        _data(data),
        _size(size),
        _destructor(std::move(destructor))
    {
    }

    File::~File()
    {
        if (_destructor != nullptr && _data != nullptr) {
            _destructor(_data);
        }
    }

    bool File::isValid() const
    {
        return _data != nullptr;
    }

    const std::byte* File::getData() const
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
        return std::string(static_cast<const char*>(static_cast<const void*>(_data)), _size);
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

//
// Created by gaeqs on 24/10/24.
//

#ifndef FILE_H
#define FILE_H

#include <optional>
#include <cstddef>
#include <nlohmann/json.hpp>

namespace neon
{
    class File
    {
        const std::byte* _data;
        size_t _size;
        std::function<void(const std::byte*)> _destructor;

      public:
        File();

        File(const File& other) = delete;

        File(File&& other) noexcept;

        File& operator=(File&& other) noexcept;

        File(const std::byte* data, size_t size, std::function<void(const std::byte*)> destructor = nullptr);

        ~File();

        [[nodiscard]] bool isValid() const;

        [[nodiscard]] const std::byte* getData() const;

        [[nodiscard]] size_t getSize() const;

        [[nodiscard]] std::optional<nlohmann::json> toJson() const;

        [[nodiscard]] std::string toString() const;

        [[nodiscard]] std::vector<std::string> readLines() const;
    };
} // namespace neon

#endif // FILE_H

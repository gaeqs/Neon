//
// Created by gaeqs on 24/10/24.
//

#ifndef FILE_H
#define FILE_H

#include <cstddef>

namespace neon {
    class File {
        const char* _data;
        size_t _size;
        bool _autoFree;

    public:
        File();

        File(const File& other) = delete;

        File(File&& other) noexcept;

        File& operator=(File&& other) noexcept;

        File(const char* data, size_t size, bool autoFree = true);

        ~File();

        [[nodiscard]] bool isValid() const;

        [[nodiscard]] const char* getData() const;

        [[nodiscard]] size_t getSize() const;
    };
}


#endif //FILE_H

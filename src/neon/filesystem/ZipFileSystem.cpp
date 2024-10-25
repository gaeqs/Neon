//
// Created by gaeqs on 25/10/24.
//

#include "ZipFileSystem.h"

#include <neon/logging/Logger.h>

neon::ZipFileSystem::ZipFileSystem(std::filesystem::path file) {
    int err;
    _zip = zip_open(file.c_str(), 0, &err);
    if (_zip == nullptr) {
        zip_error error{};
        zip_error_init_with_code(&error, err);
        neon::Logger::error(MessageBuilder()
            .print("Cannot open zip file: ")
            .print(file)
            .print(" (")
            .print(zip_error_strerror(&error)));
        zip_error_fini(&error);
        return;
    }

    zip_get_num_entries()
}

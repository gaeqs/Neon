//
// Created by gaeqs on 08/05/2025.
//

#include "FileUtils.h"

#include <neon/logging/Logger.h>

namespace neon
{

    std::optional<std::filesystem::path> getConfigPath()
    {
#ifdef _WIN32
        if (const char* appdata = std::getenv("APPDATA")) {
            return std::filesystem::path(appdata);
        }
#elif __APPLE__
        if (const char* home = std::getenv("HOME")) {
            return std::filesystem::path(home) / "Library" / "Application Support";
        }
#elif __linux__
        if (const char* xdg_config = std::getenv("XDG_CONFIG_HOME")) {
            return std::filesystem::path(xdg_config);
        } else if (const char* home = std::getenv("HOME")) {
            return std::filesystem::path(home) / ".config";
        }
#endif
        return {};
    }

    Result<std::filesystem::path, ConfigDirectoryCreationError> createConfigDirectory(const std::filesystem::path& path)
    {
        auto configPath = getConfigPath();
        if (!configPath) {
            return ConfigDirectoryCreationError::CONFIG_PATH_NOT_FOUND;
        }
        auto abs = configPath.value() / path;
        if (std::filesystem::exists(abs)) {
            if (std::filesystem::is_directory(abs)) {
                return abs;
            }
            return ConfigDirectoryCreationError::PATH_EXISTS_BUT_IS_FILE;
        }

        std::error_code code;
        std::filesystem::create_directories(abs, code);
        if (code) {
            error() << "Error creating config directory " << abs << ": " << code.message() << "(Code: " << code.value()
                    << ")";
            return ConfigDirectoryCreationError::ERROR_CREATING_DIRECTORY;
        }
        return abs;
    }

} // namespace neon
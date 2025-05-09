//
// Created by gaeqs on 08/05/2025.
//

#ifndef NEON_FILEUTILS_H
#define NEON_FILEUTILS_H

#include <optional>
#include <filesystem>

#include <neon/util/Result.h>

namespace neon
{

    enum class ConfigDirectoryCreationError
    {
        CONFIG_PATH_NOT_FOUND,
        PATH_EXISTS_BUT_IS_FILE,
        ERROR_CREATING_DIRECTORY
    };

    /**
     * @brief Retrieves the configuration file path.
     *
     * This function determines the path to the configuration
     * file used by the application. The specific path returned
     * may vary depending on the environment or settings
     * configured within the application.
     *
     * This is not Neon's configuration folder, but the general configuration
     * folder of your system.
     *
     * Examples:
     * - Windows: %APPDATA%
     * - Linux: ~/.config
     * - MacOS: "Application Support"
     *
     * @return A string representing the path to the configuration folder.
     */
    std::optional<std::filesystem::path> getConfigPath();

    /**
     * @brief Creates a configuration directory for an application if it does not already exist.
     *
     * This function checks for the existence of a directory intended for configuration files.
     * If the directory does not exist, it creates the necessary directory structure.
     *
     * Example: if you want a configuration folder for your application called "Minecraft",
     * and you give the path "Minecraft" to this function, this algorithm will create
     * the folder %CONFIG_PATH%/Minecraft, being %CONFIG_PATH% the path given by "getConfigPath()".
     *
     * @param path The path of the folder that will be created inside the configuration folder.
     * @return The path to the created directory or an error. If the error is "ERROR_CREATING_DIRECTORY", a log
     * message will be displayed too.
     */
    Result<std::filesystem::path, ConfigDirectoryCreationError> createConfigDirectory(
        const std::filesystem::path& path);

} // namespace neon

#endif // NEON_FILEUTILS_H

//
// Created by gaeqs on 12/05/25.
//

#ifndef NEON_DIALOGSLINUX_H
#define NEON_DIALOGSLINUX_H

#ifdef __linux__

    #include "Dialogs.h"

    #include <filesystem>
    #include <vector>
    #include <string>
    #include <optional>

namespace
{
    std::string execAndRead(const std::string& cmd)
    {
        std::string output;
        std::array<char, 256> buffer;
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            return {};
        }
        while (fgets(buffer.data(), buffer.size(), pipe)) {
            output += buffer.data();
        }
        pclose(pipe);
        return output;
    }

    std::vector<std::string> splitLines(const std::string& s)
    {
        std::vector<std::string> out;
        std::istringstream iss(s);
        for (std::string line; std::getline(iss, line);) {
            if (!line.empty()) {
                out.push_back(line);
            }
        }
        return out;
    }

    bool isCommandAvailable(const char* name)
    {
        std::string cmd = "command -v ";
        cmd += name;
        cmd += " >/dev/null 2>&1";
        return std::system(cmd.c_str()) == 0;
    }
} // namespace

namespace neon
{

    inline std::vector<std::filesystem::path> openFileDialogLinux(const OpenFileDialogInfo& info)
    {
        std::string cmd;
        if (isCommandAvailable("zenity")) {
            cmd = "zenity --file-selection";
            if (info.multiselect) {
                cmd += " --multiple --separator='\n'";
            }
            if (info.title) {
                cmd += " --title=\"" + *info.title + "\"";
            }
            if (info.defaultPath) {
                cmd += " --filename=\"" + info.defaultPath.value().string() + "/\"";
            }
            if (info.filters) {
                for (const auto& [name, filter] : *info.filters) {
                    cmd += " --file-filter=\"" + name + " | " + filter + "\"";
                }
            }
            if (info.pickFolders) {
                cmd += " --directory";
            }
            if (info.application != nullptr) {
                cmd += " --modal";
            }
        }

        auto out = execAndRead(cmd);
        if (out.empty()) {
            return {};
        }

        std::vector<std::filesystem::path> paths;
        for (auto& l : splitLines(out)) {
            paths.emplace_back(l);
        }
        return paths;
    }

    inline std::optional<std::filesystem::path> saveFileDialogLinux(const SaveFileDialogInfo& info)
    {
        std::string cmd;
        if (isCommandAvailable("zenity")) {

            cmd = "zenity --file-selection --save";
            if (info.title) {
                cmd += " --title=\"" + *info.title + "\"";
            }
            if (info.filters) {
                for (const auto& [name, filter] : *info.filters) {
                    cmd += " --file-filter=\"" + name + " | " + filter + "\"";
                }
            }
            if (info.application != nullptr) {
                cmd += " --modal";
            }
        }

        auto out = execAndRead(cmd);
        if (out.empty()) {
            return {};
        }
        return std::filesystem::path(out);
    }

} // namespace neon

#endif // __linux__

#endif // NEON_DIALOGSLINUX_H

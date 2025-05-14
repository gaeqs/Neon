//
// Created by gaeqs on 12/05/2025.
//

#ifndef NEON_DIALOGS_H
#define NEON_DIALOGS_H
#include <optional>
#include <string>
#include <neon/structure/Application.h>

namespace neon
{

    struct FileFilter
    {
        std::string name;
        std::string filter;
    };

    struct OpenFileDialogInfo
    {
        Application* application = nullptr;
        std::optional<std::string> title = {};
        std::optional<std::filesystem::path> defaultPath = {};
        std::optional<std::vector<FileFilter>> filters = {};
        bool multiselect = false;
        bool pickFolders = false;
    };

    struct SaveFileDialogInfo
    {
        Application* application = nullptr;
        std::optional<std::vector<FileFilter>> filters;
        std::optional<std::filesystem::path> defaultPath;
        std::optional<std::string> defaultFileName;
        std::optional<std::string> defaultExtension;
        std::optional<std::string> title;
    };

    struct NotificationInitInfo
    {
        std::string applicationName;
    };

    enum class NotificationUrgency
    {
        LOW,
        NORMAL,
        CRITICAL
    };

    struct NotificationInfo
    {
        NotificationInitInfo init;
        std::string title;
        std::string message;
        std::optional<std::string> icon = "dialog-information";
        NotificationUrgency urgency = NotificationUrgency::NORMAL;
        std::optional<std::chrono::milliseconds> timeout = {};
        std::optional<std::string> category = {};
        bool resident = false;
    };

    std::vector<std::filesystem::path> openFileDialog(const OpenFileDialogInfo& info);

    std::optional<std::filesystem::path> saveFileDialog(const SaveFileDialogInfo& info);

    void sendNotification(const NotificationInfo& info);
} // namespace neon

#endif // NEON_DIALOGS_H

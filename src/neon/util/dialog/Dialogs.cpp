//
// Created by gaeqs on 12/05/2025.
//

#include "Dialogs.h"

#include <neon/logging/Logger.h>

#ifdef WIN32
    #include "DialogsWindows.h"
#elif defined(__linux__)
    #include "DialogsLinux.h"
#endif

namespace neon
{

    std::vector<std::filesystem::path> openFileDialog(const OpenFileDialogInfo& info)
    {
#ifdef WIN32
        return openFileDialogWindows(info);
#elif defined(__linux__) && defined(USE_GTK)
        return openFileDialogLinux(info);
#else
        neon::error() << "Unsupported OS";
        return {};
#endif
    }

    std::optional<std::filesystem::path> saveFileDialog(const SaveFileDialogInfo& info)
    {
#ifdef WIN32
        return saveFileDialogWindows(info);
#elif defined(__linux__) && defined(USE_GTK)
        return saveFileDialogLinux(info);
#else
        neon::error() << "Unsupported OS";
        return {};
#endif
    }

    void sendNotification(const NotificationInfo& info)
    {
#ifdef WIN32
#elif defined(__linux__)
        sendNotificationLinux(info);
#else
        neon::error() << "Unsupported OS";
#endif
    }
} // namespace neon
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
#elif defined(__linux__)
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
#elif defined(__linux__)
        return saveFileDialogLinux(info);
#else
        neon::error() << "Unsupported OS";
        return {};
#endif
    }
} // namespace neon
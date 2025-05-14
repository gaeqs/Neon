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
    #include <mutex>
    #include <gtk/gtk.h>
    #include <libnotify/notify.h>
    #include <vulkan/VKApplication.h>

    #define GLFW_EXPOSE_NATIVE_X11

    #include <GLFW/glfw3.h>
    #include <GLFW/glfw3native.h>

namespace
{
    std::mutex gtkMutex;
    bool gtkInit = false;

    std::mutex libNotifyMutex;
    bool libNotifyInit = false;

    void initGTK()
    {
        std::unique_lock lock(gtkMutex);
        if (!gtkInit) {
            int argc = 0;
            char** argv = nullptr;
            gtk_init(&argc, &argv);
            gtkInit = true;
        }
    }

    void initNotify()
    {
        std::unique_lock lock(libNotifyMutex);
        if (!libNotifyInit) {
            notify_init("neon");
            libNotifyInit = true;
        }
    }
} // namespace

namespace neon
{

    inline std::vector<std::filesystem::path> openFileDialogLinux(const OpenFileDialogInfo& info)
    {
        initGTK();
        auto action = info.pickFolders ? GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER : GTK_FILE_CHOOSER_ACTION_OPEN;

        std::string title = info.title.value_or(info.pickFolders ? "Select Folder" : "Open File");
        GtkWidget* dialog = gtk_file_chooser_dialog_new(title.c_str(), nullptr, action, "_Cancel", GTK_RESPONSE_CANCEL,
                                                        "_Open", GTK_RESPONSE_ACCEPT, nullptr);

        gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), info.multiselect);

        if (info.defaultPath && std::filesystem::exists(*info.defaultPath)) {
            gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), info.defaultPath->c_str());
        }

        if (info.filters) {
            for (const auto& [name, patternList] : *info.filters) {
                GtkFileFilter* filter = gtk_file_filter_new();
                gtk_file_filter_set_name(filter, name.c_str());

                size_t start = 0;
                while (start < patternList.size()) {
                    size_t end = patternList.find(';', start);
                    std::string pattern =
                        patternList.substr(start, end == std::string::npos ? std::string::npos : end - start);
                    gtk_file_filter_add_pattern(filter, pattern.c_str());
                    if (end == std::string::npos) {
                        break;
                    }
                    start = end + 1;
                }

                gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
            }
        }

        std::vector<std::filesystem::path> result;
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            if (info.multiselect) {
                GSList* files = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
                for (GSList* it = files; it; it = it->next) {
                    result.emplace_back(static_cast<char*>(it->data));
                    g_free(it->data);
                }
                g_slist_free(files);
            } else {
                char* file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
                if (file) {
                    result.emplace_back(file);
                    g_free(file);
                }
            }
        }

        gtk_widget_destroy(dialog);
        while (gtk_events_pending()) {
            gtk_main_iteration();
        }

        return result;
    }

    inline std::optional<std::filesystem::path> saveFileDialogLinux(const SaveFileDialogInfo& info)
    {
        initGTK();
        auto action = GTK_FILE_CHOOSER_ACTION_SAVE;

        std::string title = info.title.value_or("Save File");
        GtkWidget* dialog = gtk_file_chooser_dialog_new(title.c_str(), nullptr, action, "_Cancel", GTK_RESPONSE_CANCEL,
                                                        "_Save", GTK_RESPONSE_ACCEPT, nullptr);

        gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

        if (info.defaultPath && std::filesystem::exists(*info.defaultPath)) {
            gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), info.defaultPath->c_str());
        }

        if (info.defaultFileName) {
            std::string name = *info.defaultFileName;
            if (info.defaultExtension) {
                const auto& ext = *info.defaultExtension;
                // append extension if not already present
                if (name.size() < ext.size() + 1 || name.substr(name.size() - ext.size() - 1) != "." + ext) {
                    name += "." + ext;
                }
            }
            gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), name.c_str());
        }

        if (info.filters) {
            for (const auto& [name, patternList] : *info.filters) {
                GtkFileFilter* filter = gtk_file_filter_new();
                gtk_file_filter_set_name(filter, name.c_str());

                size_t start = 0;
                while (start < patternList.size()) {
                    size_t end = patternList.find(';', start);
                    std::string pattern =
                        patternList.substr(start, end == std::string::npos ? std::string::npos : end - start);
                    gtk_file_filter_add_pattern(filter, pattern.c_str());
                    if (end == std::string::npos) {
                        break;
                    }
                    start = end + 1;
                }

                gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
            }
        }

        std::optional<std::filesystem::path> result;
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            if (char* file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog))) {
                result = file;
                g_free(file);
            }
        }

        gtk_widget_destroy(dialog);
        while (gtk_events_pending()) {
            gtk_main_iteration();
        }

        return result;
    }

    void sendNotificationLinux(const NotificationInfo& info)
    {
        initNotify();
        auto* n = notify_notification_new(info.title.c_str(), info.message.c_str(),
                                          info.icon.has_value() ? info.icon->c_str() : nullptr);

        switch (info.urgency) {
            case NotificationUrgency::LOW:
                notify_notification_set_urgency(n, NOTIFY_URGENCY_LOW);
                break;
            case NotificationUrgency::NORMAL:
                notify_notification_set_urgency(n, NOTIFY_URGENCY_NORMAL);
                break;
            case NotificationUrgency::CRITICAL:
                notify_notification_set_urgency(n, NOTIFY_URGENCY_CRITICAL);

                break;
        }

        if (info.timeout) {
            notify_notification_set_timeout(n, info.timeout.value().count());
        } else {
            notify_notification_set_timeout(n, -1);
        }

        if (info.category) {
            notify_notification_set_category(n, info.category->c_str());
        }

        notify_notification_set_hint(n, "resident", g_variant_new_boolean(info.resident));
        notify_notification_set_hint(n, "sound-name", g_variant_new_string("bell"));

        notify_notification_show(n, nullptr);
        g_object_unref(G_OBJECT(n));
    }

} // namespace neon

#endif // __linux__

#endif // NEON_DIALOGSLINUX_H

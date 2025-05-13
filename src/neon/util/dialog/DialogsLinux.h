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
    #include <gtk/gtkx.h>
    #include <vulkan/VKApplication.h>

    #define GLFW_EXPOSE_NATIVE_X11

    #include <GLFW/glfw3.h>
    #include <GLFW/glfw3native.h>

namespace
{
    std::mutex gtkMutex;
    bool gtkInit = false;

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
} // namespace

namespace neon
{

    inline std::vector<std::filesystem::path> openFileDialogLinux(const OpenFileDialogInfo& info)
    {
        initGTK();
        auto action = info.pickFolders ? GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER : GTK_FILE_CHOOSER_ACTION_OPEN;

        std::string title = info.title.value_or(info.pickFolders ? "Select Folder" : "Open File");

        GtkWidget* gtkParent = nullptr;
        if (info.application != nullptr) {
            if (const auto* vk = dynamic_cast<vulkan::VKApplication*>(info.application->getImplementation())) {
                Window w = glfwGetX11Window(vk->getWindow());
                if (w != None) {
                    gtkParent = gtk_window_new(GTK_WINDOW_TOPLEVEL);
                    gtk_widget_realize(gtkParent);
                    GdkWindow* gdk = gtk_widget_get_window(gtkParent);
                    GdkDisplay* gdkDisplay = gdk_window_get_display(gdk);
                    GdkWindow* foreign = gdk_x11_window_foreign_new_for_display(gdkDisplay, w);
                    gdk_window_set_transient_for(gdk, foreign);
                }
            }
        }

        GtkWidget* dialog = gtk_file_chooser_dialog_new(title.c_str(), GTK_WINDOW(gtkParent), action, "_Cancel",
                                                        GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, nullptr);

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
        if (gtkParent != nullptr) {
            gtk_widget_destroy(gtkParent);
        }

        while (gtk_events_pending()) {
            gtk_main_iteration();
        }

        return result;
    }

    inline std::optional<std::filesystem::path> saveFileDialogLinux(const SaveFileDialogInfo& info)
    {
        return {};
        // std::string cmd;
        // if (isCommandAvailable("zenity")) {
        //     cmd = "zenity --file-selection --save";
        //     if (info.title) {
        //         cmd += " --title=\"" + *info.title + "\"";
        //     }
        //     if (info.filters) {
        //         for (const auto& [name, filter] : *info.filters) {
        //             cmd += " --file-filter=\"" + name + " | " + filter + "\"";
        //         }
        //     }
        //     if (info.application != nullptr) {
        //         cmd += " --modal";
        //     }
        // }
        //
        // auto out = execAndRead(cmd);
        // if (out.empty()) {
        //     return {};
        // }
        // return std::filesystem::path(out);
    }

} // namespace neon

#endif // __linux__

#endif // NEON_DIALOGSLINUX_H

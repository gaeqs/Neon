//
// Created by gaeqs on 12/05/2025.
//

#ifndef NEON_DIALOGS_WINDOWS_H
#define NEON_DIALOGS_WINDOWS_H

#if WIN32

    #include "Dialogs.h"

    #include <windows.h>
    #include <shobjidl.h>
    #include <locale>
    #include <codecvt>

    #define GLFW_EXPOSE_NATIVE_WIN32

    #include <GLFW/glfw3.h>
    #include <GLFW/glfw3native.h>
    #include <vulkan/VKApplication.h>

namespace neon
{

    inline std::vector<std::filesystem::path> openFileDialogWindows(const OpenFileDialogInfo& info)
    {
        std::vector<std::filesystem::path> result;
        if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
            return result;
        }

        IFileOpenDialog* dlg = nullptr;
        if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg)))) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

            if (info.filters.has_value()) {
                std::vector<std::pair<std::wstring, std::wstring>> wFilters;
                std::vector<COMDLG_FILTERSPEC> filters;

                wFilters.reserve(info.filters.value().size());
                filters.reserve(info.filters.value().size());

                for (const auto& [name, filter] : *info.filters) {
                    wFilters.emplace_back(converter.from_bytes(name), converter.from_bytes(filter));
                }

                for (const auto& [name, filter] : wFilters) {
                    filters.emplace_back(name.c_str(), filter.c_str());
                }

                dlg->SetFileTypes(filters.size(), filters.data());
                dlg->SetFileTypeIndex(1);
            }

            if (info.defaultPath) {
                IShellItem* psiFolder = nullptr;
                if (SUCCEEDED(SHCreateItemFromParsingName(info.defaultPath.value().wstring().c_str(), nullptr,
                                                          IID_PPV_ARGS(&psiFolder)))) {
                    dlg->SetDefaultFolder(psiFolder);
                    psiFolder->Release();
                }
            }

            if (info.title) {
                dlg->SetTitle(converter.from_bytes(*info.title).c_str());
            }

            FILEOPENDIALOGOPTIONS opts;
            dlg->GetOptions(&opts);

            if (info.multiselect) {
                opts |= FOS_ALLOWMULTISELECT;
            }
            if (info.pickFolders) {
                opts |= FOS_PICKFOLDERS;
            }

            dlg->SetOptions(opts);

            HWND owner = nullptr;

            if (info.application != nullptr) {
                if (const auto* vk = dynamic_cast<vulkan::VKApplication*>(info.application->getImplementation())) {
                    owner = glfwGetWin32Window(vk->getWindow());
                }
            }

            if (SUCCEEDED(dlg->Show(owner))) {
                IShellItemArray* items = nullptr;
                if (SUCCEEDED(dlg->GetResults(&items))) {
                    DWORD count = 0;
                    items->GetCount(&count);
                    for (DWORD i = 0; i < count; ++i) {
                        IShellItem* item = nullptr;
                        if (SUCCEEDED(items->GetItemAt(i, &item))) {
                            PWSTR pszPath = nullptr;
                            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &pszPath))) {
                                result.emplace_back(pszPath);
                                CoTaskMemFree(pszPath);
                            }
                            item->Release();
                        }
                    }
                    items->Release();
                }
            }
            dlg->Release();
        }

        CoUninitialize();
        return result;
    }

    inline std::optional<std::filesystem::path> saveFileDialogWindows(const SaveFileDialogInfo& info)
    {
        if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
            return {};
        }

        std::filesystem::path result;

        IFileSaveDialog* dlg = nullptr;
        if (SUCCEEDED(CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg)))) {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

            if (info.filters.has_value()) {
                std::vector<std::pair<std::wstring, std::wstring>> wFilters;
                std::vector<COMDLG_FILTERSPEC> filters;

                wFilters.reserve(info.filters.value().size());
                filters.reserve(info.filters.value().size());

                for (const auto& [name, filter] : *info.filters) {
                    wFilters.emplace_back(converter.from_bytes(name), converter.from_bytes(filter));
                }

                for (const auto& [name, filter] : wFilters) {
                    filters.emplace_back(name.c_str(), filter.c_str());
                }

                dlg->SetFileTypes(filters.size(), filters.data());
                dlg->SetFileTypeIndex(1);
            }

            if (info.defaultPath.has_value()) {
                IShellItem* psi = nullptr;
                if (SUCCEEDED(SHCreateItemFromParsingName(info.defaultPath->wstring().c_str(), nullptr,
                                                          IID_PPV_ARGS(&psi)))) {
                    dlg->SetDefaultFolder(psi);
                    psi->Release();
                }
            }

            if (info.title.has_value()) {
                dlg->SetTitle(converter.from_bytes(*info.title).c_str());
            }

            if (info.defaultFileName.has_value()) {
                dlg->SetFileName(converter.from_bytes(*info.defaultFileName).c_str());
            }
            if (info.defaultExtension.has_value()) {
                dlg->SetDefaultExtension(converter.from_bytes(*info.defaultExtension).c_str());
            }

            HWND owner = nullptr;
            if (info.application) {
                if (auto* vk = dynamic_cast<vulkan::VKApplication*>(info.application->getImplementation())) {
                    owner = glfwGetWin32Window(vk->getWindow());
                }
            }

            if (SUCCEEDED(dlg->Show(owner))) {
                IShellItem* item = nullptr;
                if (SUCCEEDED(dlg->GetResult(&item))) {
                    PWSTR psz = nullptr;
                    if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &psz))) {
                        result = psz;
                        CoTaskMemFree(psz);
                    }
                    item->Release();
                }
            }

            dlg->Release();
        }

        CoUninitialize();
        return result;
    }
} // namespace neon

#endif

#endif // NEON_DIALOGSWINDOWS_H

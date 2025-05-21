//
// Created by gaeqs on 18/05/25.
//

#ifdef USE_CEF

    #include "CefAppComponent.h"

    #include <cef_app.h>

namespace
{

    std::filesystem::path getExecutablePath()
    {
    #ifdef WIN32
        std::filesystem::path path = "neon_cef_executable.exe";
    #elif defined(__linux__)
        std::filesystem::path path = "neon_cef_executable";
    #elif defined(__APPLE__)
        std::filesystem::path path = "neon_cef_executable";
    #endif

        return absolute(path);
    }

} // namespace

namespace neon
{
    void CefAppComponent::NeonCefApp::OnBeforeCommandLineProcessing(const CefString& process_type,
                                                                    CefRefPtr<CefCommandLine> command_line)
    {
        command_line->AppendSwitch("enable-gpu");
        command_line->AppendSwitch("enable-gpu-compositing");
        command_line->AppendSwitch("enable-zero-copy");
        command_line->AppendSwitch("enable-native-gpu-memory-buffers");
        command_line->AppendSwitch("ignore-gpu-blocklist");
        command_line->AppendSwitch("enable-accelerated-video-decode");
        command_line->AppendSwitch("enable-accelerated-video-encode");
        command_line->AppendSwitch("enable-webgl");
        command_line->AppendSwitchWithValue("enable-features", "Vulkan,SkiaRenderer,UseSkiaRenderer,VulkanFromANGLE");
        command_line->AppendSwitch("disable-gtk");
        command_line->AppendSwitch("enable-begin-frame-scheduling");
        command_line->AppendSwitch("disable-software-rasterizer");
        command_line->AppendSwitch("enable-accelerated-2d-canvas");
        command_line->AppendSwitch("enable-gpu-rasterization");
        command_line->AppendSwitch("use-vulkan");
    }

    CefAppComponent::CefAppComponent() :
        _app(new NeonCefApp())
    {
        CefMainArgs args;

        CefSettings settings;
        settings.no_sandbox = true;
        settings.windowless_rendering_enabled = true;
        settings.background_color = 0xFFFFFFFF;
        CefString(&settings.browser_subprocess_path) = getExecutablePath().c_str();

        if (!CefInitialize(args, settings, _app, nullptr)) {
            neon::error() << "Error while creating CEF app!";
            return;
        }
    }

    CefAppComponent::~CefAppComponent()
    {
        CefShutdown();
    }

    void CefAppComponent::onUpdate(float deltaTime)
    {
        CefDoMessageLoopWork();
    }
} // namespace neon

#endif
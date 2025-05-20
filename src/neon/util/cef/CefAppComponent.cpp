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
        #ifdef MSVC
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
        command_line->AppendSwitch("use-gl=angle");
        command_line->AppendSwitch("enable-webgl");
        command_line->AppendSwitch("enable-features=SharedArrayBuffer,CooperativeScheduling,WebAssemblyThreads");
        command_line->AppendSwitch("disable-gtk");
    }

    CefAppComponent::CefAppComponent() :
        _app(new NeonCefApp())
    {

        CefMainArgs args;

        CefSettings settings;
        settings.no_sandbox = true;
        settings.windowless_rendering_enabled = true;
        settings.background_color = 0xFFFFFFFF;

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
//
// Created by gaeqs on 18/05/25.
//

#ifdef USE_CEF

    #include "CefAppComponent.h"

    #include <cef_app.h>

namespace neon
{
    CefAppComponent::CefAppComponent() :
        _app(new NeonCefApp())
    {

        CefMainArgs args;

        CefSettings settings;
        settings.no_sandbox = true;
        settings.windowless_rendering_enabled = true;

        if (!CefInitialize(args, settings, _app, nullptr)) {
            neon::error() << "Error while creating CEF app!";
            return;
        }
        neon::debug() << "Created!";
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
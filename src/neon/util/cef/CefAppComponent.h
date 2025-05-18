//
// Created by gaeqs on 18/05/25.
//

#ifndef NEON_CEFAPPCOMPONENT_H
#define NEON_CEFAPPCOMPONENT_H

#ifdef USE_CEF

    #include <cef_app.h>
    #include <neon/structure/Component.h>

namespace neon
{
    class CefAppComponent : public Component
    {
        class NeonCefApp : public CefApp
        {
            IMPLEMENT_REFCOUNTING(NeonCefApp);
        };

        CefRefPtr<NeonCefApp> _app;

      public:
        CefAppComponent();

        ~CefAppComponent() override;

        void onUpdate(float deltaTime) override;
    };
} // namespace neon

#endif

#endif // NEON_CEFAPPCOMPONENT_H

//
// Created by gaeqs on 23/09/2024.
//

#ifndef APPLICATIONCREATEINFO_H
#define APPLICATIONCREATEINFO_H

#include <string>
#include <neon/render/texture/TextureData.h>

/**
 * This class provides the necessary information to create and configure an application instance.
 */
struct ApplicationCreateInfo
{
    /**
     * The name of the application.
     */
    std::string name = "Neon";

    /**
     * Whether the application should be executed in fullscreen mode.
     */
    bool fullscreen = false;

    /**
     * Whether v-sync should be enabled.
     */
    bool vSync = true;

    /**
     * The initial window size of the application.
     */
    rush::Vec2f windowSize = {800, 600};

    /**
     * The texture used as the window icon.
     */
    std::optional<neon::TextureData> icon = {};
};

#endif // APPLICATIONCREATEINFO_H

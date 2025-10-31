//
// Created by gaeqs on 23/09/2024.
//

#ifndef APPLICATIONCREATEINFO_H
#define APPLICATIONCREATEINFO_H

#include <string>
#include <vector>

#include <neon/render/texture/TextureData.h>

/**
 * This class provides information required to create an application.
 * It is used to encapsulate all the parameters relevant for setting up
 * an application instance.
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
     * The textures used as the window icon.
     * You can provide several versions of the icon in several resolutions.
     */
    std::vector<neon::TextureData> icon = {};

    /**
     * Represents the file system path to the Dear ImGui initialization configuration file.
     * This path is used to load or save layout settings and other ImGui state-related data.
     */
    std::string imGuiIniPath = "imgui.ini";

    bool loadDefaultRender = false;
};

#endif // APPLICATIONCREATEINFO_H

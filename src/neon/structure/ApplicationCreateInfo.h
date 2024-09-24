//
// Created by gaeqs on 23/09/2024.
//

#ifndef APPLICATIONCREATEINFO_H
#define APPLICATIONCREATEINFO_H

#include <string>

/**
 * Information required to create a Neon Application.
 */
struct ApplicationCreateInfo {
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
};

#endif //APPLICATIONCREATEINFO_H

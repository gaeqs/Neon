//
// Created by gaeqs on 23/03/2025.
//

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H
#include <extism.hpp>
#include <vector>
#include <neon/filesystem/File.h>

namespace neon {

class PluginLoader {

    std::vector<extism::Plugin> plugins;

  public:

    PluginLoader();

    void loadPlugin(const File& file);

    void loadPluginURL(const std::string& url);

};

}



#endif //PLUGINLOADER_H

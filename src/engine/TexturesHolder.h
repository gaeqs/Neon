//
// Created by grial on 21/10/22.
//

#ifndef RVTRACKING_TEXTURESHOLDER_H
#define RVTRACKING_TEXTURESHOLDER_H

#include <map>
#include <string>
#include <memory>

#include <gl/Texture.h>

class TexturesHolder {

    std::map<std::string, std::shared_ptr<Texture>> _textures;

public:

};


#endif //RVTRACKING_TEXTURESHOLDER_H

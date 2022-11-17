//
// Created by grial on 17/11/22.
//

#ifndef NEON_LOCKMOUSECOMPONENT_H
#define NEON_LOCKMOUSECOMPONENT_H

#include <engine/Engine.h>

class LockMouseComponent : public Component {

    bool _locked = false;

public:

    void onKey(const KeyboardEvent& event) override;

};


#endif //NEON_LOCKMOUSECOMPONENT_H

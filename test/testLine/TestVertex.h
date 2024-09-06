//
// Created by gaelr on 24/10/2022.
//

#ifndef RVTRACKING_TESTVERTEX_H
#define RVTRACKING_TESTVERTEX_H

#include <neon/render/model/InputDescription.h>

#include <rush/rush.h>

struct TestVertex {
    rush::Vec3f position;
    float timestamp;

    static neon::InputDescription getDescription() {
        neon::InputDescription description(
                sizeof(TestVertex),
                neon::InputRate::VERTEX
        );
        description.addAttribute(3, 0);
        description.addAttribute(1, 12);
        return description;
    }

    static TestVertex fromAssimp(
            const rush::Vec3f& position,
            const rush::Vec3f& normal,
            const rush::Vec3f& tangent,
            const rush::Vec4f& color,
            const rush::Vec2f& texCoords) {
        return {position, 0.0f};
    }
};


#endif //RVTRACKING_TESTVERTEX_H

//
// Created by gaelr on 24/10/2022.
//

#ifndef RVTRACKING_TESTVERTEX_H
#define RVTRACKING_TESTVERTEX_H

#include <engine/model/InputDescription.h>

#include <rush/rush.h>

struct TestVertex {
    rush::Vec3f position;
    rush::Vec3f color;

    static neon::InputDescription getDescription() {
        neon::InputDescription description(
            sizeof(TestVertex),
            neon::InputRate::VERTEX
        );
        description.addAttribute(3, 0);
        description.addAttribute(3, 12);
        return description;
    }

    static TestVertex fromAssimp(
        const rush::Vec3f& position,
        const rush::Vec3f& normal,
        const rush::Vec3f& tangent,
        const rush::Vec4f& color,
        const rush::Vec2f& texCoords) {
        return {position, {1.0f, 0.0f, 0.0f}};
    }
};


struct ModelVertex {
    rush::Vec3f position;
    rush::Vec3f normal;
    rush::Vec3f tangent;
    rush::Vec2f texCoords;

    static neon::InputDescription getDescription() {
        neon::InputDescription description(
                sizeof(ModelVertex),
                neon::InputRate::VERTEX
        );
        description.addAttribute(3, 0);
        description.addAttribute(3, sizeof(float) * 3);
        description.addAttribute(3, sizeof(float) * 6);
        description.addAttribute(2, sizeof(float) * 9);

        return description;
    }

    static ModelVertex fromAssimp(
            const rush::Vec3f& position,
            const rush::Vec3f& normal,
            const rush::Vec3f& tangent,
            const rush::Vec4f& color,
            const rush::Vec2f& texCoords) {
        return {position, normal, tangent, texCoords};
    }
};


#endif //RVTRACKING_TESTVERTEX_H

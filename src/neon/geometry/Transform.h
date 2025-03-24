//
// Created by gaelr on 23/10/2022.
//

#ifndef RVTRACKING_TRANSFORM_H
#define RVTRACKING_TRANSFORM_H

#include <rush/rush.h>

#include <neon/structure/IdentifiableWrapper.h>

namespace neon
{

    class GameObject;

    class Transform
    {
        uint64_t _id;

        rush::Vec3f _position;
        rush::Quatf _rotation;
        rush::Vec3f _scale;

        IdentifiableWrapper<GameObject> _gameObject;
        uint64_t _parentIdOnLastRefresh;

        bool _dirty;
        rush::Mat4f _localModel;
        rush::Mat4f _localNormal;
        rush::Mat4f _model;
        rush::Mat4f _normal;

        void recalculateIfRequired();

      public:
        Transform(const Transform& other) = delete;

        explicit Transform(IdentifiableWrapper<GameObject> object);

        [[nodiscard]] IdentifiableWrapper<GameObject> getGameObject() const;

        [[nodiscard]] const rush::Vec3f& getPosition() const;

        [[nodiscard]] const rush::Quatf& getRotation() const;

        [[nodiscard]] const rush::Vec3f& getScale() const;

        void setPosition(const rush::Vec3f& position);

        void setRotation(const rush::Quatf& rotation);

        void setScale(const rush::Vec3f& scale);

        const rush::Vec3f& move(const rush::Vec3f& offset);

        const rush::Quatf& lookAt(const rush::Vec3f& direction);

        const rush::Quatf& rotate(const rush::Vec3f& direction, float angle);

        const rush::Mat4f& getModel();

        const rush::Mat4f& getNormal();
    };
} // namespace neon

#endif //RVTRACKING_TRANSFORM_H

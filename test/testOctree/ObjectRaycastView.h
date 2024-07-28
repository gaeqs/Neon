//
// Created by gaeqs on 6/8/24.
//

#ifndef OBJECT_RAYCAST_H
#define OBJECT_RAYCAST_H
#include <vector>

#include "TestVertex.h"
#include <engine/Engine.h>

#include "DebugRenderComponent.h"

template<typename Object>
class ObjectRaycastView : public neon::Component {
    neon::IdentifiableWrapper<DebugRenderComponent> _debug;
    Object _object;
    rush::RayCastResult<3, float> _lastHit;

public:
    ObjectRaycastView(
        const neon::IdentifiableWrapper<DebugRenderComponent>& debug,
        Object object)
        : _debug(debug),
          _object(object) {
    }

    void onMouseButton(const neon::MouseButtonEvent& event) override {
        auto& camera = getRoom()->getCamera();
        rush::Vec3f forward = camera.getForward();
        rush::Vec3f origin = camera.getPosition();

        rush::Ray3f ray(origin, forward);

        rush::raycast(ray, _object, _lastHit);
    }

    void onUpdate(float deltaTime) override {
        if (_lastHit.hit) {
            _debug->drawElement(_lastHit.point,
                                {1.0f, 0.0f, 0.0f, 1.0f},
                                0.05f);
        }
    }

    void drawEditor() override {
        ImGui::Text("Hit: %s", _lastHit.hit ? "Yes" : "No");
        ImGui::Text("Point: (%f, %f, %f)",
                    _lastHit.point.x(),
                    _lastHit.point.y(),
                    _lastHit.point.z());
        ImGui::Text("Normal: (%f, %f, %f)",
                    _lastHit.normal.x(),
                    _lastHit.normal.y(),
                    _lastHit.normal.z());
        ImGui::Text("Distance: %f", _lastHit.distance);

        bool inside = _object.closestPoint(_lastHit.point) == _lastHit.point;
        ImGui::Text("Correct?: %s", inside ? "Yes" : "NO!");

    }

    KEY_REGISTER_COMPONENT(Registry, ObjectRaycastView, "Object Raycast")
};


#endif //OBJECT_RAYCAST_H

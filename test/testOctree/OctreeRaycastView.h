//
// Created by gaeqs on 6/8/24.
//

#ifndef OCTREE_RAYCAST_H
#define OCTREE_RAYCAST_H
#include <vector>

#include "TestVertex.h"
#include <neon/Engine.h>

#include "DebugRenderComponent.h"

template<typename Storage, typename Bounds, size_t MaxObjects, size_t Depth>
class OctreeRaycastView : public neon::Component {
    const uint32_t BASE_INDICES[24] = {
        // Bottom
        0, 1,
        0, 2,
        1, 3,
        2, 3,

        // Top
        4, 5,
        4, 6,
        5, 7,
        6, 7,

        // Vertical
        0, 4,
        1, 5,
        2, 6,
        3, 7,
    };

    const std::vector<rush::Vec3f> colors = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f},
    };

    neon::IdentifiableWrapper<DebugRenderComponent> _debug;
    rush::StaticTree<Storage, Bounds, 3, float, MaxObjects, Depth> _tree;
    std::shared_ptr<neon::Material> _material;
    std::shared_ptr<neon::Model> _model;
    std::vector<std::pair<rush::AABB<3, float>, size_t>> _boxes;
    rush::StaticTreeRayCastResult<Storage, Bounds, 3, float> _lastHit;


    void addBox(std::vector<TestVertex>& vertices,
                std::vector<uint32_t>& indices,
                const rush::AABB<3, float>& box,
                size_t depth) {
        size_t start = vertices.size();

        auto radius = box.radius - 0.025f * depth;
        auto min = box.center + radius;
        auto max = box.center - radius;


        rush::Vec<3, float> edges[2] = {min, max};

        for (size_t i = 0; i < 8; ++i) {
            vertices.push_back({
                {
                    edges[i & 1].x(),
                    edges[i >> 1 & 1].y(),
                    edges[i >> 2 & 1].z()
                },
                colors[depth % colors.size()]
            });
        }

        for (uint32_t base: BASE_INDICES) {
            indices.push_back(base + start);
        }
    }

    void generateModel() {
        neon::ModelCreateInfo info;
        info.meshes.push_back(std::make_unique<neon::Mesh>(
            getApplication(),
            "octree",
            _material,
            false,
            false
        ));


        // Generate vertices
        std::vector<TestVertex> vertices;
        std::vector<uint32_t> indices;

        for (const auto& [box, depth]: _boxes) {
            addBox(vertices, indices, box, depth);
        }

        info.meshes[0]->uploadVertices(vertices);
        info.meshes[0]->uploadIndices(indices);
        info.maximumInstances = 1;

        _model = std::make_shared<neon::Model>(getApplication(), "line", info);

        getGameObject()->newComponent<neon::GraphicComponent>(_model);
    }


    template<typename Tree>
    void generateBoxes(const Tree& tree, size_t depth) {
        _boxes.push_back(std::make_pair(tree.getBounds(), depth));
        if constexpr (Tree::mayHaveChildren()) {
            if (!tree.isLeaf()) {
                for (auto child: tree.getChildren()) {
                    generateBoxes(child, depth + 1);
                }
            }
        }
    }

public:
    OctreeRaycastView(
        const neon::IdentifiableWrapper<DebugRenderComponent>& debug,
        std::shared_ptr<neon::Material> material,
        rush::StaticTree<Storage, Bounds, 3, float, MaxObjects, Depth>&& tree)
        : _debug(debug),
          _tree(std::move(tree)),
          _material(std::move(material)) {
        generateBoxes(tree.getRoot(), 0);
        std::cout << "BOXES: " << _boxes.size() << std::endl;
    }

    void onStart() override {
        generateModel();
    }

    void onMouseButton(const neon::MouseButtonEvent& event) override {
        auto& camera = getRoom()->getCamera();
        rush::Vec3f forward = camera.getForward();
        rush::Vec3f origin = camera.getPosition();

        rush::Ray3f ray(origin, forward);

        _lastHit = _tree.getRoot().raycast(ray);
    }

    void onUpdate(float deltaTime) override {
        if (_lastHit.result.hit) {
            _debug->drawElement(_lastHit.result.point,
                                {1.0f, 0.0f, 0.0f, 1.0f},
                                0.05f);

            auto bounds = _lastHit.element->bounds;
            if constexpr (std::is_same_v<Bounds, rush::Triangle<float>>) {
                for (float f = 0.0f; f <= 1.0f; f += 0.01f) {
                    _debug->drawElement(
                        bounds.a * f + bounds.b * (1.0f - f),
                        rush::Vec4f(1.0f, 0.0f, 0.0f, 1.0f),
                        0.01f
                    );
                    _debug->drawElement(
                        bounds.b * f + bounds.c * (1.0f - f),
                        rush::Vec4f(1.0f, 0.0f, 0.0f, 1.0f),
                        0.01f
                    );
                    _debug->drawElement(
                        bounds.c * f + bounds.a * (1.0f - f),
                        rush::Vec4f(1.0f, 0.0f, 0.0f, 1.0f),
                        0.01f
                    );
                }
            }
        }
    }

    void drawEditor() override {
        ImGui::Text("Hit: %s", _lastHit.result.hit ? "Yes" : "No");
        ImGui::Text("Point: (%f, %f, %f)",
                    _lastHit.result.point.x(),
                    _lastHit.result.point.y(),
                    _lastHit.result.point.z());
        ImGui::Text("Normal: (%f, %f, %f)",
                    _lastHit.result.normal.x(),
                    _lastHit.result.normal.y(),
                    _lastHit.result.normal.z());
        ImGui::Text("Distance: %f", _lastHit.result.distance);
    }

    KEY_REGISTER_COMPONENT(Registry, OctreeRaycastView, "Octree Raycast")
};


#endif //OCTREE_RAYCAST_H

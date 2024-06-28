//
// Created by gaeqs on 6/8/24.
//

#ifndef OCTREE_RAYCAST_H
#define OCTREE_RAYCAST_H
#include <vector>

#include "TestVertex.h"
#include <engine/Engine.h>

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

        info.meshes[0]->setMeshData(vertices, indices);
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

        std::cout << "-----------------" << std::endl;
        std::cout << "Normalized: " << ray.isNormalized() << std::endl;
        std::cout << camera.getRotation().length() << std::endl;
        std::cout << ray.direction.length() << std::endl;

        _lastHit = _tree.getRoot().raycast(ray);
        std::cout << "ROOT LEAF? " << _tree.getRoot().isLeaf() << std::endl;
        std::cout << (_lastHit.result.hit ? "true - " : "false - ")
                << _lastHit.result.point << " - "
                << _lastHit.result.distance << " - "
                << _lastHit.result.normal;

        if (_lastHit.result.hit) {
            std::cout << " - " << _lastHit.element->bounds.center;
        }

        std::cout << std::endl;

        std::cout << "-----------------" << std::endl;
    }

    void onUpdate(float deltaTime) override {
        if (_lastHit.result.hit) {
            _debug->drawElement(_lastHit.result.point,
                                {1.0f, 0.0f, 0.0f, 1.0f},
                                0.05f);
        }
    }
};


#endif //OCTREE_RAYCAST_H

//
// Created by gaelr on 22/01/2023.
//

#ifndef NEON_DEBUGOVERLAYCOMPONENT_H
#define NEON_DEBUGOVERLAYCOMPONENT_H

#include <engine/Engine.h>

#include <implot.h>

class DebugOverlayComponent : public Component {

    uint32_t _maxProcessTimes;
    std::deque<float> _processTimes;

    static ImPlotPoint fetchProcessTime(int id, void* data);

    void drawPerformance();

    void drawProfiling();

    void drawStack(const std::string& parentId, ProfileStack* stack);

public:

    explicit DebugOverlayComponent(uint32_t maxProcessTimes);

    void onUpdate(float deltaTime) override;

};


#endif //NEON_DEBUGOVERLAYCOMPONENT_H

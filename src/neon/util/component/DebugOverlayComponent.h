//
// Created by gaelr on 22/01/2023.
//

#ifndef NEON_DEBUGOVERLAYCOMPONENT_H
#define NEON_DEBUGOVERLAYCOMPONENT_H

#include <neon/Engine.h>

#include <implot.h>

namespace neon {
    class DebugOverlayComponent : public Component {

        uint32_t _maxProcessTimes;
        std::deque<float> _processTimes;
        bool _fixedMode;

        static ImPlotPoint fetchProcessTime(int id, void* data);

        void drawPerformance();

        void drawProfiling();

        void drawStack(const std::string& parentId, ProfileStack* stack);

    public:

        DebugOverlayComponent(bool fixedMode, uint32_t maxProcessTimes);

        void onPreDraw() override;

    };
    REGISTER_COMPONENT(DebugOverlayComponent, "Debug Overlay")
}

#endif //NEON_DEBUGOVERLAYCOMPONENT_H

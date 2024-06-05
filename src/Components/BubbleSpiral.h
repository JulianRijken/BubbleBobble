#pragma once

#include <Component.h>

namespace bb
{
    using namespace jul;

    class BubbleSpiral final : public Component
    {
    public:
        inline static constexpr double SPAWN_INTERVAL = 0.3;
        inline static constexpr double BUBBLE_SPEED = 13.0;
        inline static constexpr double ROTATE_SPEED = 0.5;
        inline static constexpr int SIDES = 7;

        BubbleSpiral(GameObject* parentPtr);

    private:
        void Update() override;
        void SpawnBubble();

        double m_SpawnIntervalTimer{};
    };
}  // namespace bb

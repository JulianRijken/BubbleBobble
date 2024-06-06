#pragma once
#include <ICollisionListener.h>

#include "Component.h"
#include "Game.h"

namespace bb
{
    using namespace jul;

    class DeadEnemy final : public Component, public ICollisionListener
    {
    public:
        inline static constexpr int MAX_BOUNCES{ 3 };
        inline static constexpr double TIME_BETWEEN_BOUNCES{ 0.5 };

        DeadEnemy(GameObject* parentPtr, PickupType fruitType);

        DeadEnemy(DeadEnemy&&) = delete;
        DeadEnemy(const DeadEnemy&) = delete;
        DeadEnemy& operator=(DeadEnemy&&) = delete;
        DeadEnemy& operator=(const DeadEnemy&) = delete;

    private:
        void OnCollisionPreSolve(const Collision& collision, const b2Manifold*) override;
        void OnCollisionBegin(const Collision&) override;

        void Update() override;

        PickupType m_PickupType;

        int m_BouncedTimes{ 0 };
        bool m_ShouldGetDestroyed{ false };
        double m_TimeSinceLastBounce{ 0 };
    };

}  // namespace bb

#ifndef DEADENEMY_H
#define DEADENEMY_H
#include <ICollisionListener.h>

#include "Component.h"
#include "Game.h"

namespace bb
{
    class DeadEnemy final : public jul::Component, public jul::ICollisionListener
    {
    public:
        inline static constexpr int MAX_BOUNCES{ 3 };
        inline static constexpr double TIME_BETWEEN_BOUNCES{ 0.5 };

        DeadEnemy(jul::GameObject* parentPtr, PickupType fruitType);

        DeadEnemy(DeadEnemy&&) = delete;
        DeadEnemy(const DeadEnemy&) = delete;
        DeadEnemy& operator=(DeadEnemy&&) = delete;
        DeadEnemy& operator=(const DeadEnemy&) = delete;

    private:
        void OnCollisionBegin(const jul::Collision& /*unused*/) override;

        void Update() override;

        PickupType m_PickupType;

        int m_BouncedTimes{ 0 };
        bool m_ShouldGetDestroyed{ false };
        double m_TimeSinceLastBounce{ 0 };
    };

}  // namespace bb
#endif // DEADENEMY_H
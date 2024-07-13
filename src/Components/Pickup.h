#ifndef PICKUP_H
#define PICKUP_H

#include "Component.h"
#include "Game.h"

namespace jul
{
    class Rigidbody;
}

namespace bb
{
    class Pickup final : public jul::Component
    {
    public:
        inline static constexpr double LIFE_TIME{ 10 };

        Pickup(jul::GameObject* parentPtr, PickupType pickupType);

        Pickup(Pickup&&) = delete;
        Pickup(const Pickup&) = delete;
        Pickup& operator=(Pickup&&) = delete;
        Pickup& operator=(const Pickup&) = delete;

        PickupType Pick();

    private:
        void FixedUpdate() override;

        PickupType m_PickupType{};

        jul::Rigidbody* m_Rigidbody{ nullptr };
    };
}  // namespace bb
#endif // PICKUP_H
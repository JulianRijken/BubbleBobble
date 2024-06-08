#pragma once

#include "Component.h"
#include "Game.h"

namespace jul
{
    class Rigidbody;
}

namespace bb
{
    using namespace jul;

    class Pickup final : public Component
    {
    public:
        inline static constexpr double LIFE_TIME{ 10 };

        Pickup(GameObject* parentPtr, PickupType pickupType);

        Pickup(Pickup&&) = delete;
        Pickup(const Pickup&) = delete;
        Pickup& operator=(Pickup&&) = delete;
        Pickup& operator=(const Pickup&) = delete;

        PickupType Pick();

    private:
        void FixedUpdate() override;

        PickupType m_PickupType{};

        Rigidbody* m_Rigidbody{ nullptr };
    };
}  // namespace bb

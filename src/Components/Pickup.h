#pragma once

#include "Component.h"
#include "Game.h"

namespace bb
{
    using namespace jul;

    class Pickup : public Component
    {
    public:
        Pickup(GameObject* parentPtr, PickupType pickupType);

        Pickup(Pickup&&) = delete;
        Pickup(const Pickup&) = delete;
        Pickup& operator=(Pickup&&) = delete;
        Pickup& operator=(const Pickup&) = delete;

        PickupType Pick();

    private:
        PickupType m_PickupType{};
    };
}  // namespace bb

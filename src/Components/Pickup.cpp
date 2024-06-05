#include "Pickup.h"

#include <TweenEngine.h>

#include "GameObject.h"

bb::Pickup::Pickup(GameObject* parentPtr, PickupType pickupType) :
    Component(parentPtr),
    m_PickupType(pickupType)
{
    // Auto destroy
    TweenEngine::Start(
        { .delay = 4, .duration = 0, .invokeWhenDestroyed = false, .onEnd = [this]() { GetGameObject()->Destroy(); } },
        this);
}

bb::PickupType bb::Pickup::Pick()
{
    throw std::runtime_error("Not implemented pick");
    return m_PickupType;
}

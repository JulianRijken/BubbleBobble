#include "Pickup.h"

#include <TweenEngine.h>

#include "GameObject.h"

bb::Pickup::Pickup(GameObject* parentPtr, PickupType pickupType) :
    Component(parentPtr),
    m_PickupType(pickupType),
    m_Rigidbody(parentPtr->GetComponent<Rigidbody>())
{
    // Auto destroy
    TweenEngine::Start({ .delay = LIFE_TIME,
                         .duration = 0,
                         .invokeWhenDestroyed = false,
                         .onEnd = [this]() { GetGameObject()->Destroy(); } },
                       this);
}

bb::PickupType bb::Pickup::Pick()
{
    throw std::runtime_error("Not implemented pick");
    return m_PickupType;
}

void bb::Pickup::FixedUpdate() { m_Rigidbody->SetVelocity(glm::vec2{ 0, -9 }); }

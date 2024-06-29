#include "Pickup.h"

#include <TweenEngine.h>

#include "GameObject.h"

using jul::GameObject;
using jul::Rigidbody;
using jul::TweenEngine;

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
    GetGameObject()->Destroy();
    return m_PickupType;
}

void bb::Pickup::FixedUpdate() { m_Rigidbody->SetVelocity(glm::vec2{ 0, -9 }); }

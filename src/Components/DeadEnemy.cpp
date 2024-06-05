#include "DeadEnemy.h"

#include <GameTime.h>

#include "BoxCollider.h"
#include "GameObject.h"
#include "prefabs.h"

bb::DeadEnemy::DeadEnemy(GameObject* parentPtr, PickupType fruitType) :
    Component(parentPtr),
    m_PickupType(fruitType)
{
}

void bb::DeadEnemy::OnCollisionPreSolve(const Collision&, const b2Manifold*)
{
    // TODO: Add collision layers
    // yes this can't be solved with just get component as it's all but world and
    // world has no component
}

void bb::DeadEnemy::OnCollisionBegin(const Collision&)
{
    if(GetGameObject()->IsBeingDestroyed())
        return;

    if(m_TimeSinceLastBounce < TIME_BETWEEN_BOUNCES)
        return;

    m_TimeSinceLastBounce = 0;

    m_BouncedTimes++;

    if(m_BouncedTimes > MAX_BOUNCES)
    {
        prefabs::SpawnPickup(m_PickupType, GetTransform().GetWorldPosition());
        GetGameObject()->Destroy();
    }
}

void bb::DeadEnemy::Update() { m_TimeSinceLastBounce += GameTime::GetDeltaTime(); }

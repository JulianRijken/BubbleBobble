#include "DeadEnemy.h"

#include <GameTime.h>

#include "GameObject.h"
#include "Prefabs.h"

using jul::Collision;
using jul::GameObject;
using jul::GameTime;

bb::DeadEnemy::DeadEnemy(GameObject* parentPtr, PickupType fruitType) :
    Component(parentPtr),
    m_PickupType(fruitType)
{
}

void bb::DeadEnemy::OnCollisionBegin(const Collision& /*unused*/)
{
    if(m_ShouldGetDestroyed)
        return;

    if(GetGameObject()->IsBeingDestroyed())
        return;

    if(m_TimeSinceLastBounce < TIME_BETWEEN_BOUNCES)
        return;

    m_TimeSinceLastBounce = 0;

    m_BouncedTimes++;


    if(m_BouncedTimes > MAX_BOUNCES)
        m_ShouldGetDestroyed = true;
}

void bb::DeadEnemy::Update()
{
    if(m_ShouldGetDestroyed)
    {
        prefabs::SpawnPickup(m_PickupType, GetTransform().GetWorldPosition());
        GetGameObject()->Destroy();
        return;
    }

    m_TimeSinceLastBounce += GameTime::GetDeltaTime();
}

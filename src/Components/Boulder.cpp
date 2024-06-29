#include "Boulder.h"
#include "IDamageable.h"

using jul::BoxCollider;
using jul::Collision;
using jul::GameObject;


bb::Boulder::Boulder(GameObject* parentPtr) :
    Component(parentPtr)
{
}

void bb::Boulder::OnCollisionBegin(const Collision& collision)
{
    const auto* collider = static_cast<BoxCollider*>(collision.otherFixture->GetUserData());
    if(auto* damageable = collider->GetGameObject()->GetComponent<IDamageable>())
        damageable->OnDamage(this);
}

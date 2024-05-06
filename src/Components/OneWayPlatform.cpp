#include "OneWayPlatform.h"

#include <GameObject.h>
#include <Physics.h>

#include "Player.h"

bb::OneWayPlatform::OneWayPlatform(GameObject* parentPtr) :
    jul::Component(parentPtr, "One Way Platform")
{
}

void bb::OneWayPlatform::OnCollisionPreSolve(Collision collision, const b2Manifold*)
{
    auto* player =
        static_cast<BoxCollider*>(collision.otherFixture->GetUserData())->GetGameObject()->GetComponent<Player>();
    if(player == nullptr)
        return;

    float delta = collision.otherFixture->GetAABB(0).lowerBound.y - collision.thisFixture->GetAABB(0).upperBound.y;

    collision.contact->SetEnabled(delta > DELTA_EPSILON);
}

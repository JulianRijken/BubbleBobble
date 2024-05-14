#include "OneWayPlatform.h"

#include <GameObject.h>
#include <GameTime.h>
#include <Physics.h>
#include <Rigidbody.h>

bb::OneWayPlatform::OneWayPlatform(GameObject* parentPtr) :
    Component(parentPtr, "One Way Platform")
{
}

void bb::OneWayPlatform::OnCollisionPreSolve(const Collision& collision, const b2Manifold*)
{

    if(collision.otherFixture->GetBody()->GetType() != b2_dynamicBody)
        return;

    const float offset = collision.otherFixture->GetBody()->GetLinearVelocity().y * GameTime::GetFixedDeltaTimeF();
    const float delta =
        collision.otherFixture->GetAABB(0).lowerBound.y - offset - collision.thisFixture->GetAABB(0).upperBound.y;

    collision.contact->SetEnabled(delta > DELTA_EPSILON);
}

#include "OneWayPlatform.h"

#include <GameObject.h>
#include <Physics.h>

#include "Player.h"

bb::OneWayPlatform::OneWayPlatform(GameObject* parentPtr) :
    jul::Component(parentPtr, "One Way Platform")
{
}

void bb::OneWayPlatform::OnCollisionPreSolve(Collision collision)
{
    auto* player =
        static_cast<BoxCollider*>(collision.otherFixture->GetUserData())->GetGameObject()->GetComponent<Player>();
    if(player == nullptr)
        return;

    b2WorldManifold worldManifold;
    collision.contact->GetWorldManifold(&worldManifold);

    const glm::vec2 hitNormal = { worldManifold.normal.x, worldManifold.normal.y };
    const float dot = glm::dot(hitNormal, glm::vec2{ 0, 1 });

    float delta = collision.otherFixture->GetAABB(0).lowerBound.y - collision.thisFixture->GetAABB(0).upperBound.y;

    collision.contact->SetEnabled(dot > 1.0f - DOT_EPSILON and delta > DELTA_EPSILON);
}

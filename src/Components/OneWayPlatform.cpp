#include "OneWayPlatform.h"

#include <GameObject.h>
#include <Physics.h>

#include "Player.h"

bb::OneWayPlatform::OneWayPlatform(GameObject* parentPtr) :
    jul::Component(parentPtr, "One Way Platform")
{
}

void bb::OneWayPlatform::OnCollisionPreSolve(b2Contact* contact, b2Fixture* otherFixture)
{
    auto* player = static_cast<BoxCollider*>(otherFixture->GetUserData())->GetGameObject()->GetComponent<Player>();
    if(player == nullptr)
        return;

    b2Fixture* thisFixture = contact->GetFixtureA();
    if(otherFixture == contact->GetFixtureA())
        thisFixture = contact->GetFixtureB();


    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);

    glm::vec2 hitNormal = { worldManifold.normal.x, worldManifold.normal.y };
    float dot = glm::dot(hitNormal, glm::vec2{ 0, 1 });
    float delta = otherFixture->GetAABB(0).lowerBound.y - thisFixture->GetAABB(0).upperBound.y;

    contact->SetEnabled(dot > 1.0f - DOT_EPSILON and delta > DELTA_EPSILON);
}

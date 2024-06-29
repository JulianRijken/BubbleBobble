#include "Character.h"

#include <Box2D/Common/b2Math.h>
#include <Physics.h>
#include <Rigidbody.h>

using jul::BoxCollider;
using jul::GameObject;
using jul::Physics;
using jul::Rigidbody;

bb::Character::Character(GameObject* parentPtr, const std::string& name) :
    jul::Component(parentPtr, name)
{
}

void bb::Character::OnMoveInput(glm::vec2 input) { m_MovementInput = input; }

bool bb::Character::IsGrounded(Rigidbody* rigidbodyPtr, BoxCollider* boxColliderPtr, float distance,
                               uint16_t checkLayers) const
{
    // If the player is moving up he is for sure not grounded :)
    if(rigidbodyPtr->GetVelocity().y > 0)
        return false;

    const b2Vec2& lowerBound = rigidbodyPtr->GetBody()->GetFixtureList()[0].GetAABB(0).lowerBound;
    const float center = rigidbodyPtr->Position().x;

    const glm::vec2 halfSize = boxColliderPtr->GetSettings().size / 2.0f;
    const float castHeight = lowerBound.y + halfSize.y;
    const float castDistance = distance + halfSize.y;
    constexpr glm::vec2 castDirection = { 0, -1 };

    if(Physics::RayCast({ center - (halfSize.x), castHeight }, castDirection, castDistance, checkLayers))
        return true;
    if(Physics::RayCast({ center, castHeight }, castDirection, castDistance, checkLayers))
        return true;
    if(Physics::RayCast({ center + (halfSize.x), castHeight }, castDirection, castDistance, checkLayers))
        return true;

    return false;
}

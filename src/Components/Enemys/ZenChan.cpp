#include "ZenChan.h"

#include <Animator.h>
#include <BoxCollider.h>
#include <fmt/core.h>
#include <GameObject.h>
#include <GameTime.h>
#include <Physics.h>
#include <Rigidbody.h>

#include "IDamageable.h"
#include "Player.h"
#include "Prefabs.h"

bb::ZenChan::ZenChan(GameObject* parentPtr) :
    Character(parentPtr, "Zen Chan"),
    m_Animator(parentPtr->GetComponent<Animator>()),
    m_Rigidbody(parentPtr->GetComponent<Rigidbody>()),
    m_SpriteRenderer(parentPtr->GetComponent<SpriteRenderer>()),
    m_BoxCollider(parentPtr->GetComponent<BoxCollider>())
{
    m_Rigidbody->SetGravityScale(0.0f);
    m_Animator->Play("zenchan_normal", true);
}

void bb::ZenChan::FixedUpdate()
{
    // TODO: Think about behaviour maybe updating a frame later
    // HandleTurning();

    if(IsGrounded())
        m_Rigidbody->AddForce({ static_cast<float>(GetMoveInput().x) * MOVE_SPEED, 0 },
                              Rigidbody::ForceMode::VelocityChange);
    else
        m_Rigidbody->AddForce({ 0, -FALL_SPEED }, Rigidbody::ForceMode::VelocityChange);
}

void bb::ZenChan::Update()
{
    if(GetMoveInput().x < 0)
        m_SpriteRenderer->m_FlipX = false;
    else if(GetMoveInput().x > 0)
        m_SpriteRenderer->m_FlipX = true;
}

bool bb::ZenChan::IsGrounded() const
{
    // If the player is moving up he is for sure not grounded :)
    if(m_Rigidbody->Velocity().y > 0)
        return false;

    const b2Vec2& lowerBound = m_Rigidbody->GetBody()->GetFixtureList()[0].GetAABB(0).lowerBound;
    const float center = m_Rigidbody->Position().x;

    const glm::vec2 halfSize = m_BoxCollider->GetSettings().size / 2.0f;
    const float castHeight = lowerBound.y + halfSize.y;
    const float castDistance = GROUND_CHECK_DISTANCE + halfSize.y;
    constexpr glm::vec2 castDirection = { 0, -1 };

    if(Physics::RayCast({ center - (halfSize.x), castHeight }, castDirection, castDistance, layer::ALL_TILES))
        return true;
    if(Physics::RayCast({ center, castHeight }, castDirection, castDistance, layer::ALL_TILES))
        return true;
    if(Physics::RayCast({ center + (halfSize.x), castHeight }, castDirection, castDistance, layer::ALL_TILES))
        return true;

    return false;
}


void bb::ZenChan::OnCollisionBegin(const Collision& collision)
{
    // Allows ZenChan to damage player
    const auto* collider = static_cast<BoxCollider*>(collision.otherFixture->GetUserData());
    if(auto* damageable = collider->GetGameObject()->GetComponent<IDamageable>())
        damageable->OnDamage(this);
}

jul::Transform* bb::ZenChan::GetCaptureTransform() { return &GetTransform(); }

void bb::ZenChan::OnJumpInput() {}

void bb::ZenChan::OnAttackInput() {}

void bb::ZenChan::SpawnDeadVersion() { prefabs::SpawnZenChanDead(GetTransform().GetWorldPosition()); }

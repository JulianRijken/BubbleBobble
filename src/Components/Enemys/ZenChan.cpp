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

    if(IsGrounded(m_Rigidbody, m_BoxCollider))
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

#include "ZenChan.h"

#include <Animator.h>
#include <BoxCollider.h>
#include <GameObject.h>
#include <GameTime.h>
#include <Physics.h>
#include <Rigidbody.h>

#include "IDamageable.h"
#include "Player.h"
#include "Prefabs.h"

bb::ZenChan::ZenChan(GameObject* parentPtr) :
    Enemy(parentPtr, "Zen Chan"),
    m_Animator(parentPtr->GetComponent<Animator>()),
    m_Rigidbody(parentPtr->GetComponent<Rigidbody>()),
    m_SpriteRenderer(parentPtr->GetComponent<SpriteRenderer>()),
    m_BoxCollider(parentPtr->GetComponent<BoxCollider>())
{
    m_Rigidbody->SetGravityScale(1.0f);
    m_Animator->Play("zenchan_normal", true);
}

void bb::ZenChan::FixedUpdate()
{
    switch(m_State)
    {
        case State::Walking:
        {
            if(not IsGrounded(m_Rigidbody, m_BoxCollider))
            {
                m_State = State::Falling;
                return;
            }

            m_Rigidbody->AddForce({ GetMoveInput() * MOVE_SPEED }, Rigidbody::ForceMode::VelocityChange);
            break;
        }
        case State::Falling:
        {
            if(IsGrounded(m_Rigidbody, m_BoxCollider))
            {
                m_State = State::Walking;
                return;
            }

            m_Rigidbody->AddForce({ 0, -FALL_SPEED }, Rigidbody::ForceMode::VelocityChange);
            break;
        }
        case State::Jumping:
            if(IsGrounded(m_Rigidbody, m_BoxCollider))
            {
                m_State = State::Walking;
                return;
            }

            break;
    }
}

void bb::ZenChan::Update()
{
    if(GetMoveInput().x < 0)
        m_SpriteRenderer->m_FlipX = false;
    else if(GetMoveInput().x > 0)
        m_SpriteRenderer->m_FlipX = true;
}

void bb::ZenChan::Jump()
{
    m_State = State::Jumping;
    m_Rigidbody->AddForce({ m_Rigidbody->GetVelocity().x * CLIMB_MOMENTEM_KEEP, CLIMB_SPEED },
                          Rigidbody::ForceMode::VelocityChange);
}


void bb::ZenChan::OnCollisionBegin(const Collision& collision)
{
    // Allows ZenChan to damage player
    const auto* collider = static_cast<BoxCollider*>(collision.otherFixture->GetUserData());
    if(auto* damageable = collider->GetGameObject()->GetComponent<IDamageable>())
        damageable->OnDamage(this);
}

jul::Transform* bb::ZenChan::GetCaptureTransform() { return &GetTransform(); }


void bb::ZenChan::OnJumpInput()
{
    if(m_State != State::Walking)
        return;

    Jump();
}

void bb::ZenChan::OnAttackInput() {}

void bb::ZenChan::SpawnDeadVersion() { prefabs::SpawnZenChanDead(GetTransform().GetWorldPosition()); }

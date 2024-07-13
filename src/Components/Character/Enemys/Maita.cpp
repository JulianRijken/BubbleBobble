#include "Maita.h"

#include <Animator.h>
#include <BoxCollider.h>
#include <GameObject.h>
#include <GameTime.h>
#include <Physics.h>
#include <Rigidbody.h>

#include "IDamageable.h"
#include "MathExtensions.h"
#include "numbers"
#include "Player.h"
#include "Prefabs.h"

using jul::Animator;
using jul::BoxCollider;
using jul::Collision;
using jul::GameObject;
using jul::GameTime;
using jul::Rigidbody;
using jul::SpriteRenderer;

bb::Maita::Maita(GameObject* parentPtr) :
    Enemy(parentPtr, "Maita"),
    m_Animator(parentPtr->GetComponent<Animator>()),
    m_Rigidbody(parentPtr->GetComponent<Rigidbody>()),
    m_SpriteRenderer(parentPtr->GetComponent<SpriteRenderer>()),
    m_BoxCollider(parentPtr->GetComponent<BoxCollider>())
{
    m_Rigidbody->SetGravityScale(1.0f);
    m_Animator->Play("maita_normal", true);
}

void bb::Maita::FixedUpdate()
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

void bb::Maita::Update()
{
    m_TimeSinceLastBoulderThrow += GameTime::GetDeltaTime<float>();

    if(GetMoveInput().x < 0)
        m_SpriteRenderer->m_FlipX = false;
    else if(GetMoveInput().x > 0)
        m_SpriteRenderer->m_FlipX = true;
}

void bb::Maita::Jump()
{
    m_State = State::Jumping;
    m_Rigidbody->AddForce({ m_Rigidbody->GetVelocity().x * CLIMB_MOMENTEM_KEEP, CLIMB_SPEED },
                          Rigidbody::ForceMode::VelocityChange);
}

void bb::Maita::OnCollisionBegin(const Collision& collision)
{
    // Allows Maita to damage player
    const auto* collider = static_cast<BoxCollider*>(collision.otherFixture->GetUserData());
    if(auto* damageable = collider->GetGameObject()->GetComponent<IDamageable>())
        damageable->OnDamage(this);
}

jul::Transform* bb::Maita::GetCaptureTransform() { return &GetTransform(); }

void bb::Maita::OnJumpInput()
{
    if(m_State != State::Walking)
        return;

    Jump();
}

void bb::Maita::OnAttackInput()
{
    if(m_TimeSinceLastBoulderThrow < BOULDER_THROW_INTERVAL)
        return;

    const float power = (m_SpriteRenderer->m_FlipX ? -1.0f : 1.0f) * BOULDER_THROW_POWER;
    const float angle = (static_cast<float>(jul::math::RandomValue()) - 0.5f) + static_cast<float>(std::numbers::pi);
    prefabs::SpawnBoulder(GetTransform().GetWorldPosition(), { std::cos(angle) * power, std::sin(angle) * power });
    m_TimeSinceLastBoulderThrow = 0;
}

void bb::Maita::SpawnDeadVersion() { prefabs::SpawnMaitaDead(GetTransform().GetWorldPosition()); }

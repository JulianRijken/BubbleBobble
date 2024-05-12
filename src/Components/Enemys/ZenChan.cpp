#include "ZenChan.h"

#include <Animator.h>
#include <GameObject.h>
#include <Rigidbody.h>

bb::ZenChan::ZenChan(GameObject* parent) :
    Component(parent),
    m_Animator(parent->GetComponent<Animator>()),
    m_Rigidbody(parent->GetComponent<Rigidbody>()),
    m_SpriteRenderer(parent->GetComponent<SpriteRenderer>())
{
    m_Rigidbody->SetGravityScale(0.0f);

    m_Animator->PlayAnimation("zenchan_normal", true);
}

void bb::ZenChan::FixedUpdate()
{
    m_Rigidbody->AddForce({ m_WalkingDirection * MOVE_SPEED, 0 }, Rigidbody::ForceMode::VelocityChange);
}

void bb::ZenChan::Update() { m_SpriteRenderer->m_FlipX = m_WalkingDirection > 0; }

#include "AttackBubble.h"

#include <Animator.h>
#include <fmt/core.h>
#include <GameObject.h>
#include <GameTime.h>


bb::AttackBubble::AttackBubble(GameObject* parent, glm::vec3 fireVelocity) :
    Component(parent),
    m_Animator(parent->GetComponent<Animator>()),
    m_Rigidbody(parent->GetComponent<Rigidbody>())
{
    g_Bubbles.insert(this);

    m_Animator->PlayAnimation("Spawn");
    m_Rigidbody->AddForce(fireVelocity, Rigidbody::ForceMode::VelocityChange);

    m_Rigidbody->SetGravityScale(0.0f);
}

bb::AttackBubble::~AttackBubble() { g_Bubbles.erase(this); }

void bb::AttackBubble::OnCollisionBegin(Collision /*unused*/) { fmt::println("Bubble Colliding"); }

void bb::AttackBubble::Update()
{
    m_FloatingDuration += GameTime::GetDeltaTimeF();

    m_BubbleCenter = {};
    if(not g_Bubbles.empty())
    {
        for(auto&& bubble : g_Bubbles)
            m_BubbleCenter += glm::vec2(bubble->GetTransform().WorldPosition());

        m_BubbleCenter /= g_Bubbles.size();
    }
}

void bb::AttackBubble::FixedUpdate()
{

    // Adds drag
    m_Rigidbody->AddForce(-m_Rigidbody->Velocity(), Rigidbody::ForceMode::Force);

    if(m_FloatingDuration > DURATION_BEFORE_FLOATING)
    {
        if(m_Rigidbody->Positon().y < FLOAT_HEIGHT)
            m_Rigidbody->AddForce({ 0, BUBBLE_FLOAT_UP_FORCE }, Rigidbody::ForceMode::Force);

        const glm::vec2 direction = m_BubbleCenter - m_Rigidbody->Positon();
        m_Rigidbody->AddForce(direction * BUBBLE_ATTRACTION_FORCE, Rigidbody::ForceMode::Force);
    }
}

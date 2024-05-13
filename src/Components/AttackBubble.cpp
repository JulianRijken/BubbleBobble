#include "AttackBubble.h"

#include <Animator.h>
#include <BoxCollider.h>
#include <fmt/core.h>
#include <GameObject.h>
#include <GameTime.h>
#include <MathExtensions.h>

#include "IDamagable.h"


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

void bb::AttackBubble::OnCollisionPreSolve(Collision collision, const b2Manifold*)
{
    if(not collision.contact->IsEnabled())
        return;

    if(m_FloatingDuration < DURATION_BEFORE_FLOATING)
        return;

    auto velocityDiffb2 =
        collision.otherFixture->GetBody()->GetLinearVelocity() - collision.thisFixture->GetBody()->GetLinearVelocity();

    const float bubbleStrenght =
        (velocityDiffb2.Length() * POP_VELOCITY_STRENGTH) + (m_FloatingDuration * POP_DURATION_STRENGTH);

    if(bubbleStrenght < POP_THRESHOLD)
        return;


    collision.contact->SetEnabled(false);

    if(m_Popped)
        return;

    m_Popped = true;
    m_Animator->PlayAnimation("Pop");
}

void bb::AttackBubble::OnCollisionBegin(Collision collision)
{
    const auto* collider = static_cast<BoxCollider*>(collision.otherFixture->GetUserData());
    if(auto* damageable = collider->GetGameObject()->GetComponent<IDamagable>())
        damageable->OnDamage(this);
}

void bb::AttackBubble::Update()
{
    if(m_Popped)
    {
        m_Rigidbody->AddForce({}, jul::Rigidbody::ForceMode::VelocityChange);

        if(not m_Animator->IsPlaying())
            GetGameObject()->Destroy();
    }
    else
    {
        if(not m_Animator->IsPlaying())
            m_Animator->PlayAnimation("Idle");

        m_FloatingDuration += GameTime::GetDeltaTimeF();

        m_BubbleCenter = {};
        if(not g_Bubbles.empty())
        {
            for(auto&& bubble : g_Bubbles)
                m_BubbleCenter += glm::vec2(bubble->GetTransform().WorldPosition());

            m_BubbleCenter /= g_Bubbles.size();
        }
    }
}

void bb::AttackBubble::FixedUpdate()
{
    if(m_Popped)
        return;

    // Add noise
    constexpr float randomForce = 20.0f;
    m_Rigidbody->AddForce(
        { jul::math::RandomRange(-randomForce, randomForce), jul::math::RandomRange(-randomForce, randomForce) },
        Rigidbody::ForceMode::Force);


    // Adds drag
    m_Rigidbody->AddForce(-m_Rigidbody->Velocity(), Rigidbody::ForceMode::Force);

    if(m_FloatingDuration > DURATION_BEFORE_FLOATING)
    {
        if(m_Rigidbody->Position().y < FLOAT_HEIGHT)
            m_Rigidbody->AddForce({ 0, BUBBLE_FLOAT_UP_FORCE }, Rigidbody::ForceMode::Force);

        const glm::vec2 direction = m_BubbleCenter - m_Rigidbody->Position();
        m_Rigidbody->AddForce(direction * BUBBLE_ATTRACTION_FORCE, Rigidbody::ForceMode::Force);
    }
}

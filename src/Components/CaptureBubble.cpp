#include "CaptureBubble.h"

#include <Animator.h>
#include <BoxCollider.h>
#include <fmt/core.h>
#include <GameObject.h>
#include <GameTime.h>
#include <MathExtensions.h>

#include "IBubbleable.h"
#include "IDamageable.h"

bb::CaptureBubble::CaptureBubble(GameObject* parent, glm::vec3 fireVelocity) :
    Component(parent),
    m_Animator(parent->GetComponent<Animator>()),
    m_Rigidbody(parent->GetComponent<Rigidbody>())
{
    g_Bubbles.insert(this);

    m_Animator->PlayAnimation("Spawn");
    m_Rigidbody->AddForce(fireVelocity, Rigidbody::ForceMode::VelocityChange);

    m_Rigidbody->SetGravityScale(0.0f);
}

bb::CaptureBubble::~CaptureBubble() { g_Bubbles.erase(this); }

void bb::CaptureBubble::StartPop()
{
    m_Animator->PlayAnimation("Pop");
    m_GettingPopped = true;
}

void bb::CaptureBubble::Capture(IBubbleable* target)
{
    m_CapturedTarget = target;
    target->GetCaptureTransform()->GetGameObject()->SetActive(false);
    target->OnCapture();
}

void bb::CaptureBubble::ReleaseCapturedTarget()
{
    auto* targetTransform = m_CapturedTarget->GetCaptureTransform();
    targetTransform->SetWorldPosition(GetTransform().GetWorldPosition());
    targetTransform->GetGameObject()->SetActive(true);
    m_CapturedTarget->OnRelease();
}

void bb::CaptureBubble::OnCollisionPreSolve(const Collision& collision, const b2Manifold*)
{
    if(m_GettingPopped)
        return;

    if(not collision.contact->IsEnabled())
        return;

    if(not m_CapturedTarget)
    {
        const auto* collider = static_cast<BoxCollider*>(collision.otherFixture->GetUserData());
        if(auto* bubbleable = collider->GetGameObject()->GetComponent<IBubbleable>())
        {
            Capture(bubbleable);
            return;
        }
    }

    if(m_FloatingDuration < DURATION_BEFORE_FLOATING)
        return;

    const auto velocityDelta =
        collision.otherFixture->GetBody()->GetLinearVelocity() - collision.thisFixture->GetBody()->GetLinearVelocity();

    const float bubbleStrength =
        (velocityDelta.Length() * POP_VELOCITY_STRENGTH) + (m_FloatingDuration * POP_DURATION_STRENGTH);

    if(bubbleStrength < POP_THRESHOLD)
        return;

    collision.contact->SetEnabled(false);
    StartPop();
}


void bb::CaptureBubble::Update()
{
    if(m_GettingPopped)
    {
        m_Rigidbody->AddForce({}, jul::Rigidbody::ForceMode::VelocityChange);

        if(not m_Animator->IsPlaying())
        {
            if(m_CapturedTarget != nullptr)
                ReleaseCapturedTarget();

            GetGameObject()->Destroy();
        }
    }
    else
    {
        if(not m_Animator->IsPlaying())
            m_Animator->PlayAnimation("Idle");

        m_FloatingDuration += GameTime::GetDeltaTime<float>();

        m_BubbleCenter = {};
        if(not g_Bubbles.empty())
        {
            for(auto&& bubble : g_Bubbles)
                m_BubbleCenter += glm::vec2(bubble->GetTransform().GetWorldPosition());

            m_BubbleCenter /= g_Bubbles.size();
        }
    }
}

void bb::CaptureBubble::FixedUpdate()
{
    if(m_GettingPopped)
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

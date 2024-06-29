#include "CaptureBubble.h"

#include <Animator.h>
#include <BoxCollider.h>
#include <fmt/core.h>
#include <GameObject.h>
#include <GameTime.h>
#include <MathExtensions.h>
#include <Resources.h>

#include "Game.h"
#include "IBubbleable.h"
#include "IDamageable.h"

using jul::Animator;
using jul::BoxCollider;
using jul::Collision;
using jul::GameObject;
using jul::GameTime;
using jul::Resources;
using jul::Rigidbody;
using jul::SpriteRenderer;

bb::CaptureBubble::CaptureBubble(GameObject* parent, glm::vec3 fireVelocity) :
    Component(parent),
    m_BubbleSpriteRenderer(parent->GetComponent<SpriteRenderer>()),
    m_BubbleAnimator(parent->GetComponent<Animator>()),
    m_CapturedSpriteRenderer(parent->AddComponent<SpriteRenderer>()),
    m_CapturedAnimator(parent->AddComponent<Animator>(m_CapturedSpriteRenderer)),
    m_Rigidbody(parent->GetComponent<Rigidbody>())
{
    g_Bubbles.insert(this);

    m_BubbleAnimator->Play("Spawn");
    m_Rigidbody->AddForce(fireVelocity, Rigidbody::ForceMode::VelocityChange);

    m_Rigidbody->SetGravityScale(0.0f);
}

bb::CaptureBubble::~CaptureBubble() { g_Bubbles.erase(this); }

void bb::CaptureBubble::StartPop()
{
    m_BubbleAnimator->Play("Pop");
    m_GettingPopped = true;

    m_CapturedAnimator->Stop();
    m_CapturedSpriteRenderer->SetSprite(nullptr);
    m_BubbleSpriteRenderer->SetEnabled(true);
}

void bb::CaptureBubble::Capture(IBubbleable* target)
{
    m_CapturedTarget = target;
    target->GetCaptureTransform()->GetGameObject()->SetActive(false);
    m_CapturedSpriteRenderer->SetSprite(Resources::GetSprite(target->GetSpriteName()));
    m_CapturedAnimator->Play(target->GetSpriteAnimationName(), true);
    m_BubbleSpriteRenderer->SetEnabled(false);

    m_CapturedTarget->m_IsBubbleabled = true;
    target->OnCapture();
}

void bb::CaptureBubble::KillCapturedTarget()
{
    auto* targetTransform = m_CapturedTarget->GetCaptureTransform();

    targetTransform->SetWorldPosition(GetTransform().GetWorldPosition());
    m_CapturedTarget->SpawnDeadVersion();

    m_CapturedTarget->m_IsBubbleabled = false;
    targetTransform->GetGameObject()->Destroy();
}

void bb::CaptureBubble::ReleaseCapturedTarget()
{
    auto* targetTransform = m_CapturedTarget->GetCaptureTransform();
    targetTransform->SetWorldPosition(GetTransform().GetWorldPosition());
    targetTransform->GetGameObject()->SetActive(true);

    m_CapturedTarget->m_IsBubbleabled = false;
    m_CapturedTarget->OnRelease();
}

void bb::CaptureBubble::OnCollisionPreSolve(const Collision& collision, const b2Manifold*)
{
    if(m_FloatingDuration < 0.1)
    {
        if(collision.otherFixture->GetFilterData().categoryBits == layer::PLAYER)
            collision.contact->SetEnabled(false);
    }

    if(m_GettingPopped)
    {
        collision.contact->SetEnabled(false);
        return;
    }

    // Force enemys to igunore bubble when captured
    if(m_CapturedTarget != nullptr)
        if(collision.otherFixture->GetFilterData().categoryBits & layer::ENEMY)
            collision.contact->SetEnabled(false);


    if(not collision.contact->IsEnabled())
        return;

    if(not m_CapturedTarget)
    {
        const auto* collider = static_cast<BoxCollider*>(collision.otherFixture->GetUserData());
        if(auto* bubbleable = collider->GetGameObject()->GetComponent<IBubbleable>())
        {
            if(bubbleable->m_IsBubbleabled)
                return;

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

        if(not m_BubbleAnimator->IsPlaying())
        {
            if(m_CapturedTarget != nullptr)
            {
                if(m_EnemyPoppingOut)
                    ReleaseCapturedTarget();
                else
                    KillCapturedTarget();
            }

            GetGameObject()->Destroy();
        }
    }
    else
    {
        if(not m_BubbleAnimator->IsPlaying())
            m_BubbleAnimator->Play("Idle");

        m_FloatingDuration += GameTime::GetDeltaTime<float>();

        if(m_FloatingDuration > MAX_TIME_TILL_POP)
        {
            StartPop();
            m_EnemyPoppingOut = true;
            return;
        }

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
    m_Rigidbody->AddForce(-m_Rigidbody->GetVelocity(), Rigidbody::ForceMode::Force);

    if(m_FloatingDuration > DURATION_BEFORE_FLOATING)
    {
        if(m_Rigidbody->Position().y < FLOAT_HEIGHT)
            m_Rigidbody->AddForce({ 0, BUBBLE_FLOAT_UP_FORCE }, Rigidbody::ForceMode::Force);

        const glm::vec2 direction = m_BubbleCenter - m_Rigidbody->Position();
        m_Rigidbody->AddForce(direction * BUBBLE_ATTRACTION_FORCE, Rigidbody::ForceMode::Force);
    }
}

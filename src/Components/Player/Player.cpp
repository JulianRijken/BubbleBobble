#include "Player.h"

#include <fmt/core.h>
#include <GameObject.h>
#include <GameTime.h>
#include <MessageQueue.h>
#include <Physics.h>
#include <TweenEngine.h>

#include "CaptureBubble.h"
#include "Game.h"
#include "Pickup.h"

using jul::Animator;
using jul::BoxCollider;
using jul::Collision;
using jul::EaseFunction;
using jul::GameObject;
using jul::MessageQueue;
using jul::Rigidbody;
using jul::SpriteRenderer;
using jul::TweenEngine;


bb::Player::Player(GameObject* parentPtr, int playerIndex, SpriteRenderer* bodySpriteRenderer,
                   SpriteRenderer* bubbleSpriteRenderer, Animator* bodyAnimator, Animator* bubbleAnimator) :
    Character(parentPtr, "PlayerController"),
    m_PlayerIndex(playerIndex),
    m_BodyAnimatorPtr(bodyAnimator),
    m_BubbleAnimatorPtr(bubbleAnimator),
    m_RigidbodyPtr(parentPtr->GetComponent<Rigidbody>()),
    m_BodySpriteRendererPtr(bodySpriteRenderer),
    m_BubbleSpriteRendererPtr(bubbleSpriteRenderer),
    m_ColliderPtr(parentPtr->GetComponent<BoxCollider>()),
    m_Health(parentPtr->GetComponent<Health>())
{
    Game::GetInstance().SetPlayer(playerIndex, this);
    m_ActiveMainState->OnEnterState(*this);
}

bb::Player::~Player() { Game::GetInstance().SetPlayer(m_PlayerIndex, nullptr); }

bool bb::Player::IsDead() const { return m_ActiveMainState == m_DeathState.get(); }


void bb::Player::BubbleToPosition(const glm::vec3& position, double duration)
{
    if(m_ActiveMainState == m_DeathState.get())
        return;

    if(m_ActiveMainState != m_BubbleState.get())
        SetMainState(m_BubbleState.get());

    TweenEngine::Start(
        {
            .duration = duration,
            .onUpdate =
                [this](double value)
            {
                // Play pop animation close to the end
                if(value > 0.70)
                    if(m_BubbleAnimatorPtr->IsActiveAnimation("Idle"))
                        m_BubbleAnimatorPtr->Play("Pop");
            },
            .onEnd = [this]() { SetMainState(m_WalkingState.get()); },

        },
        this);

    TweenEngine::Start(
        {
            .from = GetTransform().GetWorldPosition().x,
            .to = position.x,
            .duration = duration,
            .easeFunction = EaseFunction::SineInOut,
            .onUpdate =
                [this](double value)
            {
                const auto& currentPosition = GetTransform().GetWorldPosition();
                GetTransform().SetWorldPosition(value, currentPosition.y, currentPosition.z);
            },
        },
        this);

    TweenEngine::Start(
        {
            .from = GetTransform().GetWorldPosition().y,
            .to = position.y,
            .duration = duration,
            .easeFunction = EaseFunction::SineInOut,
            .onUpdate =
                [this](double value)
            {
                const auto& currentPosition = GetTransform().GetWorldPosition();
                GetTransform().SetWorldPosition(currentPosition.x, value, currentPosition.z);
            },
        },
        this);
}

void bb::Player::OnJumpInput()
{
    m_ActiveMainState->OnJumpInput(*this);
    m_ActiveAttackState->OnJumpInput(*this);
}

void bb::Player::OnAttackInput()
{
    m_ActiveMainState->OnAttackInput(*this);
    m_ActiveAttackState->OnAttackInput(*this);
}

void bb::Player::SetMainState(PlayerState* nextState)
{
    m_ActiveMainState->OnExitState(*this);
    nextState->OnEnterState(*this);
    m_ActiveMainState = nextState;
}

void bb::Player::SetAttackState(PlayerState* nextState)
{
    m_ActiveAttackState->OnExitState(*this);
    nextState->OnEnterState(*this);
    m_ActiveAttackState = nextState;
}

void bb::Player::HandleFlip()
{
    // Not just setting it with the if as 0 should not change
    if(GetMoveInput().x < 0)
        m_BodySpriteRendererPtr->m_FlipX = true;
    else if(GetMoveInput().x > 0)
        m_BodySpriteRendererPtr->m_FlipX = false;
}

void bb::Player::Respawn()
{
    glm::vec3 position = m_PlayerIndex == 0 ? Game::PLAYER_1_DEFAULT_POSITION : Game::PLAYER_2_DEFAULT_POSITION;
    GetTransform().SetWorldPosition(position);
    SetMainState(m_WalkingState.get());

    m_Respawning = true;

    TweenEngine::Cancel(this);
    TweenEngine::Start(

        { .duration = 3.0,
          .onUpdate =
              [this](double)
          {
              const bool enabled = m_BodySpriteRendererPtr->IsEnabled();
              m_BodySpriteRendererPtr->SetEnabled(not enabled);
          },
          .onEnd =
              [this]()
          {
              m_Respawning = false;
              m_BodySpriteRendererPtr->SetEnabled(true);
          } },
        this);
}

void bb::Player::ObtainPickup(PickupType pickupType)
{
    MessageQueue::Broadcast(MessageType::PlayerPickup,
                            { GetTransform().GetWorldPosition(), m_PlayerIndex, pickupType });
}


void bb::Player::Update()
{
    m_ActiveMainState->Update(*this);
    m_ActiveAttackState->Update(*this);
}

void bb::Player::FixedUpdate()
{
    m_ActiveMainState->FixedUpdate(*this);
    m_ActiveAttackState->FixedUpdate(*this);
}

void bb::Player::OnDamage(Component* instigator)
{
    if(m_Respawning)
        return;

    // TODO: Try to avoid dynamic cast
    if(dynamic_cast<CaptureBubble*>(instigator))
        return;

    m_ActiveAttackState->OnPlayerDamage(*this);
    m_ActiveMainState->OnPlayerDamage(*this);
}

void bb::Player::OnCollisionPreSolve(const Collision& collision, const b2Manifold*)
{
    if(collision.otherFixture->GetFilterData().categoryBits == layer::ENEMY)
        collision.contact->SetEnabled(false);


    if(m_ActiveMainState != m_WalkingState.get() and m_ActiveMainState != m_JumpingState.get())
        return;

    const auto* collider = static_cast<BoxCollider*>(collision.otherFixture->GetUserData());
    if(auto* pickup = collider->GetGameObject()->GetComponent<Pickup>())
    {
        ObtainPickup(pickup->Pick());
        collision.contact->SetEnabled(false);
    }
}

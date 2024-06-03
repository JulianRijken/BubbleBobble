#include "PlayerState.h"

#include <Animator.h>
#include <Box2D/Box2D.h>
#include <fmt/core.h>
#include <GameTime.h>
#include <MessageQueue.h>
#include <ResourceManager.h>
#include <SceneManager.h>
#include <SpriteRenderer.h>
#include <TweenEngine.h>

#include "AttackBubble.h"
#include "Game.h"
#include "Player.h"

/////////////////////
/// WALKING STATE ///
/////////////////////
void bb::PlayerWalkingState::OnEnterState(Player& player)
{
    m_TimeWalking = 0.0f;
    player.m_BodyAnimatorPtr->PlayAnimation(player.m_IdleAnimationName, true);
    player.m_RigidbodyPtr->SetGravityScale(WALK_GRAVITY_SCALE);
}

void bb::PlayerWalkingState::Update(Player& player)
{
    m_TimeWalking += GameTime::GetDeltaTime<float>();

    if(not player.IsGrounded() and m_TimeWalking > MIN_TIME_WALKING_BEFORE_GROUND_CHECK)
    {
        player.SetMainState(player.m_JumpingState.get());
        return;
    }

    player.HandleFlip();

    if(not player.m_BodyAnimatorPtr->IsPlaying())
        player.m_BodyAnimatorPtr->PlayAnimation(player.m_IdleAnimationName, true);


    if(std::abs(player.m_RigidbodyPtr->Velocity().x) > 0.0f)
    {
        if(player.m_BodyAnimatorPtr->IsActiveAnimation(player.m_IdleAnimationName))
            player.m_BodyAnimatorPtr->PlayAnimation(player.m_WalkAnimationName, true);
    }
    else
    {
        if(player.m_BodyAnimatorPtr->IsActiveAnimation(player.m_WalkAnimationName))
            player.m_BodyAnimatorPtr->PlayAnimation(player.m_IdleAnimationName, true);
    }
}

void bb::PlayerWalkingState::FixedUpdate(Player& player)
{
    player.m_RigidbodyPtr->AddForce({ player.m_MovementInput * MOVE_SPEED, player.m_RigidbodyPtr->Velocity().y },
                                 Rigidbody::ForceMode::VelocityChange);
}

void bb::PlayerWalkingState::OnJumpInput(Player& player)
{
    if(player.IsGrounded())
    {
        MessageQueue::Broadcast(MessageType::PlayerJump);
        player.m_RigidbodyPtr->AddForce({ player.m_RigidbodyPtr->Velocity().x, PlayerJumpingState::JUMP_FORCE },
                                     Rigidbody::ForceMode::VelocityChange);
        player.SetMainState(player.m_JumpingState.get());
        return;
    }
}

void bb::PlayerWalkingState::OnAttackInput(Player& player)
{
    if(player.m_ActiveAttackState != player.m_AttackingState.get())
        player.SetAttackState(player.m_AttackingState.get());
}

void bb::PlayerWalkingState::OnExitState(Player& player) { player.m_RigidbodyPtr->SetGravityScale(1.0f); }

void bb::PlayerWalkingState::OnPlayerDamage(Player& player) { player.SetMainState(player.m_DeathState.get()); }

/////////////////////
/// JUMPING STATE ///
/////////////////////
void bb::PlayerJumpingState::OnEnterState(Player& player)
{
    m_SlowFallHeight = player.m_RigidbodyPtr->Position().y;
    m_TimeInJump = 0.0f;
    player.m_RigidbodyPtr->SetGravityScale(0.0f);

    m_Falling = true;
    m_HasManualControl = false;

    if(player.m_RigidbodyPtr->Velocity().y > 0)
    {
        m_Falling = false;
        player.m_BodyAnimatorPtr->PlayAnimation(player.m_JumpAnimationName);
    }
    else
    {
        player.m_BodyAnimatorPtr->PlayAnimation(player.m_FallingAnimationName, true);
    }
}

void bb::PlayerJumpingState::Update(Player& player)
{
    // Go out of state
    if(player.IsGrounded())
    {
        player.SetMainState(player.m_WalkingState.get());
        return;
    }

    player.HandleFlip();

    m_TimeInJump += GameTime::GetDeltaTime<float>();

    // Tell player he is falling
    if(player.m_RigidbodyPtr->Position().y < m_SlowFallHeight)
        m_Falling = true;

    // Force falling animation when done with jump
    if(not player.m_BodyAnimatorPtr->IsPlaying())
        player.m_BodyAnimatorPtr->PlayAnimation(player.m_FallingAnimationName, true);
}

void bb::PlayerJumpingState::FixedUpdate(Player& player)
{
    const glm::vec2 currentVelocity = player.m_RigidbodyPtr->Velocity();

    if(m_Falling)
    {
        player.m_RigidbodyPtr->AddForce({ player.m_MovementInput * MOVE_SPEED, -FALL_SPEED },
                                     Rigidbody::ForceMode::VelocityChange);
    }
    else
    {
        // Add gravity manually
        player.m_RigidbodyPtr->AddForce({ 0.0f, GRAVITY_FOCE }, Rigidbody::ForceMode::Force);


        if(not m_HasManualControl)
        {
            if((currentVelocity.x > 0 and player.m_MovementInput < 0) or
               (currentVelocity.x < 0 and player.m_MovementInput > 0) or currentVelocity.x == 0)
            {
                m_HasManualControl = true;
            }
        }


        if(m_HasManualControl)
        {
            player.m_RigidbodyPtr->AddForce({ player.m_MovementInput * MOVE_SPEED, currentVelocity.y },
                                         Rigidbody::ForceMode::VelocityChange);
        }
    }
}

void bb::PlayerJumpingState::OnExitState(Player& player) { player.m_RigidbodyPtr->SetGravityScale(1.0f); }

void bb::PlayerJumpingState::OnAttackInput(Player& player)
{
    if(player.m_ActiveAttackState != player.m_AttackingState.get())
        player.SetAttackState(player.m_AttackingState.get());
}

void bb::PlayerJumpingState::OnPlayerDamage(Player& player) { player.SetMainState(player.m_DeathState.get()); }


////////////////////
/// ATTACK STATE ///
/////////////////////
void bb::PlayerAttackingState::OnEnterState(Player& player)
{
    if(GameTime::GetElapsedTime() - m_TimeOfLastAttack < TIME_BETWEEN_FIRE)
    {
        player.m_ActiveAttackState = player.m_NullState.get();
        return;
    }

    m_TimeOfLastAttack = GameTime::GetElapsedTime<float>();

    player.m_BodyAnimatorPtr->PlayAnimation(player.m_AttackAnimationName);
    MessageQueue::Broadcast(MessageType::PlayerAttack);

    player.m_Score += 1000;
    player.m_OnScoreChangeEvent.Invoke(player.m_Score);

    glm::vec3 spawnPosition = player.GetTransform().GetWorldPosition();
    const float direction = player.m_BodySpriteRendererPtr->m_FlipX ? -1.0f : 1.0f;

    spawnPosition.x += player.m_ColliderPtr->GetSettings().size.x * static_cast<float>(direction);

    auto* bubble = SceneManager::GetInstance().AddGameObject("AttackBubble", spawnPosition);
    bubble->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("AttackBubble"), 0);
    bubble->AddComponent<Animator>();
    bubble->AddComponent<Rigidbody>();
    bubble->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.5f,
        .restitution = 0.8f,
        .size = {1.5, 1.5f},
    });
    bubble->AddComponent<AttackBubble>(glm::vec3{ direction * FIRE_POWER, 0, 0 });
}

void bb::PlayerAttackingState::Update(Player& player)
{
    if(not player.m_BodyAnimatorPtr->IsActiveAnimation(player.m_AttackAnimationName))
        player.SetAttackState(player.m_NullState.get());
}

///////////////////
/// DEATH STATE ///
///////////////////
void bb::PlayerDeathState::OnEnterState(Player& player)
{
    player.m_RigidbodyPtr->SetMode(Rigidbody::Mode::Static);
    player.m_ColliderPtr->SetSensor(true);

    MessageQueue::Broadcast(MessageType::PlayerDied);
    player.m_BodyAnimatorPtr->PlayAnimation(player.m_DeathAnimationName);
    player.m_Lives--;
    player.m_OnDeathEvent.Invoke(player.m_Lives);
}


void bb::PlayerDeathState::OnExitState(Player& player)
{
    player.m_RigidbodyPtr->SetMode(Rigidbody::Mode::Dynamic);
    player.m_ColliderPtr->SetSensor(false);
}

////////////////////
/// BUBBLE STATE ///
////////////////////
void bb::PlayerBubbleState::OnEnterState(Player& player)
{
    player.m_BubbleAnimatorPtr->PlayAnimation("Spawn");
    player.m_BodyAnimatorPtr->PlayAnimation("InBubble", true);
    player.m_RigidbodyPtr->SetMode(Rigidbody::Mode::Static);
}

void bb::PlayerBubbleState::OnExitState(Player& player)
{
    player.m_BubbleAnimatorPtr->PlayAnimation("Pop");
    player.m_RigidbodyPtr->SetMode(Rigidbody::Mode::Dynamic);
}

void bb::PlayerBubbleState::Update(Player& player)
{
    if(not player.m_BubbleAnimatorPtr->IsPlaying())
        player.m_BubbleAnimatorPtr->PlayAnimation("Idle", true);
}

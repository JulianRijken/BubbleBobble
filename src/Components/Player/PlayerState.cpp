#include "PlayerState.h"

#include <Animator.h>
#include <GameTime.h>
#include <MessageQueue.h>
#include <SpriteRenderer.h>

#include <glm/geometric.hpp>

#include "Game.h"
#include "Player.h"

void bb::PlayerWalkingState::OnEnterState(Player& player)
{
    m_TimeWalking = 0.0f;
    player.m_AnimatorPtr->PlayAnimation(player.m_IdleAnimationName, true);
}

void bb::PlayerWalkingState::Update(Player& player)
{
    m_TimeWalking += GameTime::GetDeltaTimeF();

    if(not player.IsGrounded() and m_TimeWalking > MIN_TIME_WALKING_BEFORE_GROUND_CHECK)
    {
        player.SetState(player.m_JumpingState.get());
        return;
    }

    player.HandleFlip();

    if(std::abs(player.m_Rigidbody->Velocity().x) > 2)
    {
        if(player.m_AnimatorPtr->IsActiveAnimation(player.m_IdleAnimationName))
            player.m_AnimatorPtr->PlayAnimation(player.m_WalkAnimationName, true);
    }
    else
    {
        if(player.m_AnimatorPtr->IsActiveAnimation(player.m_WalkAnimationName))
            player.m_AnimatorPtr->PlayAnimation(player.m_IdleAnimationName, true);
    }
}

void bb::PlayerWalkingState::FixedUpdate(Player& player)
{
    glm::vec2 currentVelocity = player.m_Rigidbody->Velocity();
    player.m_Rigidbody->AddForce({ player.m_MovementInput * 8, currentVelocity.y },
                                 Rigidbody::ForceMode::VelocityChange);
}

void bb::PlayerWalkingState::OnJumpInput(Player& player)
{
    if(player.IsGrounded())
    {
        player.m_Rigidbody->AddForce({ player.m_Rigidbody->Velocity().x, 10 }, Rigidbody::ForceMode::VelocityChange);
        player.SetState(player.m_JumpingState.get());
        return;
    }
}

void bb::PlayerJumpingState::OnEnterState(Player& player)
{
    m_SlowFalHeight = player.m_Rigidbody->Positon().y;
    m_TimeInJump = 0.0f;
    m_LastHorizontalVelocity = player.m_Rigidbody->Velocity().x;
    player.m_Rigidbody->SetGravityScale(0.0f);

    if(player.m_Rigidbody->Velocity().y > 0)
    {
        m_Falling = false;
        player.m_AnimatorPtr->PlayAnimation(player.m_JumpAnimationName);
    }
    else
    {
        player.m_AnimatorPtr->PlayAnimation(player.m_FallingAnimationName, true);
    }
}

void bb::PlayerJumpingState::Update(Player& player)
{
    // Go out of state
    if(player.IsGrounded())
    {
        player.SetState(player.m_WalkingState.get());
        return;
    }

    player.HandleFlip();

    m_TimeInJump += GameTime::GetDeltaTimeF();

    // Tell player he is falling
    if(player.m_Rigidbody->Positon().y < m_SlowFalHeight)
        m_Falling = true;

    // Force falling animation when done with jump
    if(not player.m_AnimatorPtr->IsPlaying())
        player.m_AnimatorPtr->PlayAnimation(player.m_FallingAnimationName, true);
}

void bb::PlayerJumpingState::FixedUpdate(Player& player)
{

    if(m_Falling)
        player.m_Rigidbody->AddForce({ player.m_MovementInput, -6 }, Rigidbody::ForceMode::VelocityChange);
    else
        player.m_Rigidbody->AddForce({ 0.0f, -9.81f }, Rigidbody::ForceMode::Force);

    // if(player.m_MovementInput > 0 and m_LastVelocity < 0)
    //     m_LastVelocity = 0;
    // else if(player.m_MovementInput < 0 and m_LastVelocity > 0)
    //     m_LastVelocity = 0;

    // if(std::abs(m_LastVelocity) > 0.5f)
    // {
    //     const glm::vec2 currentVelocity = player.m_Rigidbody->Velocity();
    //     player.m_Rigidbody->AddForce({ m_LastVelocity, currentVelocity.y }, Rigidbody::ForceMode::VelocityChange);
    // }
    // else
    // {
    //     player.m_Rigidbody->AddForce({ player.m_MovementInput * 1, 0 }, Rigidbody::ForceMode::Impulse);
    //     player.m_Rigidbody->AddForce({ -player.m_Rigidbody->Velocity().x * 20, 0 }, Rigidbody::ForceMode::Force);
    // }


    // if(player.m_Rigidbody->Positon().y < m_SlowFalHeight)
    //     player.m_Rigidbody->AddForce({ 0, -player.m_Rigidbody->Velocity().y * 3 }, Rigidbody::ForceMode::Force);
}

void bb::PlayerJumpingState::OnExitState(Player& player) { player.m_Rigidbody->SetGravityScale(1.0f); }

void bb::PlayerAttackignState::OnEnterState(Player& player)
{
    player.m_AnimatorPtr->PlayAnimation("Attack", false);

    MessageQueue::Broadcast(MessageType::PlayerAttack);

    // TODO: Remove from attack
    player.AddScore();
}

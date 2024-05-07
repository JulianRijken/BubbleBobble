#include "PlayerState.h"

#include <Animator.h>
#include <Box2D/Box2D.h>
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
    player.m_Rigidbody->SetGravityScale(WALK_GRAVITY_SCALE);
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
    player.m_Rigidbody->AddForce({ player.m_MovementInput * 8, player.m_Rigidbody->Velocity().y },
                                 Rigidbody::ForceMode::VelocityChange);
}

void bb::PlayerWalkingState::OnJumpInput(Player& player)
{
    if(player.IsGrounded())
    {
        player.m_Rigidbody->AddForce({ player.m_Rigidbody->Velocity().x, PlayerJumpingState::JUMP_FORCE },
                                     Rigidbody::ForceMode::VelocityChange);
        player.SetState(player.m_JumpingState.get());
        return;
    }
}

void bb::PlayerWalkingState::OnAttackInput(Player& player) { player.SetState(player.m_AttackignState.get()); }

void bb::PlayerWalkingState::OnExitState(Player& player) { player.m_Rigidbody->SetGravityScale(1.0f); }

void bb::PlayerJumpingState::OnEnterState(Player& player)
{
    m_SlowFallHeight = player.m_Rigidbody->Positon().y;
    m_TimeInJump = 0.0f;
    player.m_Rigidbody->SetGravityScale(0.0f);

    m_Falling = true;
    m_HasManualControl = false;

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
    if(player.m_Rigidbody->Positon().y < m_SlowFallHeight)
        m_Falling = true;

    // Force falling animation when done with jump
    if(not player.m_AnimatorPtr->IsPlaying())
        player.m_AnimatorPtr->PlayAnimation(player.m_FallingAnimationName, true);
}

void bb::PlayerJumpingState::FixedUpdate(Player& player)
{
    glm::vec2 currentVelocity = player.m_Rigidbody->Velocity();

    if(m_Falling)
    {
        player.m_Rigidbody->AddForce({ player.m_MovementInput * MOVE_SPEED, -FALL_SPEED },
                                     Rigidbody::ForceMode::VelocityChange);
    }
    else
    {
        // Add gravity manually
        player.m_Rigidbody->AddForce({ 0.0f, GRAVITY_FOCE }, Rigidbody::ForceMode::Force);


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
            player.m_Rigidbody->AddForce({ player.m_MovementInput * MOVE_SPEED, currentVelocity.y },
                                         Rigidbody::ForceMode::VelocityChange);
        }
    }


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

void bb::PlayerJumpingState::OnAttackInput(Player& player) { player.SetState(player.m_AttackignState.get()); }

void bb::PlayerAttackignState::OnEnterState(Player& player)
{
    player.m_AnimatorPtr->PlayAnimation("Attack");

    MessageQueue::Broadcast(MessageType::PlayerAttack);

    // TODO: Remove from attack
    player.AddScore();
}

void bb::PlayerAttackignState::Update(Player& player)
{
    if(not player.m_AnimatorPtr->IsPlaying())
        player.SetState(player.m_WalkingState.get());
}

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

#include "CaptureBubble.h"
#include "Game.h"
#include "Player.h"
#include "Prefabs.h"

/////////////////////
/// WALKING STATE ///
/////////////////////
void bb::PlayerWalkingState::OnEnterState(Player& player)
{
    m_TimeWalking = 0.0f;
    player.m_BodyAnimatorPtr->Play(player.IDLE_ANIMATION_NAME, true);
    player.m_RigidbodyPtr->SetGravityScale(WALK_GRAVITY_SCALE);
}

void bb::PlayerWalkingState::Update(Player& player)
{
    m_TimeWalking += GameTime::GetDeltaTime<float>();

    if(not player.IsGrounded(player.m_RigidbodyPtr, player.m_ColliderPtr) and
       m_TimeWalking > MIN_TIME_WALKING_BEFORE_GROUND_CHECK)
    {
        player.SetMainState(player.m_JumpingState.get());
        return;
    }

    player.HandleFlip();

    if(not player.m_BodyAnimatorPtr->IsPlaying())
        player.m_BodyAnimatorPtr->Play(player.IDLE_ANIMATION_NAME, true);


    if(std::abs(player.m_RigidbodyPtr->GetVelocity().x) > 0.0f)
    {
        if(player.m_BodyAnimatorPtr->IsActiveAnimation(player.IDLE_ANIMATION_NAME))
            player.m_BodyAnimatorPtr->Play(player.WALK_ANIMATION_NAME, true);
    }
    else
    {
        if(player.m_BodyAnimatorPtr->IsActiveAnimation(player.WALK_ANIMATION_NAME))
            player.m_BodyAnimatorPtr->Play(player.IDLE_ANIMATION_NAME, true);
    }
}

void bb::PlayerWalkingState::FixedUpdate(Player& player)
{
    player.m_RigidbodyPtr->AddForce({ player.GetMoveInput().x * MOVE_SPEED, player.m_RigidbodyPtr->GetVelocity().y },
                                    Rigidbody::ForceMode::VelocityChange);
}

void bb::PlayerWalkingState::OnJumpInput(Player& player)
{
    if(player.IsGrounded(
           player.m_RigidbodyPtr, player.m_ColliderPtr, player.GROUND_CHECK_DISTANCE, player.GROUND_CHECK_LAYERS))
    {
        MessageQueue::Broadcast(MessageType::PlayerJump);
        player.m_RigidbodyPtr->AddForce({ player.m_RigidbodyPtr->GetVelocity().x, PlayerJumpingState::JUMP_FORCE },
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

    if(player.m_RigidbodyPtr->GetVelocity().y > 0)
    {
        m_Falling = false;
        player.m_BodyAnimatorPtr->Play(player.JUMP_ANIMATION_NAME);
    }
    else
    {
        player.m_BodyAnimatorPtr->Play(player.FALLING_ANIMATION_NAME, true);
    }
}

void bb::PlayerJumpingState::Update(Player& player)
{
    // Go out of state
    if(player.IsGrounded(
           player.m_RigidbodyPtr, player.m_ColliderPtr, player.GROUND_CHECK_DISTANCE, player.GROUND_CHECK_LAYERS))
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
        player.m_BodyAnimatorPtr->Play(player.FALLING_ANIMATION_NAME, true);
}

void bb::PlayerJumpingState::FixedUpdate(Player& player)
{
    const glm::vec2 currentVelocity = player.m_RigidbodyPtr->GetVelocity();

    if(m_Falling)
    {
        player.m_RigidbodyPtr->AddForce({ player.GetMoveInput().x * MOVE_SPEED, -FALL_SPEED },
                                        Rigidbody::ForceMode::VelocityChange);
    }
    else
    {
        // Add gravity manually
        player.m_RigidbodyPtr->AddForce({ 0.0f, GRAVITY_FOCE }, Rigidbody::ForceMode::Force);


        if(not m_HasManualControl)
        {
            if((currentVelocity.x > 0 and player.GetMoveInput().x < 0) or
               (currentVelocity.x < 0 and player.GetMoveInput().x > 0) or currentVelocity.x == 0)
            {
                m_HasManualControl = true;
            }
        }


        if(m_HasManualControl)
        {
            player.m_RigidbodyPtr->AddForce({ player.GetMoveInput().x * MOVE_SPEED, currentVelocity.y },
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

    player.m_BodyAnimatorPtr->Play(player.ATTACK_ANIMATION_NAME);
    MessageQueue::Broadcast(MessageType::PlayerAttack);

    glm::vec3 spawnPosition = player.GetTransform().GetWorldPosition();
    const int direction = player.m_BodySpriteRendererPtr->m_FlipX ? -1 : 1;

    const float bubbleSize{ 1.5f };
    float castDistance = player.m_ColliderPtr->GetSettings().size.x / 2 + bubbleSize;
    float firePower = FIRE_POWER;
    RayCastResult out;
    if(Physics::RayCast(
           player.GetTransform().GetWorldPosition(), { direction, 0 }, castDistance, out, layer::ALL_TILES))
    {
        spawnPosition.x = out.point.x;
        spawnPosition.y -= player.m_ColliderPtr->GetSettings().size.y / 2.0f;
        firePower = 0.0;
    }
    else
    {
        spawnPosition.x += castDistance * static_cast<float>(direction);
    }

    prefabs::SpawnCaptureBubble(spawnPosition, { direction * firePower, 0 });
}

void bb::PlayerAttackingState::Update(Player& player)
{
    if(not player.m_BodyAnimatorPtr->IsActiveAnimation(player.ATTACK_ANIMATION_NAME))
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
    player.m_BodyAnimatorPtr->Play(player.DEATH_ANIMATION_NAME);
    player.m_Health->Damage();
}


void bb::PlayerDeathState::OnExitState(Player& player)
{
    player.m_RigidbodyPtr->SetMode(Rigidbody::Mode::Dynamic);
    player.m_ColliderPtr->SetSensor(false);
}

void bb::PlayerDeathState::Update(Player& player)
{
    if(player.m_BodyAnimatorPtr->IsPlaying())
        return;

    if(not player.m_Health->IsDead())
        player.Respawn();
}

////////////////////
/// BUBBLE STATE ///
////////////////////
void bb::PlayerBubbleState::OnEnterState(Player& player)
{
    player.m_BubbleAnimatorPtr->Play("Spawn");
    player.m_BodyAnimatorPtr->Play("InBubble", true);
    player.m_RigidbodyPtr->SetMode(Rigidbody::Mode::Static);
}

void bb::PlayerBubbleState::OnExitState(Player& player)
{
    player.m_RigidbodyPtr->SetMode(Rigidbody::Mode::Dynamic);
}

void bb::PlayerBubbleState::Update(Player& player)
{
    if(not player.m_BubbleAnimatorPtr->IsPlaying() and player.m_BubbleAnimatorPtr->IsActiveAnimation("Spawn"))
        player.m_BubbleAnimatorPtr->Play("Idle", true);
}

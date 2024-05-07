#include "Player.h"

#include <fmt/core.h>
#include <GameTime.h>
#include <MessageQueue.h>
#include <Physics.h>

#include "Game.h"
#include "GameObject.h"

bb::Player::Player(GameObject* parentPtr, int playerIndex, Animator* animator, SpriteRenderer* spriteRenderer,
                   Rigidbody* rigidbody, BoxCollider* collider) :
    Component(parentPtr, "PlayerController"),
    m_PlayerIndex(playerIndex),
    m_AnimatorPtr(animator),
    m_Rigidbody(rigidbody),
    m_SpriteRenderer(spriteRenderer),
    m_Collider(collider)
{
    if(m_AnimatorPtr == nullptr)
        m_AnimatorPtr = parentPtr->GetComponent<Animator>();

    if(m_SpriteRenderer == nullptr)
        m_SpriteRenderer = parentPtr->GetComponent<SpriteRenderer>();

    if(m_Rigidbody == nullptr)
        m_Rigidbody = parentPtr->GetComponent<Rigidbody>();

    if(m_Collider == nullptr)
        m_Collider = parentPtr->GetComponent<BoxCollider>();

    assert(m_AnimatorPtr);
    assert(m_SpriteRenderer);
    assert(m_Rigidbody);

    Game::GetInstance().SetPlayer(playerIndex, this);
    m_WalkingState->OnEnterState(*this);
}

bb::Player::~Player() { Game::GetInstance().SetPlayer(m_PlayerIndex, nullptr); }


void bb::Player::Kill()
{
    MessageQueue::Broadcast(MessageType::PlayerDied);

    assert(m_AnimatorPtr);
    m_AnimatorPtr->PlayAnimation(m_DeathAnimationName);

    m_Lives--;

    m_OnDeathEvent.Invoke(m_Lives);
}


void bb::Player::AddScore()
{
    m_Score += 100;

    // Can be used by individual observers and just this player instance
    m_OnScoreChangeEvent.Invoke(m_Score);
}


// TODO: This is getting called directly from the input
//       make sure this is not happening more than once a frame!
void bb::Player::UpdateMoveInput(float input) { m_MovementInput = input; }

bool bb::Player::IsGrounded()
{
    // If the player is moving up he is for sure not grounded :)
    if(m_Rigidbody->Velocity().y > 0)
        return false;

    const b2Vec2& lowerBound = m_Rigidbody->GetBody()->GetFixtureList()[0].GetAABB(0).lowerBound;
    const float center = m_Rigidbody->Positon().x;

    const glm::vec2 halfSize = m_Collider->GetSettings().size / 2.0f;
    const float castHeight = lowerBound.y + halfSize.y;
    const float castDistance = GROUND_CHECK_DISTANCE + halfSize.y;
    constexpr glm::vec2 CAST_DIRECTIOn = { 0, -1 };

    if(Physics::RayCast({ center - (halfSize.x), castHeight }, CAST_DIRECTIOn, castDistance))
        return true;
    if(Physics::RayCast({ center, castHeight }, CAST_DIRECTIOn, castDistance))
        return true;
    if(Physics::RayCast({ center + (halfSize.x), castHeight }, CAST_DIRECTIOn, castDistance))
        return true;

    return false;
}

void bb::Player::SetState(PlayerState* nextState)
{
    m_ActiveState->OnExitState(*this);
    nextState->OnEnterState(*this);
    m_ActiveState = nextState;
}

void bb::Player::HandleFlip()
{
    // Not just setting it with the if as 0 should not change
    if(m_MovementInput < 0)
        m_SpriteRenderer->m_FlipX = true;
    else if(m_MovementInput > 0)
        m_SpriteRenderer->m_FlipX = false;
}

void bb::Player::OnMoveLeftInput(InputContext /*unused*/) { UpdateMoveInput(-1.0f); }

void bb::Player::OnMoveRightInput(InputContext /*unused*/) { UpdateMoveInput(1.0f); }

void bb::Player::OnMoveStickInput(InputContext context) { UpdateMoveInput(std::get<float>(context.value())); }

void bb::Player::OnJumpInput(InputContext /*unused*/) { m_ActiveState->OnJumpInput(*this); }

void bb::Player::OnAttackInput(InputContext /*unused*/) { m_ActiveState->OnAttackInput(*this); }

void bb::Player::Update()
{
    m_ActiveState->Update(*this);
    // Todo input should also check for up events
    m_MovementInput = 0;
}

void bb::Player::FixedUpdate() { m_ActiveState->FixedUpdate(*this); }

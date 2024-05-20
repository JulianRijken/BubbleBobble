#include "Player.h"

#include <fmt/core.h>
#include <GameTime.h>
#include <MessageQueue.h>
#include <Physics.h>

#include "AttackBubble.h"
#include "Game.h"
#include "GameObject.h"

bb::Player::Player(GameObject* parentPtr, int playerIndex) :
    Component(parentPtr, "PlayerController"),
    m_PlayerIndex(playerIndex),
    m_AnimatorPtr(parentPtr->GetComponent<Animator>()),
    m_Rigidbody(parentPtr->GetComponent<Rigidbody>()),
    m_SpriteRenderer(parentPtr->GetComponent<SpriteRenderer>()),
    m_Collider(parentPtr->GetComponent<BoxCollider>())
{
    Game::GetInstance().SetPlayer(playerIndex, this);
    m_WalkingState->OnEnterState(*this);
}

bb::Player::~Player() { Game::GetInstance().SetPlayer(m_PlayerIndex, nullptr); }


void bb::Player::AddScore()
{
    m_Score += 100;

    // Can be used by individual observers and just this player instance
    m_OnScoreChangeEvent.Invoke(m_Score);
}


void bb::Player::UpdateMoveInput(float input) { m_MovementInput = input; }

bool bb::Player::IsGrounded() const
{
    // If the player is moving up he is for sure not grounded :)
    if(m_Rigidbody->Velocity().y > 0)
        return false;

    const b2Vec2& lowerBound = m_Rigidbody->GetBody()->GetFixtureList()[0].GetAABB(0).lowerBound;
    const float center = m_Rigidbody->Position().x;

    const glm::vec2 halfSize = m_Collider->GetSettings().size / 2.0f;
    const float castHeight = lowerBound.y + halfSize.y;
    const float castDistance = GROUND_CHECK_DISTANCE + halfSize.y;
    constexpr glm::vec2 castDirection = { 0, -1 };

    if(Physics::RayCast({ center - (halfSize.x), castHeight }, castDirection, castDistance))
        return true;
    if(Physics::RayCast({ center, castHeight }, castDirection, castDistance))
        return true;
    if(Physics::RayCast({ center + (halfSize.x), castHeight }, castDirection, castDistance))
        return true;

    return false;
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
    if(m_MovementInput < 0)
        m_SpriteRenderer->m_FlipX = true;
    else if(m_MovementInput > 0)
        m_SpriteRenderer->m_FlipX = false;
}


// TODO: Pfff this sucks and should be fixed using the input system just like the controllers
void bb::Player::OnMoveLeftInput(const InputContext& context)
{
    if(context.state == ButtonState::Down)
        UpdateMoveInput(-1.0f);
    else if(m_MovementInput < 0)
        UpdateMoveInput(0.0f);
}

void bb::Player::OnMoveRightInput(const InputContext& context)
{
    if(context.state == ButtonState::Down)
        UpdateMoveInput(1.0f);
    else if(m_MovementInput > 0)
        UpdateMoveInput(0.0f);
}

void bb::Player::OnMoveStickInput(const InputContext& context) { UpdateMoveInput(std::get<float>(context.value.value())); }

void bb::Player::OnJumpInput(const InputContext& /*unused*/)
{
    m_ActiveMainState->OnJumpInput(*this);
    m_ActiveAttackState->OnJumpInput(*this);
}

void bb::Player::OnAttackInput(const InputContext& /*unused*/)
{
    m_ActiveMainState->OnAttackInput(*this);
    m_ActiveAttackState->OnAttackInput(*this);
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
    // TODO: Maybe avoid a dynamic cast
    if(dynamic_cast<AttackBubble*>(instigator))
        return;

    m_ActiveAttackState->OnPlayerDamage(*this);
    m_ActiveMainState->OnPlayerDamage(*this);
}

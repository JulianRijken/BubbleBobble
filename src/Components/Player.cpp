#include "Player.h"

#include <GameTime.h>

#include "Game.h"
#include "GameObject.h"
#include "MessageQueue.h"

bb::Player::Player(GameObject* parentPtr, int playerIndex, Animator* animator, SpriteRenderer* spriteRenderer,
                   Rigidbody* rigidbody) :
    Component(parentPtr, "PlayerController"),
    m_PlayerIndex(playerIndex),
    m_AnimatorPtr(animator),
    m_SpriteRenderer(spriteRenderer),
    m_Rigidbody(rigidbody)
{
    if(m_AnimatorPtr == nullptr)
        m_AnimatorPtr = parentPtr->GetComponent<Animator>();

    if(m_SpriteRenderer == nullptr)
        m_SpriteRenderer = parentPtr->GetComponent<SpriteRenderer>();

    if(m_Rigidbody == nullptr)
        m_Rigidbody = parentPtr->GetComponent<Rigidbody>();

    assert(m_AnimatorPtr);
    assert(m_SpriteRenderer);

    m_AnimatorPtr->PlayAnimation("Idle", true);

    Game::GetInstance().SetPlayer(playerIndex, this);
}

bb::Player::~Player() { Game::GetInstance().SetPlayer(m_PlayerIndex, nullptr); }


void bb::Player::Kill()
{
    if(m_IsDead)
        return;

    assert(m_AnimatorPtr);
    m_AnimatorPtr->PlayAnimation(m_DeathAnimationName);

    m_Lives--;

    m_OnDeathEvent.Invoke(m_Lives);

    if(m_Lives == 0)
        m_IsDead = true;
}

void bb::Player::Attack()
{
    if(not m_IsDead)
        m_AnimatorPtr->PlayAnimation("Attack",false);

    // TODO: Remove from attack
    AddScore();
}

void bb::Player::AddScore()
{
    m_Score += 100;

    // Can be used by individual observers and just this player instance
    m_OnScoreChangeEvent.Invoke(m_Score);

    // Global event for whole game and all players (also non-blocking)
    // ALso does not care who listens and listeners don't care who sends
    MessageQueue::Broadcast(Message{ MessageType::PlayerScoreChanged, { m_Score } });
}

// TODO: This is getting called directly from the input
//       make sure this is not happening more than once a frame!
void bb::Player::Move(float input)
{
    if(m_IsDead)
        return;

    m_MovementInput = input;

    if(input > 0)
        m_SpriteRenderer->m_FlipX = false;
    else
    if(input < 0)
        m_SpriteRenderer->m_FlipX = true;
}

// TODO: Find a way to make the input context optional like the timer in afterburner
void bb::Player::OnTestLivesInput(InputContext /*unused*/) { Kill(); }

void bb::Player::OnMoveLeftInput(InputContext) { Move(-1.0f); }

void bb::Player::OnMoveRightInput(InputContext /*unused*/)
{
    Move(1.0f);
}

void bb::Player::OnMoveStickInput(InputContext context)
{
    assert(context.has_value());

    if(context.has_value())
        Move(std::get<float>(context.value()));
}

void bb::Player::OnAttackInput(InputContext /*unused*/)
{
    Attack();
}

void bb::Player::Update()
{
    if(m_IsDead)
        return;

    if(not m_AnimatorPtr->IsPlaying())   
        m_AnimatorPtr->PlayAnimation(m_IdleAnimationName);
}

void bb::Player::FixedUpdate()
{
    // TODO Make sure add force multiplies by my fixed time step
    m_Rigidbody->AddForce({ m_MovementInput * 5, 0 }, Rigidbody::ForceMode::Impulse);
    m_Rigidbody->AddForce({ -m_Rigidbody->Velocity().x * 40, 0 }, Rigidbody::ForceMode::Force);

    if(m_Rigidbody->Positon().y < -40)
        m_Rigidbody->SetPosition({ 0, 30 });

    // Todo input should also check for up events
    m_MovementInput = 0;
}

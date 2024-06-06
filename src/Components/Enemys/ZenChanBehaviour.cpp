#include "ZenChanBehaviour.h"

#include <GameTime.h>
#include <Physics.h>
#include <Player.h>

#include "Game.h"
#include "GameObject.h"
#include "ZenChan.h"

bb::ZenChanBehaviour::ZenChanBehaviour(GameObject* parentPtr, ZenChan* target) :
    Component(parentPtr, "ZenChanBehaviour"),
    m_Target(target)
{
}

void bb::ZenChanBehaviour::HandleTurning()
{
    if(m_TimeSinceLastTurn < MIN_TIME_BETWEEN_TURN)
        return;

    const glm::vec2 from = m_Target->GetRigidbody()->Position();
    const glm::vec2 direction = { static_cast<float>(m_WalkingDirection), 0 };
    const float distance = m_Target->GetBoxCollider()->GetSettings().size.x / 2.0f + 0.1f;

    if(Physics::RayCast(from, direction, distance, layer::ALL_TILES))
    {
        m_WalkingDirection *= -1;
        m_TimeSinceLastTurn = 0.0f;
    }
}

void bb::ZenChanBehaviour::FixedUpdate()
{
    HandleTurning();
    m_Target->OnMoveInput({ m_WalkingDirection, 0 });
}

void bb::ZenChanBehaviour::Update()
{
    Player* player1 = Game::GetInstance().GetPlayer(0);
    Player* player2 = Game::GetInstance().GetPlayer(0);

    glm::vec3 targetPosition{};
    const glm::vec3 currentPositon{ GetTransform().GetWorldPosition() };

    if(player1 != nullptr and player2 != nullptr)
    {
        if(glm::distance(player1->GetTransform().GetWorldPosition(), currentPositon) <
           glm::distance(player2->GetTransform().GetWorldPosition(), currentPositon))
        {
            targetPosition = player1->GetTransform().GetWorldPosition();
        }
        else
        {
            targetPosition = player2->GetTransform().GetWorldPosition();
        }
    }
    else if(player1 != nullptr)
    {
        targetPosition = player1->GetTransform().GetWorldPosition();
    }

    else if(player2 != nullptr)
    {
        targetPosition = player2->GetTransform().GetWorldPosition();
    }

    m_TargetDirection = targetPosition - currentPositon;
    m_TimeSinceLastTurn += GameTime::GetDeltaTime<float>();
}

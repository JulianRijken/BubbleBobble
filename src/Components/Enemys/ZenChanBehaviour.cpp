#include "ZenChanBehaviour.h"

#include <GameTime.h>
#include <Physics.h>
#include <Player.h>

#include "Game.h"
#include "GameObject.h"
#include "MathExtensions.h"
#include "ZenChan.h"

bb::ZenChanBehaviour::ZenChanBehaviour(GameObject* parentPtr, ZenChan* target) :
    Component(parentPtr, "ZenChanBehaviour"),
    m_Target(target),
    m_WalkingDirection(math::RandomValue() > 0.5 ? 1 : -1)
{
}

void bb::ZenChanBehaviour::HandleTurning()
{
    if(m_Target->GetState() == ZenChan::State::Walking)

        if(m_TimeSinceLastTurn < MIN_TIME_BETWEEN_WALL_TURN)
            return;

    const glm::vec2 from = m_Target->GetRigidbody()->Position();
    const glm::vec2 direction = { static_cast<float>(m_WalkingDirection), 0 };
    const float distance = m_Target->GetBoxCollider()->GetSettings().size.x / 2.0f + 0.1f;

    if(Physics::RayCast(from, direction, distance, layer::ALL_TILES | layer::ENEMY))
    {
        m_WalkingDirection *= -1;
        m_TimeSinceLastTurn = 0.0f;
    }
}


void bb::ZenChanBehaviour::Update()
{
    m_TimeSinceLastJump += GameTime::GetDeltaTime<float>();
    m_TimeSinceLastFacePlayer += GameTime::GetDeltaTime<float>();


    if(m_Target->GetState() == ZenChan::State::Walking)
        HandleTurning();

    m_Target->OnMoveInput({ m_WalkingDirection, 0 });


    const glm::vec2 halfSize = m_Target->GetBoxCollider()->GetSettings().size * 0.5f;
    const glm::vec2 center = m_Target->GetRigidbody()->Position();
    const glm::vec2 from = { center.x, center.y + halfSize.y };
    const glm::vec2 castDirection = { 0, 1 };

    const bool hasPlatformAbove = Physics::RayCast(from, castDirection, bb::ZenChan::CLIMB_HEIGHT, layer::ALL_TILES);
    const bool walkingSameDirection = ((m_TargetDirection.x < 0) == (m_WalkingDirection < 0));
    const bool isUnderPlayer = m_TargetDirection.y > 1;
    const bool waitedLongEnough = m_TimeSinceLastJump > m_JumpInterval;


    if(isUnderPlayer and hasPlatformAbove and walkingSameDirection and waitedLongEnough)
    {
        m_TimeSinceLastJump = 0;
        m_JumpInterval = jul::math::RandomRange(JUMP_INTERVAL.x, JUMP_INTERVAL.y);
        m_Target->OnJumpInput();
    }

    if(m_TimeSinceLastFacePlayer > m_FacePlayerInterval and m_TargetDirection.length() > 5)
    {
        if(not walkingSameDirection)
        {
            m_WalkingDirection *= -1;
            m_TimeSinceLastFacePlayer = 0;
            m_FacePlayerInterval = jul::math::RandomRange(FACE_TARGET_INTERVAL.x, FACE_TARGET_INTERVAL.y);
            return;
        }
    }


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

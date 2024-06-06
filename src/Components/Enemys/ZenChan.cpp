#include "ZenChan.h"

#include <Animator.h>
#include <BoxCollider.h>
#include <fmt/core.h>
#include <GameObject.h>
#include <GameTime.h>
#include <Physics.h>
#include <Rigidbody.h>

#include "IDamageable.h"
#include "Player.h"
#include "Prefabs.h"


bb::ZenChan::ZenChan(GameObject* parent) :
    Component(parent),
    m_Animator(parent->GetComponent<Animator>()),
    m_Rigidbody(parent->GetComponent<Rigidbody>()),
    m_SpriteRenderer(parent->GetComponent<SpriteRenderer>()),
    m_Collider(parent->GetComponent<BoxCollider>())
{
    m_Rigidbody->SetGravityScale(0.0f);
    m_Animator->Play("zenchan_normal", true);
}

void bb::ZenChan::FixedUpdate()
{
    HandleTurning();

    bool grounded = IsGrounded();

    if(grounded)
        m_Rigidbody->AddForce({ static_cast<float>(m_WalkingDirection) * MOVE_SPEED, 0 },
                              Rigidbody::ForceMode::VelocityChange);
    else
        m_Rigidbody->AddForce({ 0, -FALL_SPEED }, Rigidbody::ForceMode::VelocityChange);
}

bool bb::ZenChan::IsGrounded() const
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

    if(Physics::RayCast({ center - (halfSize.x), castHeight }, castDirection, castDistance, layer::ALL_TILES))
        return true;
    if(Physics::RayCast({ center, castHeight }, castDirection, castDistance, layer::ALL_TILES))
        return true;
    if(Physics::RayCast({ center + (halfSize.x), castHeight }, castDirection, castDistance, layer::ALL_TILES))
        return true;

    return false;
}

void bb::ZenChan::HandleTurning()
{
    if(m_TimeSinceLastTurn < MIN_TIME_BETWEEN_TURN)
        return;

    const glm::vec2 from = m_Rigidbody->Position();
    const glm::vec2 direction = { static_cast<float>(m_WalkingDirection), 0 };
    const float distance = m_Collider->GetSettings().size.x / 2.0f + 0.1f;

    if(Physics::RayCast(from, direction, distance, layer::ALL_TILES))
    {
        m_WalkingDirection *= -1;
        m_TimeSinceLastTurn = 0.0f;
    }
}

void bb::ZenChan::OnCollisionBegin(const Collision& collision)
{
    // Allows ZenChan to damage player
    const auto* collider = static_cast<BoxCollider*>(collision.otherFixture->GetUserData());
    if(auto* damageable = collider->GetGameObject()->GetComponent<IDamageable>())
        damageable->OnDamage(this);
}

jul::Transform* bb::ZenChan::GetCaptureTransform() { return &GetTransform(); }

void bb::ZenChan::SpawnDeadVersion() { prefabs::SpawnZenChanDead(GetTransform().GetWorldPosition()); }

void bb::ZenChan::Jump() {}

void bb::ZenChan::Update()
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
    m_SpriteRenderer->m_FlipX = m_WalkingDirection > 0;
}

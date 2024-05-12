#include "ZenChan.h"

#include <Animator.h>
#include <GameObject.h>
#include <GameTime.h>
#include <Physics.h>
#include <Rigidbody.h>
#include <BoxCollider.h>

#include <fmt/core.h>

#include "IDamagable.h"

bb::ZenChan::ZenChan(GameObject* parent) :
    Component(parent),
    m_Animator(parent->GetComponent<Animator>()),
    m_Rigidbody(parent->GetComponent<Rigidbody>()),
    m_SpriteRenderer(parent->GetComponent<SpriteRenderer>()),
    m_Collider(parent->GetComponent<BoxCollider>())
{
    m_Rigidbody->SetGravityScale(0.0f);
    m_Animator->PlayAnimation("zenchan_normal", true);
}

void bb::ZenChan::FixedUpdate()
{
    if(IsGrounded())
        m_Rigidbody->AddForce({ m_WalkingDirection * MOVE_SPEED, 0 }, Rigidbody::ForceMode::VelocityChange);
    else
        m_Rigidbody->AddForce({ 0, -FALL_SPEED }, Rigidbody::ForceMode::VelocityChange);

    HandleTurning();

    // {
    //     glm::vec2 from = m_Rigidbody->Positon();
    //     from.x += m_Collider->GetSettings().size.x / 2.0f + 1.0f;

    //     const float distance = m_Collider->GetSettings().size.y / 2.0f + 0.1f;
    //     RayCastResult result;
    //     if(Physics::RayCast(from, { 0, -1 }, distance, result))
    //         fmt::println("Jump");
    // }
}

bool bb::ZenChan::IsGrounded()
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

void bb::ZenChan::HandleTurning()
{
    if(m_TimeSinceLastTrun < MIN_TIME_BETWEEN_TURN)
        return;

    const glm::vec2 from = m_Rigidbody->Position();
    const glm::vec2 direction = { static_cast<float>(m_WalkingDirection), 0 };
    const float distance = m_Collider->GetSettings().size.x / 2.0f + 0.1f;
    RayCastResult result;
    if(Physics::RayCast(from, direction, distance, result))
    {
        // TODO: Welp I could have implemented layers or tags
        //       But noooo Julian has to get component every physics tick!@
        if(result.hitCollider->GetGameObject()->GetComponent<IDamagable>())
            return;

        m_WalkingDirection *= -1;
        m_TimeSinceLastTrun = 0.0f;
    }
}

void bb::ZenChan::OnCollisionBegin(Collision collision)
{
    const auto* collider = static_cast<BoxCollider*>(collision.otherFixture->GetUserData());

    if(auto* damageable = collider->GetGameObject()->GetComponent<IDamagable>())
        damageable->OnDamage();
}

void bb::ZenChan::Update()
{
    m_TimeSinceLastTrun += GameTime::GetDeltaTimeF();
    m_SpriteRenderer->m_FlipX = m_WalkingDirection > 0;
}

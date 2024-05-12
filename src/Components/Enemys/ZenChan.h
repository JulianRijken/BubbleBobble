#pragma once
#include <Component.h>
#include <ICollisionListener.h>

using namespace jul;

namespace jul
{
    class Animator;
    class Rigidbody;
    class SpriteRenderer;
    class BoxCollider;
}

namespace bb
{
    class ZenChan final : public Component, public ICollisionListener
    {
    public:
        static constexpr float MOVE_SPEED{ 8.0f };
        static constexpr float GROUND_CHECK_DISTANCE{ 0.1f };
        static constexpr float FALL_SPEED{ 5.0f };
        static constexpr float MIN_TIME_BETWEEN_TURN{ 1.0f };

        ZenChan(GameObject* parent);

    private:
        void FixedUpdate() override;
        void Update() override;
        bool IsGrounded();

        void HandleTurning();
        void OnCollisionBegin(Collision collision) override;

        Animator* m_Animator{ nullptr };
        Rigidbody* m_Rigidbody{ nullptr };
        SpriteRenderer* m_SpriteRenderer{ nullptr };
        BoxCollider* m_Collider{ nullptr };

        int m_WalkingDirection{ 1 };
        float m_TimeSinceLastTrun{};
    };
}  // namespace bb

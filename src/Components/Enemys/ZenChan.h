#pragma once
#include <Component.h>
#include <IBubbleable.h>
#include <ICollisionListener.h>

#include "IDamageable.h"


namespace jul
{
    class Animator;
    class Rigidbody;
    class SpriteRenderer;
    class BoxCollider;
}

namespace bb
{
    using namespace jul;

    class ZenChan final : public Component, public ICollisionListener, public IBubbleable
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
        bool IsGrounded() const;

        void HandleTurning();
        void OnCollisionBegin(const Collision& collision) override;

        Transform* GetCaptureTransform() override;
        void OnCapture() override;
        void OnRelease() override;

        Animator* m_Animator{ nullptr };
        Rigidbody* m_Rigidbody{ nullptr };
        SpriteRenderer* m_SpriteRenderer{ nullptr };
        BoxCollider* m_Collider{ nullptr };

        int m_WalkingDirection{ 1 };
        float m_TimeSinceLastTurn{};
    };
}  // namespace bb



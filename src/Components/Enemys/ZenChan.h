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
        [[nodiscard]] bool IsGrounded() const;

        [[nodiscard]] std::string GetSpriteName() override { return "Enemys"; }

        [[nodiscard]] std::string GetSpriteAnimationName() override { return "zenchan_bubble"; }

        [[nodiscard]] Transform* GetCaptureTransform() override;

        void FixedUpdate() override;
        void Update() override;
        void OnCollisionBegin(const Collision& collision) override;
        void SpawnDeadVersion() override;

        void HandleTurning();
        void Jump();

        Animator* m_Animator{ nullptr };
        Rigidbody* m_Rigidbody{ nullptr };
        SpriteRenderer* m_SpriteRenderer{ nullptr };
        BoxCollider* m_Collider{ nullptr };

        glm::vec2 m_TargetDirection{};
        int m_WalkingDirection{ 1 };
        float m_TimeSinceLastTurn{};
    };
}  // namespace bb



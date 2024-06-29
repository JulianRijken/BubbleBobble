#pragma once
#include <IBubbleable.h>
#include <ICollisionListener.h>

#include "Enemy.h"

namespace jul
{
    class Animator;
    class Rigidbody;
    class SpriteRenderer;
    class BoxCollider;
}

namespace bb
{
    class ZenChan final : public Enemy, public jul::ICollisionListener, public IBubbleable
    {
    public:
        static constexpr float MOVE_SPEED{ 8.0f };
        static constexpr float CHARGE_SPEED_MULTIPLIER{ 1.8f };
        static constexpr float FALL_SPEED{ 5.0f };
        static constexpr float CLIMB_SPEED{ 10.0f };
        static constexpr float CLIMB_HEIGHT{ 5.0f };
        static constexpr float CLIMB_MOMENTEM_KEEP{ 0.2f };

        enum class State
        {
            Walking,
            Falling,
            Jumping
        };

        ZenChan(jul::GameObject* parent);

        [[nodiscard]] jul::Rigidbody* GetRigidbody() { return m_Rigidbody; }

        [[nodiscard]] jul::BoxCollider* GetBoxCollider() { return m_BoxCollider; }

        [[nodiscard]] State GetState() { return m_State; }

        void OnJumpInput() override;
        void OnAttackInput() override;

    private:
        [[nodiscard]] std::string GetSpriteName() override { return "Enemys"; }

        [[nodiscard]] std::string GetSpriteAnimationName() override { return "zenchan_bubble"; }

        [[nodiscard]] jul::Transform* GetCaptureTransform() override;

        void OnRelease() override;

        void FixedUpdate() override;
        void Update() override;

        void Jump();
        void OnCollisionBegin(const jul::Collision& collision) override;
        void SpawnDeadVersion() override;

        bool m_Charging{ false };
        State m_State{};
        jul::Animator* m_Animator{ nullptr };
        jul::Rigidbody* m_Rigidbody{ nullptr };
        jul::SpriteRenderer* m_SpriteRenderer{ nullptr };
        jul::BoxCollider* m_BoxCollider{ nullptr };
    };
}  // namespace bb



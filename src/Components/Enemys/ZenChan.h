#pragma once
#include <Character.h>
#include <IBubbleable.h>
#include <ICollisionListener.h>

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

    class ZenChan final : public Character, public ICollisionListener, public IBubbleable
    {
    public:
        static constexpr float MOVE_SPEED{ 8.0f };
        static constexpr float GROUND_CHECK_DISTANCE{ 0.1f };
        static constexpr float FALL_SPEED{ 5.0f };

        ZenChan(GameObject* parent);

        [[nodiscard]] Rigidbody* GetRigidbody() { return m_Rigidbody; }

        [[nodiscard]] BoxCollider* GetBoxCollider() { return m_BoxCollider; }

    private:
        [[nodiscard]] std::string GetSpriteName() override { return "Enemys"; }

        [[nodiscard]] std::string GetSpriteAnimationName() override { return "zenchan_bubble"; }

        [[nodiscard]] Transform* GetCaptureTransform() override;


        void OnJumpInput() override;
        void OnAttackInput() override;

        void FixedUpdate() override;
        void Update() override;
        void OnCollisionBegin(const Collision& collision) override;
        void SpawnDeadVersion() override;


        Animator* m_Animator{ nullptr };
        Rigidbody* m_Rigidbody{ nullptr };
        SpriteRenderer* m_SpriteRenderer{ nullptr };
        BoxCollider* m_BoxCollider{ nullptr };
    };
}  // namespace bb



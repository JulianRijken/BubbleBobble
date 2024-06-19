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
    class Maita final : public Enemy, public ICollisionListener, public IBubbleable
    {
    public:
        static constexpr float MOVE_SPEED{ 6.0f };
        static constexpr float FALL_SPEED{ 8.0f };
        static constexpr float CLIMB_SPEED{ 12.0f };
        static constexpr float CLIMB_HEIGHT{ 5.0f };
        static constexpr float CLIMB_MOMENTEM_KEEP{ 0.5f };

        static constexpr float BOULDER_THROW_POWER{ 15 };

        static constexpr float BOULDER_THROW_INTERVAL{ 4.0 };

        enum class State
        {
            Walking,
            Falling,
            Jumping
        };

        Maita(GameObject* parent);

        [[nodiscard]] Rigidbody* GetRigidbody() { return m_Rigidbody; }

        [[nodiscard]] BoxCollider* GetBoxCollider() { return m_BoxCollider; }

        [[nodiscard]] State GetState() { return m_State; }

        void OnJumpInput() override;
        void OnAttackInput() override;

    private:
        [[nodiscard]] std::string GetSpriteName() override { return "Enemys"; }

        [[nodiscard]] std::string GetSpriteAnimationName() override { return "maita_bubble"; }

        [[nodiscard]] Transform* GetCaptureTransform() override;

        void FixedUpdate() override;
        void Update() override;

        void Jump();
        void OnCollisionBegin(const Collision& collision) override;
        void SpawnDeadVersion() override;


        float m_TimeSinceLastBoulderThrow{};
        State m_State{};

        Animator* m_Animator{ nullptr };
        Rigidbody* m_Rigidbody{ nullptr };
        SpriteRenderer* m_SpriteRenderer{ nullptr };
        BoxCollider* m_BoxCollider{ nullptr };
    };
}  // namespace bb



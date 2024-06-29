#ifndef MAITA_H
#define MAITA_H

#include <ICollisionListener.h>

#include "Enemy.h"
#include "IBubbleable.h"

namespace jul
{
    class SpriteRenderer;
    class Animator;
}  // namespace jul

namespace bb
{
    class Maita final : public Enemy, public jul::ICollisionListener, public IBubbleable
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

        Maita(jul::GameObject* parent);

        Maita(Maita&&) = delete;
        Maita(const Maita&) = delete;
        Maita& operator=(Maita&&) = delete;
        Maita& operator=(const Maita&) = delete;

        [[nodiscard]] jul::Rigidbody* GetRigidbody() { return m_Rigidbody; }

        [[nodiscard]] jul::BoxCollider* GetBoxCollider() { return m_BoxCollider; }

        [[nodiscard]] State GetState() { return m_State; }

        void OnJumpInput() override;
        void OnAttackInput() override;

    private:
        [[nodiscard]] std::string GetSpriteName() override { return "Enemys"; }

        [[nodiscard]] std::string GetSpriteAnimationName() override { return "maita_bubble"; }

        [[nodiscard]] jul::Transform* GetCaptureTransform() override;

        void FixedUpdate() override;
        void Update() override;

        void Jump();
        void OnCollisionBegin(const jul::Collision& collision) override;
        void SpawnDeadVersion() override;

        float m_TimeSinceLastBoulderThrow{};
        State m_State{};

        jul::Animator* m_Animator{ nullptr };
        jul::Rigidbody* m_Rigidbody{ nullptr };
        jul::SpriteRenderer* m_SpriteRenderer{ nullptr };
        jul::BoxCollider* m_BoxCollider{ nullptr };
    };
}  // namespace bb
#endif  // MAITA_H;

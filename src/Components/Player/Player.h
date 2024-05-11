#pragma once

#include <Animator.h>
#include <BoxCollider.h>
#include <Command.h>
#include <Component.h>
#include <Event.h>
#include <Rigidbody.h>

#include <memory>

#include "PlayerState.h"


using namespace jul;

namespace bb
{
    class Player : public Component
    {
        friend class PlayerWalkingState;
        friend class PlayerJumpingState;
        friend class PlayerAttackignState;

    public:
        static constexpr float GROUND_CHECK_DISTANCE{ 0.5f };

        Player(GameObject* parentPtr, int playerIndex, Animator* animator = nullptr,
               SpriteRenderer* spriteRenderer = nullptr, Rigidbody* rigidbody = nullptr,
               BoxCollider* collider = nullptr);

        ~Player() override;

        Player(const Player&) = delete;
        Player(Player&&) noexcept = delete;
        Player& operator=(const Player&) = delete;
        Player& operator=(Player&&) noexcept = delete;

        [[nodiscard]] Event<int>& GetOnDeath() { return m_OnDeathEvent; }

        [[nodiscard]] Event<int>& GetOnScoreChange() { return m_OnScoreChangeEvent; }

        void Kill();
        void AddScore();

        void OnMoveLeftInput(InputContext context);
        void OnMoveRightInput(InputContext context);
        void OnMoveStickInput(InputContext context);

        void OnJumpInput(InputContext context);
        void OnAttackInput(InputContext context);

    private:
        void Update() override;
        void FixedUpdate() override;
        void UpdateMoveInput(float input);

        bool IsGrounded();

        void SetMovementState(PlayerState* nextState);
        void SetAttackState(PlayerState* nextState);

        void HandleFlip();

        std::unique_ptr<PlayerWalkingState> m_WalkingState{ std::make_unique<PlayerWalkingState>() };
        std::unique_ptr<PlayerJumpingState> m_JumpingState{ std::make_unique<PlayerJumpingState>() };
        std::unique_ptr<PlayerAttackignState> m_AttackignState{ std::make_unique<PlayerAttackignState>() };

        PlayerState* m_ActiveMovementState{ m_WalkingState.get() };
        PlayerState* m_ActiveAttackState{ nullptr };

        Event<int> m_OnDeathEvent{};
        Event<int> m_OnScoreChangeEvent{};

        int m_Lives{ 3 };
        int m_Score{0};
        int m_PlayerIndex{ 0 };

        float m_MovementInput{};

        std::string m_DeathAnimationName{ "Death" };
        std::string m_IdleAnimationName{"Idle"};
        std::string m_WalkAnimationName{ "Walk" };
        std::string m_JumpAnimationName{ "Jump" };
        std::string m_FallingAnimationName{ "Falling" };
        std::string m_AttackAnimationName{ "Attack" };

        Animator* m_AnimatorPtr{ nullptr };
        Rigidbody* m_Rigidbody{ nullptr };
        SpriteRenderer* m_SpriteRenderer{ nullptr };
        BoxCollider* m_Collider{ nullptr };
    };
}

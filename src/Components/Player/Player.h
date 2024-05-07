#pragma once

#include <Animator.h>
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
        Player(GameObject* parentPtr, int playerIndex, Animator* animator = nullptr,
               SpriteRenderer* spriteRenderer = nullptr, Rigidbody* rigidbody = nullptr);

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

        void SetState(PlayerState* nextState);

        void HandleFlip();

        std::unique_ptr<PlayerWalkingState> m_WalkingState{ std::make_unique<PlayerWalkingState>() };
        std::unique_ptr<PlayerJumpingState> m_JumpingState{ std::make_unique<PlayerJumpingState>() };
        std::unique_ptr<PlayerAttackignState> m_AttackignState{ std::make_unique<PlayerAttackignState>() };

        PlayerState* m_ActiveState{ m_WalkingState.get() };

        Event<int> m_OnDeathEvent{};
        Event<int> m_OnScoreChangeEvent{};

        int m_Lives{ 3 };
        int m_Score{0};
        int m_PlayerIndex{ 0 };

        float m_MovementInput{};

        float m_ColiderWidth{ 1.8f };
        float m_GroundDistanceCheck{ 0.4f };

        std::string m_DeathAnimationName{ "Death" };
        std::string m_IdleAnimationName{"Idle"};
        std::string m_WalkAnimationName{ "Walk" };
        std::string m_JumpAnimationName{ "Jump" };
        std::string m_FallingAnimationName{ "Falling" };

        Animator* m_AnimatorPtr{ nullptr };
        Rigidbody* m_Rigidbody{ nullptr };
        SpriteRenderer* m_SpriteRenderer{ nullptr };
    };
}

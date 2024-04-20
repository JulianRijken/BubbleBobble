#pragma once
#include "Command.h"
#include "Component.h"

#include "Animator.h"
#include "Event.h"


using namespace jul;

namespace bb
{
    class Player : public Component
    {
    public:
        Player(GameObject* parentPtr, int playerIndex, Animator* animator = nullptr,
               SpriteRenderer* spriteRenderer = nullptr);

        ~Player() override;

        Player(const Player&) = delete;
        Player(Player&&) noexcept = delete;
        Player& operator=(const Player&) = delete;
        Player& operator=(Player&&) noexcept = delete;

        [[nodiscard]] Event<int>& GetOnDeath() { return m_OnDeathEvent; }
        [[nodiscard]] Event<int>& GetOnScoreChange() {return m_OnScoreChangeEvent; }


        void Kill();
        void Attack();
        void AddScore();

        void Jump();
        void Move(float input);


        void OnTestLivesInput(InputContext context);
        void OnMoveLeftInput(InputContext context);
        void OnMoveRightInput(InputContext context);
        void OnMoveStickInput(InputContext context);
        // void OnJumpInput(InputContext context);
        void OnAttackInput(InputContext context);

    private:

        void Update() override;

        Event<int> m_OnDeathEvent{};
        Event<int> m_OnScoreChangeEvent{};

        int m_Lives{3};
        int m_Score{0};
        int m_PlayerIndex{ 0 };
        bool m_IsDead{false};

        std::string m_DeathAnimationName{"Death"};
        std::string m_IdleAnimationName{"Idle"};
        std::string m_WalkAnimationName{"Walk"};
        Animator* m_AnimatorPtr{nullptr};
        SpriteRenderer* m_SpriteRenderer{nullptr};
    };
}

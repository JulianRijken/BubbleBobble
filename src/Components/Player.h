#pragma once
#include <unordered_set>

#include "Animator.h"
#include "Command.h"
#include "Component.h"
#include "Event.h"
#include "ICollisionListener.h"
#include "Rigidbody.h"


using namespace jul;

namespace bb
{
    class Player : public Component, public ICollisionListener
    {
    public:
        Player(GameObject* parentPtr, int playerIndex, Animator* animator = nullptr,
               SpriteRenderer* spriteRenderer = nullptr, Rigidbody* rigidbody = nullptr);

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
        void FixedUpdate() override;

        Event<int> m_OnDeathEvent{};
        Event<int> m_OnScoreChangeEvent{};

        int m_Lives{3};
        int m_Score{0};
        int m_PlayerIndex{ 0 };
        bool m_IsDead{false};
        bool m_MovingTroughPlatform{ false };


        float m_MovementInput{};

        std::unordered_set<b2Contact*> activeContacts{};
        std::unordered_set<b2Contact*> clearedContacts{};


        std::string m_DeathAnimationName{ "Death" };
        std::string m_IdleAnimationName{"Idle"};
        std::string m_WalkAnimationName{"Walk"};


        Animator* m_AnimatorPtr{ nullptr };
        SpriteRenderer* m_SpriteRenderer{nullptr};
        Rigidbody* m_Rigidbody{ nullptr };


        void OnCollisionPreSolve(b2Contact* contact) override;
        void OnCollisionBegin(b2Contact*) override;
        void OnCollisionEnd(b2Contact*) override;
        void OnCollisionPostSolve(b2Contact*) override;
    };
}

#pragma once

#include <Component.h>
#include <Event.h>
#include <ICollisionListener.h>
#include <IDamageable.h>
#include <InputContext.h>

#include <glm/vec3.hpp>
#include <memory>

#include "Game.h"
#include "PlayerState.h"

namespace jul
{
    class BoxCollider;
    class SpriteRenderer;
    class Rigidbody;
    class Animator;
}  // namespace jul

namespace bb
{
    using namespace jul;

    class Player final : public Component, public IDamageable, public ICollisionListener
    {
        friend class PlayerWalkingState;
        friend class PlayerJumpingState;
        friend class PlayerAttackingState;
        friend class PlayerDeathState;
        friend class PlayerBubbleState;

    public:
        static constexpr float GROUND_CHECK_DISTANCE{ 0.5f };

        Player(GameObject* parentPtr, int playerIndex, SpriteRenderer* bodySpriteRenderer,
               SpriteRenderer* bubbleSpriteRenderer, Animator* bodyAnimator, Animator* bubbleAnimator);

        ~Player() override;

        Player(const Player&) = delete;
        Player(Player&&) noexcept = delete;
        Player& operator=(const Player&) = delete;
        Player& operator=(Player&&) noexcept = delete;

        [[nodiscard]] Event<int>& GetOnDeathEvent() { return m_OnDeathEvent; }

        [[nodiscard]] Event<int>& GetOnScoreChangeEvent() { return m_OnScoreChangeEvent; }

        [[nodiscard]] int GetLives() const { return m_Lives; }

        void AddScore();
        void BubbleToPosition(const glm::vec3& position, double duration);

        void OnMoveLeftInput(const InputContext& context);
        void OnMoveRightInput(const InputContext& context);
        void OnMoveStickInput(const InputContext& context);

        void OnJumpInput(const InputContext& context);
        void OnAttackInput(const InputContext& context);

    private:
        [[nodiscard]] bool IsGrounded() const;

        void Update() override;
        void FixedUpdate() override;
        void UpdateMoveInput(float input);

        void SetMainState(PlayerState* nextState);
        void SetAttackState(PlayerState* nextState);

        void HandleFlip();

        void ObtainPickup(PickupType pickupType);

        void OnDamage(jul::Component* instigator) override;
        void OnCollisionPreSolve(const Collision& collision, const b2Manifold*) override;

        std::unique_ptr<PlayerWalkingState> m_WalkingState{ std::make_unique<PlayerWalkingState>() };
        std::unique_ptr<PlayerJumpingState> m_JumpingState{ std::make_unique<PlayerJumpingState>() };
        std::unique_ptr<PlayerAttackingState> m_AttackingState{ std::make_unique<PlayerAttackingState>() };
        std::unique_ptr<PlayerNullState> m_NullState{ std::make_unique<PlayerNullState>() };
        std::unique_ptr<PlayerDeathState> m_DeathState{ std::make_unique<PlayerDeathState>() };
        std::unique_ptr<PlayerBubbleState> m_BubbleState{ std::make_unique<PlayerBubbleState>() };

        PlayerState* m_ActiveMainState{ m_BubbleState.get() };
        PlayerState* m_ActiveAttackState{ m_NullState.get() };

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

        Animator* m_BodyAnimatorPtr{ nullptr };
        Animator* m_BubbleAnimatorPtr{ nullptr };
        Rigidbody* m_RigidbodyPtr{ nullptr };
        SpriteRenderer* m_BodySpriteRendererPtr{ nullptr };
        SpriteRenderer* m_BubbleSpriteRendererPtr{ nullptr };
        BoxCollider* m_ColliderPtr{ nullptr };
    };
}

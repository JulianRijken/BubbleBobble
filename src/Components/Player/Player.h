#pragma once

#include <Character.h>
#include <Event.h>
#include <ICollisionListener.h>
#include <IDamageable.h>
#include <InputContext.h>

#include "Health.h"
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
    class Player final : public Character, public IDamageable, public ICollisionListener
    {
        friend class PlayerWalkingState;
        friend class PlayerJumpingState;
        friend class PlayerAttackingState;
        friend class PlayerDeathState;
        friend class PlayerBubbleState;

    public:
        Player(GameObject* parentPtr, int playerIndex, SpriteRenderer* bodySpriteRenderer,
               SpriteRenderer* bubbleSpriteRenderer, Animator* bodyAnimator, Animator* bubbleAnimator);

        ~Player() override;

        Player(const Player&) = delete;
        Player(Player&&) noexcept = delete;
        Player& operator=(const Player&) = delete;
        Player& operator=(Player&&) noexcept = delete;

        [[nodiscard]] bool IsDead() const;

        void BubbleToPosition(const glm::vec3& position, double duration);
        void OnJumpInput() override;
        void OnAttackInput() override;

    private:

        void Update() override;
        void FixedUpdate() override;
        void UpdateMoveInput(float input);

        void SetMainState(PlayerState* nextState);
        void SetAttackState(PlayerState* nextState);

        void HandleFlip();
        void Respawn();

        void ObtainPickup(PickupType pickupType);
        void OnDamage(jul::Component* instigator) override;
        void OnCollisionPreSolve(const Collision& collision, const b2Manifold* /*unused*/) override;

        const std::string DEATH_ANIMATION_NAME{ "Death" };
        const std::string IDLE_ANIMATION_NAME{ "Idle" };
        const std::string WALK_ANIMATION_NAME{ "Walk" };
        const std::string JUMP_ANIMATION_NAME{ "Jump" };
        const std::string FALLING_ANIMATION_NAME{ "Falling" };
        const std::string ATTACK_ANIMATION_NAME{ "Attack" };

        static constexpr float GROUND_CHECK_DISTANCE{ 0.5f };
        static constexpr uint16_t GROUND_CHECK_LAYERS{ layer::ALL_TILES | layer::CAPTURE_BUBBLE };

        std::unique_ptr<PlayerWalkingState> m_WalkingState{ std::make_unique<PlayerWalkingState>() };
        std::unique_ptr<PlayerJumpingState> m_JumpingState{ std::make_unique<PlayerJumpingState>() };
        std::unique_ptr<PlayerAttackingState> m_AttackingState{ std::make_unique<PlayerAttackingState>() };
        std::unique_ptr<PlayerNullState> m_NullState{ std::make_unique<PlayerNullState>() };
        std::unique_ptr<PlayerDeathState> m_DeathState{ std::make_unique<PlayerDeathState>() };
        std::unique_ptr<PlayerBubbleState> m_BubbleState{ std::make_unique<PlayerBubbleState>() };

        PlayerState* m_ActiveMainState{ m_BubbleState.get() };
        PlayerState* m_ActiveAttackState{ m_NullState.get() };

        int m_Score{0};
        int m_PlayerIndex{ 0 };
        bool m_Respawning{ false };

        Animator* m_BodyAnimatorPtr{ nullptr };
        Animator* m_BubbleAnimatorPtr{ nullptr };
        Rigidbody* m_RigidbodyPtr{ nullptr };
        SpriteRenderer* m_BodySpriteRendererPtr{ nullptr };
        SpriteRenderer* m_BubbleSpriteRendererPtr{ nullptr };
        BoxCollider* m_ColliderPtr{ nullptr };
        Health* m_Health{ nullptr };
    };
}

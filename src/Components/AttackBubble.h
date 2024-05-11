#pragma once

#include <Animator.h>
#include <Component.h>
#include <ICollisionListener.h>

#include <glm/vec3.hpp>
#include <unordered_set>

using namespace jul;

namespace jul
{
    class Animator;
}

namespace bb
{
    class AttackBubble final : public Component, public ICollisionListener
    {
    public:
        static constexpr float BUBBLE_FLOAT_UP_FORCE = 3.0f;
        static constexpr float BUBBLE_ATTRACTION_FORCE = 0.3f;
        static constexpr float DURATION_BEFORE_FLOATING = 1.0f;
        static constexpr float FLOAT_HEIGHT = 3;

        AttackBubble(GameObject* parent, glm::vec3 fireVelocity = {});
        ~AttackBubble() override;

    private:
        void OnCollisionBegin(Collision /*unused*/) override;
        void Update() override;
        void FixedUpdate() override;

        glm::vec2 m_BubbleCenter{};

        Animator* m_Animator{ nullptr };
        Rigidbody* m_Rigidbody{ nullptr };

        float m_FloatingDuration{};

        static inline std::unordered_set<AttackBubble*> g_Bubbles{};
    };

}  // namespace bb

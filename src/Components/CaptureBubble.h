#pragma once

#include <Animator.h>
#include <Component.h>
#include <ICollisionListener.h>

#include <glm/vec3.hpp>
#include <unordered_set>

#include "IBubbleable.h"


namespace jul
{
    class Animator;
}

namespace bb
{
    using namespace jul;

    class CaptureBubble final : public Component, public ICollisionListener
    {
    public:
        static constexpr float BUBBLE_FLOAT_UP_FORCE = 3.0f;
        static constexpr float BUBBLE_ATTRACTION_FORCE = 0.3f;
        static constexpr float DURATION_BEFORE_FLOATING = 1.0f;
        static constexpr float FLOAT_HEIGHT = 3;

        static constexpr float POP_DURATION_STRENGTH = 1;
        static constexpr float POP_VELOCITY_STRENGTH = 1;
        static constexpr float POP_THRESHOLD = 15;

        CaptureBubble(GameObject* parent, glm::vec3 fireVelocity = {});
        ~CaptureBubble() override;

    private:
        void StartPop();

        void Capture(IBubbleable* target);
        void ReleaseCapturedTarget();

        void OnCollisionPreSolve(const Collision& collision, const b2Manifold*) override;

        void Update() override;
        void FixedUpdate() override;

        glm::vec2 m_BubbleCenter{};

        IBubbleable* m_CapturedTarget{ nullptr };
        SpriteRenderer* m_BubbleSpriteRenderer{ nullptr };
        Animator* m_BubbleAnimator{ nullptr };

        SpriteRenderer* m_CapturedSpriteRenderer{ nullptr };
        Animator* m_CapturedAnimator{ nullptr };

        Rigidbody* m_Rigidbody{ nullptr };

        float m_FloatingDuration{};
        bool m_GettingPopped{ false };

        static inline std::unordered_set<CaptureBubble*> g_Bubbles{};
    };

}  // namespace bb

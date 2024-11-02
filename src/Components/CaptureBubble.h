#ifndef CAPTURE_BUBBLE_H
#define CAPTURE_BUBBLE_H

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
    class CaptureBubble final : public jul::Component, public jul::ICollisionListener
    {
    public:
        static constexpr float BUBBLE_FLOAT_UP_FORCE = 3.0f;
        static constexpr float BUBBLE_ATTRACTION_FORCE = 0.3f;
        static constexpr float DURATION_BEFORE_FLOATING = 1.0f;
        static constexpr float FLOAT_HEIGHT = 3;

        static constexpr float POP_DURATION_STRENGTH = 1;
        static constexpr float POP_VELOCITY_STRENGTH = 1;
        static constexpr float POP_THRESHOLD = 15;

        static constexpr float MAX_TIME_TILL_POP = 8.0f;

        CaptureBubble(jul::GameObject* parent, glm::vec3 fireVelocity = {});
        ~CaptureBubble() override;

    private:
        void StartPop();

        void Capture(IBubbleable* target);

        void KillCapturedTarget();
        void ReleaseCapturedTarget();

        void OnCollisionPreSolve(const jul::Collision& collision, const b2Manifold* /*unused*/) override;

        void Update() override;
        void FixedUpdate() override;

        glm::vec2 m_BubbleCenter{};

        IBubbleable* m_CapturedTarget{ nullptr };
        jul::SpriteRenderer* m_BubbleSpriteRenderer{ nullptr };
        jul::Animator* m_BubbleAnimator{ nullptr };

        jul::SpriteRenderer* m_CapturedSpriteRenderer{ nullptr };
        jul::Animator* m_CapturedAnimator{ nullptr };

        jul::Rigidbody* m_Rigidbody{ nullptr };

        float m_FloatingDuration{};
        bool m_GettingPopped{ false };
        bool m_EnemyPoppingOut{ false };

        static inline std::unordered_set<CaptureBubble*> g_Bubbles{};
    };

}  // namespace bb
#endif  // CAPTURE_BUBBLE_H

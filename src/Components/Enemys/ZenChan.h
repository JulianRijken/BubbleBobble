#pragma once
#include "Component.h"

using namespace jul;

namespace jul
{
    class Animator;
    class Rigidbody;
    class SpriteRenderer;
}

namespace bb
{
    class ZenChan : public Component
    {
    public:
        static constexpr float MOVE_SPEED{ 4.0f };

        ZenChan(GameObject* parent);

    private:
        void FixedUpdate() override;
        void Update() override;

        Animator* m_Animator;
        Rigidbody* m_Rigidbody;
        SpriteRenderer* m_SpriteRenderer;

        int m_WalkingDirection{ 1 };
    };
}  // namespace bb

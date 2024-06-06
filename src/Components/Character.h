#pragma once

#include <Component.h>

#include <glm/vec2.hpp>

namespace bb
{
    class Character : public jul::Component
    {
    public:
        Character(jul::GameObject* parentPtr);

        virtual void OnMoveInput(glm::vec2 direction) = 0;
        virtual void OnJumpInput() = 0;
        virtual void OnAttackInput() = 0;
    };
}

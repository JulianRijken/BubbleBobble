#pragma once

#include <Component.h>

#include <glm/vec2.hpp>

#include "Game.h"

namespace bb
{
    class Character : public jul::Component
    {
    public:
        explicit Character(jul::GameObject* parentPtr = nullptr, const std::string& name = "Character");
        ~Character() override = default;

        Character(const Character&) = delete;
        Character(Character&&) noexcept = delete;
        Character& operator=(const Character&) = delete;
        Character& operator=(Character&&) noexcept = delete;

        [[nodiscard]] glm::vec2 GetMoveInput() const { return m_MovementInput; }

        virtual void OnJumpInput() = 0;
        virtual void OnAttackInput() = 0;
        void OnMoveInput(glm::vec2 input);

        [[nodiscard]] bool IsGrounded(jul::Rigidbody* rigidbodyPtr, jul::BoxCollider* boxColliderPtr,
                                      float distance = 0.2f, uint16_t checkLayers = layer::ALL_TILES) const;

    private:
        glm::vec2 m_MovementInput{};
    };
}

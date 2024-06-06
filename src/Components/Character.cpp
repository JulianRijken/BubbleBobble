#include "Character.h"

bb::Character::Character(jul::GameObject* parentPtr, const std::string& name) :
    jul::Component(parentPtr, name)
{
}

void bb::Character::OnMoveInput(glm::vec2 input) { m_MovementInput = input; }

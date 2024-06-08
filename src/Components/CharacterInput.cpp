#include "CharacterInput.h"

#include <GameObject.h>
#include <Input.h>

#include "Character.h"
#include "Game.h"

bb::CharacterInput::CharacterInput(GameObject* parentPtr, bool canUseKeyboard, int controllerIndex,
                                   Character* characterPtr) :
    Component(parentPtr, "CharacterInput"),
    m_CharacterPtr(characterPtr)
{
    if(m_CharacterPtr == nullptr)
        m_CharacterPtr = parentPtr->GetComponent<Character>();


    Input::Bind(InputBind::MoveLeft, controllerIndex, canUseKeyboard, this, &CharacterInput::OnMoveLeftInput);
    Input::Bind(InputBind::MoveRight, controllerIndex, canUseKeyboard, this, &CharacterInput::OnMoveRightInput);
    Input::Bind(InputBind::MoveStick, controllerIndex, canUseKeyboard, this, &CharacterInput::OnMoveStickInput);
    Input::Bind(InputBind::Attack, controllerIndex, canUseKeyboard, this, &CharacterInput::OnAttackInput);
    Input::Bind(InputBind::Jump, controllerIndex, canUseKeyboard, this, &CharacterInput::OnJumpInput);
}

// TODO: Pfff this sucks and should be fixed using the input system just like the controllers
void bb::CharacterInput::OnMoveLeftInput(const InputContext& context)
{
    if(context.state == ButtonState::Down)
        m_CharacterPtr->OnMoveInput({ -1, 0 });
    else if(m_CharacterPtr->GetMoveInput().x < 0)
        m_CharacterPtr->OnMoveInput({ 0, 0 });
}

void bb::CharacterInput::OnMoveRightInput(const InputContext& context)
{
    if(context.state == ButtonState::Down)
        m_CharacterPtr->OnMoveInput({ 1, 0 });
    else if(m_CharacterPtr->GetMoveInput().x > 0)
        m_CharacterPtr->OnMoveInput({ 0, 0 });
}

void bb::CharacterInput::OnMoveStickInput(const InputContext& context)
{
    m_CharacterPtr->OnMoveInput({ std::get<float>(context.value.value()), 0 });
}

void bb::CharacterInput::OnJumpInput(const InputContext& context)
{
    if(context.state != ButtonState::Down)
        return;

    m_CharacterPtr->OnJumpInput();
}

void bb::CharacterInput::OnAttackInput(const InputContext& context)
{
    if(context.state != ButtonState::Down)
        return;

    m_CharacterPtr->OnAttackInput();
}

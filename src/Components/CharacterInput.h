#pragma once
#include <Component.h>
#include <InputContext.h>

namespace bb
{
    class Character;

    class CharacterInput final : public jul::Component, public jul::IEventListener
    {
    public:
        CharacterInput(jul::GameObject* parentPtr, bool canUseKeyboard, int controllerIndex,
                       Character* characterPtr = nullptr);

        void OnMoveLeftInput(const jul::InputContext& context);
        void OnMoveRightInput(const jul::InputContext& context);
        void OnMoveStickInput(const jul::InputContext& context);
        void OnJumpInput(const jul::InputContext& context);
        void OnAttackInput(const jul::InputContext& context);

    private:
        Character* m_CharacterPtr{ nullptr };
    };

}  // namespace bb

#pragma once
#include <Component.h>
#include <InputContext.h>

namespace bb
{

    class Character;
    using namespace jul;

    class CharacterInput : public Component, public IEventListener
    {
    public:
        CharacterInput(GameObject* parentPtr, bool canUseKeyboard, int controllerIndex,
                       Character* characterPtr = nullptr);

        void OnMoveLeftInput(const InputContext& context);
        void OnMoveRightInput(const InputContext& context);
        void OnMoveStickInput(const InputContext& context);
        void OnJumpInput(const InputContext& context);
        void OnAttackInput(const InputContext& context);

    private:
        Character* m_CharacterPtr{ nullptr };
    };

}  // namespace bb

#pragma once

#include <AutoMove.h>
#include <Component.h>

#include <vector>

#include "InputContext.h"


namespace bb
{

    using namespace jul;

    class MainMenu : public Component
    {
    public:
        static inline constexpr int BUBBLE_COUNT_IN_SELECT_SCREEN = 5;

        MainMenu(GameObject* parentPtr, Transform* logoTransformPtr, GameObject* intoTextPtr, GameObject* introScreen,
                 GameObject* selectScreen);

        void OnSelectButton(const InputContext& context);

    private:
        void OnLogoLand();

        bool m_IntroFinished{ false };
        Transform* m_LogoTransformPtr;
        GameObject* m_IntoTextPtr;
        GameObject* m_IntroScreen;
        GameObject* m_SelectScreen;
        std::vector<AutoMove*> m_Bubbles;
    };
}  // namespace bb

#pragma once

#include <AutoMove.h>
#include <Component.h>

#include <vector>

#include "Event.h"
#include "InputContext.h"

namespace bb
{

    using namespace jul;

    class MainMenu : public Component, public EventListener
    {
    public:
        static inline constexpr int BUBBLE_COUNT_IN_SELECT_SCREEN = 5;

        MainMenu(GameObject* parentPtr, Transform* logoTransformPtr, GameObject* intoTextPtr, GameObject* introScreen,
                 GameObject* selectScreen, Transform* selectBubble, std::vector<Transform*>&& options);

        void OnSelectButton(const InputContext& context);
        void OnUpButton(const InputContext& context);
        void OnDownButton(const InputContext& context);


    private:
        void Update() override;

        void OnLogoLand();
        void ShowSelectScreen();
        void UpdateSelectBubblePosition();

        glm::vec3 m_SelectBubbleTargetLocation;

        int m_SelectedItem{};
        bool m_IntroFinished{ false };
        bool m_OpenedSelectScreen{ false };
        Transform* m_LogoTransformPtr;
        Transform* m_SelectBubble;
        GameObject* m_IntoTextPtr;
        GameObject* m_IntroScreen;
        GameObject* m_SelectScreen;
        std::vector<Transform*> m_Options;
        std::vector<AutoMove*> m_Bubbles;
    };
}  // namespace bb

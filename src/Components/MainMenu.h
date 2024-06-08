#pragma once

#include <AutoMove.h>
#include <Component.h>

#include <vector>

#include "Event.h"
#include "InputContext.h"

namespace bb
{

    using namespace jul;

    class MainMenu final : public Component, public IEventListener
    {
    public:
        static inline constexpr int BUBBLE_COUNT_IN_SELECT_SCREEN = 5;
        static inline constexpr int SELECT_BUBBLE_TEXT_OFFSET = -2;
        static inline constexpr double SELECT_BUBBLE_LERP_DURATION = 0.2;

        MainMenu(GameObject* parentPtr, Transform* logoTransformPtr, GameObject* intoTextPtr, GameObject* introScreen,
                 GameObject* selectScreen, Transform* selectBubble, std::vector<Transform*>&& options);

        ~MainMenu() override = default;
        MainMenu(const MainMenu&) = delete;
        MainMenu(MainMenu&&) noexcept = delete;
        MainMenu& operator=(const MainMenu&) = delete;
        MainMenu& operator=(MainMenu&&) noexcept = delete;

        void OnSelectButton(const InputContext& context);
        void OnUpButton(const InputContext& context);
        void OnDownButton(const InputContext& context);

    private:
        void Update() override;

        void LoadSelectedMode() const;
        void OnLogoLand();
        void TransitionInToGame();
        void ShowSelectScreen();
        void UpdateSelectBubblePosition();

        glm::vec3 m_SelectBubbleTargetLocation;

        int m_SelectedItem{};
        bool m_SelectedMode{ false };
        bool m_IntroFinished{ false };
        bool m_OpenedSelectScreen{ false };

        Transform* m_LogoTransformPtr{ nullptr };
        Transform* m_SelectBubble{ nullptr };
        GameObject* m_IntoTextPtr{ nullptr };
        GameObject* m_IntroScreen{ nullptr };
        GameObject* m_SelectScreen{ nullptr };
        std::vector<Transform*> m_Options{};
        std::vector<AutoMove*> m_Bubbles{};
    };
}  // namespace bb

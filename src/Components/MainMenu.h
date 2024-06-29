#pragma once

#include <Component.h>

#include <glm/vec3.hpp>
#include <vector>

#include "Event.h"
#include "InputContext.h"


namespace bb
{
    class AutoMove;

    class MainMenu final : public jul::Component, public jul::IEventListener
    {
    public:
        static inline constexpr int BUBBLE_COUNT_IN_SELECT_SCREEN = 5;
        static inline constexpr int SELECT_BUBBLE_TEXT_OFFSET = -2;
        static inline constexpr double SELECT_BUBBLE_LERP_DURATION = 0.2;

        MainMenu(jul::GameObject* parentPtr, jul::Transform* logoTransformPtr, jul::GameObject* intoTextPtr,
                 jul::GameObject* introScreen, jul::GameObject* selectScreen, jul::Transform* selectBubble,
                 std::vector<jul::Transform*>&& options);

        ~MainMenu() override = default;
        MainMenu(const MainMenu&) = delete;
        MainMenu(MainMenu&&) noexcept = delete;
        MainMenu& operator=(const MainMenu&) = delete;
        MainMenu& operator=(MainMenu&&) noexcept = delete;

        void OnSelectButton(const jul::InputContext& context);
        void OnUpButton(const jul::InputContext& context);
        void OnDownButton(const jul::InputContext& context);

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

        jul::Transform* m_LogoTransformPtr{ nullptr };
        jul::Transform* m_SelectBubble{ nullptr };
        jul::GameObject* m_IntoTextPtr{ nullptr };
        jul::GameObject* m_IntroScreen{ nullptr };
        jul::GameObject* m_SelectScreen{ nullptr };
        std::vector<jul::Transform*> m_Options{};
        std::vector<AutoMove*> m_Bubbles{};
    };
}  // namespace bb

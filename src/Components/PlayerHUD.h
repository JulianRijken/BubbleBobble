#pragma once
#include <TextRenderer.h>

#include "Component.h"
#include "Player.h"

namespace bb
{
    class PlayerHUD : public Component
    {
    public:
        inline static constexpr int SCORE_CHANGE_PER_SECOND = 1000;

        PlayerHUD(GameObject* parentPtr, Player* player, TextRenderer* scoreText, TextRenderer* livesText,
                  const SDL_Color& color);

    private:
        void Update() override;
        void UpdateScore(int score);
        void UpdateLives(int lives);

        int m_TargetScore{};

        // Yes a float for score :(
        // I need half score for the interpolation to work
        // (Frame independently)
        float m_CurrentScore{};

        TextRenderer* m_ScoreText{ nullptr };
        TextRenderer* m_LivesText{ nullptr };
    };
}

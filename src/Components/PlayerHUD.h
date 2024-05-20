#pragma once
#include <TextRenderer.h>

#include "Component.h"
#include "Player.h"

namespace bb
{
    class PlayerHUD : public Component, public EventListener
    {
    public:
        inline static constexpr int SCORE_CHANGE_PER_SECOND = 5000;

        PlayerHUD(GameObject* parentPtr, Player* player, TextRenderer* scoreText, TextRenderer* livesText,
                  const SDL_Color& color);

    private:
        void Update() override;
        void UpdateScore(int score);
        void UpdateLives(int lives);

        int m_Score{};
        float m_VisualScore{};

        TextRenderer* m_ScoreText{ nullptr };
        TextRenderer* m_LivesText{ nullptr };

        Player* m_Player{ nullptr };
    };
}

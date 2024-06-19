#pragma once
#include <Component.h>
#include <Event.h>
#include <MessageQueue.h>
#include <TextRenderer.h>

namespace jul
{
    class TextRenderer;
    class GameObject;
}  // namespace jul

namespace bb
{
    class PlayerHUD final : public Component, public IEventListener
    {
    public:
        inline static constexpr int SCORE_CHANGE_PER_SECOND = 400;

        PlayerHUD(GameObject* parentPtr, TextRenderer* scoreText, TextRenderer* livesText, int playerIndex);
        ~PlayerHUD();

    private:
        void Update() override;
        void UpdateScore(int score);
        void UpdateLives(int lives);
        void OnLevelTransitionChange(bool inTransition, int levelIndex);

        void OnCharacterLivesChanged(const Message& message);
        void OnCharacterScoreChanged(const Message& message);

        int m_PlayerIndex{};

        // Score as a double (yes a double)
        // This is because we need half score for frame independent animation!
        double m_VisualScore{};
        int m_TargetScore{};

        TextRenderer* m_ScoreText{ nullptr };
        TextRenderer* m_LivesText{ nullptr };
    };
}

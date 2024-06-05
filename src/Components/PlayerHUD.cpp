#include "PlayerHUD.h"

#include <fmt/format.h>

#include "Game.h"
#include "GameObject.h"
#include "GameTime.h"
#include "MathExtensions.h"

bb::PlayerHUD::PlayerHUD(GameObject* parentPtr, Player* player, TextRenderer* scoreText, TextRenderer* livesText,
                         const SDL_Color& color) :
    Component(parentPtr, "ScoreDisplay"),
    m_ScoreText(scoreText),
    m_LivesText(livesText),
    m_PlayerPtr(player)
{
    assert(m_ScoreText);
    assert(m_LivesText);

    m_ScoreText->SetColor(color);
    m_LivesText->SetColor(color);

    Game::GetInstance().GetLevelTransitionChangeEvent().AddListener(this, &PlayerHUD::OnLevelTransitionChange);

    if(m_PlayerPtr != nullptr)
    {
        m_PlayerPtr->GetOnScoreChangeEvent().AddListener(this, &PlayerHUD::UpdateScore);
        m_PlayerPtr->GetOnDeathEvent().AddListener(this, &PlayerHUD::UpdateLives);

        UpdateScore(1000);
        UpdateLives(m_PlayerPtr->GetLives());
    }
    else
    {
        UpdateScore(0);
        UpdateLives(0);
    }

    // Force jump visual at start
    m_VisualScore = m_Score;
}


void bb::PlayerHUD::Update()
{
    m_VisualScore = jul::math::MoveTowards(
        m_VisualScore, static_cast<double>(m_Score), SCORE_CHANGE_PER_SECOND * jul::GameTime::GetDeltaTime());

    m_ScoreText->SetText(std::to_string(static_cast<int>(std::ceil(m_VisualScore))));
}

void bb::PlayerHUD::UpdateScore(int score) { m_Score = score; }

void bb::PlayerHUD::UpdateLives(int lives)
{
    // Lives are actaully displayed in hex values haha
    m_LivesText->SetText(fmt::format("{:X}", lives));
}

void bb::PlayerHUD::OnLevelTransitionChange(bool inTransition, int levelIndex)
{
    GetGameObject()->SetActive(not inTransition and levelIndex > 0);
}

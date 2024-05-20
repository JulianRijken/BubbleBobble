#include "PlayerHUD.h"

#include <fmt/format.h>

#include "GameObject.h"
#include "GameTime.h"
#include "MathExtensions.h"

bb::PlayerHUD::PlayerHUD(GameObject* parentPtr, Player* player, TextRenderer* scoreText, TextRenderer* livesText,
                         const SDL_Color& color) :
    Component(parentPtr, "ScoreDisplay"),
    m_ScoreText(scoreText),
    m_LivesText(livesText),
    m_Player(player)
{
    assert(m_ScoreText);
    assert(m_LivesText);

    m_ScoreText->SetColor(color);
    m_LivesText->SetColor(color);

    m_Player->GetOnScoreChangeEvent().AddListener(this, &PlayerHUD::UpdateScore);
    m_Player->GetOnDeathEvent().AddListener(this, &PlayerHUD::UpdateLives);

    UpdateScore(1000);
    UpdateLives(3);  // TODO lives hardcoded should ideally be changed
}


void bb::PlayerHUD::Update()
{
    m_VisualScore = jul::math::MoveTowards(
        m_VisualScore, static_cast<float>(m_Score), SCORE_CHANGE_PER_SECOND * jul::GameTime::GetDeltaTimeF());

    m_ScoreText->SetText(std::to_string(static_cast<int>(std::ceil(m_VisualScore))));
}

void bb::PlayerHUD::UpdateScore(int score) { m_Score = score; }

void bb::PlayerHUD::UpdateLives(int lives)
{
    // Lives are actaully displayed in hex values haha
    m_LivesText->SetText(fmt::format("{:X}", lives));
}

#include "PlayerHUD.h"

#include <fmt/format.h>

#include "GameObject.h"
#include "GameTime.h"
#include "MathExtensions.h"


bb::PlayerHUD::PlayerHUD(GameObject* parentPtr, Player* player, TextRenderer* scoreText, TextRenderer* livesText,
                         const SDL_Color& color) :
    Component(parentPtr, "ScoreDisplay"),
    m_ScoreText(scoreText),
    m_LivesText(livesText)
{
    assert(m_ScoreText);
    assert(m_LivesText);

    m_ScoreText->SetColor(color);
    m_LivesText->SetColor(color);

    player->GetOnScoreChange().AddListener(this, &PlayerHUD::UpdateScore);
    player->GetOnDeath().AddListener(this, &PlayerHUD::UpdateLives);


    UpdateScore(1000);
    UpdateLives(3);  // TODO lives hardcoded should ideally be changed
}

void bb::PlayerHUD::Update()
{
    m_CurrentScore = jul::math::MoveTowards(
        m_CurrentScore, static_cast<float>(m_TargetScore), SCORE_CHANGE_PER_SECOND * jul::GameTime::GetDeltaTimeF());

    m_ScoreText->SetText(std::to_string(static_cast<int>(std::ceil(m_CurrentScore))));
}

void bb::PlayerHUD::UpdateScore(int score) { m_TargetScore = score; }

void bb::PlayerHUD::UpdateLives(int lives)
{
    // Lives are actaully displayed in hex values haha
    m_LivesText->SetText(fmt::format("{:X}", lives));
}

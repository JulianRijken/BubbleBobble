#include "PlayerHUD.h"

#include <fmt/format.h>

#include "Game.h"
#include "GameObject.h"
#include "GameTime.h"
#include "MathExtensions.h"

bb::PlayerHUD::PlayerHUD(GameObject* parentPtr, TextRenderer* scoreText, TextRenderer* livesText, int playerIndex) :
    Component(parentPtr, "ScoreDisplay"),
    m_PlayerIndex(playerIndex),
    m_ScoreText(scoreText),
    m_LivesText(livesText)
{
    assert(m_ScoreText);
    assert(m_LivesText);

    m_ScoreText->SetColor(playerIndex == 0 ? Game::PLAYER_1_COLOR : Game::PLAYER_2_COLOR);
    m_LivesText->SetColor(playerIndex == 0 ? Game::PLAYER_1_COLOR : Game::PLAYER_2_COLOR);

    Game::GetInstance().GetLevelTransitionChangeEvent().AddListener(this, &PlayerHUD::OnLevelTransitionChange);

    MessageQueue::AddListener(MessageType::CharacterLivesChange, this, &PlayerHUD::OnCharacterLivesChanged);
    MessageQueue::AddListener(MessageType::CharacterScoreChange, this, &PlayerHUD::OnCharacterScoreChanged);

    UpdateScore(0);
    UpdateLives(0);

    // Force jump visual at start
    m_VisualScore = m_TargetScore;
}

bb::PlayerHUD::~PlayerHUD() { MessageQueue::RemoveListenerInstance(this); }


void bb::PlayerHUD::Update()
{
    m_VisualScore = jul::math::MoveTowards(
        m_VisualScore, static_cast<double>(m_TargetScore), SCORE_CHANGE_PER_SECOND * jul::GameTime::GetDeltaTime());

    m_ScoreText->SetText(std::to_string(static_cast<int>(std::ceil(m_VisualScore))));
}

void bb::PlayerHUD::UpdateScore(int score) { m_TargetScore = score; }

void bb::PlayerHUD::UpdateLives(int lives)
{
    // Lives are actaully displayed in hex values haha
    m_LivesText->SetText(fmt::format("{:X}", lives));
}

void bb::PlayerHUD::OnLevelTransitionChange(bool inTransition, int levelIndex)
{
    GetGameObject()->SetActive(not inTransition and levelIndex > 0);
}

void bb::PlayerHUD::OnCharacterLivesChanged(const Message& message)
{
    if(std::any_cast<int>(message.args[0]) == m_PlayerIndex)
        UpdateLives(std::any_cast<int>(message.args[1]));
}

void bb::PlayerHUD::OnCharacterScoreChanged(const Message& message)
{
    if(std::any_cast<int>(message.args[0]) == m_PlayerIndex)
        UpdateScore(std::any_cast<int>(message.args[1]));
}

#include "GameScore.h"

#include <MessageQueue.h>

#include <cassert>

#include "Game.h"

bb::GameScore::GameScore()
{
    ResetScore();
    MessageQueue::AddListener(MessageType::PlayerPickup, this, &GameScore::OnMessage);
}

bb::GameScore::~GameScore() { MessageQueue::RemoveListenerInstance(this); }

int bb::GameScore::GetScore(int playerIndex) const
{
    assert(playerIndex < PLAYER_COUNT);

    return m_PlayerScore[playerIndex];
}

void bb::GameScore::ResetScore()
{
    for(int playerIndex = 0; playerIndex < PLAYER_COUNT; ++playerIndex)
    {
        m_PlayerScore[playerIndex] = -1;
        MessageQueue::Broadcast(MessageType::CharacterScoreChange, { playerIndex, m_PlayerScore[playerIndex] });
    }
}

void bb::GameScore::AddScore(int playerIndex, int delta)
{
    assert(playerIndex < PLAYER_COUNT);

    // We use -1 as a way to check if any score has been set
    if(m_PlayerScore[playerIndex] == -1)
        m_PlayerScore[playerIndex] = 0;

    m_PlayerScore[playerIndex] += delta;
    MessageQueue::Broadcast(MessageType::CharacterScoreChange, { playerIndex, m_PlayerScore[playerIndex] });
}

void bb::GameScore::OnMessage(const Message& message)
{
    switch(static_cast<MessageType>(message.id))
    {
        case MessageType::PlayerPickup:
        {
            auto playerIndex = std::any_cast<int>(message.args[1]);
            auto pickupType = std::any_cast<PickupType>(message.args[2]);

            assert(Game::PICKUP_VALUES.contains(pickupType));
            const int score = Game::PICKUP_VALUES.at(pickupType);

            AddScore(playerIndex, score);
        }
        break;
        default:
            break;
    }
}

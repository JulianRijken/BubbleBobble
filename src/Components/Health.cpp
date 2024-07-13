#include "Health.h"

#include <MessageQueue.h>

#include "Game.h"

bb::Health::Health(jul::GameObject* parentPtr, int startingLives, int playerIndex) :
    Component(parentPtr),
    m_LivesLeft(startingLives),
    m_PlayerIndex(playerIndex)
{
    jul::MessageQueue::Broadcast(MessageType::CharacterLivesChange, { m_PlayerIndex, m_LivesLeft });
}

void bb::Health::Damage()
{
    if(m_Dead)
        return;

    m_LivesLeft--;

    // If out of lives
    if(m_LivesLeft < 0)
    {
        m_OnDeath.Invoke();
        m_Dead = true;
        return;
    }

    jul::MessageQueue::Broadcast(MessageType::CharacterLivesChange, { m_PlayerIndex, m_LivesLeft });
}

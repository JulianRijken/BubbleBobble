#include "Game.h"

bb::Game::Game()
{
    MessageQueue::AddListener(MessageType::GameStart, this, &Game::OnMessage);
    MessageQueue::AddListener(MessageType::PlayerAttack, this, &Game::OnMessage);
    MessageQueue::AddListener(MessageType::PlayerDied, this, &Game::OnMessage);
}

bb::Player* bb::Game::GetPlayer(int playerIndex) { return m_Players[playerIndex]; }


void bb::Game::SetPlayer(int playerIndex, Player* player) { m_Players[playerIndex] = player; }

void bb::Game::OnMessage(const Message& message)
{
    switch(static_cast<MessageType>(message.id))
    {
        case MessageType::GameStart:
            Locator::Get<Sound>().PlaySound((int)Sounds::GameStart);
            break;
        case MessageType::PlayerDied:
            Locator::Get<Sound>().PlaySound((int)Sounds::Death);
            break;
        case MessageType::PlayerAttack:
            Locator::Get<Sound>().PlaySound((int)Sounds::FireBubble);
            break;
    }
}

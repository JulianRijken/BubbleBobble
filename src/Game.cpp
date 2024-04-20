#include "Game.h"

bb::Player* bb::Game::GetPlayer(int playerIndex) { return m_Players[playerIndex]; }

void bb::Game::SetPlayer(int playerIndex, Player* player) { m_Players[playerIndex] = player; }

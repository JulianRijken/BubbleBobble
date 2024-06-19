#pragma once

#include <MessageQueue.h>

#include <array>

namespace bb
{

    class GameScore final
    {
    public:
        GameScore();
        ~GameScore();

        [[nodiscard]] int GetScore(int playerIndex) const;
        void ResetScore();

    private:
        void AddScore(int playerIndex, int delta);
        void OnMessage(const jul::Message& message);
        std::array<int, 2> m_PlayerScore{};
        const int PLAYER_COUNT{ static_cast<int>(m_PlayerScore.size()) };
    };

}  // namespace bb

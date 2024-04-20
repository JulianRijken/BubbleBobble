#pragma once
#include <Player.h>
#include <Singleton.h>

#include <array>

namespace bb
{
    class Game final : public jul::Singleton<Game>
    {
    public:
        Player* GetPlayer(int playerIndex);
        void SetPlayer(int playerIndex, Player* player);

    private:
        std::array<Player*, 2> m_Players{};
    };
}  // namespace bb

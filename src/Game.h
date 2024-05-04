#pragma once
#include <Locator.h>
#include <MessageQueue.h>
#include <Player.h>
#include <Singleton.h>
#include <Sound.h>

#include <array>

namespace bb
{
    enum class MessageType
    {
        PlayerDied,
        PlayerAttack,
        GameStart,
    };

    enum class InputBind
    {
        TestLivesButton,
        ToggleSound,
        Jump,
        MoveLeft,
        MoveRight,
        MoveStick,
        Attack,
    };

    enum class Sounds
    {
        GameStart,
        FireBubble,
        Death
    };


    class Game final : public jul::Singleton<Game>
    {
    public:
        Game();

        Player* GetPlayer(int playerIndex);
        void SetPlayer(int playerIndex, Player* player);

    private:
        std::array<Player*, 2> m_Players{};

        void OnMessage(const Message& message);
    };
}  // namespace bb

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

    enum class BlockSolidity
    {
        None,
        Semi,
        Solid
    };

    struct Block
    {
        glm::vec2 block;
        BlockSolidity solidity;
    };

    struct Map
    {
        std::vector<Block> blocks;
    };


    class Game final : public jul::Singleton<Game>
    {
    public:
        Game();

        static Map LoadMap(std::string fileName);


        Player* GetPlayer(int playerIndex);
        void SetPlayer(int playerIndex, Player* player);

    private:
        std::array<Player*, 2> m_Players{};

        void OnMessage(const Message& message);
    };
}  // namespace bb

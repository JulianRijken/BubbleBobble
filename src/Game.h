#pragma once
#include <Locator.h>
#include <MessageQueue.h>
#include <Player.h>
#include <Singleton.h>
#include <Sound.h>

#include <array>


struct SDL_Surface;

namespace bb
{
    enum class MessageType
    {
        PlayerDied,
        PlayerAttack,
        PlayerJump,
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
        UiSelect,
        UiUp,
        UiDown
    };

    enum class Sounds
    {
        GameStart,
        FireBubble,
        Death,
        Jump
    };

    enum class BlockSolidity
    {
        None,
        Semi,
        Solid
    };

    struct Block
    {
        glm::vec2 position;
        BlockSolidity solidity;
    };

    struct Map
    {
        std::vector<Block> blocks;
    };

    // going for https://www.youtube.com/watch?v=VyK_cpp9pT4
    // BubbleBobble NES
    class Game final : public jul::Singleton<Game>
    {
    public:
        void Initialize();

        [[nodiscard]] Player* GetPlayer(int playerIndex) const;
        void SetPlayer(int playerIndex, Player* player);

        std::vector<Map>& GetMaps() { return m_Maps; }

    private:
        std::array<Player*, 2> m_Players{};
        std::vector<Map> m_Maps{};

        void OnMessage(const Message& message);

        void ParseMaps(const std::string& fileName);

        static SDL_Surface* JxlToSurface(const std::string& fileName);
    };

}  // namespace bb

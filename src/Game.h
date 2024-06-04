#pragma once
#include <Locator.h>
#include <MessageQueue.h>
#include <Player.h>
#include <Scene.h>
#include <Singleton.h>
#include <Sound.h>

#include <array>

struct SDL_Surface;

namespace jul
{
    class Camera;
}

namespace bb
{
    using namespace jul;


    enum class MessageType
    {
        PlayerDied,
        PlayerAttack,
        PlayerJump,
        GameStart,
    };

    enum class InputBind
    {
        ForceReset,
        TestLivesButton,
        ToggleSound,
        Jump,
        MoveLeft,
        MoveRight,
        MoveStick,
        Attack,
        UiSelect,
        UiUp,
        UiDown,
        DebugIncreaseTimeScale,
        DebugDecreaseTimeScale
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
    class Game final : public jul::Singleton<Game>, public IEventListener
    {
    public:
        inline const static std::string BUBBLE_SPRITE_NAME = "BubbleCharacter";
        inline const static std::string BOBBLE_SPRITE_NAME = "BobbleCharacter";
        inline constexpr static int GRID_SIZE_X = 32;
        inline constexpr static int GRID_SIZE_Y = 28;
        inline constexpr static int PIXELS_PER_GRID_CELL = 8;
        inline constexpr static double LEVEL_TRANSITION_DURATION = 5.0;

        [[nodiscard]] Player* GetPlayer(int playerIndex) const;

        [[nodiscard]] std::vector<Map>& GetMaps() { return m_Maps; }

        void Initialize();

        void ForceResetGame(const InputContext& context);
        void IncreaseTimeScale(const InputContext& context);
        void DecreaseTimeScale(const InputContext& context);


        void SetPlayer(int playerIndex, Player* player);
        void SetMainCamera(Camera* camera);
        GameObject* SpawnLevelTiles(Scene& scene, int levelIndex, glm::vec3 spawnLocation = {});
        Player* SpawnPlayer(Scene& scene, int playerIndex, glm::vec3 spawnLocation = {});
        void TransitionLevel(bool resetPlayers = true);


    private:
        std::array<Player*, 2> m_Players{ nullptr, nullptr };
        std::vector<Map> m_Maps{};
        Camera* m_MainCamera{};

        void OnMessage(const Message& message);
        void ParseMaps(const std::string& fileName);

        static SDL_Surface* JxlToSurface(const std::string& fileName);
    };

}  // namespace bb

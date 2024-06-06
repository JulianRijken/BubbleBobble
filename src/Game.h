#pragma once
#include <Event.h>
#include <InputContext.h>
#include <Locator.h>
#include <MessageQueue.h>
#include <Scene.h>
#include <Singleton.h>
#include <Sound.h>

#include <array>
#include <glm/vec2.hpp>

#include "Scenes.h"

struct SDL_Surface;

namespace jul
{
    class Camera;
}

namespace bb
{

    class Player;
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

    enum class PickupType
    {
        Fries,
        Watermelon
    };

    namespace layer
    {
        constexpr uint16_t Index(int index) { return 1 << index; }

        constexpr uint16_t ALL = 0xFFFF;
        constexpr uint16_t ENEMY = Index(1);
        constexpr uint16_t ENEMY_DEAD = Index(2);
        constexpr uint16_t PICKUP = Index(3);
        constexpr uint16_t PLAYER = Index(4);
        constexpr uint16_t CAPTURE_BUBBLE = Index(5);
        constexpr uint16_t TILE = Index(6);
        constexpr uint16_t TILE_SEMI_SOLID = Index(7);
        constexpr uint16_t INVIS_WALLS = Index(8);
    }  // namespace layer


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
        inline constexpr static double LEVEL_TRANSITION_DURATION = 4.0;
        inline constexpr static std::array<scenes::Id, 4> LEVELS{
            scenes::Id::IntroLevel, scenes::Id::Level1, scenes::Id::Level2, scenes::Id::Level3
        };

        [[nodiscard]] Player* GetPlayer(int playerIndex) const;

        [[nodiscard]] std::vector<Map>& GetMaps() { return m_Maps; }

        [[nodiscard]] Scene* GetActiveLevelScene() const;

        [[nodiscard]] Event<bool, int>& GetLevelTransitionChangeEvent() { return m_LevelTransitionChangeEvent; }

        void Initialize();

        // Starts the actual game with a mode
        void StartGame(int mode);

        void TransitionToLevel(int levelIndex, bool delayLoading = true, bool resetPlayers = true);
        void SetPlayer(int playerIndex, Player* player);
        void SetMainCamera(Camera* camera);

        GameObject* SpawnLevelTiles(int levelIndex);

        void OnForceResetGame(const InputContext& context);
        void OnIncreaseTimeScale(const InputContext& context);
        void OnDecreaseTimeScale(const InputContext& context);

    private:
        void OnMessage(const Message& message);
        void ParseMaps(const std::string& fileName);

        static SDL_Surface* JxlToSurface(const std::string& fileName);


        std::array<Player*, 2> m_Players{ nullptr, nullptr };
        std::vector<Map> m_Maps{};

        int m_ActiveLevelIndex{ 0 };

        Camera* m_MainCameraPtr{ nullptr };
        GameObject* m_ActiveLevelTilesPtr{ nullptr };

        Event<bool, int> m_LevelTransitionChangeEvent{};
    };

}  // namespace bb

#ifndef GAME_H
#define GAME_H

#include <Event.h>
#include <InputContext.h>
#include <Locator.h>
#include <MessageQueue.h>
#include <Scene.h>
#include <SDL_pixels.h>
#include <Singleton.h>
#include <Sound.h>

#include <array>
#include <glm/vec2.hpp>
#include <unordered_map>

#include "GameScore.h"
#include "Scenes.h"

struct SDL_Surface;

namespace jul
{
    class Camera;
}

namespace bb
{

    class Player;

    enum class MessageType
    {
        PlayerDied,
        PlayerAttack,
        PlayerJump,

        // position = glm::vec3
        // playerIndex = int
        // pickupType = PickupType
        PlayerPickup,

        // playerIndex = int
        // lives = int
        CharacterLivesChange,

        // playerIndex = int
        // score = int
        CharacterScoreChange,

        // playerIndex = int
        // score = int
        CharacterScoreAdded,

        GameStart,
        ShowScoreScreen
    };

    enum class InputBind
    {
        ForceReset,
        ForceTransition,
        ForceEnd,
        TestLivesButton,
        ToggleSound,
        ToggleDebug,
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

    enum class GameMode
    {
        P1,
        P2,
        VS
    };


    enum class PickupType
    {
        Fries,
        Watermelon
    };

    enum class GameState
    {
        MainMenu,
        Intro,
        Game,
        ScoreScreen
    };

    enum class EnemyType
    {
        ZenChan,
        Maita
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
        constexpr uint16_t BOULDER = Index(9);
        constexpr uint16_t ALL_TILES = TILE | TILE_SEMI_SOLID;
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
    class Game final : public jul::Singleton<Game>, public jul::IEventListener
    {
    public:
        inline const static std::string BUBBLE_SPRITE_NAME = "BubbleCharacter";
        inline const static std::string BOBBLE_SPRITE_NAME = "BobbleCharacter";
        inline constexpr static int GRID_SIZE_X = 32;
        inline constexpr static int GRID_SIZE_Y = 28;
        inline constexpr static int STARTING_LIVES = 3;
        inline constexpr static int PIXELS_PER_GRID_CELL = 8;
        inline constexpr static double LEVEL_TRANSITION_DURATION = 4.0;
        inline constexpr static SDL_Color PLAYER_1_COLOR = { 92, 230, 52, 255 };
        inline constexpr static SDL_Color PLAYER_2_COLOR = { 52, 168, 230, 255 };
        inline constexpr static glm::vec3 PLAYER_1_DEFAULT_POSITION = { -12, -10, 0 };
        inline constexpr static glm::vec3 PLAYER_2_DEFAULT_POSITION = { 12, -10, 0 };


        inline const static std::unordered_map<PickupType, int> PICKUP_VALUES{
            {PickupType::Watermelon, 100},
            {     PickupType::Fries, 200},
        };

        inline constexpr static std::array<scenes::Id, 4> LEVELS{
            scenes::Id::IntroLevel, scenes::Id::Level1, scenes::Id::Level2, scenes::Id::Level3
        };

        [[nodiscard]] Player* GetPlayer(int playerIndex) const;

        [[nodiscard]] std::vector<Map>& GetMaps() { return m_Maps; }

        [[nodiscard]] jul::Scene* GetActiveLevelScene() const;

        [[nodiscard]] jul::Camera* GetMainCamera() const { return m_MainCameraPtr; }

        [[nodiscard]] jul::Event<bool, int>& GetLevelTransitionChangeEvent() { return m_LevelTransitionChangeEvent; }

        [[nodiscard]] GameMode GetActiveGameMode() { return m_ActiveGameMode; }

        [[nodiscard]] GameScore& GetGameScore() { return m_GameScore; }

        [[nodiscard]] int GetActiveLevelIndex() const { return m_ActiveLevelIndex; }

        void Initialize();

        // Starts the actual game with a mode
        void StartGame(GameMode mode);

        void TryTransitionNextLevel(bool onlyLoadAfterTransition = true, bool resetPlayers = true);
        void TryTransitionLevel(int levelIndex, bool onlyLoadAfterTransition = true, bool resetPlayers = true);
        void SetPlayer(int playerIndex, Player* player);
        void SetMainCamera(jul::Camera* camera);

        jul::GameObject* SpawnLevelTiles(int levelIndex);

        void OnResetGameButton(const jul::InputContext& context);
        void OnTransitionGameButton(const jul::InputContext& context);
        void OnEndGameButton(const jul::InputContext& context);
        void OnIncreaseTimeScaleButton(const jul::InputContext& context);
        void OnDecreaseTimeScaleButton(const jul::InputContext& context);

        void ResetGame();
        void ShowScoreScreen();

    private:
        void OnMessage(const jul::Message& message);
        void ParseMaps(const std::string& fileName);

        static SDL_Surface* JxlToSurface(const std::string& fileName);

        std::array<Player*, 2> m_Players{ nullptr, nullptr };
        std::vector<Map> m_Maps{};

        // Transition
        bool m_InTransition{ false };
        int m_ActiveLevelIndex{ 0 };
        jul::GameObject* m_ActiveLevelTilesPtr{ nullptr };

        GameState m_GameState{};
        GameMode m_ActiveGameMode{};
        jul::Camera* m_MainCameraPtr{ nullptr };

        jul::Event<bool, int> m_LevelTransitionChangeEvent{};
        GameScore m_GameScore{};
    };

}  // namespace bb
#endif  // GAME_H

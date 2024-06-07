#include "Game.h"

#include <Animator.h>
#include <Camera.h>
#include <fmt/core.h>
#include <GameObject.h>
#include <GameTime.h>
#include <Input.h>
#include <ResourceManager.h>
#include <SceneManager.h>
#include <SDL_image.h>
#include <SpriteRenderer.h>
#include <TextRenderer.h>
#include <TweenEngine.h>

#include <filesystem>
#include <fstream>

#include "OneWayPlatform.h"
#include "Player.h"
#include "ZenChan.h"

void bb::Game::Initialize()
{
    MessageQueue::AddListener(MessageType::GameStart, this, &Game::OnMessage);
    MessageQueue::AddListener(MessageType::PlayerAttack, this, &Game::OnMessage);
    MessageQueue::AddListener(MessageType::PlayerDied, this, &Game::OnMessage);
    MessageQueue::AddListener(MessageType::PlayerJump, this, &Game::OnMessage);

    ParseMaps("Levels/Levels.jxl");

    Input::Bind((int)InputBind::ForceReset, 0, false, this, &Game::OnResetGameButton);
    Input::Bind((int)InputBind::ForceTransition, 0, false, this, &Game::OnTransitionGameButton);
    Input::Bind((int)InputBind::DebugIncreaseTimeScale, 0, false, this, &Game::OnIncreaseTimeScale);
    Input::Bind((int)InputBind::DebugDecreaseTimeScale, 0, false, this, &Game::OnDecreaseTimeScale);

    Input::Bind((int)InputBind::ForceReset, 1, true, this, &Game::OnResetGameButton);
    Input::Bind((int)InputBind::ForceTransition, 1, true, this, &Game::OnTransitionGameButton);
    Input::Bind((int)InputBind::DebugIncreaseTimeScale, 1, true, this, &Game::OnIncreaseTimeScale);
    Input::Bind((int)InputBind::DebugDecreaseTimeScale, 1, true, this, &Game::OnDecreaseTimeScale);
}

void bb::Game::StartGame(GameMode mode)
{
    m_ActiveGameMode = mode;
    m_ActiveLevelIndex = 0;
    m_ActiveLevelTilesPtr = nullptr;
    m_MainCameraPtr = nullptr;
    m_InTransition = false;
    m_GameState = GameState::Intro;

    switch(mode)
    {
        case GameMode::P1:
            SceneManager::GetInstance().LoadScene((int)scenes::Id::OnePlayerMode);
            break;
        case GameMode::P2:
            SceneManager::GetInstance().LoadScene((int)scenes::Id::TwoPlayerMode);
            break;
        case GameMode::VS:
            SceneManager::GetInstance().LoadScene((int)scenes::Id::VersusMode);
            break;
    }

    // Locator::Get<Sound>().PlaySound((int)Sounds::GameStart);
}


bb::Player* bb::Game::GetPlayer(int playerIndex) const { return m_Players[playerIndex]; }

jul::Scene* bb::Game::GetActiveLevelScene() const
{
    const scenes::Id currentLevelId = LEVELS[m_ActiveLevelIndex];
    return SceneManager::GetInstance().FindScene((int)currentLevelId);
}

void bb::Game::SetPlayer(int playerIndex, Player* player) { m_Players[playerIndex] = player; }

void bb::Game::SetMainCamera(Camera* camera) { m_MainCameraPtr = camera; }


jul::GameObject* bb::Game::SpawnLevelTiles(int levelIndex)
{
    if(levelIndex >= static_cast<int>(m_Maps.size()) or levelIndex < 0)
        throw std::runtime_error(fmt::format("Map not loaded: {}", levelIndex));

    auto* levelParent = SceneManager::GetPrimaryScene().AddGameObject(fmt::format("Level {}", levelIndex));

    auto& maps = GetMaps();
    for(auto&& block : maps[levelIndex].blocks)
    {
        auto* tile = SceneManager::GetPrimaryScene().AddGameObject(
            "LevelTile", glm::vec3{ block.position.x, block.position.y, 0 }, levelParent, false);

        if(block.solidity == BlockSolidity::Semi)
        {
            tile->AddComponent<SpriteRenderer>(
                ResourceManager::GetSprite("LevelTiles"), -50, glm::ivec2{ 0, levelIndex });
            tile->AddComponent<Rigidbody>(Rigidbody::Settings{ .mode = Rigidbody::Mode::Static });
            tile->AddComponent<OneWayPlatform>();
            tile->AddComponent<BoxCollider>(BoxCollider::Settings{
                .size{ 1.0f, 1.0f },
                .center{ 0.5f, -0.5f },
                .mask = { .category = layer::TILE_SEMI_SOLID }
            });
        }

        if(block.solidity == BlockSolidity::Solid)
        {
            tile->AddComponent<SpriteRenderer>(
                ResourceManager::GetSprite("LevelTiles"), -50, glm::ivec2{ 0, levelIndex });
            tile->AddComponent<BoxCollider>(BoxCollider::Settings{
                .size{ 1.0f, 1.0f },
                .center{ 0.5f, -0.5f },
                .mask = { .category = layer::TILE }
            });
        }

        if(block.solidity == BlockSolidity::None)
        {
            tile->AddComponent<SpriteRenderer>(
                ResourceManager::GetSprite("LevelTiles"), -50, glm::ivec2{ 0, levelIndex });
        }
    }

    return levelParent;
}

void bb::Game::TryTransitionLevel(int levelIndex, bool onlyLoadAfterTransition, bool resetPlayers)
{
    if(m_InTransition)
    {
        std::cerr << "Trying to transition when when already in transition" << std::endl;
        return;
    }

    if(levelIndex < 0)
    {
        std::cerr << "Trying to transition to invalid index " << levelIndex << std::endl;
        return;
    }

    if(levelIndex >= static_cast<int>(LEVELS.size()))
    {
        fmt::println("Level index {} is not yet implemented :(", levelIndex);
        ResetGame();
        return;
    }

    fmt::println("Transitioning to level {}", levelIndex);
    m_InTransition = true;
    m_LevelTransitionChangeEvent.Invoke(true, levelIndex);

    TweenEngine::Start(
        {
            .from = GRID_SIZE_Y,
            .to = 0,
            .duration = LEVEL_TRANSITION_DURATION,
            .easeFunction = EaseFunction::SineOut,
            .onStart =
                [this, levelIndex, onlyLoadAfterTransition, resetPlayers]()
            {
                // Move players to default positions
                if(resetPlayers)
                {
                    if(m_Players[0])
                    {
                        m_Players[0]->GetTransform().Translate(0, GRID_SIZE_Y, 0);
                        m_Players[0]->BubbleToPosition({ -12, -10, 0 }, LEVEL_TRANSITION_DURATION);
                    }

                    if(m_Players[1])
                    {
                        m_Players[1]->GetTransform().Translate(0, GRID_SIZE_Y, 0);
                        m_Players[1]->BubbleToPosition({ 12, -10, 0 }, LEVEL_TRANSITION_DURATION);
                    }
                }


                // Unload old level
                if(auto* activeLevel = GetActiveLevelScene())
                    activeLevel->Unload();


                // Get rid of old tiles
                if(m_ActiveLevelTilesPtr != nullptr)
                {
                    m_ActiveLevelTilesPtr->GetTransform().SetWorldPosition(0, GRID_SIZE_Y, 0);
                    m_ActiveLevelTilesPtr->DestroyDelayed(LEVEL_TRANSITION_DURATION);
                }

                // Spawn new tiles
                if(levelIndex > 0)
                {
                    // We don't spawn the first level as it's the intro level
                    m_ActiveLevelTilesPtr = SpawnLevelTiles(levelIndex - 1);

                    // If we are past the into we change the state to game
                    m_GameState = GameState::Game;
                }


                // Move camera up
                m_MainCameraPtr->GetTransform().SetWorldPosition(0, GRID_SIZE_Y, 0);

                // Load next level after delay
                TweenEngine::Start(
                    {
                        .delay = onlyLoadAfterTransition ? LEVEL_TRANSITION_DURATION : 0,
                        .duration = 0,
                        .invokeWhenDestroyed = false,
                        .onEnd =
                            [levelIndex, this]()
                        {
                            // Load next level
                            m_ActiveLevelIndex = levelIndex;

                            const scenes::Id levelToLoad = LEVELS[m_ActiveLevelIndex];
                            SceneManager::GetInstance().LoadScene((int)levelToLoad, SceneLoadMode::Additive);

                            m_InTransition = false;
                            m_LevelTransitionChangeEvent.Invoke(false, m_ActiveLevelIndex);
                        },
                    },
                    m_MainCameraPtr);
            },
            .onUpdate = [this](double value) { m_MainCameraPtr->GetTransform().SetWorldPosition(0, value, 0); },
        },
        m_MainCameraPtr);
}

void bb::Game::OnResetGameButton(const InputContext& context)
{
    if(context.state != ButtonState::Down)
        return;

    ResetGame();
}

void bb::Game::OnTransitionGameButton(const InputContext& context)
{
    if(context.state != ButtonState::Down)
        return;

    if(m_GameState != GameState::Game)
    {
        fmt::println("Transition only works when in game and not in intro");
        return;
    }

    TryTransitionLevel(m_ActiveLevelIndex + 1);
}

void bb::Game::OnIncreaseTimeScale(const InputContext& context)
{
    if(context.state != ButtonState::Down)
        return;

    const double currentTimeScale = GameTime::GetTimeScale();
    if(GameTime::GetTimeScale() >= 1)
        GameTime::SetTimeScale(std::round(currentTimeScale + 3));
    else
        GameTime::SetTimeScale(currentTimeScale + 0.1);

    fmt::println("Time Scale Changed: {}", GameTime::GetTimeScale());
}

void bb::Game::OnDecreaseTimeScale(const InputContext& context)
{
    if(context.state != ButtonState::Down)
        return;

    const double currentTimeScale = GameTime::GetTimeScale();
    if(GameTime::GetTimeScale() > 1)
        GameTime::SetTimeScale(std::round(currentTimeScale - 3));
    else
        GameTime::SetTimeScale(currentTimeScale - 0.1);

    fmt::println("Time Scale Changed: {}", GameTime::GetTimeScale());
}

void bb::Game::ResetGame()
{
    m_GameState = GameState::MainMenu;
    SceneManager::GetInstance().LoadScene((int)scenes::Id::MainMenu, SceneLoadMode::OverrideForce);
}


void bb::Game::OnMessage(const Message& message)
{
    switch(static_cast<MessageType>(message.id))
    {
        case MessageType::GameStart:
        {
            GameMode mode = std::any_cast<GameMode>(message.arguments[0]);
            StartGame(mode);
        }
            break;
        case MessageType::PlayerDied:
            Locator::Get<Sound>().PlaySound((int)Sounds::Death);
            break;
        case MessageType::PlayerAttack:
            Locator::Get<Sound>().PlaySound((int)Sounds::FireBubble);
            break;
        case MessageType::PlayerJump:
            Locator::Get<Sound>().PlaySound((int)Sounds::Jump);
            break;
    }
}

void bb::Game::ParseMaps(const std::string& fileName)
{
    SDL_Surface* surface = JxlToSurface(fileName);

    const auto* pixels = static_cast<uint32_t*>(surface->pixels);

    const int levelCount = (surface->h / GRID_SIZE_Y);


    m_Maps.clear();
    m_Maps.reserve(levelCount);

    for(int levelIndex = 0; levelIndex < levelCount; ++levelIndex)
    {
        Map map{};

        for(int y = 0; y < GRID_SIZE_Y; ++y)
        {
            for(int x = 0; x < GRID_SIZE_X; ++x)
            {
                const uint32_t pixel = pixels[(y + levelIndex * GRID_SIZE_Y) * GRID_SIZE_X + x];
                const int red = static_cast<int>((pixel) & 0xFF);
                const int green = static_cast<int>((pixel >> 8) & 0xFF);
                const int blue = static_cast<int>((pixel >> 16) & 0xFF);

                int solidity{ -1 };
                if(red == 255)
                    solidity = 0;
                else if(green == 255)
                    solidity = 1;
                else if(blue == 255)
                    solidity = 2;

                if(solidity == -1)
                    continue;

                map.blocks.push_back({
                    .position = {x - (GRID_SIZE_X / 2), (GRID_SIZE_Y - y) - (GRID_SIZE_Y / 2)},
                    .solidity = static_cast<BlockSolidity>(solidity)
                });
            }
        }

        m_Maps.emplace_back(std::move(map));
    }


    SDL_FreeSurface(surface);
}

// Levels come in the jxl format
// They are RGBA 8bits
SDL_Surface* bb::Game::JxlToSurface(const std::string& fileName)
{
    const std::filesystem::path filePath = ResourceManager::GetAssetsPath() / fileName;

    if(not exists(filePath))
        throw std::runtime_error("File does not exist: " + filePath.string());

    std::ifstream inFile;
    inFile.open(filePath);

    if(not inFile.is_open())
        throw std::runtime_error("File already open" + filePath.string());

    const std::string pathString = filePath.string();
    SDL_Surface* surface = IMG_Load(pathString.c_str());

    if(surface == nullptr)
        throw std::runtime_error("Failed to load image: " + pathString);

    return surface;
}

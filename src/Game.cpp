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

    Input::Bind((int)InputBind::ForceReset, 0, true, this, &Game::OnForceResetGame);
    Input::Bind((int)InputBind::DebugIncreaseTimeScale, 0, true, this, &Game::OnIncreaseTimeScale);
    Input::Bind((int)InputBind::DebugDecreaseTimeScale, 0, true, this, &Game::OnDecreaseTimeScale);
}

void bb::Game::StartGame(int)
{
    m_ActiveLevelIndex = 0;
    m_ActiveLevelTilesPtr = nullptr;
    SceneManager::GetInstance().LoadScene((int)scenes::Id::Main);
    // Locator::Get<Sound>().PlaySound((int)Sounds::GameStart);
}

bb::Player* bb::Game::GetPlayer(int playerIndex) const { return m_Players[playerIndex]; }

jul::Scene* bb::Game::GetActiveLevelScene()
{
    const scenes::Id currentLevelId = LEVELS[m_ActiveLevelIndex];
    return SceneManager::GetInstance().FindScene((int)currentLevelId);
}

void bb::Game::SetPlayer(int playerIndex, Player* player) { m_Players[playerIndex] = player; }

void bb::Game::SetMainCamera(Camera* camera) { m_MainCameraPtr = camera; }


bb::Player* bb::Game::SpawnPlayer(Scene& scene, int playerIndex, glm::vec3 spawnLocation)
{
    if(m_Players[playerIndex] != nullptr)
        throw std::runtime_error(fmt::format("Player {} already exists", playerIndex));

    auto spriteName = playerIndex == 0 ? BUBBLE_SPRITE_NAME : BOBBLE_SPRITE_NAME;

    auto* playerGameObject = scene.AddGameObject("Player", spawnLocation);
    auto* bodySprite = playerGameObject->AddComponent<SpriteRenderer>(ResourceManager::GetSprite(spriteName), 0);
    auto* bubbleSprite = playerGameObject->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("BubbleLarge"), 0);
    auto* bodyAnimator = playerGameObject->AddComponent<Animator>(bodySprite);
    auto* bubbleAnimator = playerGameObject->AddComponent<Animator>(bubbleSprite);
    playerGameObject->AddComponent<Rigidbody>();
    playerGameObject->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 0.1f,
        .size = {1.80f, 1.95f},
    });

    return playerGameObject->AddComponent<Player>(playerIndex, bodySprite, bubbleSprite, bodyAnimator, bubbleAnimator);
}

jul::GameObject* bb::Game::SpawnLevelTiles(int levelIndex)
{
    if(levelIndex >= static_cast<int>(m_Maps.size()))
        throw std::runtime_error(fmt::format("Map not loaded: {}", levelIndex));

    auto* levelParent = SceneManager::GetInstance().AddGameObject(fmt::format("Level {}", levelIndex));

    auto& maps = GetMaps();
    for(auto&& block : maps[levelIndex].blocks)
    {
        auto* tile = SceneManager::GetInstance().AddGameObject(
            "LevelTile", glm::vec3{ block.position.x, block.position.y, 0 }, levelParent, false);

        if(block.solidity == BlockSolidity::Semi)
        {
            tile->AddComponent<SpriteRenderer>(
                ResourceManager::GetSprite("LevelTiles"), -50, glm::ivec2{ 0, levelIndex });
            tile->AddComponent<Rigidbody>(Rigidbody::Settings{ .mode = Rigidbody::Mode::Static });
            tile->AddComponent<OneWayPlatform>();
            tile->AddComponent<BoxCollider>(BoxCollider::Settings{
                .size{1.0f,  1.0f},
                .center{0.5f, -0.5f}
            });
        }

        if(block.solidity == BlockSolidity::Solid)
        {
            tile->AddComponent<SpriteRenderer>(
                ResourceManager::GetSprite("LevelTiles"), -50, glm::ivec2{ 0, levelIndex });
            tile->AddComponent<BoxCollider>(BoxCollider::Settings{
                .size{1.0f,  1.0f},
                .center{0.5f, -0.5f}
            });
        }
    }

    return levelParent;
}

void bb::Game::TransitionToLevel(int levelIndex, bool delayLoading, bool resetPlayers)
{
    // Unload old level
    if(auto* activeLevel = GetActiveLevelScene())
        activeLevel->Unload();

    if(levelIndex > 0)
    {
        if(m_ActiveLevelTilesPtr != nullptr)
        {
            m_ActiveLevelTilesPtr->GetTransform().SetWorldPosition(0, GRID_SIZE_Y, 0);
            auto* oldTiles = m_ActiveLevelTilesPtr;
            TweenEngine::Start(
                {
                    .delay = LEVEL_TRANSITION_DURATION,
                    .invokeWhenDestroyed = false,
                    .onEnd = [oldTiles]() { oldTiles->Destroy(); },
                },
                m_MainCameraPtr);
        }

        m_ActiveLevelTilesPtr = SpawnLevelTiles(levelIndex - 1);
    }


    m_MainCameraPtr->GetTransform().SetWorldPosition(0, GRID_SIZE_Y, 0);

    TweenEngine::Start(
        {
            .duration = delayLoading ? LEVEL_TRANSITION_DURATION : 0,
            .onEnd =
                [levelIndex, this]()
            {
                // Load next level
                const scenes::Id levelToLoad = LEVELS[levelIndex];
                SceneManager::GetInstance().LoadScene((int)levelToLoad, SceneLoadMode::Additive);
                m_ActiveLevelIndex = levelIndex;
            },
        },
        m_MainCameraPtr);

    TweenEngine::Start(
        {
            .from = GRID_SIZE_Y,
            .to = 0,
            .duration = LEVEL_TRANSITION_DURATION,
            .easeFunction = EaseFunction::SineOut,
            .onUpdate = [this](double value) { m_MainCameraPtr->GetTransform().SetWorldPosition(0, value, 0); },
        },
        m_MainCameraPtr);

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
}

void bb::Game::OnForceResetGame(const InputContext& context)
{
    if(context.state != ButtonState::Down)
        return;

    SceneManager::GetInstance().LoadScene((int)scenes::Id::MainMenu);
}

void bb::Game::OnIncreaseTimeScale(const InputContext& context)
{
    if(context.state != ButtonState::Down)
        return;

    const double currentTimeScale = GameTime::GetTimeScale();
    if(GameTime::GetTimeScale() >= 1)
        GameTime::SetTimeScale(std::round(currentTimeScale + 1));
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
        GameTime::SetTimeScale(std::round(currentTimeScale - 1));
    else
        GameTime::SetTimeScale(currentTimeScale - 0.1);

    fmt::println("Time Scale Changed: {}", GameTime::GetTimeScale());
}


void bb::Game::OnMessage(const Message& message)
{
    switch(static_cast<MessageType>(message.id))
    {
        case MessageType::GameStart:
        {
            int mode = std::any_cast<int>(message.arguments[0]);
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

    constexpr int levelCount = 2;
    constexpr int levelWidth = 32;
    constexpr int levelHeight = 28;


    m_Maps.clear();
    m_Maps.reserve(levelCount);

    for(int levelIndex = 0; levelIndex < levelCount; ++levelIndex)
    {
        Map map{};

        for(int y = 0; y < levelHeight; ++y)
        {
            for(int x = 0; x < levelWidth; ++x)
            {
                const uint32_t pixel = pixels[(y + levelIndex * levelHeight) * levelWidth + x];
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
                    .position = {x - (levelWidth / 2), (levelHeight - y) - (levelHeight / 2)},
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

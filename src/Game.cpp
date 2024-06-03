#include "Game.h"

#include <fmt/core.h>
#include <GameObject.h>
#include <GameTime.h>
#include <Input.h>
#include <OneWayPlatform.h>
#include <PlayerHUD.h>
#include <ResourceManager.h>
#include <SceneManager.h>
#include <SDL_image.h>
#include <TextRenderer.h>
#include <ZenChan.h>

#include <filesystem>
#include <fstream>

void bb::Game::Initialize()
{
    MessageQueue::AddListener(MessageType::GameStart, this, &Game::OnMessage);
    MessageQueue::AddListener(MessageType::PlayerAttack, this, &Game::OnMessage);
    MessageQueue::AddListener(MessageType::PlayerDied, this, &Game::OnMessage);
    MessageQueue::AddListener(MessageType::PlayerJump, this, &Game::OnMessage);

    ParseMaps("Levels/Levels.jxl");

    Input::Bind((int)InputBind::ForceReset, 0, true, this, &Game::ForceResetGame);
    Input::Bind((int)InputBind::DebugIncreaseTimeScale, 0, true, this, &Game::IncreaseTimeScale);
    Input::Bind((int)InputBind::DebugDecreaseTimeScale, 0, true, this, &Game::DecreaseTimeScale);
}

bb::Player* bb::Game::GetPlayer(int playerIndex) const { return m_Players[playerIndex]; }


void bb::Game::SetPlayer(int playerIndex, Player* player) { m_Players[playerIndex] = player; }

jul::GameObject* bb::Game::SpawnLevel(Scene& scene, int levelIndex, glm::vec3 spawnLocation)
{
    auto* levelParent = scene.AddGameObject(fmt::format("Level {}", levelIndex), spawnLocation);

    auto& maps = Game::GetInstance().GetMaps();
    for(auto&& block : maps[levelIndex].blocks)
    {
        auto* tile = scene.AddGameObject("LevelTile", { block.position.x, block.position.y, 0 }, levelParent, false);

        if(block.solidity == BlockSolidity::Semi)
        {
            tile->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("LevelTiles"), -50, glm::ivec2{ 2, 0 });
            tile->AddComponent<Rigidbody>(Rigidbody::Settings{ .mode = Rigidbody::Mode::Static });
            tile->AddComponent<OneWayPlatform>();
            tile->AddComponent<BoxCollider>(BoxCollider::Settings{
                .size{1.0f,  1.0f},
                .center{0.5f, -0.5f}
            });
        }

        if(block.solidity == BlockSolidity::Solid)
        {
            tile->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("LevelTiles"), -50, glm::ivec2{ 1, 2 });
            tile->AddComponent<BoxCollider>(BoxCollider::Settings{
                .size{1.0f,  1.0f},
                .center{0.5f, -0.5f}
            });
        }
    }

    return levelParent;
}

bb::Player* bb::Game::SpawnPlayer(Scene& scene, int playerIndex, glm::vec3 spawnLocation)
{
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

void bb::Game::ForceResetGame(const InputContext& context)
{
    if(context.state != ButtonState::Down)
        return;

    SceneManager::LoadScene("MainMenu");
}

void bb::Game::IncreaseTimeScale(const InputContext& context)
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

void bb::Game::DecreaseTimeScale(const InputContext& context)
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
            // Locator::Get<Sound>().PlaySound((int)Sounds::GameStart);
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

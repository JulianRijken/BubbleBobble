#include "Game.h"

#include <fmt/core.h>
#include <GameObject.h>
#include <OneWayPlatform.h>
#include <PlayerHUD.h>
#include <ResourceManager.h>
#include <SDL_image.h>
#include <TextRenderer.h>
#include <ZenChan.h>

#include <filesystem>
#include <fstream>

bb::Game::Game()
{
    MessageQueue::AddListener(MessageType::GameStart, this, &Game::OnMessage);
    MessageQueue::AddListener(MessageType::PlayerAttack, this, &Game::OnMessage);
    MessageQueue::AddListener(MessageType::PlayerDied, this, &Game::OnMessage);
    MessageQueue::AddListener(MessageType::PlayerJump, this, &Game::OnMessage);

    ParseMaps("Levels/Levels.jxl");
}

bb::Player* bb::Game::GetPlayer(int playerIndex) const { return m_Players[playerIndex]; }


void bb::Game::SetPlayer(int playerIndex, Player* player) { m_Players[playerIndex] = player; }

void bb::Game::OnMessage(const Message& message)
{
    switch(static_cast<MessageType>(message.id))
    {
        case MessageType::GameStart:
            Locator::Get<Sound>().PlaySound((int)Sounds::GameStart);
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

void bb::MainScene(Scene& scene)
{
    // Player 1
    auto* player1GameObject = scene.AddGameObject("BubbleCharacter", { -3, 10, 0 });
    player1GameObject->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("BubbleCharacter"), 0);
    player1GameObject->AddComponent<Animator>();
    player1GameObject->AddComponent<Rigidbody>();
    player1GameObject->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 0.1f,
        .size = {1.80f, 1.95f},
    });
    player1GameObject->AddComponent<bb::Player>(0);

    // Player 2
    auto* player2GameObject = scene.AddGameObject("BobbleCharacter", { 3, 10, 0 });
    player2GameObject->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("BobbleCharacter"), 0);
    player2GameObject->AddComponent<Animator>();
    player2GameObject->AddComponent<Rigidbody>();
    player2GameObject->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 0.1f,
        .size = {1.80f, 1.95f},
    });
    player2GameObject->AddComponent<Player>(1);


    auto* zenchanGO = scene.AddGameObject("ZenChan", { 3, 5, 0 });
    zenchanGO->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("Enemys"), 0);
    zenchanGO->AddComponent<Animator>();
    zenchanGO->AddComponent<Rigidbody>();
    zenchanGO->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 0.1f,
        .size = {1.90f, 1.90f},
    });
    zenchanGO->AddComponent<ZenChan>();

    GameObject* player1Hud = scene.AddGameObject("Player1HUD");
    {
        auto* livesGameObject = scene.AddGameObject("LivesText", { -15, -12, 0 });
        auto* livesText = livesGameObject->AddComponent<TextRenderer>(
            "error", ResourceManager::GetFont("NES"), 100, glm::vec2{ 0, 0 });
        livesGameObject->AddComponent<SpriteRenderer>(
            ResourceManager::GetSprite("LevelTiles"), 90, glm::ivec2{ 4, 20 });


        auto* scoreGameObject = scene.AddGameObject("ScoreText", { -4, 13, 0 });
        auto* scoreText = scoreGameObject->AddComponent<TextRenderer>(
            "error", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 1, 0 });


        livesGameObject->GetTransform().SetParent(&player1Hud->GetTransform(), false);
        scoreGameObject->GetTransform().SetParent(&player1Hud->GetTransform(), false);

        player1Hud->AddComponent<PlayerHUD>(
            Game::GetInstance().GetPlayer(0), scoreText, livesText, SDL_Color(255, 255, 255, 255));
    }

    GameObject* player2Hud = scene.AddGameObject("Player2HUD");
    {
        auto* livesGameObject = scene.AddGameObject("LivesText", { 14, -12, 0 });
        auto* livesText = livesGameObject->AddComponent<TextRenderer>(
            "error", ResourceManager::GetFont("NES"), 100, glm::vec2{ 0, 0 });
        livesGameObject->AddComponent<SpriteRenderer>(
            ResourceManager::GetSprite("LevelTiles"), 90, glm::ivec2{ 4, 20 });


        auto* scoreGameObject = scene.AddGameObject("ScoreText", { 4, 13, 0 });
        auto* scoreText = scoreGameObject->AddComponent<TextRenderer>(
            "error", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0, 0 });


        livesGameObject->GetTransform().SetParent(&player2Hud->GetTransform(), false);
        scoreGameObject->GetTransform().SetParent(&player2Hud->GetTransform(), false);

        player2Hud->AddComponent<PlayerHUD>(
            Game::GetInstance().GetPlayer(1), scoreText, livesText, SDL_Color(255, 255, 255, 255));
    }


    auto& maps = Game::GetInstance().GetMaps();

    for(auto&& block : maps[1].blocks)
    {
        auto* tile = scene.AddGameObject("LevelTile", { block.position.x, block.position.y, 0 });

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
}

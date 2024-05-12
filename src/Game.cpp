#include "Game.h"

#include <fmt/core.h>
#include <ResourceManager.h>
#include <SDL_image.h>

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

bb::Player* bb::Game::GetPlayer(int playerIndex) { return m_Players[playerIndex]; }


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

    uint32_t* pixels = static_cast<uint32_t*>(surface->pixels);

    const int levelCount = 2;
    const int levelWidth = 32;
    const int levelHeight = 28;


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

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

    auto map = LoadMap("Level1.jxl");
    std::ignore = map;
}

bb::Map bb::Game::LoadMap(std::string fileName)
{
    const std::filesystem::path filePath = ResourceManager::GetAssetsPath() / "Levels" / fileName;

    if(not exists(filePath))
        throw std::runtime_error("File does not exist: " + filePath.string());

    std::ifstream inFile;
    inFile.open(filePath);

    if(not inFile.is_open())
        throw std::runtime_error("File already open" + filePath.string());

    std::string pathString = filePath.string();
    auto* surface = IMG_Load(pathString.c_str());

    if(surface == nullptr)
        throw std::runtime_error("Failed to load image: " + pathString);

    // Assuming the image is RGBA
    Uint32* pixels = static_cast<Uint32*>(surface->pixels);

    for(int y = 0; y < surface->h; ++y)
    {
        for(int x = 0; x < surface->w; ++x)
        {
            Uint32 pixel = pixels[y * surface->w + x];
            fmt::print("Pixel ({}, {}): A={}, B={}, G={}, R={}\n",
                       x,
                       y,
                       static_cast<int>((pixel >> 24) & 0xFF),  // Red
                       static_cast<int>((pixel >> 16) & 0xFF),  // Green
                       static_cast<int>((pixel >> 8) & 0xFF),   // Blue
                       static_cast<int>((pixel) & 0xFF));       // Alpha
        }
    }

    SDL_FreeSurface(surface);

    return Map{};
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
    }
}

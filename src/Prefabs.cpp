#include "Prefabs.h"

#include <Animator.h>
#include <AttackBubble.h>
#include <BoxCollider.h>
#include <Camera.h>
#include <fmt/core.h>
#include <GameSettings.h>
#include <ResourceManager.h>
#include <Rigidbody.h>
#include <Scene.h>
#include <SceneManager.h>
#include <SpriteRenderer.h>
#include <TextRenderer.h>
#include <ZenChan.h>

#include "Game.h"
#include "Player.h"
#include "PlayerHUD.h"

void bb::prefabs::SpawnBubble(const glm::vec3& spawnPosition, float firePower, int direction)
{
    SpawnBubble(spawnPosition, firePower, { direction, 0 });
}

void bb::prefabs::SpawnBubble(const glm::vec3& spawnPosition, float firePower, glm::vec2 direction)
{
    Scene* activeScene = Game::GetInstance().GetActiveLevelScene();
    if(activeScene == nullptr)
        throw std::runtime_error("Spawning Bubble with no active level scene");

    auto* bubble = activeScene->AddGameObject("AttackBubble", spawnPosition);
    bubble->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("AttackBubble"), 0);
    bubble->AddComponent<Animator>();
    bubble->AddComponent<Rigidbody>();
    bubble->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.5f,
        .restitution = 0.8f,
        .size = {1.5, 1.5f},
    });

    direction = glm::normalize(direction);
    const glm::vec2 velocity = direction * firePower;
    bubble->AddComponent<AttackBubble>(glm::vec3{ velocity.x, velocity.y, 0 });
}


void bb::prefabs::SpawnZenChan(const glm::vec3& spawnPosition)
{
    Scene* activeScene = Game::GetInstance().GetActiveLevelScene();
    if(activeScene == nullptr)
        throw std::runtime_error("Spawning ZenChen with no active level scene");

    auto* zenchanGO = activeScene->AddGameObject("ZenChan", spawnPosition);
    zenchanGO->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("Enemys"), 0);
    zenchanGO->AddComponent<Animator>();
    zenchanGO->AddComponent<Rigidbody>();
    zenchanGO->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 0.1f,
        .size = {1.90f, 1.90f},
    });
    zenchanGO->AddComponent<ZenChan>();
}

bb::Player* bb::prefabs::SpawnPlayer(jul::Scene& scene, int playerIndex, glm::vec3 spawnLocation)
{
    if(Game::GetInstance().GetPlayer(playerIndex) != nullptr)
        throw std::runtime_error(fmt::format("Player {} already exists", playerIndex));

    auto spriteName = playerIndex == 0 ? Game::BUBBLE_SPRITE_NAME : Game::BOBBLE_SPRITE_NAME;

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

void bb::prefabs::SpawnPlayerHUD(jul::Scene& scene, int playerIndex)
{
    GameObject* player1Hud = scene.AddGameObject(fmt::format("Player {}", playerIndex));
    {
        auto* livesGameObject = scene.AddGameObject("LivesText", { playerIndex ? -15 : 14, -12, 0 });
        auto* livesText = livesGameObject->AddComponent<TextRenderer>(
            "error", ResourceManager::GetFont("NES"), 100, glm::vec2{ 0, 0 });
        livesGameObject->AddComponent<SpriteRenderer>(
            ResourceManager::GetSprite("LevelTiles"), 90, glm::ivec2{ 4, 20 });


        auto* scoreGameObject = scene.AddGameObject("ScoreText", { playerIndex ? -4 : 4, 13, 0 });
        auto* scoreText = scoreGameObject->AddComponent<TextRenderer>(
            "error", ResourceManager::GetFont("NES"), 100, glm ::vec2{ playerIndex ? 1 : 0, 0 });


        livesGameObject->GetTransform().SetParent(&player1Hud->GetTransform(), false);
        scoreGameObject->GetTransform().SetParent(&player1Hud->GetTransform(), false);

        player1Hud->AddComponent<PlayerHUD>(
            Game::GetInstance().GetPlayer(playerIndex), scoreText, livesText, SDL_Color(255, 255, 255, 255));
    }
}

void bb::prefabs::SpawnMainCamera(jul::Scene& scene)
{
    auto* cameraGameObject = scene.AddGameObject("Camera");
    auto* cameraPtr = cameraGameObject->AddComponent<Camera>(14, GameSettings::GetAspectRatio());
    cameraGameObject->GetTransform().SetWorldPosition({ 0, 0, 0 });
    Game::GetInstance().SetMainCamera(cameraPtr);
}

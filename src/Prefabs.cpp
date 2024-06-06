#include "Prefabs.h"

#include <Animator.h>
#include <BoxCollider.h>
#include <Camera.h>
#include <CaptureBubble.h>
#include <DeadEnemy.h>
#include <fmt/core.h>
#include <GameSettings.h>
#include <ResourceManager.h>
#include <Rigidbody.h>
#include <Scene.h>
#include <SceneManager.h>
#include <SpriteRenderer.h>
#include <TextRenderer.h>
#include <ZenChan.h>
#include <ZenChanBehaviour.h>

#include "Game.h"
#include "MathExtensions.h"
#include "Pickup.h"
#include "Player.h"
#include "PlayerHUD.h"

void bb::prefabs::SpawnCaptureBubble(const glm::vec3& spawnPosition, glm::vec2 velocity)
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
        .size = {                              1.5,1.5f                                                   },
        .mask = {.category = layer::CAPTURE_BUBBLE,
                 .collideWith = layer::CAPTURE_BUBBLE | layer::PLAYER | layer::ENEMY | layer::TILE}
    });

    bubble->AddComponent<CaptureBubble>(glm::vec3{ velocity.x, velocity.y, 0 });
}

bb::ZenChan* bb::prefabs::SpawnZenChanWithBehaviour(const glm::vec3& spawnPosition)
{
    ZenChan* zenChanPtr = SpawnZenChan(spawnPosition);
    zenChanPtr->GetGameObject()->AddComponent<ZenChanBehaviour>(zenChanPtr);
    return zenChanPtr;
}

bb::ZenChan* bb::prefabs::SpawnZenChan(const glm::vec3& spawnPosition)
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
        .size = {                   1.90f,                   1.90f                                          },
        .mask = {.category = layer::ENEMY,
                 .collideWith = layer::PLAYER | layer::TILE | layer::TILE_SEMI_SOLID | layer::CAPTURE_BUBBLE}
    });
    return zenchanGO->AddComponent<ZenChan>();
}

void bb::prefabs::SpawnZenChanDead(const glm::vec3& spawnPosition)
{
    Scene* activeScene = Game::GetInstance().GetActiveLevelScene();
    if(activeScene == nullptr)
        throw std::runtime_error("Spawning Dead ZenChen with no active level scene");

    auto* deadZenChen = activeScene->AddGameObject("Dead ZenChen", spawnPosition);
    auto* spriteRenderer = deadZenChen->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("Enemys"), 0);
    deadZenChen->AddComponent<Animator>(spriteRenderer, "zenchan_dead");
    auto* rigidbody = deadZenChen->AddComponent<Rigidbody>();
    deadZenChen->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 1.0f,
        .size = {                        1.90f,1.90f                                               },
        .mask = {.category = layer::ENEMY_DEAD,
                 .collideWith = layer::TILE | layer::TILE_SEMI_SOLID | layer::INVIS_WALLS}
    });
    deadZenChen->AddComponent<DeadEnemy>(PickupType::Watermelon);

    const double angle = glm::radians(jul::math::RandomValue() > 0.5 ? jul::math::RandomRange(50.0, 70.0) + 90.0
                                                                     : jul::math::RandomRange(50.0, 70.0));
    const glm::vec2 flyDirection{ std::cos(angle), std::sin(angle) };
    const glm::vec2 velocity = flyDirection * jul::math::RandomRange(15.0f, 20.0f);
    rigidbody->AddForce(glm::vec3{ velocity.x, velocity.y, 0 }, jul::Rigidbody::ForceMode::Impulse);
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
        .restitution = 0.2f,
        .size = {                    1.80f,                                     1.95f},
        .mask = {.category = layer::PLAYER, .collideWith = layer::ALL ^ layer::PLAYER}
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

void bb::prefabs::SpawnPickup(PickupType pickup, const glm::vec3& spawnPosition)
{
    Scene* activeScene = Game::GetInstance().GetActiveLevelScene();
    if(activeScene == nullptr)
        throw std::runtime_error("Spawning ZenChen with no active level scene");

    auto* fruit = activeScene->AddGameObject("Fruit", spawnPosition);
    fruit->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("Items"), 0, glm::ivec2{ 25, 0 });
    fruit->AddComponent<Rigidbody>(jul::Rigidbody::Settings{ .mode = jul::Rigidbody::Mode::Dynamic });
    fruit->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 1.0f,
        .size = {                    1.90f,              1.90f                                           },
        .mask = {.category = layer::PICKUP,
                 .collideWith = layer::TILE | layer::TILE_SEMI_SOLID | layer::PLAYER | layer::INVIS_WALLS},
        .isSensor = false,
    });
    fruit->AddComponent<Pickup>(pickup);
}

void bb::prefabs::SpawnSideWalls(Scene& scene)
{
    auto* leftWall = scene.AddGameObject("Walls", { -Game::GRID_SIZE_X / 2 + 1, 0, 0 });
    leftWall->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 0.1f,
        .size = { 1.0, 10000 },
        .center{ 0.5f, -0.5 },
        .mask = {
                 .category = layer::INVIS_WALLS,
                 }
    });

    auto* rightWall = scene.AddGameObject("Walls", { Game::GRID_SIZE_X / 2 - 2, 0, 0 });
    rightWall->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 0.1f,
        .size = { 1.0, 10000 },
        .center{ 0.5f, -0.5 },
        .mask = {
                 .category = layer::INVIS_WALLS,
                 }
    });
}

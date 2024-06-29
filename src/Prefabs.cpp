#include "Prefabs.h"

#include <Animator.h>
#include <BoxCollider.h>
#include <Camera.h>
#include <CaptureBubble.h>
#include <CharacterInput.h>
#include <DeadEnemy.h>
#include <fmt/core.h>
#include <GameSettings.h>
#include <Health.h>
#include <Resources.h>
#include <Rigidbody.h>
#include <Scene.h>
#include <SceneManager.h>
#include <SpriteRenderer.h>
#include <TextRenderer.h>
#include <TweenEngine.h>

#include "Boulder.h"
#include "Game.h"
#include "LevelHUD.h"
#include "Maita.h"
#include "MaitaBehaviour.h"
#include "MathExtensions.h"
#include "Pickup.h"
#include "Player.h"
#include "PlayerHUD.h"
#include "ZenChan.h"
#include "ZenChanBehaviour.h"

using jul::Animator;
using jul::BoxCollider;
using jul::Camera;
using jul::GameObject;
using jul::GameSettings;
using jul::Resources;
using jul::Rigidbody;
using jul::Scene;
using jul::SpriteRenderer;
using jul::TextRenderer;
using jul::TweenEngine;


void bb::prefabs::SpawnCaptureBubble(const glm::vec3& spawnPosition, glm::vec2 velocity)
{
    Scene* activeScene = Game::GetInstance().GetActiveLevelScene();
    assert(activeScene);

    auto* bubble = activeScene->AddGameObject("AttackBubble", spawnPosition);
    bubble->AddComponent<SpriteRenderer>(Resources::GetSprite("AttackBubble"), 0);
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

void bb::prefabs::SpawnBoulder(const glm::vec3& spawnPosition, glm::vec2 velocity)
{
    Scene* activeScene = Game::GetInstance().GetActiveLevelScene();
    assert(activeScene);

    auto* boulder = activeScene->AddGameObject("Boulder", spawnPosition);
    auto* sprite = boulder->AddComponent<SpriteRenderer>(Resources::GetSprite("Enemys"), 0);
    boulder->AddComponent<Animator>(sprite, "boulder_throw", false);
    auto* rigidbody = boulder->AddComponent<Rigidbody>();
    boulder->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 0.9f,
        .size = {                       1.5,                                            1.5f},
        .mask = {.category = layer::BOULDER, .collideWith = layer::PLAYER | layer::ALL_TILES}
    });
    boulder->AddComponent<Boulder>();
    rigidbody->AddForce(glm::vec3{ velocity.x, velocity.y, 0 }, jul::Rigidbody::ForceMode::VelocityChange);

    boulder->DestroyDelayed(3.0);
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
    assert(activeScene);

    auto* zenchanGO = activeScene->AddGameObject("ZenChan", spawnPosition);
    zenchanGO->AddComponent<SpriteRenderer>(Resources::GetSprite("Enemys"), 0);
    zenchanGO->AddComponent<Animator>();
    zenchanGO->AddComponent<Rigidbody>();
    zenchanGO->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 0.1f,
        .size = {                   1.80f,                   1.90f                                          },
        .mask = {.category = layer::ENEMY,
                 .collideWith = layer::PLAYER | layer::TILE | layer::TILE_SEMI_SOLID | layer::CAPTURE_BUBBLE}
    });
    return zenchanGO->AddComponent<ZenChan>();
}

void bb::prefabs::SpawnZenChanDead(const glm::vec3& spawnPosition)
{
    Scene* activeScene = Game::GetInstance().GetActiveLevelScene();
    assert(activeScene);

    auto* deadZenChen = activeScene->AddGameObject("Dead Maita", spawnPosition);
    auto* spriteRenderer = deadZenChen->AddComponent<SpriteRenderer>(Resources::GetSprite("Enemys"), 0);
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

bb::Maita* bb::prefabs::SpawnMaitaWithBehaviour(const glm::vec3& spawnPosition)
{
    Maita* maitaPtr = SpawnMaita(spawnPosition);
    maitaPtr->GetGameObject()->AddComponent<MaitaBehaviour>(maitaPtr);
    return maitaPtr;
}

bb::Maita* bb::prefabs::SpawnMaita(const glm::vec3& spawnPosition)
{
    Scene* activeScene = Game::GetInstance().GetActiveLevelScene();
    assert(activeScene);

    auto* maitaGoPtr = activeScene->AddGameObject("Maita", spawnPosition);
    maitaGoPtr->AddComponent<SpriteRenderer>(Resources::GetSprite("Enemys"), 0);
    maitaGoPtr->AddComponent<Animator>();
    maitaGoPtr->AddComponent<Rigidbody>();
    maitaGoPtr->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 0.1f,
        .size = {                   1.80f,                   1.90f                                          },
        .mask = {.category = layer::ENEMY,
                 .collideWith = layer::PLAYER | layer::TILE | layer::TILE_SEMI_SOLID | layer::CAPTURE_BUBBLE}
    });
    return maitaGoPtr->AddComponent<Maita>();
}

void bb::prefabs::SpawnMaitaDead(const glm::vec3& spawnPosition)
{
    Scene* activeScene = Game::GetInstance().GetActiveLevelScene();
    assert(activeScene);

    auto* deadMaita = activeScene->AddGameObject("Dead Maita", spawnPosition);
    auto* spriteRenderer = deadMaita->AddComponent<SpriteRenderer>(Resources::GetSprite("Enemys"), 0);
    deadMaita->AddComponent<Animator>(spriteRenderer, "maita_dead");
    auto* rigidbody = deadMaita->AddComponent<Rigidbody>();
    deadMaita->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 1.0f,
        .size = {                        1.90f,1.90f                                               },
        .mask = {.category = layer::ENEMY_DEAD,
                 .collideWith = layer::TILE | layer::TILE_SEMI_SOLID | layer::INVIS_WALLS}
    });
    deadMaita->AddComponent<DeadEnemy>(PickupType::Fries);

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

    auto* bodySprite = playerGameObject->AddComponent<SpriteRenderer>(Resources::GetSprite(spriteName), 0);
    auto* bubbleSprite = playerGameObject->AddComponent<SpriteRenderer>(Resources::GetSprite("BubbleLarge"), 0);
    auto* bodyAnimator = playerGameObject->AddComponent<Animator>(bodySprite);
    auto* bubbleAnimator = playerGameObject->AddComponent<Animator>(bubbleSprite);
    playerGameObject->AddComponent<Rigidbody>();
    playerGameObject->AddComponent<BoxCollider>(BoxCollider::Settings{
        .friction = 0.0f,
        .restitution = 0.15f,
        .size = {                    1.80f,                                     1.95f},
        .mask = {.category = layer::PLAYER, .collideWith = layer::ALL ^ layer::PLAYER}
    });
    playerGameObject->AddComponent<Health>(Game::STARTING_LIVES, playerIndex);
    auto* playerPtr =
        playerGameObject->AddComponent<Player>(playerIndex, bodySprite, bubbleSprite, bodyAnimator, bubbleAnimator);

    return playerPtr;
}

void bb::prefabs::SpawnPlayerHUD(jul::Scene& scene, int playerIndex)
{
    GameObject* player1Hud = scene.AddGameObject(fmt::format("Player {}", playerIndex));
    {
        auto* livesGameObject = scene.AddGameObject("LivesText", { playerIndex ? 14 : -15, -12, 0 });
        auto* livesText = livesGameObject->AddComponent<TextRenderer>(
            "error", Resources::GetFont("NES"), 100, glm::vec2{ 0, 0 });
        livesGameObject->AddComponent<SpriteRenderer>(
            Resources::GetSprite("LevelTiles"), 90, glm::ivec2{ 4, 20 });


        auto* scoreGameObject = scene.AddGameObject("ScoreText", { playerIndex ? 4 : -4, 13, 0 });
        auto* scoreText = scoreGameObject->AddComponent<TextRenderer>(
            "error", Resources::GetFont("NES"), -100, glm ::vec2{ playerIndex ? 0 : 1, 0 });


        livesGameObject->GetTransform().SetParent(&player1Hud->GetTransform(), false);
        scoreGameObject->GetTransform().SetParent(&player1Hud->GetTransform(), false);

        player1Hud->AddComponent<PlayerHUD>(scoreText, livesText, playerIndex);
    }
}

void bb::prefabs::SpawnMainCamera(jul::Scene& scene)
{
    auto* cameraGameObject = scene.AddGameObject("Camera");
    auto* cameraPtr = cameraGameObject->AddComponent<Camera>(14.0f, GameSettings::GetAspectRatio());
    cameraGameObject->GetTransform().SetWorldPosition({ 0, 0, 0 });
    Game::GetInstance().SetMainCamera(cameraPtr);
}

void bb::prefabs::SpawnPickup(PickupType pickup, const glm::vec3& spawnPosition)
{
    Scene* activeScene = Game::GetInstance().GetActiveLevelScene();
    assert(activeScene);

    glm::ivec2 itemCell{};

    if(pickup == PickupType::Fries)
        itemCell = { 25, 0 };
    else if(pickup == PickupType::Watermelon)
        itemCell = { 19, 0 };


    auto* fruit = activeScene->AddGameObject("Fruit", spawnPosition);
    fruit->AddComponent<SpriteRenderer>(Resources::GetSprite("Items"), 0, itemCell);
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

void bb::prefabs::SpawnScoreText(const glm::vec3& spawnPosition, int score, const SDL_Color& color)
{
    Scene* activeScene = Game::GetInstance().GetActiveLevelScene();
    assert(activeScene);

    auto* scoreText = activeScene->AddGameObject("ScoreText", spawnPosition);
    scoreText->AddComponent<TextRenderer>(
        std::to_string(score), Resources::GetFont("NES"), 0, glm::vec2{ 0.5, 0.5 }, true, 1.0, color);

    TweenEngine::Start({ .from = scoreText->GetTransform().GetWorldPosition().y,
                         .to = scoreText->GetTransform().GetWorldPosition().y + 4.0,
                         .duration = 2.0,
                         .invokeWhenDestroyed = false,
                         .onUpdate =
                             [scoreText](double value)
                         {
                             glm::vec3 targetPosition = scoreText->GetTransform().GetWorldPosition();
                             targetPosition.y = static_cast<float>(value);
                             scoreText->GetTransform().SetWorldPosition(targetPosition);
                         },
                         .onEnd = [scoreText]() { scoreText->Destroy(); } },
                       scoreText);
}

void bb::prefabs::SpawnLevelHUD(Scene& scene)
{
    auto* levelHudGoPtr = scene.AddGameObject("Level HUD");
    levelHudGoPtr->AddComponent<LevelHUD>();
}

#include "Scenes.h"

#include <Animator.h>
#include <AutoRotateAround.h>
#include <BoxCollider.h>
#include <Camera.h>
#include <GameObject.h>
#include <GameSettings.h>
#include <GameTime.h>
#include <MainMenu.h>
#include <OneWayPlatform.h>
#include <Player.h>
#include <PlayerHUD.h>
#include <ResourceManager.h>
#include <Rigidbody.h>
#include <SceneManager.h>
#include <SpriteRenderer.h>
#include <TextRenderer.h>
#include <TweenEngine.h>
#include <ZenChan.h>

#include <numbers>
#include <vector>

#include "BubbleSpiral.h"
#include "CharacterInput.h"
#include "FpsCounter.h"
#include "Game.h"
#include "LevelState.h"
#include "Prefabs.h"
#include "ScoreScreen.h"
#include "Transform.h"

void bb::scenes::BindScenes()
{
    SceneManager::GetInstance().BindScene((int)Id::MainMenu, MainMenuScene);

    SceneManager::GetInstance().BindScene((int)Id::OnePlayerMode, OnePlayerModeScene);
    SceneManager::GetInstance().BindScene((int)Id::TwoPlayerMode, TwoPlayerModeScene);
    SceneManager::GetInstance().BindScene((int)Id::VersusMode, VersusModeScene);

    SceneManager::GetInstance().BindScene((int)Id::IntroLevel, IntroLevelScene);
    SceneManager::GetInstance().BindScene((int)Id::Level1, Level1Scene);
    SceneManager::GetInstance().BindScene((int)Id::Level2, Level2Scene);
    SceneManager::GetInstance().BindScene((int)Id::Level3, Level3Scene);

    SceneManager::GetInstance().BindScene((int)Id::ScoreScreen, ScoreScene);
}

void bb::scenes::AssertModeSceneLoaded()
{
    assert((SceneManager::GetPrimaryScene().GetId() == (int)scenes::Id::OnePlayerMode or
            SceneManager::GetPrimaryScene().GetId() == (int)scenes::Id::TwoPlayerMode or
            SceneManager::GetPrimaryScene().GetId() == (int)scenes::Id::VersusMode) &&
           "Loaded Scene Requires Mode Scene To Be Loaded!");
}


void bb::scenes::OnePlayerModeScene(Scene& scene)
{
    prefabs::SpawnMainCamera(scene);

    auto* player1 = prefabs::SpawnPlayer(scene, 0, { -3, 0, 0 });
    player1->GetGameObject()->AddComponent<CharacterInput>(true, 0);

    prefabs::SpawnPlayerHUD(scene, 0);
    prefabs::SpawnLevelHUD(scene);
    prefabs::SpawnSideWalls(scene);

    Game::GetInstance().TryTransitionLevel(0, false, false);
}

void bb::scenes::TwoPlayerModeScene(Scene& scene)
{
    prefabs::SpawnMainCamera(scene);

    auto* player1 = prefabs::SpawnPlayer(scene, 0, { -3, 0, 0 });
    player1->GetGameObject()->AddComponent<CharacterInput>(false, 0);

    auto* player2 = prefabs::SpawnPlayer(scene, 1, { 3, 0, 0 });
    player2->GetGameObject()->AddComponent<CharacterInput>(true, 1);

    prefabs::SpawnPlayerHUD(scene, 0);
    prefabs::SpawnPlayerHUD(scene, 1);
    prefabs::SpawnLevelHUD(scene);
    prefabs::SpawnSideWalls(scene);

    Game::GetInstance().TryTransitionLevel(0, false, false);
}

void bb::scenes::VersusModeScene(Scene& scene)
{
    prefabs::SpawnMainCamera(scene);

    auto* player2 = prefabs::SpawnPlayer(scene, 0, { -3, 0, 0 });
    player2->GetGameObject()->AddComponent<CharacterInput>(true, 0);

    prefabs::SpawnPlayerHUD(scene, 0);
    prefabs::SpawnPlayerHUD(scene, 1);
    prefabs::SpawnLevelHUD(scene);
    prefabs::SpawnSideWalls(scene);

    Game::GetInstance().TryTransitionLevel(0, false, false);
}


void bb::scenes::MainMenuScene(Scene& scene)
{
    prefabs::SpawnMainCamera(scene);
    auto* mainMenuGO = scene.AddGameObject("MainMenu");

    ////////////////////
    /// INTRO SCREEN ///
    ////////////////////
    auto* introScreen = scene.AddGameObject("Intro Screen", {}, mainMenuGO);

    auto* logo = scene.AddGameObject("Logo", { 0, 0, 0 }, introScreen);
    logo->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("Logo"), 1);

    auto* intoScreenInfo = scene.AddGameObject("Into Screen Info", { 0, 0, 0 }, introScreen);
    {
        auto* julgenLogo = scene.AddGameObject("Julgen_Retro_Logo", { 0, -5, 0 }, intoScreenInfo);
        julgenLogo->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("Julgen_Retro_Logo"), 1);

        auto* introText = scene.AddGameObject("Main Menu Text", { 0, -8, 0 }, intoScreenInfo);
        introText->AddComponent<TextRenderer>(
            "Made With Julgen", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

        auto* nameText = scene.AddGameObject("Name Text", { 0, -10, 0 }, intoScreenInfo);
        nameText->AddComponent<TextRenderer>(
            "Created By Julian Rijken", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);
    }
    intoScreenInfo->SetActive(false);


    /////////////////////
    /// SELECT SCREEN ///
    /////////////////////
    GameObject* selectBubble{ nullptr };
    GameObject* p1Text{ nullptr };
    GameObject* p2Text{ nullptr };
    GameObject* vsText{ nullptr };
    GameObject* leaderBoardText{ nullptr };
    auto* selectScreen = scene.AddGameObject("Select Screen", {}, mainMenuGO);
    {
        selectBubble = scene.AddGameObject("Select Bugble", { -7, 8, 0 }, selectScreen);
        selectBubble->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("AttackBubble"), 1);
        selectBubble->AddComponent<Animator>(nullptr, "Idle");

        auto* titleText = scene.AddGameObject("Text", { -5, 8, 0 }, selectScreen);
        titleText->AddComponent<TextRenderer>(
            "BUBBLE BOBBLE", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.0f, 0.5f }, true);

        p1Text = scene.AddGameObject("Text", { -5, 6, 0 }, selectScreen);
        p1Text->AddComponent<TextRenderer>(
            "1P START", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.0f, 0.5f }, true);

        p2Text = scene.AddGameObject("Text", { -5, 4, 0 }, selectScreen);
        p2Text->AddComponent<TextRenderer>(
            "2P START", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.0f, 0.5f }, true);

        vsText = scene.AddGameObject("Text", { -5, 2, 0 }, selectScreen);
        vsText->AddComponent<TextRenderer>(
            "VS START", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.0f, 0.5f }, true);

        leaderBoardText = scene.AddGameObject("Text", { -5, 0, 0 }, selectScreen);
        leaderBoardText->AddComponent<TextRenderer>(
            "LEADERBOARD", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.0f, 0.5f }, true);
    }
    selectScreen->SetActive(false);


    mainMenuGO->AddComponent<MainMenu>(&logo->GetTransform(),
                                       intoScreenInfo,
                                       introScreen,
                                       selectScreen,
                                       &selectBubble->GetTransform(),
                                       std::vector<Transform*>{ &p1Text->GetTransform(),
                                                                &p2Text->GetTransform(),
                                                                &vsText->GetTransform(),
                                                                &leaderBoardText->GetTransform() });
}


void bb::scenes::IntroLevelScene(Scene& scene)
{
    scenes::AssertModeSceneLoaded();

    constexpr double rotateDistance = 3.5;
    constexpr double rotateSpeed = 3.0;
    constexpr double heightOffset = -3.0;
    constexpr double sidewaysOffset = 6.0;
    constexpr double intoDuration = 7.0;

    auto* nameText = SceneManager::GetPrimaryScene().AddGameObject("Name Text", glm::vec3{ 0, 13, 0 });
    nameText->AddComponent<TextRenderer>("Now it is the beginning of\n"
                                         "A fantastic story! let us\n"
                                         "make A journey to\n"
                                         "the cave of monsters!\n \n"
                                         "Good Luck!",
                                         ResourceManager::GetFont("NES"),
                                         100,
                                         glm ::vec2{ 0.5f, 0.0f },
                                         true);


    auto* bubbleSpiral = scene.AddGameObject("Bubble Spiral");
    bubbleSpiral->AddComponent<BubbleSpiral>();
    // Auto destroy bubble spiral
    TweenEngine::Start({ .delay = intoDuration * 0.6,
                         .invokeWhenDestroyed = false,
                         .onEnd = [bubbleSpiral]() { bubbleSpiral->Destroy(); } },
                       bubbleSpiral);


    TweenEngine::Start({ .delay = intoDuration,
                         .duration = Game::LEVEL_TRANSITION_DURATION,
                         .invokeWhenDestroyed = false,
                         .onStart = [nameText]() { nameText->GetTransform().Translate(0, Game::GRID_SIZE_Y, 0); },
                         .onEnd = [nameText]() { nameText->Destroy(); } },
                       nameText);

    if(Game::GetInstance().GetPlayer(0) != nullptr)
    {
        TweenEngine::Start(
            {
                .duration = intoDuration,
                .onUpdate =
                    [](double)
                {
                    glm::vec2 circleOffset{ std::cos(GameTime::GetElapsedTime() * rotateSpeed) * rotateDistance,
                                            std::sin(GameTime::GetElapsedTime() * rotateSpeed) * rotateDistance };

                    circleOffset.y += heightOffset;
                    circleOffset.x -= sidewaysOffset;
                    Game::GetInstance().GetPlayer(0)->GetTransform().SetWorldPosition(
                        circleOffset.x, circleOffset.y, 0);
                },
            },
            Game::GetInstance().GetPlayer(0));
    }

    if(Game::GetInstance().GetPlayer(1) != nullptr)
    {
        TweenEngine::Start(
            {
                .duration = intoDuration,
                .onUpdate =
                    [](double)
                {
                    glm::vec2 circleOffset{
                        std::cos(GameTime::GetElapsedTime() * rotateSpeed + std::numbers::pi) * rotateDistance,
                        std::sin(GameTime::GetElapsedTime() * rotateSpeed + std::numbers::pi) * rotateDistance
                    };

                    circleOffset.y += heightOffset;
                    circleOffset.x += sidewaysOffset;
                    Game::GetInstance().GetPlayer(1)->GetTransform().SetWorldPosition(
                        circleOffset.x, circleOffset.y, 0);
                },
            },
            Game::GetInstance().GetPlayer(1));
    }

    TweenEngine::Start(
        {
            .duration = intoDuration,
            .invokeWhenDestroyed = false,
            .onEnd = []() { Game::GetInstance().TryTransitionLevel(1); },
        },
        nameText);
}

void bb::scenes::Level1Scene(Scene& scene)
{
    scenes::AssertModeSceneLoaded();

    const std::vector<LevelState::EnemySpawn> enemys{
        {.type = EnemyType::ZenChan, .delay = 0, .location = glm::vec3{ -2, 6, 0 }},
        {  .type = EnemyType::Maita, .delay = 1,  .location = glm::vec3{ 0, 6, 0 }},
        {.type = EnemyType::ZenChan, .delay = 2,  .location = glm::vec3{ 2, 6, 0 }}
    };

    auto* levelStateGoPtr = scene.AddGameObject("Level1State");
    levelStateGoPtr->AddComponent<LevelState>(enemys);
}

void bb::scenes::Level2Scene(Scene& scene)
{
    scenes::AssertModeSceneLoaded();

    const std::vector<LevelState::EnemySpawn> enemys{
        {.type = EnemyType::ZenChan, .delay = 0,  .location = glm::vec3{ -4, 4, 0 }},
        {.type = EnemyType::ZenChan, .delay = 0, .location = glm::vec3{ -1, 10, 0 }},
        {.type = EnemyType::ZenChan, .delay = 0,  .location = glm::vec3{ 1, 10, 0 }},
        {.type = EnemyType::ZenChan, .delay = 0,   .location = glm::vec3{ 4, 4, 0 }}
    };

    auto* levelStateGoPtr = scene.AddGameObject("Level2State");
    levelStateGoPtr->AddComponent<LevelState>(enemys);
}

void bb::scenes::Level3Scene(Scene& scene)
{
    scenes::AssertModeSceneLoaded();

    const std::vector<LevelState::EnemySpawn> enemys{
        {.type = EnemyType::ZenChan, .delay = 2, .location = glm::vec3{ -6, 4, 0 }},
        {.type = EnemyType::ZenChan, .delay = 0, .location = glm::vec3{ -4, 8, 0 }},
        {.type = EnemyType::ZenChan, .delay = 2,  .location = glm::vec3{ 4, 8, 0 }},
        {.type = EnemyType::ZenChan, .delay = 0,  .location = glm::vec3{ 6, 4, 0 }}
    };

    auto* levelStateGoPtr = scene.AddGameObject("Level3State");
    levelStateGoPtr->AddComponent<LevelState>(enemys);
}


void bb::scenes::ScoreScene(Scene& scene)
{
    prefabs::SpawnMainCamera(scene);

    auto* scoreScreen = scene.AddGameObject("ScoreScreen");
    scoreScreen->AddComponent<ScoreScreen>();
}

#include "Scenes.h"

#include <Animator.h>
#include <AutoRotateAround.h>
#include <BoxCollider.h>
#include <Camera.h>
#include <GameObject.h>
#include <GameSettings.h>
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

#include <vector>

#include "FpsCounter.h"
#include "Game.h"
#include "Transform.h"

void bb::MainScene(Scene& scene)
{
    auto* cameraGameObject = scene.AddGameObject("Camera");
    cameraGameObject->AddComponent<Camera>(14, GameSettings::GetAspectRatio());
    cameraGameObject->GetTransform().SetWorldPosition({ 0, 2, 0 });


    TweenEngine::Start(
        {
            .from = -26,
            .to = 0,
            .duration = 2.0,
            .easeFunction = EaseFunction::SineOut,
            .onUpdate = [cameraGameObject](double value)
            { cameraGameObject->GetTransform().SetWorldPosition(0, value, 0); },
        },
        cameraGameObject);


    auto* nameText = scene.AddGameObject("Name Text", { 0, 5, 0 });
    nameText->AddComponent<TextRenderer>("Now it is the beginning of\
A fantastic story! let us\
make A journy to\
the cave of monsters!\
Good Luck!",
                                         ResourceManager::GetFont("NES"),
                                         100,
                                         glm ::vec2{ 0.5f, 0.5f },
                                         true);


    // Player 1
    Game::SpawnPlayer(scene, 0, { 3, 10, 0 });

    // Player 2
    auto* player2GameObject = Game::SpawnPlayer(scene, 1, { 3, 10, 0 });

    TweenEngine::Start({ .delay = 2.0,
                         .to = 6.28,
                         .duration = 6.0,
                         .easeFunction = EaseFunction::Linear,
                         .onUpdate =
                             [player2GameObject](double value)
                         {
                             const glm::vec2 position{ std::cos(value) * 10.0f, std::sin(value) * 10.0f };
                             player2GameObject->GetTransform().SetWorldPosition(0, 0, 0);
                         },
                         .onEnd =
                             [player2GameObject]() {
                                 player2GameObject->BubbleToPosition({ -12, -11, 0 }, 3.0f);
                             } },
                       player2GameObject);


    Game::SpawnLevel(scene, 1, { 0, -26, 0 });


    // auto* zenchanGO = scene.AddGameObject("ZenChan", { 3, 5, 0 });
    // zenchanGO->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("Enemys"),0);
    // zenchanGO->AddComponent<Animator>();
    // zenchanGO->AddComponent<Rigidbody>();
    // zenchanGO->AddComponent<BoxCollider>(BoxCollider::Settings{
    //     .friction = 0.0f,
    //     .restitution = 0.1f,
    //     .size = {1.90f, 1.90f},
    // });
    // zenchanGO->AddComponent<ZenChan>();

    // GameObject* player1Hud = scene.AddGameObject("Player1HUD");
    // {
    //     auto* livesGameObject = scene.AddGameObject("LivesText", { -15, -12, 0 });
    //     auto* livesText = livesGameObject->AddComponent<TextRenderer>(
    //         "error", ResourceManager::GetFont("NES"), 100, glm::vec2{ 0, 0 });
    //     livesGameObject->AddComponent<SpriteRenderer>(
    //         ResourceManager::GetSprite("LevelTiles"), 90, glm::ivec2{ 4, 20 });


    //     auto* scoreGameObject = scene.AddGameObject("ScoreText", { -4, 13, 0 });
    //     auto* scoreText = scoreGameObject->AddComponent<TextRenderer>(
    //         "error", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 1, 0 });


    //     livesGameObject->GetTransform().SetParent(&player1Hud->GetTransform(), false);
    //     scoreGameObject->GetTransform().SetParent(&player1Hud->GetTransform(), false);

    //     player1Hud->AddComponent<PlayerHUD>(
    //         Game::GetInstance().GetPlayer(0), scoreText, livesText, SDL_Color(255, 255, 255, 255));
    // }

    // GameObject* player2Hud = scene.AddGameObject("Player2HUD");
    // {
    //     auto* livesGameObject = scene.AddGameObject("LivesText", { 14, -12, 0 });
    //     auto* livesText = livesGameObject->AddComponent<TextRenderer>(
    //         "error", ResourceManager::GetFont("NES"), 100, glm::vec2{ 0, 0 });
    //     livesGameObject->AddComponent<SpriteRenderer>(
    //         ResourceManager::GetSprite("LevelTiles"), 90, glm::ivec2{ 4, 20 });


    //     auto* scoreGameObject = scene.AddGameObject("ScoreText", { 4, 13, 0 });
    //     auto* scoreText = scoreGameObject->AddComponent<TextRenderer>(
    //         "error", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0, 0 });


    //     livesGameObject->GetTransform().SetParent(&player2Hud->GetTransform(), false);
    //     scoreGameObject->GetTransform().SetParent(&player2Hud->GetTransform(), false);

    //     player2Hud->AddComponent<PlayerHUD>(
    //         Game::GetInstance().GetPlayer(1), scoreText, livesText, SDL_Color(255, 255, 255, 255));
    // }
}

void bb::Levels(Scene&) {}

void bb::MainMenuScene(Scene& scene)
{
    // auto* fpsCounter = scene.AddGameObject("FPS_Counter", { -12, 12, 0 });
    // fpsCounter->AddComponent<TextRenderer>("0", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f },
    // true); fpsCounter->AddComponent<FpsCounter>();
    auto* cameraGameObject = scene.AddGameObject("Camera");
    cameraGameObject->AddComponent<Camera>(14, GameSettings::GetAspectRatio());


    ////////////////////
    /// INTRO SCREEN ///
    ////////////////////
    auto* introScreen = scene.AddGameObject("Intro Screen");

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
    GameObject* selectBubble;
    GameObject* p1Text;
    GameObject* p2Text;
    GameObject* p3Text;
    auto* selectScreen = scene.AddGameObject("Select Screen");
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

        p3Text = scene.AddGameObject("Text", { -5, 2, 0 }, selectScreen);
        p3Text->AddComponent<TextRenderer>(
            "VERSUS", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.0f, 0.5f }, true);
    }
    selectScreen->SetActive(false);


    auto* mainMenuGO = scene.AddGameObject("MainMenu");
    mainMenuGO->AddComponent<MainMenu>(
        &logo->GetTransform(),
        intoScreenInfo,
        introScreen,
        selectScreen,
        &selectBubble->GetTransform(),
        std::vector<Transform*>{ &p1Text->GetTransform(), &p2Text->GetTransform(), &p3Text->GetTransform() });
}

void bb::TestScene(Scene&) {}

void bb::SceneGraphTestScene(Scene& scene)
{
    GameObject* bubbleBase = scene.AddGameObject("Bubble");
    bubbleBase->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("Dot"), 1);
    bubbleBase->AddComponent<AutoRotateAround>(4.0f, 4.0f);


    const GameObject* lastBubble = bubbleBase;
    for(int i = 0; i < 10; ++i)
    {
        GameObject* bubble = scene.AddGameObject("Bubble");
        bubble->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("AttackBubble"), i);
        bubble->AddComponent<AutoRotateAround>(1.0f, 0.1f * i);
        bubble->GetTransform().SetParent(&lastBubble->GetTransform());

        if(i == 4)
            bubble->SetActive(false);

        if(i == 6)
        {
            // TODO: A issue showed up here
            // because the visibility chagnes instantly instead of at the end of the frame
            // The positon the rotate around had will be shown for a frame as that is in Unpdate
            // The tween on end happends afte rupdate so now later update is needed in the rotate around

            // Show main menu logo
            TweenEngine::Start(
                {
                    .duration = 2.0f,
                    .onEnd = [bubbleBase, bubble]() { bubble->GetTransform().SetParent(&bubbleBase->GetTransform()); },
                },
                nullptr);
        }

        lastBubble = bubble;
    }
}

void bb::BindScenes()
{
    SceneManager::GetInstance().BindScene("Main", MainScene);
    SceneManager::GetInstance().BindScene("MainMenu", MainMenuScene);
}

#include <Animator.h>
#include <Box2D/Box2D.h>
#include <fmt/core.h>
#include <GameSettings.h>
#include <Input.h>
#include <Julgen.h>
#include <Locator.h>
#include <MathExtensions.h>
#include <MessageQueue.h>
#include <Physics.h>
#include <ResourceManager.h>
#include <Rigidbody.h>
#include <SceneManager.h>
#include <Sound.h>
#include <TextRenderer.h>

#include "AutoMove.h"
#include "Game.h"
#include "OneWayPlatform.h"
#include "Player.h"
#include "PlayerHUD.h"
#include "ZenChan.h"

using namespace jul;
using namespace bb;

class PlayerInputCommand final : public BaseCommand
{
    using InputFunction = std::function<void(const InputContext&)>;

public:
    PlayerInputCommand(int playerIndex, void (Player::*memberFunction)(const InputContext&)) :
        m_Function(
            [playerIndex, memberFunction](const InputContext& context)
            {
                Player* player = Game::GetInstance().GetPlayer(playerIndex);
                if(player == nullptr)
                    return;

                (player->*memberFunction)(context);
            })
    {
    }

    void Execute(const InputContext& context) override { m_Function(context); }

private:
    InputFunction m_Function;
};

void LoadResources()
{
    ResourceManager::BindSound(Sounds::GameStart, "SFX/The Quest Begins.ogg", true);
    ResourceManager::BindSound(Sounds::FireBubble, "SFX/Bubble Bobble SFX (2).wav");
    ResourceManager::BindSound(Sounds::Death, "SFX/Bubble Bobble SFX (3).wav");
    ResourceManager::BindSound(Sounds::Jump, "SFX/Jump.wav");


    ResourceManager::LoadFont("Lingua", "Lingua.otf", 36);
    ResourceManager::LoadFont("LinguaSmall", "Lingua.otf", 16);
    ResourceManager::LoadFont("NES", "NES_Font.ttf", 8);

    ResourceManager::LoadSprite("background", "background.tga", 32);
    ResourceManager::LoadSprite("Dot", "Dot.png", 32);

    ResourceManager::LoadSprite("DebugCube", "DebugCube.png", 4, { 0, 0 });

    ResourceManager::LoadSprite("AttackBubble",
                                "AttackBubble.png",
                                8,
                                {
                                    0.5f, 0.5f
    },
                                1,
                                7,
                                { { "Spawn", SpriteAnimation{ { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 } }, 12 } },
                                  { "Idle", SpriteAnimation{ { { 3, 0 }, { 4, 0 } }, 12 } },
                                  { "Pop", SpriteAnimation{ { { 5, 0 }, { 6, 0 } }, 8 } } });

    ResourceManager::LoadSprite("LevelTiles", "LevelTiles.png", 8, { 0.0f, 0.0f }, 25, 5);

    // TODO: as you can see animations around bound to a sprite
    //       in this senario you can see how that's not optimal
    ResourceManager::LoadSprite("BubbleCharacter",
                                "BubbleCharacter.png",
                                8,
                                {
                                    0.5f, 0.5f
    },
                                4,
                                6,
                                { { "Idle", SpriteAnimation{ { { 0, 0 }, { 1, 0 } }, 7 } },
                                  { "Walk", SpriteAnimation{ { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 } }, 7 } },
                                  { "Jump", SpriteAnimation{ { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 } }, 4 } },
                                  { "Falling", SpriteAnimation{ { { 2, 1 }, { 3, 1 } }, 4 } },
                                  { "Death",
                                    SpriteAnimation{ { { 0, 3 },
                                                       { 1, 3 },
                                                       { 2, 3 },
                                                       { 3, 3 },
                                                       { 0, 3 },
                                                       { 1, 3 },
                                                       { 2, 3 },
                                                       { 3, 3 },
                                                       { 4, 3 },
                                                       { 5, 3 },
                                                       { 6, 3 } },
                                                     10 } },
                                  { "Attack", SpriteAnimation{ { { 0, 2 } }, 6 } } });

    ResourceManager::LoadSprite("BobbleCharacter",
                                "BobbleCharacter.png",
                                8,
                                {
                                    0.5f, 0.5f
    },
                                4,
                                6,
                                { { "Idle", SpriteAnimation{ { { 0, 0 }, { 1, 0 } }, 7 } },
                                  { "Walk", SpriteAnimation{ { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 } }, 7 } },
                                  { "Jump", SpriteAnimation{ { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 } }, 4 } },
                                  { "Falling", SpriteAnimation{ { { 2, 1 }, { 3, 1 } }, 4 } },
                                  { "Death",
                                    SpriteAnimation{ { { 0, 3 },
                                                       { 1, 3 },
                                                       { 2, 3 },
                                                       { 3, 3 },
                                                       { 0, 3 },
                                                       { 1, 3 },
                                                       { 2, 3 },
                                                       { 3, 3 },
                                                       { 4, 3 },
                                                       { 5, 3 },
                                                       { 6, 3 } },
                                                     10 } },
                                  { "Attack", SpriteAnimation{ { { 0, 2 } }, 6 } } });


    ResourceManager::LoadSprite("BubbleParticle",
                                "BubbleParticle.png",
                                8,
                                {
                                    0.5f, 0.5f
    },
                                1,
                                4,
                                {
                                    { "Twinkle", SpriteAnimation{ { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 } }, 4 } },
                                });

    ResourceManager::LoadSprite("Enemys",
                                "Enemys.png",
                                8,
                                {
                                    0.5f, 0.5f
    },
                                12,
                                16,
                                {
                                    { "zenchan_normal", SpriteAnimation{ { { 0, 0 }, { 1, 0 } }, 4 } },
                                });
}

void MainMenuScene(Scene& scene)
{
    for(int x = -16; x < 16; ++x)
    {
        for(int y = -15; y < 15; ++y)
        {
            if(math::RandomValue<float>() < 0.95f)
                continue;

            auto* levelTile = scene.AddGameObject("LevelTile", { x, y, 0 });
            levelTile->AddComponent<SpriteRenderer>(
                ResourceManager::GetSprite("LevelTiles"), -100, glm::ivec2{ 4, jul::math::RandomRange(13, 14) });
            levelTile->AddComponent<AutoMove>(glm::vec3{ 0.0f, 2.0f, 0 }, glm::vec3{ 16, 15, 1 });

            auto* bubbleParticle = scene.AddGameObject("Bubble Particle", { x, y, 0 });
            bubbleParticle->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("BubbleParticle"), -90);
            bubbleParticle->AddComponent<Animator>()->PlayAnimation("Twinkle", true, jul::math::RandomValue<float>());
            bubbleParticle->AddComponent<AutoMove>(
                glm::vec3{ jul::math::RandomRange(-1.0f, 1.0f), jul::math::RandomRange(2.0f, 4.0f), 0 },
                glm::vec3{ 16, 15, 1 });
        }
    }
}

void MainScene(Scene& scene)
{
    // // Player 1
    // auto* player1GameObject = scene.AddGameObject("BubbleCharacter", { -3, 10, 0 });
    // player1GameObject->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("BubbleCharacter"), 0);
    // player1GameObject->AddComponent<Animator>();
    // player1GameObject->AddComponent<Rigidbody>();
    // player1GameObject->AddComponent<BoxCollider>(BoxCollider::Settings{
    //     .friction = 0.0f,
    //     .restitution = 0.1f,
    //     .size = {1.80f, 1.95f},
    // });
    // player1GameObject->AddComponent<bb::Player>(0);

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


    // auto* scoreInfoText = scene.AddGameObject("InfoText", { 30, 70, 0 });
    // scoreInfoText->AddComponent<TextRenderer>(
    //     "Attack: Z or gamepad A", ResourceManager::GetFont("NESSmall"), 100, true);

    // auto* livesInfoText = scene.AddGameObject("InfoText", { 30, 90, 0 });
    // livesInfoText->AddComponent<TextRenderer>("Kill: X or gamepad B", ResourceManager::GetFont("NESSmall"), 100,
    // true);

    // auto* moveInfoText = scene.AddGameObject("InfoText", { 30, 110, 0 });
    // moveInfoText->AddComponent<TextRenderer>(
    //     "MOVE PLAYER: A-D OR D-PAD OR L-STICK", ResourceManager::GetFont("NESSmall"), 100, true);


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

void TestScene(Scene& scene)
{
    auto* go = scene.AddGameObject("BubbleCharacter", { -2, 0, 0 });
    go->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("BubbleCharacter"), 0);
    go->AddComponent<Animator>();
    go->AddComponent<Rigidbody>(Rigidbody::Settings{});
    go->AddComponent<bb::Player>(1);
}

void InitControls()
{
    Input::RegisterCommand<PlayerInputCommand>((int)InputBind::MoveLeft, 0, false, 0, &bb::Player::OnMoveLeftInput);
    Input::RegisterCommand<PlayerInputCommand>((int)InputBind::MoveRight, 0, false, 0, &bb::Player::OnMoveRightInput);
    Input::RegisterCommand<PlayerInputCommand>((int)InputBind::MoveStick, 0, false, 0, &bb::Player::OnMoveStickInput);
    Input::RegisterCommand<PlayerInputCommand>((int)InputBind::Attack, 0, false, 0, &bb::Player::OnAttackInput);
    Input::RegisterCommand<PlayerInputCommand>((int)InputBind::Jump, 0, false, 0, &bb::Player::OnJumpInput);

    Input::RegisterCommand<PlayerInputCommand>((int)InputBind::MoveLeft, 1, true, 1, &bb::Player::OnMoveLeftInput);
    Input::RegisterCommand<PlayerInputCommand>((int)InputBind::MoveRight, 1, true, 1, &bb::Player::OnMoveRightInput);
    Input::RegisterCommand<PlayerInputCommand>((int)InputBind::MoveStick, 1, true, 1, &bb::Player::OnMoveStickInput);
    Input::RegisterCommand<PlayerInputCommand>((int)InputBind::Attack, 1, true, 1, &bb::Player::OnAttackInput);
    Input::RegisterCommand<PlayerInputCommand>((int)InputBind::Jump, 1, true, 1, &bb::Player::OnJumpInput);

    Input::RegisterCommand<MuteGameCommand>((int)InputBind::ToggleSound, 1, true);
}

void jul::Julgen::PreInit()
{
    // 32 by 28 tiles
    GameSettings::g_WindowTitle = "Bubble Bobble Made In Julgen";
    GameSettings::g_RenderWidth = 32 * 8;
    GameSettings::g_RenderHeight = 28 * 8;


    GameSettings::g_WindowWidth = 32 * 8 * 4;
    GameSettings::g_WindowHeight = 28 * 8 * 4;
    GameSettings::g_ShowConsole = true;


    Input::AddAction(InputBind::TestLivesButton, { { SDL_SCANCODE_X }, { SDL_CONTROLLER_BUTTON_B }, {} });
    Input::AddAction(
        InputBind::Jump,
        {
            { SDL_SCANCODE_SPACE, SDL_SCANCODE_W, SDL_SCANCODE_X, SDL_SCANCODE_UP },
            { SDL_CONTROLLER_BUTTON_B },
            {}
    });
    Input::AddAction(InputBind::MoveLeft,
                     {
                         { SDL_SCANCODE_A, SDL_SCANCODE_LEFT },
                         { SDL_CONTROLLER_BUTTON_DPAD_LEFT },
                         {}
    });
    Input::AddAction(InputBind::MoveRight,
                     {
                         { SDL_SCANCODE_D, SDL_SCANCODE_RIGHT },
                         { SDL_CONTROLLER_BUTTON_DPAD_RIGHT },
                         {}
    });
    Input::AddAction(InputBind::MoveStick, { {}, {}, { SDL_CONTROLLER_AXIS_LEFTX } });
    Input::AddAction(InputBind::Attack,
                     {
                         { SDL_SCANCODE_E, SDL_SCANCODE_Z },
                         { SDL_CONTROLLER_BUTTON_A },
                         {},
    });

    Input::AddAction(InputBind::ToggleSound, { { SDL_SCANCODE_M }, {}, {} });

    InitControls();
}

void jul::Julgen::GameStart()
{
    // going for https://www.youtube.com/watch?v=VyK_cpp9pT4
    // BubbleBobble NES
    LoadResources();

    MessageQueue::Broadcast(MessageType::GameStart);


    SceneManager::GetInstance().LoadScene("mainScene", MainScene);
    SceneManager::GetInstance().LoadScene("mainMenu", MainMenuScene, SceneLoadMode::Additive);

    // // Unload and load for testing purpouses
    // SceneManager::GetInstance().UnloadScene("mainScene");

    // SceneManager::GetInstance().LoadScene("mainScene", MainScene);
    // SceneManager::GetInstance().LoadScene("mainMenu", MainMenuScene, SceneLoadMode::Additive);
}

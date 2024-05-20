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

#include "Game.h"
#include "MainMenu.h"
#include "Player.h"
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
    ResourceManager::LoadSprite("Logo", "Logo.png", 8, { 0.5f, 0.5f });

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
    auto* logo = scene.AddGameObject("Logo", { 0, 0, 0 });
    logo->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("Logo"), 1);

    auto* mainMenuGO = scene.AddGameObject("MainMenu");
    mainMenuGO->AddComponent<MainMenu>(&logo->GetTransform());
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


    SceneManager::GetInstance().LoadScene("mainMenu", MainMenuScene);

    // // Unload and load for testing purpouses
    // SceneManager::GetInstance().UnloadScene("mainScene");

    // SceneManager::GetInstance().LoadScene("mainScene", MainScene);
    // SceneManager::GetInstance().LoadScene("mainMenu", MainMenuScene, SceneLoadMode::Additive);
}

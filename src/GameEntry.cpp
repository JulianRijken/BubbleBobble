#include <Game.h>
#include <GameSettings.h>
#include <Input.h>
#include <Julgen.h>
#include <Player.h>
#include <ResourceManager.h>
#include <SceneManager.h>

#include "Scenes.h"


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
    ResourceManager::LoadSprite("Julgen_Retro_Logo", "Julgen_Logo_BB_Retro.png", 8, { 0.5f, 0.5f });


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


    auto loadPLayerSprite = [](const std::string& spriteName, const std::string& fileName)
    {
        ResourceManager::LoadSprite(spriteName,
                                    fileName,
                                    8,
                                    {
                                        0.5f, 0.5f
        },
                                    4,
                                    6,
                                    { { "Idle", SpriteAnimation{ { { 0, 0 }, { 1, 0 } }, 7 } },
                                      { "InBubble", SpriteAnimation{ { { 1, 2 }, { 2, 2 } }, 4 } },
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
    };

    loadPLayerSprite("BubbleCharacter", "BubbleCharacter.png");
    loadPLayerSprite("BobbleCharacter", "BobbleCharacter.png");


    ResourceManager::LoadSprite("BubbleLarge",
                                "BubbleLarge.png",
                                8,
                                {
                                    0.5f, 0.5f
    },
                                1,
                                9,
                                {
                                    { "Hidden", SpriteAnimation{ { { 8, 0 } }, 4 } },
                                    { "Spawn", SpriteAnimation{ { { 0, 0 }, { 1, 0 }, { 2, 0 } }, 4 } },
                                    { "Idle", SpriteAnimation{ { { 3, 0 }, { 4, 0 } }, 4 } },
                                    { "Pop",
                                      SpriteAnimation{ { { 5, 0 },
                                                         { 6, 0 },
                                                         { 5, 0 },
                                                         { 6, 0 },
                                                         { 5, 0 },
                                                         { 6, 0 },
                                                         { 5, 0 },
                                                         { 6, 0 },
                                                         { 5, 0 },
                                                         { 6, 0 },
                                                         { 5, 0 },
                                                         { 6, 0 },
                                                         { 5, 0 },
                                                         { 6, 0 },
                                                         { 5, 0 },
                                                         { 6, 0 },
                                                         { 7, 0 },
                                                         { 8, 0 } },
                                                       12 } },
                                });


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

    ResourceManager::LoadSprite(
        "Enemys",
        "Enemys.png",
        8,
        {
            0.5f, 0.5f
    },
        12,
        16,
        {
            { "zenchan_normal", SpriteAnimation{ { { 0, 0 }, { 1, 0 } }, 4 } },
            { "zenchan_bubble", SpriteAnimation{ { { 6, 0 }, { 7, 0 } }, 4 } },
            { "zenchan_dead", SpriteAnimation{ { { 12, 0 }, { 13, 0 }, { 14, 0 }, { 15, 0 } }, 4 } },
        });
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
    GameSettings::g_RenderWidth = Game::GRID_SIZE_X * Game::PIXELS_PER_GRID_CELL;
    GameSettings::g_RenderHeight = Game::GRID_SIZE_Y * Game::PIXELS_PER_GRID_CELL;


    GameSettings::g_WindowWidth = Game::GRID_SIZE_X * Game::PIXELS_PER_GRID_CELL * 4;
    GameSettings::g_WindowHeight = Game::GRID_SIZE_Y * Game::PIXELS_PER_GRID_CELL * 4;
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
    Input::AddAction(InputBind::UiSelect,
                     {
                         { SDL_SCANCODE_E, SDL_SCANCODE_KP_ENTER, SDL_SCANCODE_SPACE },
                         {},
                         {}
    });
    Input::AddAction(InputBind::UiDown,
                     {
                         { SDL_SCANCODE_DOWN, SDL_SCANCODE_Z, SDL_SCANCODE_S },
                         {},
                         {}
    });
    Input::AddAction(InputBind::UiUp,
                     {
                         { SDL_SCANCODE_UP, SDL_SCANCODE_X, SDL_SCANCODE_W },
                         {},
                         {}
    });
    Input::AddAction(InputBind::DebugIncreaseTimeScale, { { SDL_SCANCODE_RIGHTBRACKET }, {}, {} });
    Input::AddAction(InputBind::DebugDecreaseTimeScale, { { SDL_SCANCODE_LEFTBRACKET }, {}, {} });

    Input::AddAction(InputBind::ForceReset, { { SDL_SCANCODE_ESCAPE }, {}, {} });


    InitControls();
}

void jul::Julgen::GameStart()
{
    LoadResources();
    Game::GetInstance().Initialize();
    bb::scenes::BindScenes();
    SceneManager::GetInstance().LoadScene((int)scenes::Id::TwoPlayerMode);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

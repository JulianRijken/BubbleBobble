#include <Animator.h>
#include <Box2D/Box2D.h>
#include <fmt/core.h>
#include <GameSettings.h>
#include <Input.h>
#include <Julgen.h>
#include <Locator.h>
#include <MathExtensions.h>
#include <Physics.h>
#include <ResourceManager.h>
#include <Rigidbody.h>
#include <SceneManager.h>
#include <TextRenderer.h>

#include "AutoMove.h"
#include "FpsCounter.h"
#include "Game.h"
#include "Player.h"
#include "PlayerHUD.h"

using namespace jul;
using namespace bb;

enum class InputBind
{
    TestLivesButton,
    Jump,
    MoveLeft,
    MoveRight,
    MoveStick,
    Attack,
};

class PlayerInputCommand final : public BaseCommand
{
    using InputFunction = std::function<void(InputContext)>;

public:
    PlayerInputCommand(int playerIndex, void (Player::*memberFunction)(InputContext)) :
        m_Function(
            [playerIndex, memberFunction](InputContext context)
            {
                Player* player = Game::GetInstance().GetPlayer(playerIndex);
                if(player == nullptr)
                    return;

                (player->*memberFunction)(context);
            })
    {
    }

    void Execute(InputContext context) override { m_Function(context); }

private:
    InputFunction m_Function;
};

void LoadResources()
{
    ResourceManager::LoadFont("Lingua", "Lingua.otf", 36);
    ResourceManager::LoadFont("LinguaSmall", "Lingua.otf", 16);
    ResourceManager::LoadFont("NES", "NES_Font.ttf", 36);
    ResourceManager::LoadFont("NESSmall", "NES_Font.ttf", 16);

    ResourceManager::LoadSprite("background", "background.tga", 32);
    ResourceManager::LoadSprite("Dot", "Dot.png", 32);

    ResourceManager::LoadSprite("LevelTiles", "LevelTiles.png", 8, { 0, 0 }, 25, 5);

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
                                  { "Attack", SpriteAnimation{ { { 0, 2 } }, 1 } } });

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
                                  { "Attack", SpriteAnimation{ { { 0, 2 } }, 1 } } });

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
}

void MainMenuScene(Scene& scene)
{
    for(int x = -16; x < 16; ++x)
    {
        for(int y = -15; y < 15; ++y)
        {
            if(math::RandomValue<float>() > 0.95f)
            {
                auto* levelTile = scene.AddGameObject("LevelTile", { x, y, 0 });
                levelTile->AddComponent<SpriteRenderer>(
                    ResourceManager::GetSprite("LevelTiles"), -100, glm::ivec2{ 4, jul::math::RandomRange(13, 14) });
                levelTile->AddComponent<AutoMove>(glm::vec3{ 0.0f, 2.0f, 0 }, glm::vec3{ 16, 15, 1 });

                auto* bubbleParticle = scene.AddGameObject("BubbleCharacter", { x, y, 0 });
                bubbleParticle->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("BubbleParticle"), -90);
                bubbleParticle->AddComponent<Animator>()->PlayAnimation(
                    "Twinkle", true, jul::math::RandomValue<float>());
                bubbleParticle->AddComponent<AutoMove>(
                    glm::vec3{ jul::math::RandomRange(-1.0f, 1.0f), jul::math::RandomRange(2.0f, 4.0f), 0 },
                    glm::vec3{ 16, 15, 1 });
            }
        }
    }
}

void MainScene(Scene& scene)
{
    GameObject* fpsCounter = scene.AddGameObject("Fps Counter", { 20, 20, 0 });
    fpsCounter->AddComponent<TextRenderer>("error", ResourceManager::GetFont("LinguaSmall"), 100);
    fpsCounter->AddComponent<FpsCounter>();

    // Player 1
    auto* player1GameObject = scene.AddGameObject("BubbleCharacter", { -2, 10, 0 });
    player1GameObject->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("BubbleCharacter"), 0);
    player1GameObject->AddComponent<Animator>();
    player1GameObject->AddComponent<Rigidbody>();
    auto* player1 = player1GameObject->AddComponent<bb::Player>(0);

    // Player 2
    auto* player2GameObject = scene.AddGameObject("BobbleCharacter", { 2, 15, 0 });
    player2GameObject->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("BobbleCharacter"), 0);
    player2GameObject->AddComponent<Animator>();
    player2GameObject->AddComponent<Rigidbody>();
    auto* player2 = player2GameObject->AddComponent<bb::Player>(1);

    auto* scoreInfoText = scene.AddGameObject("InfoText", { 30, 70, 0 });
    scoreInfoText->AddComponent<TextRenderer>(
        "add Score: Z or gamepad A", ResourceManager::GetFont("NESSmall"), 100, true);

    auto* livesInfoText = scene.AddGameObject("InfoText", { 30, 90, 0 });
    livesInfoText->AddComponent<TextRenderer>(
        "add Lives: X or gamepad B", ResourceManager::GetFont("NESSmall"), 100, true);

    auto* moveInfoText = scene.AddGameObject("InfoText", { 30, 110, 0 });
    moveInfoText->AddComponent<TextRenderer>(
        "MOVE PLAYER: A-D OR D-PAD OR L-STICK", ResourceManager::GetFont("NESSmall"), 100, true);

    GameObject* player1Hud = scene.AddGameObject("PlayerHUD", { 30, 150, 0 });
    {
        auto* livesGameObject = scene.AddGameObject("LivesText", { 0, 0, 0 });
        auto* livesText = livesGameObject->AddComponent<TextRenderer>("error", ResourceManager::GetFont("NES"), 100);

        auto* scoreGameObject = scene.AddGameObject("ScoreText", { 0, 50, 0 });
        auto* scoreText = scoreGameObject->AddComponent<TextRenderer>("error", ResourceManager::GetFont("NES"), 100);

        scoreGameObject->GetTransform().SetParent(&player1Hud->GetTransform(), false);
        livesGameObject->GetTransform().SetParent(&player1Hud->GetTransform(), false);
        player1Hud->AddComponent<bb::PlayerHUD>(player1, scoreText, livesText, SDL_Color(92, 230, 52, 255));
    }

    GameObject* player2Hud = scene.AddGameObject("PlayerHUD", { 30, 300, 0 });
    {
        auto* livesGameObject = scene.AddGameObject("LivesText", { 0, 0, 0 });
        auto* livesText = livesGameObject->AddComponent<TextRenderer>("error", ResourceManager::GetFont("NES"), 100);

        auto* scoreGameObject = scene.AddGameObject("ScoreText", { 0, 50, 0 });
        auto* scoreText = scoreGameObject->AddComponent<TextRenderer>("error", ResourceManager::GetFont("NES"), 100);

        scoreGameObject->GetTransform().SetParent(&player2Hud->GetTransform(), false);
        livesGameObject->GetTransform().SetParent(&player2Hud->GetTransform(), false);
        player2Hud->AddComponent<bb::PlayerHUD>(player2, scoreText, livesText, SDL_Color(52, 168, 230, 255));
    }

    for(int i = -5; i < 5; ++i)
    {
        {
            auto* levelTile = scene.AddGameObject("LevelTile", { i, -1, 0 });
            levelTile->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("LevelTiles"), -50, glm::ivec2{ 0, 0 });
        }
        {
            auto* levelTile = scene.AddGameObject("LevelTile", { i, -2, 0 });
            levelTile->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("LevelTiles"), -50, glm::ivec2{ 0, 0 });
        }
    }
}

void TestScene(Scene& scene)
{
    auto* go = scene.AddGameObject("BubbleCharacter", { -2, 0, 0 });
    go->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("BubbleCharacter"), 0);
    go->AddComponent<Animator>();
    go->AddComponent<Rigidbody>();
    go->AddComponent<bb::Player>(1);
}

void InitControls()
{
    Input::RegisterCommand<PlayerInputCommand>(
        (int)InputBind::TestLivesButton, ButtonState::Down, 0, false, 0, &bb::Player::OnTestLivesInput);
    Input::RegisterCommand<PlayerInputCommand>(
        (int)InputBind::MoveLeft, ButtonState::Held, 0, false, 0, &bb::Player::OnMoveLeftInput);
    Input::RegisterCommand<PlayerInputCommand>(
        (int)InputBind::MoveRight, ButtonState::Held, 0, false, 0, &bb::Player::OnMoveRightInput);
    Input::RegisterCommand<PlayerInputCommand>(
        (int)InputBind::MoveStick, ButtonState::Held, 0, false, 0, &bb::Player::OnMoveStickInput);
    Input::RegisterCommand<PlayerInputCommand>(
        (int)InputBind::Attack, ButtonState::Down, 0, false, 0, &bb::Player::OnAttackInput);

    Input::RegisterCommand<PlayerInputCommand>(
        (int)InputBind::TestLivesButton, ButtonState::Down, 1, true, 1, &bb::Player::OnTestLivesInput);
    Input::RegisterCommand<PlayerInputCommand>(
        (int)InputBind::MoveLeft, ButtonState::Held, 1, true, 1, &bb::Player::OnMoveLeftInput);
    Input::RegisterCommand<PlayerInputCommand>(
        (int)InputBind::MoveRight, ButtonState::Held, 1, true, 1, &bb::Player::OnMoveRightInput);
    Input::RegisterCommand<PlayerInputCommand>(
        (int)InputBind::MoveStick, ButtonState::Held, 1, true, 1, &bb::Player::OnMoveStickInput);
    Input::RegisterCommand<PlayerInputCommand>(
        (int)InputBind::Attack, ButtonState::Down, 1, true, 1, &bb::Player::OnAttackInput);
}

void jul::Julgen::PreInit()
{
    GameSettings::s_WindowTitle = "Bubble Bobble Made In Julgen";
    GameSettings::s_RenderWidth = 32 * 8 * 4;
    GameSettings::s_RenderHeight = 26 * 8 * 4;
    GameSettings::s_WindowWidth = GameSettings::s_RenderWidth;
    GameSettings::s_WindowHeight = GameSettings::s_RenderHeight;

    Input::AddAction(InputBind::TestLivesButton, { { SDL_SCANCODE_X }, { SDL_CONTROLLER_BUTTON_B }, {} });
    Input::AddAction(InputBind::Jump, { { SDL_SCANCODE_SPACE }, { SDL_CONTROLLER_BUTTON_B }, {} });
    Input::AddAction(InputBind::MoveLeft, { { SDL_SCANCODE_A }, { SDL_CONTROLLER_BUTTON_DPAD_LEFT }, {} });
    Input::AddAction(InputBind::MoveRight, { { SDL_SCANCODE_D }, { SDL_CONTROLLER_BUTTON_DPAD_RIGHT }, {} });
    Input::AddAction(InputBind::MoveStick, { {}, {}, { SDL_CONTROLLER_AXIS_LEFTX } });
    Input::AddAction(InputBind::Attack,
                     {
                         { SDL_SCANCODE_E, SDL_SCANCODE_Z },
                         { SDL_CONTROLLER_BUTTON_A },
                         {}
    });

    InitControls();
}


void jul::Julgen::GameStart()
{
    // going for https://www.youtube.com/watch?v=VyK_cpp9pT4
    // BubbleBobble NES
    LoadResources();


    SceneManager::GetInstance().LoadScene("mainScene", MainScene);
    SceneManager::GetInstance().LoadScene("mainMenu", MainMenuScene, SceneLoadMode::Additive);
    // SceneManager::GetInstance().LoadScene("testScene", TestScene);


    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, -11.0f);
    b2Body* groundBody = Locator::Get<Physics>().GetWorld().CreateBody(&groundBodyDef);

    // Collider
    b2PolygonShape groundBox;
    groundBox.SetAsBox(5.0f, 10.0f);
    groundBody->CreateFixture(&groundBox, 0.0f);
}

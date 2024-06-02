#include "MainMenu.h"

#include <Animator.h>
#include <AutoMove.h>
#include <Command.h>
#include <EaseFunction.h>
#include <fmt/core.h>
#include <GameObject.h>
#include <GameTime.h>
#include <Input.h>
#include <MathExtensions.h>
#include <ResourceManager.h>
#include <SceneManager.h>
#include <SpriteRenderer.h>
#include <Transform.h>
#include <Tween.h>
#include <TweenEngine.h>

#include "Game.h"

bb::MainMenu::MainMenu(GameObject* parentPtr, Transform* logoTransformPtr, GameObject* intoTextPtr,
                       GameObject* introScreen, GameObject* selectScreen, Transform* selectBubble,
                       std::vector<Transform*>&& options) :
    Component(parentPtr, "Main Menu"),
    m_LogoTransformPtr(logoTransformPtr),
    m_SelectBubble(selectBubble),
    m_IntoTextPtr(intoTextPtr),
    m_IntroScreen(introScreen),
    m_SelectScreen(selectScreen),
    m_Options(std::move(options))
{

    Input::Bind((int)InputBind::UiSelect, 1, true, this, &bb::MainMenu::OnSelectButton);
    Input::Bind((int)InputBind::UiDown, 1, true, this, &bb::MainMenu::OnDownButton);
    Input::Bind((int)InputBind::UiUp, 1, true, this, &bb::MainMenu::OnUpButton);

    // Todo allow controller index -1 for all controllers
    Input::Bind((int)InputBind::UiSelect, 0, false, this, &bb::MainMenu::OnSelectButton);
    Input::Bind((int)InputBind::UiDown, 0, false, this, &bb::MainMenu::OnDownButton);
    Input::Bind((int)InputBind::UiUp, 0, false, this, &bb::MainMenu::OnUpButton);


    // Show main menu logo
    TweenEngine::Start({ .from = 20.0f,
                         .to = 4.0f,
                         .duration = 2.0f,
                         .easeFunction = EaseFunction::SineOut,
                         .onUpdate = [this](float value) { m_LogoTransformPtr->SetWorldPosition(0, value, 0); },
                         .onEnd = [this]() { OnLogoLand(); } },
                       GetGameObject());


    Scene& scene = GetGameObject()->GetScene();

    for(int x = -16; x < 16; ++x)
    {
        for(int y = -15; y < 15; ++y)
        {
            if(math::RandomValue() < 0.90)
                continue;

            auto* bubbleParticle = scene.AddGameObject("Bubble Particle", { x, y, 0 });
            bubbleParticle->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("BubbleParticle"), -90);
            bubbleParticle->AddComponent<Animator>()->PlayAnimation("Twinkle", true, jul::math::RandomValue<float>());
            auto* autoMove = bubbleParticle->AddComponent<AutoMove>(
                glm::vec3{ 0, jul::math::RandomRange(1.5f, 4.0f), 0 }, glm::vec3{ 16, 15, 1 });

            m_Bubbles.emplace_back(autoMove);
        }
    }

    UpdateSelectBubblePosition();
}

void bb::MainMenu::OnSelectButton(const InputContext& context)
{
    if(context.state != jul::ButtonState::Down)
        return;


    if(m_OpenedSelectScreen)
    {
        // Select item
        SceneManager::GetInstance().LoadScene("Main");
    }
    else
    {
        // Show select screen

        if(not m_IntroFinished)
            return;

        ShowSelectScreen();
        m_OpenedSelectScreen = true;
    }
}

void bb::MainMenu::OnUpButton(const InputContext& context)
{
    if(context.state != jul::ButtonState::Down)
        return;

    m_SelectedItem++;
    if(m_SelectedItem > static_cast<int>(m_Options.size() - 1))
        m_SelectedItem = 0;

    UpdateSelectBubblePosition();
}

void bb::MainMenu::OnDownButton(const InputContext& context)
{
    if(context.state != jul::ButtonState::Down)
        return;


    m_SelectedItem--;
    if(m_SelectedItem < 0)
        m_SelectedItem = static_cast<int>(m_Options.size() - 1);

    UpdateSelectBubblePosition();
}

void bb::MainMenu::Update()
{
    const glm::vec3 location = jul::math::LerpSmooth(
        m_SelectBubble->LocalPosition(), m_SelectBubbleTargetLocation, 0.5f, GameTime::GetDeltaTime());

    m_SelectBubble->SetLocalPosition(location);
}


void bb::MainMenu::OnLogoLand()
{
    for(auto&& bubble : m_Bubbles)
    {
        auto velocity = bubble->GetVelocity();
        velocity.x = jul::math::RandomRange(-3.0f, 3.0f);
        bubble->SetVelocity(velocity);
    }

    // Show credits text
    TweenEngine::Start(
        { .from = -15.0f,
          .to = 0.0f,
          .duration = 2.0f,
          .easeFunction = EaseFunction::BounceOut,
          .onStart = [this]() { m_IntoTextPtr->SetActive(true); },
          .onUpdate = [this](float value) { m_IntoTextPtr->GetTransform().SetWorldPosition(0, value, 0); },
          .onEnd = [this]() { m_IntroFinished = true; } },
        GetGameObject());
}

void bb::MainMenu::ShowSelectScreen()
{
    TweenEngine::TweenEngine::Start(
        {
            .from = 0,
            .to = 50,
            .duration = 3.0,
            .easeFunction = EaseFunction::SineIn,
            .onUpdate = [this](float value) { m_IntroScreen->GetTransform().SetWorldPosition(0, value, 0); },
        },
        GetGameObject());

    m_SelectScreen->SetActive(true);
    TweenEngine::TweenEngine::Start(
        {
            .from = -50,
            .to = 0,
            .duration = 4.0,
            .easeFunction = EaseFunction::SineOut,
            .onUpdate = [this](float value) { m_SelectScreen->GetTransform().SetWorldPosition(0, value, 0); },
        },
        GetGameObject());


    TweenEngine::TweenEngine::Start(
        { .duration = 4.0,
          .onUpdate =
              [this](double time)
          {
              auto activeBubbles = static_cast<size_t>(std::lerp(
                  static_cast<double>(m_Bubbles.size() - 1), static_cast<double>(BUBBLE_COUNT_IN_SELECT_SCREEN), time));

              const double curvedSpeed = jul::math::MapWave(time, 0.0, 1.0, 1.0, -0.25) * 0.2;

              for(size_t bubbleIndex{}; bubbleIndex < m_Bubbles.size(); ++bubbleIndex)
              {
                  m_Bubbles[bubbleIndex]->GetGameObject()->SetActive(bubbleIndex < activeBubbles);
                  m_Bubbles[bubbleIndex]->GetTransform().Translate(
                      0, curvedSpeed * m_Bubbles[bubbleIndex]->GetVelocity().y, 0);
              }
          },
          .onEnd = [this]() { m_IntroScreen->SetActive(false); } },

        GetGameObject());
}

void bb::MainMenu::UpdateSelectBubblePosition()
{
    auto* selectedOption = m_Options[m_SelectedItem];
    auto targetPosition = selectedOption->LocalPosition();
    targetPosition.x -= 2;
    m_SelectBubbleTargetLocation = targetPosition;
}

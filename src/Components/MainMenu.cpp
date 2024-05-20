#include "MainMenu.h"

#include <Animator.h>
#include <AutoMove.h>
#include <fmt/core.h>
#include <GameObject.h>
#include <MathExtensions.h>
#include <ResourceManager.h>
#include <SceneManager.h>
#include <SpriteRenderer.h>
#include <Transform.h>
#include <Tween.h>
#include <TweenEngine.h>

#include "Game.h"


bb::MainMenu::MainMenu(GameObject* parentPtr, Transform* logoTransform) :
    Component(parentPtr, "Main Menu"),
    m_LogoTransform(logoTransform)
{

    // Show main menu logo
    TweenEngine::Start({ .from = 20.0f,
                         .to = 0.0f,
                         .duration = 2.0f,
                         .onUpdate = [this](float value) { m_LogoTransform->SetWorldPosition(0, value, 0); },
                         .onEnd = [this]() { OnLogoLand(); } },
                       GetGameObject());


    Scene& scene = GetGameObject()->GetScene();

    for(int x = -16; x < 16; ++x)
    {
        for(int y = -15; y < 15; ++y)
        {
            if(math::RandomValue<float>() < 0.90f)
                continue;

            auto* bubbleParticle = scene.AddGameObject("Bubble Particle", { x, y, 0 });
            bubbleParticle->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("BubbleParticle"), -90);
            bubbleParticle->AddComponent<Animator>()->PlayAnimation("Twinkle", true, jul::math::RandomValue<float>());
            bubbleParticle->AddComponent<AutoMove>(
                glm::vec3{ jul::math::RandomRange(-1.0f, 1.0f), jul::math::RandomRange(2.0f, 4.0f), 0 },
                glm::vec3{ 16, 15, 1 });
        }
    }
}

void bb::MainMenu::OnLogoLand()
{
    fmt::println("Show Text!");


    SceneManager::GetInstance().LoadScene("mainScene", MainScene);
}

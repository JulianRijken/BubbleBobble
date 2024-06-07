#include "ScoreScreen.h"

#include <Animator.h>
#include <GameObject.h>
#include <ResourceManager.h>
#include <Scene.h>
#include <SpriteRenderer.h>
#include <TextRenderer.h>

bb::ScoreScreen::ScoreScreen(GameObject* parentPtr) :
    Component(parentPtr)
{
    Scene& scene = GetGameObject()->GetScene();


    screenElementsPtr = scene.AddGameObject("Score Screen");
    {
        auto* titleText = scene.AddGameObject("Text", { 0, 8, 0 }, screenElementsPtr);
        titleText->AddComponent<TextRenderer>("Enter 1up Initials!",
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1.0,
                                              SDL_Color{ 0, 255, 0, 255 });


        auto* scoreInfo = scene.AddGameObject("Text", { -8, 4, 0 }, screenElementsPtr);
        scoreInfo->AddComponent<TextRenderer>("SCORE",
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1.0,
                                              SDL_Color{ 247, 239, 0, 255 });

        auto* roundInfo = scene.AddGameObject("Text", { 0, 4, 0 }, screenElementsPtr);
        roundInfo->AddComponent<TextRenderer>("ROUND",
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1.0,
                                              SDL_Color{ 247, 239, 0, 255 });


        auto* nameInfo = scene.AddGameObject("Text", { 8, 4, 0 }, screenElementsPtr);
        nameInfo->AddComponent<TextRenderer>("Name",
                                             ResourceManager::GetFont("NES"),
                                             100,
                                             glm ::vec2{ 0.5f, 0.5f },
                                             true,
                                             1.0,
                                             SDL_Color{ 247, 239, 0, 255 });


        auto* score = scene.AddGameObject("Text", { -8, 2, 0 }, screenElementsPtr);
        score->AddComponent<TextRenderer>(
            "30000", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

        auto* round = scene.AddGameObject("Text", { 0, 2, 0 }, screenElementsPtr);
        round->AddComponent<TextRenderer>("ALL", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

        auto* name = scene.AddGameObject("Text", { 8, 2, 0 }, screenElementsPtr);
        name->AddComponent<TextRenderer>("...", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);


        const int startHeight = -0;

        const std::unordered_map<int, std::string> numberToText{
            {1, "1ST"},
            {2, "2ND"},
            {3, "3RD"},
            {4, "4TH"},
            {5, "5TH"},
        };

        for(int i = 1; i <= 5; ++i)
        {
            auto* number = scene.AddGameObject("Text", { -8, startHeight - i * 2, 0 }, screenElementsPtr);
            number->AddComponent<TextRenderer>(
                numberToText.at(i), ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

            auto* score = scene.AddGameObject("Text", { -2, startHeight - i * 2, 0 }, screenElementsPtr);
            score->AddComponent<TextRenderer>(
                std::to_string(i * i * i), ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

            auto* round = scene.AddGameObject("Text", { 4, startHeight - i * 2, 0 }, screenElementsPtr);
            round->AddComponent<TextRenderer>(
                std::to_string(i * i * i), ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

            auto* name = scene.AddGameObject("Text", { 9, startHeight - i * 2, 0 }, screenElementsPtr);
            name->AddComponent<TextRenderer>(
                "...", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);
        }


        // selectBubble = scene.AddGameObject("dsadasdsa Bugble", { -7, 8, 0 }, selectScreen);
        // selectBubble->AddComponent<SpriteRenderer>(ResourceManager::GetSprite("AttackBubble"), 1);
        // selectBubble->AddComponent<Animator>(nullptr, "Idle");

        // auto* titleText = scene.AddGameObject("Text", { -5, 8, 0 }, selectScreen);
        // titleText->AddComponent<TextRenderer>(
        //     "BUBBLE BOBBLE", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.0f, 0.5f }, true);

        // p1Text = scene.AddGameObject("Text", { -5, 6, 0 }, selectScreen);
        // p1Text->AddComponent<TextRenderer>(
        //     "1P START", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.0f, 0.5f }, true);

        // p2Text = scene.AddGameObject("Text", { -5, 4, 0 }, selectScreen);
        // p2Text->AddComponent<TextRenderer>(
        //     "2P START", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.0f, 0.5f }, true);

        // p3Text = scene.AddGameObject("Text", { -5, 2, 0 }, selectScreen);
        // p3Text->AddComponent<TextRenderer>(
        //     "VS START", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.0f, 0.5f }, true);
    }
}

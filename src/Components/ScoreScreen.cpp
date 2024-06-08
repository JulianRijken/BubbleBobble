#include "ScoreScreen.h"

#include <Animator.h>
#include <fmt/core.h>
#include <GameObject.h>
#include <ResourceManager.h>
#include <Scene.h>
#include <SpriteRenderer.h>
#include <TextRenderer.h>
#include <TweenEngine.h>

#include <filesystem>
#include <fstream>

#include "Game.h"


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


        try
        {
            auto scores = ParseScores("C:/Git/BubbleBobble/Scores/Scores.txt");
            std::sort(scores.begin(), scores.end());

            // fmt::println("Loaded Scores:");
            // for(auto&& score : scores)
            //     fmt::println("{} {} {}", score.score, score.round, score.name);

            for(int i = 0; i < std::min(static_cast<int>(scores.size()), 5); ++i)
            {
                auto* number = scene.AddGameObject("Text", { -8, startHeight - i * 2, 0 }, screenElementsPtr);
                number->SetActive(false);
                number->AddComponent<TextRenderer>(
                    numberToText.at(i + 1), ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

                auto* score = scene.AddGameObject("Text", { -2, startHeight - i * 2, 0 }, screenElementsPtr);
                score->SetActive(false);
                score->AddComponent<TextRenderer>(std::to_string(scores[i].score),
                                                  ResourceManager::GetFont("NES"),
                                                  100,
                                                  glm ::vec2{ 0.5f, 0.5f },
                                                  true);

                auto* round = scene.AddGameObject("Text", { 4, startHeight - i * 2, 0 }, screenElementsPtr);
                round->SetActive(false);
                round->AddComponent<TextRenderer>(std::to_string(scores[i].round),
                                                  ResourceManager::GetFont("NES"),
                                                  100,
                                                  glm ::vec2{ 0.5f, 0.5f },
                                                  true);

                auto* name = scene.AddGameObject("Text", { 9, startHeight - i * 2, 0 }, screenElementsPtr);
                name->SetActive(false);
                name->AddComponent<TextRenderer>(
                    scores[i].name, ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

                auto tweenText = [](GameObject* target, double delay, double duration)
                {
                    TweenEngine::Start(
                        {
                            .delay = delay,
                            .from = -Game::GRID_SIZE_Y,
                            .to = target->GetTransform().GetWorldPosition().y,
                            .duration = duration,
                            .easeFunction = EaseFunction::SineOut,
                            .onStart = [target]() { target->SetActive(true); },
                            .onUpdate =
                                [target](double value)
                            {
                                glm::vec3 targetPosition = target->GetTransform().GetWorldPosition();
                                targetPosition.y = value;
                                target->GetTransform().SetWorldPosition(targetPosition);
                            },
                        },
                        target);
                };

                const double defaultDuration = 1.0;
                const double delay = 0.4;
                const double durationScale = 0.1;
                tweenText(number, 0, i * durationScale + defaultDuration);
                tweenText(score, delay, i * durationScale + defaultDuration);
                tweenText(round, delay * 2, i * durationScale + defaultDuration);
                tweenText(name, delay * 3, i * durationScale + defaultDuration);
            }
        }
        catch(const std::runtime_error& error)
        {
            fmt::println("Failed to load file {}", error.what());
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

bool bb::ScoreScreen::IsNumber(const std::string& string)
{
    // Avoid empty string
    if(string.empty())
        return false;

    // Avoid decimal values
    if(string.find('.') != std::string::npos)
        return false;

    // Check if string only contains digits
    return std::ranges::find_if(string, [](unsigned char character) { return not std::isdigit(character); }) ==
           string.end();
}

std::vector<bb::ScoreScreen::UserScore> bb::ScoreScreen::ParseScores(const std::filesystem::path& filePath)
{
    if(not std::filesystem::exists(filePath))
        throw std::runtime_error("File Does Not Exist");

    std::ifstream inFile;
    inFile.open(filePath);

    if(not inFile.is_open())
        throw std::runtime_error("File Already Open");


    std::vector<bb::ScoreScreen::UserScore> scores{};

    std::string line;
    while(std::getline(inFile, line))
    {
        std::istringstream lineStringStream(line);
        std::string token;
        std::vector<std::string> tokens;

        while(std::getline(lineStringStream, token, ' '))
            tokens.push_back(token);

        if(tokens.size() != 3)
            continue;

        if(not IsNumber(tokens[0]))
            continue;

        if(not IsNumber(tokens[1]))
            continue;

        if(std::any_of(tokens[2].begin(), tokens[2].end(), ::isdigit))
            continue;

        scores.emplace_back(std::stoi(tokens[0]), std::stoi(tokens[1]), tokens[2]);
    }

    return scores;
}

#include "ScoreScreen.h"

#include <Animator.h>
#include <Camera.h>
#include <fmt/core.h>
#include <GameObject.h>
#include <Input.h>
#include <ResourceManager.h>
#include <Scene.h>
#include <SpriteRenderer.h>
#include <TextRenderer.h>
#include <TweenEngine.h>

#include <filesystem>
#include <fstream>

#include "Game.h"
#include "GameScore.h"

using jul::ButtonState;
using jul::Camera;
using jul::EaseFunction;
using jul::GameObject;
using jul::Input;
using jul::InputContext;
using jul::ResourceManager;
using jul::Scene;
using jul::TextRenderer;
using jul::TweenEngine;

bb::ScoreScreen::ScoreScreen(GameObject* parentPtr) :
    Component(parentPtr),
    m_SelectedPlayerWriting(0)
{
    Scene& scene = GetGameObject()->GetScene();

    m_LocalP1Score.score = Game::GetInstance().GetGameScore().GetScore(0);
    m_LocalP1Score.round = Game::GetInstance().GetActiveLevelIndex();
    m_LocalP2Score.score = Game::GetInstance().GetGameScore().GetScore(1);
    m_LocalP2Score.round = Game::GetInstance().GetActiveLevelIndex();

    Input::Bind((int)InputBind::UiSelect, 1, true, this, &bb::ScoreScreen::OnSelectButton);
    Input::Bind((int)InputBind::UiSelect, 0, false, this, &bb::ScoreScreen::OnSelectButton);

    if(m_LocalP1Score.score == -1)
    {
        if(m_LocalP2Score.score != -1)
        {
            m_LocalP1Score = m_LocalP2Score;
            m_LocalP2Score.score = -1;
        }
    }


    m_ScreenElementsPtr = scene.AddGameObject("Score Screen");
    {
        int heightOffset = 5;

        if(m_LocalP1Score.score == -1 and m_LocalP2Score.score == -1)
        {

            auto* titleText = scene.AddGameObject("Text", { 0, 11, 0 }, m_ScreenElementsPtr);
            titleText->AddComponent<TextRenderer>("Bubble Bobble Leaderboard",
                                                  ResourceManager::GetFont("NES"),
                                                  100,
                                                  glm ::vec2{ 0.5f, 0.5f },
                                                  true,
                                                  1.0,
                                                  SDL_Color{ 0, 255, 0, 255 });

            auto* noPlayerText = scene.AddGameObject("Text", { 0, 9, 0 }, m_ScreenElementsPtr);
            noPlayerText->AddComponent<TextRenderer>(
                "ARE YOU READY TO BE THE BEST!", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

            m_TopCount = 10;
            m_FillingInScore = false;
            ShowTop();
            return;
        }


        auto* titleText = scene.AddGameObject("Text", { 0, 11, 0 }, m_ScreenElementsPtr);
        titleText->AddComponent<TextRenderer>("Enter 1up Initials!",
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1.0,
                                              SDL_Color{ 0, 255, 0, 255 });

        const int topTextHeight = 7;


        auto* playerInfo = scene.AddGameObject("Text", { -9, topTextHeight, 0 }, m_ScreenElementsPtr);
        playerInfo->AddComponent<TextRenderer>("PLAYER",
                                               ResourceManager::GetFont("NES"),
                                               100,
                                               glm ::vec2{ 0.5f, 0.5f },
                                               true,
                                               1.0,
                                               SDL_Color{ 247, 239, 0, 255 });

        auto* scoreInfo = scene.AddGameObject("Text", { -2, topTextHeight, 0 }, m_ScreenElementsPtr);
        scoreInfo->AddComponent<TextRenderer>("SCORE",
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1.0,
                                              SDL_Color{ 247, 239, 0, 255 });


        auto* roundInfo = scene.AddGameObject("Text", { 4, topTextHeight, 0 }, m_ScreenElementsPtr);
        roundInfo->AddComponent<TextRenderer>("ROUND",
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1.0,
                                              SDL_Color{ 247, 239, 0, 255 });


        auto* nameInfo = scene.AddGameObject("Text", { 10, topTextHeight, 0 }, m_ScreenElementsPtr);
        nameInfo->AddComponent<TextRenderer>("NAME",
                                             ResourceManager::GetFont("NES"),
                                             100,
                                             glm ::vec2{ 0.5f, 0.5f },
                                             true,
                                             1.0,
                                             SDL_Color{ 247, 239, 0, 255 });


        /// Player
        for(int playerIndex = 0; playerIndex < 2; ++playerIndex)
        {
            const UserScore localUserScore = playerIndex == 0 ? m_LocalP1Score : m_LocalP2Score;

            if(localUserScore.score == -1)
                continue;

            heightOffset += -playerIndex * 2;

            auto* player = scene.AddGameObject("Text", { -9, heightOffset, 0 }, m_ScreenElementsPtr);
            player->AddComponent<TextRenderer>(
                playerIndex == 0 ? "P1" : "P2", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

            std::string scoreText = localUserScore.score == -1 ? "NUL" : std::to_string(localUserScore.score);

            auto* score = scene.AddGameObject("Text", { -2, heightOffset, 0 }, m_ScreenElementsPtr);
            score->AddComponent<TextRenderer>(
                scoreText, ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

            std::string roundText = localUserScore.round <= 0 ? "NUL" : std::to_string(localUserScore.round);
            auto* round = scene.AddGameObject("Text", { 4, heightOffset, 0 }, m_ScreenElementsPtr);
            round->AddComponent<TextRenderer>(
                roundText, ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

            auto& letter = playerIndex == 0 ? m_P1LetterTextRendererPtrs : m_P2LetterTextRendererPtrs;

            auto* letter1 = scene.AddGameObject("Text", { 9, heightOffset, 0 }, m_ScreenElementsPtr);
            letter[0] = letter1->AddComponent<TextRenderer>(
                ".", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

            auto* letter2 = scene.AddGameObject("Text", { 10, heightOffset, 0 }, m_ScreenElementsPtr);
            letter[1] = letter2->AddComponent<TextRenderer>(
                ".", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

            auto* letter3 = scene.AddGameObject("Text", { 11, heightOffset, 0 }, m_ScreenElementsPtr);
            letter[2] = letter3->AddComponent<TextRenderer>(
                ".", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);
        }

        ShowTop();


        Input::Bind((int)InputBind::UiDown, 1, true, this, &bb::ScoreScreen::OnDownButton);
        Input::Bind((int)InputBind::UiUp, 1, true, this, &bb::ScoreScreen::OnUpButton);
        Input::Bind((int)InputBind::UiDown, 0, false, this, &bb::ScoreScreen::OnDownButton);
        Input::Bind((int)InputBind::UiUp, 0, false, this, &bb::ScoreScreen::OnUpButton);
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

void bb::ScoreScreen::CreateScoresFileIfNotExist(const std::filesystem::path& filePath)
{
    if(not std::filesystem::exists(filePath))
    {
        fmt::println("Scores Does Not Exits {}", filePath.string());
        std::filesystem::create_directory(filePath.parent_path());
        std::ofstream{ filePath.string() };
        fmt::println("New File Created {}", filePath.string());
    }
}

std::vector<bb::ScoreScreen::UserScore> bb::ScoreScreen::ParseScores(const std::filesystem::path& filePath)
{
    CreateScoresFileIfNotExist(filePath);

    std::ifstream inFile;
    inFile.open(filePath);

    if(not inFile.is_open())
        throw std::runtime_error("File Already Open");

    if(inFile.peek() == std::ifstream::traits_type::eof())
        throw std::runtime_error("File Empty");

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

void bb::ScoreScreen::WriteScore(const UserScore& userScore, const std::filesystem::path& filePath)
{
    CreateScoresFileIfNotExist(filePath);

    std::ofstream outFile;
    // Open file in append mode
    outFile.open(filePath, std::ios_base::app);

    if(not outFile.is_open())
        throw std::runtime_error("Unable to open file for writing");

    outFile << '\n' << userScore.score << " " << userScore.round << " " << userScore.name;
    outFile.close();
}

void bb::ScoreScreen::OnSelectButton(const InputContext& context)
{
    if(context.state != ButtonState::Down)
        return;

    if(not m_FillingInScore)
    {
        TryLeaveScoreScreen();
        return;
    }

    if(m_P1LetterTextRendererPtrs[m_SelectedLetterRenderer]->GetText() == ".")
        return;

    m_SelectedLetterRenderer++;
    m_CurrentLetter = 0;

    if(m_SelectedLetterRenderer >= static_cast<int>(m_P1LetterTextRendererPtrs.size()))
        OnScoreFilledIn();
    else
        UpdateSelectedLetter();
}

void bb::ScoreScreen::OnUpButton(const InputContext& context)
{
    if(context.state != ButtonState::Down)
        return;

    if(not m_FillingInScore)
        return;

    m_CurrentLetter++;
    if(m_CurrentLetter >= static_cast<int>(LETTER_OPTIONS.size()))
        m_CurrentLetter = 0;

    UpdateSelectedLetter();
}

void bb::ScoreScreen::OnDownButton(const InputContext& context)
{
    if(context.state != ButtonState::Down)
        return;

    if(not m_FillingInScore)
        return;

    m_CurrentLetter--;
    if(m_CurrentLetter < 0)
        m_CurrentLetter = static_cast<int>(LETTER_OPTIONS.size()) - 1;

    UpdateSelectedLetter();
}

void bb::ScoreScreen::UpdateSelectedLetter()
{
    if(not m_FillingInScore)
        return;

    auto& letterTextRendererPtrs =
        m_SelectedPlayerWriting == 0 ? m_P1LetterTextRendererPtrs : m_P2LetterTextRendererPtrs;

    for(auto* textRenderer : letterTextRendererPtrs)
        if(textRenderer->GetText() == ".")
            textRenderer->SetColor(200, 200, 200, 255);
        else
            textRenderer->SetColor(255, 255, 255, 255);

    const std::string letter{ LETTER_OPTIONS[m_CurrentLetter] };
    auto* textRendererPtr = letterTextRendererPtrs[m_SelectedLetterRenderer];
    textRendererPtr->SetText(letter);
    textRendererPtr->SetColor(SDL_Color{ 0, 255, 0, 0 });
}

void bb::ScoreScreen::OnScoreFilledIn()
{
    auto& letterTextRendererPtrs =
        m_SelectedPlayerWriting == 0 ? m_P1LetterTextRendererPtrs : m_P2LetterTextRendererPtrs;

    UserScore& userScore = m_SelectedPlayerWriting == 0 ? m_LocalP1Score : m_LocalP2Score;

    // Set all letters to white
    for(auto* textRenderer : letterTextRendererPtrs)
        textRenderer->SetColor(255, 255, 255, 255);

    auto newScore = UserScore{ userScore.score,
                               userScore.round,
                               fmt::format("{}{}{}",
                                           letterTextRendererPtrs[0]->GetText(),
                                           letterTextRendererPtrs[1]->GetText(),
                                           letterTextRendererPtrs[2]->GetText()) };

    fmt::println("Score Filled In");

    WriteScore(newScore, SCORE_PATH);
    ShowTop(newScore);

    assert(m_FillingInScore && "Can't fill in score again");
    m_SelectedPlayerWriting++;
    m_SelectedLetterRenderer = 0;

    if(m_SelectedPlayerWriting == 1 and m_LocalP2Score.score == -1)
        m_FillingInScore = false;

    if(m_SelectedPlayerWriting == 2)
        m_FillingInScore = false;
}

void bb::ScoreScreen::ShowTop(std::optional<UserScore> showcaseScore)
{

    const int startHeight = m_TopCount == 10 ? 5 : -2;

    const std::unordered_map<int, std::string> numberToText{
        { 1,  "1ST"},
        { 2,  "2ND"},
        { 3,  "3RD"},
        { 4,  "4TH"},
        { 5,  "5TH"},
        { 6,  "6ST"},
        { 7,  "7ND"},
        { 8,  "8RD"},
        { 9,  "9TH"},
        {10, "10TH"},
    };

    auto& scene = GetGameObject()->GetScene();

    if(m_TopFiveScoresPtr != nullptr)
        m_TopFiveScoresPtr->Destroy();

    m_TopFiveScoresPtr = scene.AddGameObject("TopFiveScores");


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
                    targetPosition.y = static_cast<float>(value);
                    target->GetTransform().SetWorldPosition(targetPosition);
                },
            },
            target);
    };

    try
    {
        std::vector<UserScore> scores = ParseScores(SCORE_PATH);
        std::sort(scores.begin(), scores.end(), std::greater<UserScore>());

        auto* topFiveText = scene.AddGameObject("Text", { 0, startHeight + 2, 0 }, m_TopFiveScoresPtr);
        topFiveText->AddComponent<TextRenderer>(m_TopCount == 10 ? "BEST 10" : "BEST 5",
                                                ResourceManager::GetFont("NES"),
                                                100,
                                                glm ::vec2{ 0.5f, 0.5f },
                                                true,
                                                1,
                                                SDL_Color{ 240, 0, 0, 0 });

        for(int i = 0; i < std::min(static_cast<int>(scores.size()), m_TopCount); ++i)
        {
            const UserScore scoreToDisplay = scores[i];
            const double scale = m_TopCount == 10 ? 1.8 : 2;

            bool isShowcaseScore{ false };

            if(showcaseScore.has_value())
                if(scoreToDisplay == showcaseScore.value())
                    isShowcaseScore = true;

            const SDL_Color textColor =
                isShowcaseScore ? SDL_Color{ 247, 239, 0, 255 } : SDL_Color{ 255, 255, 255, 255 };

            auto* number = scene.AddGameObject("Text", { -9, startHeight - i * scale, 0 }, m_TopFiveScoresPtr);
            number->SetActive(false);
            number->AddComponent<TextRenderer>(numberToText.at(i + 1),
                                               ResourceManager::GetFont("NES"),
                                               100,
                                               glm ::vec2{ 0.5f, 0.5f },
                                               true,
                                               1,
                                               textColor);

            auto* score = scene.AddGameObject("Text", { -2, startHeight - i * scale, 0 }, m_TopFiveScoresPtr);
            score->SetActive(false);
            score->AddComponent<TextRenderer>(std::to_string(scoreToDisplay.score),
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1,
                                              textColor);

            auto* round = scene.AddGameObject("Text", { 4, startHeight - i * scale, 0 }, m_TopFiveScoresPtr);
            round->SetActive(false);
            round->AddComponent<TextRenderer>(std::to_string(scoreToDisplay.round),
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1,
                                              textColor);

            auto* name = scene.AddGameObject("Text", { 9, startHeight - i * scale, 0 }, m_TopFiveScoresPtr);
            name->SetActive(false);
            name->AddComponent<TextRenderer>(scoreToDisplay.name,
                                             ResourceManager::GetFont("NES"),
                                             100,
                                             glm ::vec2{ 0.5f, 0.5f },
                                             true,
                                             1,
                                             textColor);


            const double defaultDuration = 1.0;
            const double delay = 0.4;
            const double durationScale = 0.1;
            tweenText(number, 0, i * durationScale + defaultDuration);
            tweenText(score, delay, i * durationScale + defaultDuration);
            tweenText(round, delay * 2, i * durationScale + defaultDuration);
            tweenText(name, delay * 3, i * durationScale + defaultDuration);
        }
    }
    catch(...)
    {
        auto* firstOneText = scene.AddGameObject("Text", { 0, -2, 0 }, m_TopFiveScoresPtr);
        firstOneText->SetActive(false);
        firstOneText->AddComponent<TextRenderer>(
            "You are the first one!", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

        tweenText(firstOneText, 0, 2);
    }
}

void bb::ScoreScreen::TryLeaveScoreScreen()
{
    Camera* camera = Game::GetInstance().GetMainCamera();
    assert(camera);

    if(TweenEngine::HasActiveTweens(camera))
        return;

    Game::GetInstance().GetGameScore().ResetScore();

    TweenEngine::Start({ .from = camera->GetTransform().GetWorldPosition().y,
                         .to = -Game::GRID_SIZE_Y,
                         .duration = 2.0,
                         .easeFunction = EaseFunction::SineIn,
                         .onUpdate =
                             [camera](double value)
                         {
                             glm::vec3 targetPosition = camera->GetTransform().GetWorldPosition();
                             targetPosition.y = static_cast<float>(value);
                             camera->GetTransform().SetWorldPosition(targetPosition);
                         },
                         .onEnd = []() { Game::GetInstance().ResetGame(); } },
                       camera);
}

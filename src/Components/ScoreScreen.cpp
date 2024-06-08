#include "ScoreScreen.h"

#include <Animator.h>
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


bb::ScoreScreen::ScoreScreen(GameObject* parentPtr) :
    Component(parentPtr)
{
    Scene& scene = GetGameObject()->GetScene();


    m_ScreenElementsPtr = scene.AddGameObject("Score Screen");
    {
        auto* titleText = scene.AddGameObject("Text", { 0, 10, 0 }, m_ScreenElementsPtr);
        titleText->AddComponent<TextRenderer>("Enter 1up Initials!",
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1.0,
                                              SDL_Color{ 0, 255, 0, 255 });


        auto* scoreInfo = scene.AddGameObject("Text", { -8, 6, 0 }, m_ScreenElementsPtr);
        scoreInfo->AddComponent<TextRenderer>("SCORE",
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1.0,
                                              SDL_Color{ 247, 239, 0, 255 });

        auto* roundInfo = scene.AddGameObject("Text", { 0, 6, 0 }, m_ScreenElementsPtr);
        roundInfo->AddComponent<TextRenderer>("ROUND",
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1.0,
                                              SDL_Color{ 247, 239, 0, 255 });


        auto* nameInfo = scene.AddGameObject("Text", { 8, 6, 0 }, m_ScreenElementsPtr);
        nameInfo->AddComponent<TextRenderer>("Name",
                                             ResourceManager::GetFont("NES"),
                                             100,
                                             glm ::vec2{ 0.5f, 0.5f },
                                             true,
                                             1.0,
                                             SDL_Color{ 247, 239, 0, 255 });


        auto* score = scene.AddGameObject("Text", { -8, 4, 0 }, m_ScreenElementsPtr);
        score->AddComponent<TextRenderer>(
            "30000", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

        auto* round = scene.AddGameObject("Text", { 0, 4, 0 }, m_ScreenElementsPtr);
        round->AddComponent<TextRenderer>("ALL", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

        auto* letter1 = scene.AddGameObject("Text", { 7, 4, 0 }, m_ScreenElementsPtr);
        m_LetterTextRendererPtrs[0] = letter1->AddComponent<TextRenderer>(
            ".", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

        auto* letter2 = scene.AddGameObject("Text", { 8, 4, 0 }, m_ScreenElementsPtr);
        m_LetterTextRendererPtrs[1] = letter2->AddComponent<TextRenderer>(
            ".", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

        auto* letter3 = scene.AddGameObject("Text", { 9, 4, 0 }, m_ScreenElementsPtr);
        m_LetterTextRendererPtrs[2] = letter3->AddComponent<TextRenderer>(
            ".", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);


        ShowTopFive();

        Input::Bind((int)InputBind::UiSelect, 1, true, this, &bb::ScoreScreen::OnSelectButton);
        Input::Bind((int)InputBind::UiDown, 1, true, this, &bb::ScoreScreen::OnDownButton);
        Input::Bind((int)InputBind::UiUp, 1, true, this, &bb::ScoreScreen::OnUpButton);
        Input::Bind((int)InputBind::UiSelect, 0, false, this, &bb::ScoreScreen::OnSelectButton);
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
        Game::GetInstance().ResetGame();
        return;
    }

    if(m_LetterTextRendererPtrs[m_SelectedLetterRenderer]->GetText() == ".")
        return;

    m_SelectedLetterRenderer++;
    m_CurrentLetter = 0;

    if(m_SelectedLetterRenderer >= static_cast<int>(m_LetterTextRendererPtrs.size()))
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
    for(auto* textRenderer : m_LetterTextRendererPtrs)
        if(textRenderer->GetText() == ".")
            textRenderer->SetColor(200, 200, 200, 255);
        else
            textRenderer->SetColor(255, 255, 255, 255);

    const std::string letter{ LETTER_OPTIONS[m_CurrentLetter] };
    auto* textRendererPtr = m_LetterTextRendererPtrs[m_SelectedLetterRenderer];
    textRendererPtr->SetText(letter);
    textRendererPtr->SetColor(SDL_Color{ 0, 255, 0, 0 });
}

void bb::ScoreScreen::OnScoreFilledIn()
{
    assert(m_FillingInScore && "Can't fill in score again");
    m_FillingInScore = false;

    // Set all letters to white
    for(auto* textRenderer : m_LetterTextRendererPtrs)
        textRenderer->SetColor(255, 255, 255, 255);

    auto newScore = UserScore{ 123,
                               1,
                               fmt::format("{}{}{}",
                                           m_LetterTextRendererPtrs[0]->GetText(),
                                           m_LetterTextRendererPtrs[1]->GetText(),
                                           m_LetterTextRendererPtrs[2]->GetText()) };

    fmt::println("Score Filled In");

    WriteScore(newScore, SCORE_PATH);
    ShowTopFive(newScore);
}

void bb::ScoreScreen::ShowTopFive(std::optional<UserScore> showcaseScore)
{

    const int startHeight = -2;

    const std::unordered_map<int, std::string> numberToText{
        {1, "1ST"},
        {2, "2ND"},
        {3, "3RD"},
        {4, "4TH"},
        {5, "5TH"},
    };

    auto& scene = GetGameObject()->GetScene();

    if(m_TopFiveScores != nullptr)
        m_TopFiveScores->Destroy();

    m_TopFiveScores = scene.AddGameObject("TopFiveScores");


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

    try
    {
        std::vector<UserScore> scores = ParseScores(SCORE_PATH);
        std::sort(scores.begin(), scores.end(), std::greater<UserScore>());

        auto* topFiveText = scene.AddGameObject("Text", { 0, 0, 0 }, m_TopFiveScores);
        topFiveText->AddComponent<TextRenderer>("BEST 5",
                                                ResourceManager::GetFont("NES"),
                                                100,
                                                glm ::vec2{ 0.5f, 0.5f },
                                                true,
                                                1,
                                                SDL_Color{ 240, 0, 0, 0 });

        for(int i = 0; i < std::min(static_cast<int>(scores.size()), 5); ++i)
        {
            UserScore scoreToDisplay = scores[i];

            bool isShowcaseScore{ false };

            if(showcaseScore.has_value())
                if(scoreToDisplay == showcaseScore.value())
                    isShowcaseScore = true;

            SDL_Color textColor = isShowcaseScore ? SDL_Color{ 0, 255, 255, 255 } : SDL_Color{ 255, 255, 255, 255 };

            auto* number = scene.AddGameObject("Text", { -9, startHeight - i * 2, 0 }, m_TopFiveScores);
            number->SetActive(false);
            number->AddComponent<TextRenderer>(numberToText.at(i + 1),
                                               ResourceManager::GetFont("NES"),
                                               100,
                                               glm ::vec2{ 0.5f, 0.5f },
                                               true,
                                               1,
                                               textColor);

            auto* score = scene.AddGameObject("Text", { -2, startHeight - i * 2, 0 }, m_TopFiveScores);
            score->SetActive(false);
            score->AddComponent<TextRenderer>(std::to_string(scoreToDisplay.score),
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1,
                                              textColor);

            auto* round = scene.AddGameObject("Text", { 4, startHeight - i * 2, 0 }, m_TopFiveScores);
            round->SetActive(false);
            round->AddComponent<TextRenderer>(std::to_string(scoreToDisplay.round),
                                              ResourceManager::GetFont("NES"),
                                              100,
                                              glm ::vec2{ 0.5f, 0.5f },
                                              true,
                                              1,
                                              textColor);

            auto* name = scene.AddGameObject("Text", { 9, startHeight - i * 2, 0 }, m_TopFiveScores);
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
    catch(const std::runtime_error& error)
    {
        auto* firstOneText = scene.AddGameObject("Text", { 0, -2, 0 }, m_TopFiveScores);
        firstOneText->SetActive(false);
        firstOneText->AddComponent<TextRenderer>(
            "You are the first one!", ResourceManager::GetFont("NES"), 100, glm ::vec2{ 0.5f, 0.5f }, true);

        tweenText(firstOneText, 0, 2);
    }
}

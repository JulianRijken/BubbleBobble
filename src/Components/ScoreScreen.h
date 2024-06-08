#pragma once

#include <Component.h>

#include <array>
#include <filesystem>
#include <optional>

namespace jul
{
    struct InputContext;
    class TextRenderer;
}  // namespace jul

namespace bb
{
    using namespace jul;

    class ScoreScreen final : public Component, public IEventListener
    {

    public:
        struct UserScore
        {
            int score;
            int round;
            std::string name;

            bool operator==(const UserScore& other) const
            {
                return score == other.score && round == other.round && name == other.name;
            }

            bool operator>(const UserScore& other) const
            {
                if(score != other.score)
                    return score > other.score;

                if(round != other.round)
                    return round > other.round;

                return name > name;
            }
        };

        ScoreScreen(GameObject* parentPtr);

        ~ScoreScreen() override = default;
        ScoreScreen(const ScoreScreen&) = delete;
        ScoreScreen(ScoreScreen&&) noexcept = delete;
        ScoreScreen& operator=(const ScoreScreen&) = delete;
        ScoreScreen& operator=(ScoreScreen&&) noexcept = delete;


    private:
        bool IsNumber(const std::string& string);

        void CreateScoresFileIfNotExist(const std::filesystem::path& filePath);
        std::vector<UserScore> ParseScores(const std::filesystem::path& filePath);
        void WriteScore(const UserScore& userScore, const std::filesystem::path& filePath);


        void OnSelectButton(const InputContext& context);
        void OnUpButton(const InputContext& context);
        void OnDownButton(const InputContext& context);

        void UpdateSelectedLetter();
        void OnScoreFilledIn();
        void ShowTopFive(std::optional<UserScore> showcaseScore = std::nullopt);

        void TryLeaveScoreScreen();

        const std::string LETTER_OPTIONS{ ".ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
        const std::filesystem::path SCORE_PATH{ "./Scores/Scores.txt" };

        bool m_FillingInScore{ true };
        int m_CurrentLetter{ 0 };
        int m_SelectedLetterRenderer{ 0 };
        std::array<TextRenderer*, 3> m_LetterTextRendererPtrs{ nullptr, nullptr, nullptr };

        GameObject* m_ScreenElementsPtr{ nullptr };
        GameObject* m_TopFiveScoresPtr{ nullptr };
    };
}  // namespace bb

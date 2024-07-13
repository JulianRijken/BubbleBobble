#ifndef SCORESCREEN_H
#define SCORESCREEN_H

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
    class ScoreScreen final : public jul::Component, public jul::IEventListener
    {

    public:
        struct UserScore
        {
            int score = -1;
            int round = -1;
            std::string name{ "..." };

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

        ScoreScreen(jul::GameObject* parentPtr);
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

        void OnSelectButton(const jul::InputContext& context);
        void OnUpButton(const jul::InputContext& context);
        void OnDownButton(const jul::InputContext& context);

        void UpdateSelectedLetter();
        void OnScoreFilledIn();
        void ShowTop(std::optional<UserScore> showcaseScore = std::nullopt);

        void TryLeaveScoreScreen();

        const std::string LETTER_OPTIONS{ ".ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
        const std::filesystem::path SCORE_PATH{ "./Scores/Scores.txt" };

        bool m_FillingInScore{ true };
        int m_CurrentLetter{ 0 };
        int m_SelectedLetterRenderer{ 0 };
        int m_SelectedPlayerWriting{ 0 };
        int m_TopCount{ 5 };

        std::array<jul::TextRenderer*, 3> m_P1LetterTextRendererPtrs{ nullptr, nullptr, nullptr };
        std::array<jul::TextRenderer*, 3> m_P2LetterTextRendererPtrs{ nullptr, nullptr, nullptr };


        UserScore m_LocalP1Score{};
        UserScore m_LocalP2Score{};

        jul::GameObject* m_ScreenElementsPtr{ nullptr };
        jul::GameObject* m_TopFiveScoresPtr{ nullptr };
    };
}  // namespace bb
#endif // SCORESCREEN_H
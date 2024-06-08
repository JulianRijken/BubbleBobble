#pragma once

#include <Component.h>

#include <filesystem>


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

            bool operator<(const UserScore& other) const
            {
                if(score != other.score)
                    return score > other.score;

                return round > other.round;
            }
        };

        ScoreScreen(GameObject* parentPtr);

        ~ScoreScreen() override = default;
        ScoreScreen(const ScoreScreen&) = delete;
        ScoreScreen(ScoreScreen&&) noexcept = delete;
        ScoreScreen& operator=(const ScoreScreen&) = delete;
        ScoreScreen& operator=(ScoreScreen&&) noexcept = delete;

        bool IsNumber(const std::string& string);
        std::vector<UserScore> ParseScores(const std::filesystem::path& filePath);

        // void OnSelectButton(const InputContext& context);
        // void OnUpButton(const InputContext& context);
        // void OnDownButton(const InputContext& context);

    private:
        GameObject* screenElementsPtr{};
    };
}  // namespace bb

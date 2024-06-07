#pragma once

#include <Component.h>

namespace bb
{
    using namespace jul;

    class ScoreScreen final : public Component, public IEventListener
    {
    public:
        ScoreScreen(GameObject* parentPtr);

        ~ScoreScreen() override = default;
        ScoreScreen(const ScoreScreen&) = delete;
        ScoreScreen(ScoreScreen&&) noexcept = delete;
        ScoreScreen& operator=(const ScoreScreen&) = delete;
        ScoreScreen& operator=(ScoreScreen&&) noexcept = delete;

        // void OnSelectButton(const InputContext& context);
        // void OnUpButton(const InputContext& context);
        // void OnDownButton(const InputContext& context);

    private:
        GameObject* screenElementsPtr{};
    };
}  // namespace bb

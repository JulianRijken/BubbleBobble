#pragma once
#include <Component.h>
#include <Event.h>
#include <MessageQueue.h>
#include <TextRenderer.h>

namespace jul
{
    class TextRenderer;
    class GameObject;
}  // namespace jul

namespace bb
{
    class LevelHUD final : public jul::Component, public jul::IEventListener
    {
    public:
        inline static constexpr int SCORE_CHANGE_PER_SECOND = 400;

        LevelHUD(jul::GameObject* parentPt);

    private:
        void OnLevelTransitionChange(bool inTransition, int levelIndex);
        void UpdateText(int levelIndex);

        jul::TextRenderer* m_LevelNumberText{ nullptr };
    };
}

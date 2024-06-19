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
    class LevelHUD final : public Component, public IEventListener
    {
    public:
        inline static constexpr int SCORE_CHANGE_PER_SECOND = 400;

        LevelHUD(GameObject* parentPt);

    private:
        void OnLevelTransitionChange(bool inTransition, int levelIndex);
        void UpdateText(int levelIndex);

        TextRenderer* m_LevelNumberText{ nullptr };
    };
}

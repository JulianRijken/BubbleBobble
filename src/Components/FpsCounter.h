#pragma once
#include <Component.h>

using namespace jul;

namespace jul
{
    class TextRenderer;
}

namespace bb
{
    class FpsCounter final : public jul::Component
    {
    public:
        FpsCounter(jul::GameObject* parent);

    private:
        void Update() override;

        TextRenderer* m_TextRenderer{};
    };

}  // namespace bb

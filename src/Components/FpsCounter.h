#pragma once
#include <Component.h>


namespace jul
{
    class TextRenderer;
}

namespace bb
{
    using namespace jul;

    class FpsCounter final : public jul::Component
    {
    public:
        FpsCounter(jul::GameObject* parent);

    private:
        void Update() override;

        TextRenderer* m_TextRenderer{};
    };

}  // namespace bb

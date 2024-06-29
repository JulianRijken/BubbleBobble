#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H
#include <Component.h>


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

        jul::TextRenderer* m_TextRenderer{};
    };

}  // namespace bb
#endif // FPSCOUNTER_H
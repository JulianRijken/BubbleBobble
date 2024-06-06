#pragma once
#include <Component.h>

#include <glm/vec2.hpp>

namespace bb
{
    using namespace jul;

    class ZenChan;

    class ZenChanBehaviour : public Component
    {
    public:
        static constexpr float MIN_TIME_BETWEEN_TURN{ 1.0f };

        explicit ZenChanBehaviour(GameObject* parentPtr, ZenChan* target);

        ~ZenChanBehaviour() override = default;

        ZenChanBehaviour(const ZenChanBehaviour&) = delete;
        ZenChanBehaviour(ZenChanBehaviour&&) noexcept = delete;
        ZenChanBehaviour& operator=(const ZenChanBehaviour&) = delete;
        ZenChanBehaviour& operator=(ZenChanBehaviour&&) noexcept = delete;

    private:
        void FixedUpdate() override;
        void Update() override;

        void HandleTurning();


        ZenChan* m_Target;
        int m_WalkingDirection{ 1 };

        glm::vec2 m_TargetDirection{};
        float m_TimeSinceLastTurn{};
    };

}  // namespace bb

#pragma once
#include <Component.h>

#include <glm/vec2.hpp>

namespace bb
{
    class ZenChan;

    class ZenChanBehaviour final : public Component
    {
    public:
        static constexpr float MIN_TIME_BETWEEN_WALL_TURN{ 1.0f };
        static constexpr glm::vec2 JUMP_INTERVAL{ 1.0f, 1.5f };
        static constexpr glm::vec2 FACE_TARGET_INTERVAL{ 2.0, 3.5f };

        explicit ZenChanBehaviour(GameObject* parentPtr, ZenChan* target);

        ~ZenChanBehaviour() override = default;

        ZenChanBehaviour(const ZenChanBehaviour&) = delete;
        ZenChanBehaviour(ZenChanBehaviour&&) noexcept = delete;
        ZenChanBehaviour& operator=(const ZenChanBehaviour&) = delete;
        ZenChanBehaviour& operator=(ZenChanBehaviour&&) noexcept = delete;

    private:
        void Update() override;
        void HandleTurning();

        float m_JumpInterval{ JUMP_INTERVAL.y };
        float m_FacePlayerInterval{ FACE_TARGET_INTERVAL.y };

        ZenChan* m_Target;
        int m_WalkingDirection{ 1 };
        float m_TimeSinceLastJump{};
        float m_TimeSinceLastFacePlayer{};

        glm::vec2 m_TargetDirection{};
        float m_TimeSinceLastTurn{};
    };

}  // namespace bb

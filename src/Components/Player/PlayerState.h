#pragma once

namespace bb
{
    class Player;

    class PlayerState
    {
    public:
        virtual ~PlayerState() = default;

        PlayerState(PlayerState&&) = delete;
        PlayerState(const PlayerState&) = delete;
        PlayerState& operator=(PlayerState&&) = delete;
        PlayerState& operator=(const PlayerState&) = delete;


        virtual void OnEnterState(Player& /*unused*/){};

        virtual void Update(Player& /*unused*/){};
        virtual void FixedUpdate(Player& /*unused*/){};

        virtual void OnAttackInput(Player& /*unused*/){};
        virtual void OnJumpInput(Player& /*unused*/){};
        virtual void OnMoveInput(Player& /*unused*/){};

        virtual void OnExitState(Player& /*unused*/){};


    protected:
        PlayerState() = default;
    };

    class PlayerWalkingState final : public PlayerState
    {
    public:
        void OnEnterState(Player& player) override;
        void Update(Player& player) override;
        void FixedUpdate(Player& player) override;
        void OnJumpInput(Player& player) override;

    private:
        float m_TimeWalking{};
        static constexpr float MIN_TIME_WALKING_BEFORE_GROUND_CHECK{ 0.2f };
    };

    class PlayerJumpingState final : public PlayerState
    {
    public:
        static constexpr float MIN_JUMP_TIME{ 0.5f };

        void OnEnterState(Player& player) override;
        void Update(Player& player) override;
        void FixedUpdate(Player& player) override;
        void OnExitState(Player& player) override;

    private:
        float m_SlowFalHeight{};
        float m_TimeInJump{};
        float m_LastHorizontalVelocity{};
        bool m_Falling{ true };
        float m_Gravity{};
    };

    class PlayerAttackignState final : public PlayerState
    {
    public:
        void OnEnterState(Player& player) override;
    };
}  // namespace bb

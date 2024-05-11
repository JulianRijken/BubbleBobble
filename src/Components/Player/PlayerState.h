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
        static constexpr float WALK_GRAVITY_SCALE{ 3.0f };
        static constexpr float MOVE_SPEED{ 8.0f };
        static constexpr float MIN_TIME_WALKING_BEFORE_GROUND_CHECK{ 0.5f };

        void OnEnterState(Player& player) override;
        void Update(Player& player) override;
        void FixedUpdate(Player& player) override;
        void OnJumpInput(Player& player) override;
        void OnAttackInput(Player& player) override;
        void OnExitState(Player& player) override;

    private:
        float m_TimeWalking{};
    };

    class PlayerJumpingState final : public PlayerState
    {
    public:
        static constexpr float MIN_JUMP_TIME{ 0.5f };
        static constexpr float MOVE_SPEED{ 4.0f };
        static constexpr float FALL_SPEED{ 6.0f };
        static constexpr float GRAVITY_FOCE{ -30.0f };
        static constexpr float JUMP_FORCE{ 18.0f };

        void OnEnterState(Player& player) override;
        void Update(Player& player) override;
        void FixedUpdate(Player& player) override;
        void OnExitState(Player& player) override;
        void OnAttackInput(Player& player) override;

    private:
        float m_SlowFallHeight{};
        float m_TimeInJump{};
        bool m_Falling{};
        bool m_HasManualControl{};
    };

    class PlayerAttackignState final : public PlayerState
    {
    public:
        static constexpr float FIRE_POWER = 20.0;
        static constexpr float TIME_BETWEEN_FIRE = 0.5f;

        void OnEnterState(Player& player) override;
        void Update(Player& player) override;

    private:
        float m_TimeOfLastAttack{};
    };
}  // namespace bb

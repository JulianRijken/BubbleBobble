#pragma once
#include <Scene.h>

namespace bb::scenes
{
    enum class Id
    {
        MainMenu,
        OnePlayerMode,
        TwoPlayerMode,
        VersusMode,
        IntroLevel,
        Level1,
        Level2,
        Level3,
        ScoreScreen,
    };

    void BindScenes();

    // Used to make sure scenes that depend on the
    // mode scenes don't get loaded
    void AssertModeSceneLoaded();

    void OnePlayerModeScene(jul::Scene& scene);
    void TwoPlayerModeScene(jul::Scene& scene);
    void VersusModeScene(jul::Scene& scene);

    void IntroLevelScene(jul::Scene& scene);
    void Level1Scene(jul::Scene& scene);
    void Level2Scene(jul::Scene& scene);
    void Level3Scene(jul::Scene& scene);
    void ScoreScene(jul::Scene& scene);

    void MainMenuScene(jul::Scene& scene);
}  // namespace bb

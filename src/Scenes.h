#pragma once
#include <Scene.h>

namespace bb::scenes
{
    using namespace jul;

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

    void OnePlayerModeScene(Scene& scene);
    void TwoPlayerModeScene(Scene& scene);
    void VersusModeScene(Scene& scene);

    void IntroLevelScene(Scene& scene);
    void Level1Scene(Scene& scene);
    void Level2Scene(Scene& scene);
    void Level3Scene(Scene& scene);
    void ScoreScene(Scene& scene);

    void MainMenuScene(Scene& scene);


    ///////////////////
    /// TEST SCENES ///
    //////////////////
    void SceneGraphTestScene(Scene& scene);

}  // namespace bb

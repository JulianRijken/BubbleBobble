#pragma once
#include <Scene.h>

namespace bb
{
    using namespace jul;

    enum class SceneID
    {
        MainMenu,
        Main,
        IntroLevel,
        Level1,
        Level2,
        Level3,
    };


    void BindScenes();

    void MainScene(Scene& scene);

    void IntroLevel(Scene& scene);


    void MainMenuScene(Scene& scene);


    ///////////////////
    /// TEST SCENES ///
    //////////////////

    void TestScene(Scene& scene);

    void SceneGraphTestScene(Scene& scene);

}  // namespace bb

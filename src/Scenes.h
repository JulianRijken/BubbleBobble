#pragma once
#include <Scene.h>

namespace bb::scenes
{
    using namespace jul;

    enum class Id
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
    void Level1(Scene& scene);
    void Level2(Scene& scene);
    void Level3(Scene& scene);

    void IntroLevel(Scene& scene);


    void MainMenuScene(Scene& scene);


    ///////////////////
    /// TEST SCENES ///
    //////////////////

    void TestScene(Scene& scene);

    void SceneGraphTestScene(Scene& scene);

}  // namespace bb

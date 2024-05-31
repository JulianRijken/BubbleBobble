#pragma once
#include <Scene.h>

// TODO: It would be amazing to replace this with secene serialization!
namespace bb
{
    using namespace jul;

    void BindScenes();

    void MainScene(Scene& scene);

    void MainMenuScene(Scene& scene);

    void TestScene(Scene& scene);

    void SceneGraphTestScene(Scene& scene);

}  // namespace bb

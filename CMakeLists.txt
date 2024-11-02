cmake_minimum_required(VERSION 3.18)
project(BubbleBobble)

add_subdirectory(Julgen)

set(TARGET_NAME BubbleBobble)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


# Add all files from game
set(BB_SOURCE_FILES
    ${SOURCE_DIR}/Game.cpp                                          ${SOURCE_DIR}/Game.h
    ${SOURCE_DIR}/GameEntry.cpp
    ${SOURCE_DIR}/GameScore.h                                       ${SOURCE_DIR}/GameScore.cpp
    ${SOURCE_DIR}/IBubbleable.h
    ${SOURCE_DIR}/IDamageable.h
    ${SOURCE_DIR}/Prefabs.h                                         ${SOURCE_DIR}/Prefabs.cpp
    ${SOURCE_DIR}/Scenes.h                                          ${SOURCE_DIR}/Scenes.cpp

    # Components/
    ${SOURCE_DIR}/Components/Boulder.h                              ${SOURCE_DIR}/Components/Boulder.cpp
    ${SOURCE_DIR}/Components/Health.h                               ${SOURCE_DIR}/Components/Health.cpp
    ${SOURCE_DIR}/Components/DeadEnemy.h                            ${SOURCE_DIR}/Components/DeadEnemy.cpp
    ${SOURCE_DIR}/Components/Pickup.h                               ${SOURCE_DIR}/Components/Pickup.cpp
    ${SOURCE_DIR}/Components/CaptureBubble.h                        ${SOURCE_DIR}/Components/CaptureBubble.cpp
    ${SOURCE_DIR}/Components/OneWayPlatform.h                       ${SOURCE_DIR}/Components/OneWayPlatform.cpp

    # Components/Character/
    ${SOURCE_DIR}/Components/Character/Character.h                  ${SOURCE_DIR}/Components/Character/Character.cpp
    ${SOURCE_DIR}/Components/Character/CharacterInput.h             ${SOURCE_DIR}/Components/Character/CharacterInput.cpp

    # Components/Character/Enemys/
    ${SOURCE_DIR}/Components/Character/Enemys/Enemy.h               ${SOURCE_DIR}/Components/Character/Enemys/Enemy.cpp
    ${SOURCE_DIR}/Components/Character/Enemys/Maita.h               ${SOURCE_DIR}/Components/Character/Enemys/Maita.cpp
    ${SOURCE_DIR}/Components/Character/Enemys/MaitaBehaviour.h      ${SOURCE_DIR}/Components/Character/Enemys/MaitaBehaviour.cpp
    ${SOURCE_DIR}/Components/Character/Enemys/ZenChan.h             ${SOURCE_DIR}/Components/Character/Enemys/ZenChan.cpp
    ${SOURCE_DIR}/Components/Character/Enemys/ZenChanBehaviour.h    ${SOURCE_DIR}/Components/Character/Enemys/ZenChanBehaviour.cpp

    # Components/Character/Player/
    ${SOURCE_DIR}/Components/Character/Player/Player.h              ${SOURCE_DIR}/Components/Character/Player/Player.cpp
    ${SOURCE_DIR}/Components/Character/Player/PlayerState.h         ${SOURCE_DIR}/Components/Character/Player/PlayerState.cpp

    # Components/Level/
    ${SOURCE_DIR}/Components/Level/AutoMove.h                       ${SOURCE_DIR}/Components/Level/AutoMove.cpp
    ${SOURCE_DIR}/Components/Level/AutoRotateAround.h               ${SOURCE_DIR}/Components/Level/AutoRotateAround.cpp
    ${SOURCE_DIR}/Components/Level/BubbleSpiral.h                   ${SOURCE_DIR}/Components/Level/BubbleSpiral.cpp
    ${SOURCE_DIR}/Components/Level/LevelState.h                     ${SOURCE_DIR}/Components/Level/LevelState.cpp

    # Components/UI/
    ${SOURCE_DIR}/Components/UI/FpsCounter.h                        ${SOURCE_DIR}/Components/UI/FpsCounter.cpp
    ${SOURCE_DIR}/Components/UI/LevelHUD.h                          ${SOURCE_DIR}/Components/UI/LevelHUD.cpp
    ${SOURCE_DIR}/Components/UI/MainMenu.h                          ${SOURCE_DIR}/Components/UI/MainMenu.cpp
    ${SOURCE_DIR}/Components/UI/PlayerHUD.h                         ${SOURCE_DIR}/Components/UI/PlayerHUD.cpp
    ${SOURCE_DIR}/Components/UI/ScoreScreen.h                       ${SOURCE_DIR}/Components/UI/ScoreScreen.cpp
)

# Adds the source files to the executible
add_executable(${TARGET_NAME}
    ${BB_SOURCE_FILES}
)

# Include source directories
target_include_directories(${TARGET_NAME} PUBLIC
    ${SOURCE_DIR}
    ${SOURCE_DIR}/Components
    ${SOURCE_DIR}/Components/UI
    ${SOURCE_DIR}/Components/Level
    ${SOURCE_DIR}/Components/Character
    ${SOURCE_DIR}/Components/Character/Player
    ${SOURCE_DIR}/Components/Character/Enemys
)

# enable c++20 features
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_20)
set_property(TARGET ${TARGET_NAME} PROPERTY
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
    CXX_EXTENSIONS FALSE
)

# compile at max warning level + treat warnings as errors
target_compile_options(${TARGET_NAME} PUBLIC
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /MP>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Visual studio needs to be special
if(WIN32)
    # Fix startup project for visual stuidio
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
    # Fix working directory for visual studio
    set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TARGET_NAME}>")
endif()


target_link_libraries(${TARGET_NAME} PRIVATE julgen)

cmake_minimum_required(VERSION 3.18)
project(BubbleBobble)

add_subdirectory(Julgen)

set(TARGET_NAME BubbleBobble)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


# Add all files from game
set(BB_SOURCE_FILES
    ${SOURCE_DIR}/GameEntry.cpp
    ${SOURCE_DIR}/Game.cpp                          ${SOURCE_DIR}/Game.h
    ${SOURCE_DIR}/Scenes.h                          ${SOURCE_DIR}/Scenes.cpp
    ${SOURCE_DIR}/Prefabs.h                         ${SOURCE_DIR}/Prefabs.cpp
    ${SOURCE_DIR}/IDamageable.h
    ${SOURCE_DIR}/IBubbleable.h

    ${SOURCE_DIR}/Components/DeadEnemy.h            ${SOURCE_DIR}/Components/DeadEnemy.cpp
    ${SOURCE_DIR}/Components/Pickup.h               ${SOURCE_DIR}/Components/Pickup.cpp
    ${SOURCE_DIR}/Components/CaptureBubble.h        ${SOURCE_DIR}/Components/CaptureBubble.cpp

    ${SOURCE_DIR}/Components/Player/Player.h        ${SOURCE_DIR}/Components/Player/Player.cpp
    ${SOURCE_DIR}/Components/Player/PlayerState.h   ${SOURCE_DIR}/Components/Player/PlayerState.cpp

    ${SOURCE_DIR}/Components/Enemys/ZenChan.h       ${SOURCE_DIR}/Components/Enemys/ZenChan.cpp

    ${SOURCE_DIR}/Components/BubbleSpiral.h         ${SOURCE_DIR}/Components/BubbleSpiral.cpp
    ${SOURCE_DIR}/Components/OneWayPlatform.h       ${SOURCE_DIR}/Components/OneWayPlatform.cpp
    ${SOURCE_DIR}/Components/AutoMove.h             ${SOURCE_DIR}/Components/AutoMove.cpp
    ${SOURCE_DIR}/Components/AutoRotateAround.h     ${SOURCE_DIR}/Components/AutoRotateAround.cpp
    ${SOURCE_DIR}/Components/FpsCounter.h           ${SOURCE_DIR}/Components/FpsCounter.cpp
    ${SOURCE_DIR}/Components/Character.h            ${SOURCE_DIR}/Components/Character.cpp
    ${SOURCE_DIR}/Components/PlayerHUD.h            ${SOURCE_DIR}/Components/PlayerHUD.cpp
    ${SOURCE_DIR}/Components/MainMenu.h             ${SOURCE_DIR}/Components/MainMenu.cpp
)

# Adds the source files to the executible
add_executable(${TARGET_NAME}
    ${BB_SOURCE_FILES}
    src/Components/Enemys/ZenChanState.h src/Components/Enemys/ZenChanState.cpp
    src/CharacterInput.h src/CharacterInput.cpp
)

# Include source directories
target_include_directories(${TARGET_NAME} PUBLIC
    ${SOURCE_DIR}
    ${SOURCE_DIR}/Components
    ${SOURCE_DIR}/Components/Player
    ${SOURCE_DIR}/Components/Enemys
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

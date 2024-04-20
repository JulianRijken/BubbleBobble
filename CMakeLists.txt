cmake_minimum_required(VERSION 3.26)
project(BubbleBobble)

add_subdirectory(Julgen)
set(TARGET_NAME BubbleBobble)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(BB_SOURCE_FILES
     ${SOURCE_DIR}/GameEntry.cpp
     ${SOURCE_DIR}/Game.h                           ${SOURCE_DIR}/Game.cpp

     ${SOURCE_DIR}/Components/AutoMove.cpp          ${SOURCE_DIR}/Components/AutoMove.h
     ${SOURCE_DIR}/Components/AutoRotateAround.cpp  ${SOURCE_DIR}/Components/AutoRotateAround.h
     ${SOURCE_DIR}/Components/Bounce.cpp            ${SOURCE_DIR}/Components/Bounce.h
     ${SOURCE_DIR}/Components/FpsCounter.cpp        ${SOURCE_DIR}/Components/FpsCounter.h
     ${SOURCE_DIR}/Components/PlotExample.cpp       ${SOURCE_DIR}/Components/PlotExample.h
     ${SOURCE_DIR}/Components/Player.cpp            ${SOURCE_DIR}/Components/Player.h
     ${SOURCE_DIR}/Components/Character.cpp         ${SOURCE_DIR}/Components/Character.h
     ${SOURCE_DIR}/Components/PlayerHUD.cpp         ${SOURCE_DIR}/Components/PlayerHUD.h
)
add_executable(${TARGET_NAME}
    ${BB_SOURCE_FILES}
)

target_include_directories(${TARGET_NAME} PUBLIC
    ${SOURCE_DIR}
    ${SOURCE_DIR}/Components
)



HandleAssets($<TARGET_FILE_DIR:${TARGET_NAME}>)


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

if(WIN32)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
endif()

target_link_libraries(${TARGET_NAME} PRIVATE julgen)

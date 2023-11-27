cmake_minimum_required(VERSION 3.21)

project(
    offscreen
    VERSION 0.1
    DESCRIPTION "offscreen sample"
    LANGUAGES CXX
)

set(SRC_FILES
    offscreen_sample.h
    offscreen_sample.cpp
    main.cpp
)

set(SHADERS
    offscreen.vert
    offscreen.frag
    onscreen.vert
    onscreen.frag
)

configure_sample(offscreen)
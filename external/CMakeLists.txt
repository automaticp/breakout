include(FetchContent)

FetchContent_Declare(
    ogl-learning
    GIT_REPOSITORY https://github.com/automaticp/ogl-learning.git
    GIT_TAG        37eb4180da5feb71991ea7b0b89c12080978c503
)

FetchContent_MakeAvailable(ogl-learning)

add_library(learnlib::learnlib ALIAS learnlib)




find_package(box2d CONFIG REQUIRED)
set_target_properties(box2d::box2d PROPERTIES IMPORTED_GLOBAL TRUE)

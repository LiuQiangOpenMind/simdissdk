if(NOT TARGET simUtil)
    return()
endif()

project(EXAMPLE_SKYMODEL)

set(PROJECT_FILES
    ExampleSkyModel.cpp
)

add_executable(example_skymodel ${PROJECT_FILES})
target_link_libraries(example_skymodel PRIVATE simVis simUtil)
set_target_properties(example_skymodel PROPERTIES
    FOLDER "Examples"
    PROJECT_LABEL "Example - Sky Model"
)
vsi_install_target(example_skymodel SDK_Examples)

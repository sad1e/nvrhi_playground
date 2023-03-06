add_executable(playground WIN32 playground.cpp playground_app.h playground_ui.h playground_impl.cpp)
target_link_libraries(playground donut_render donut_app donut_engine)

set_target_properties(playground PROPERTIES FOLDER "NVRHI Playground")
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /MP")
endif()

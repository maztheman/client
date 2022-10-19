find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

add_executable(client 
    commands.cpp
    commands.hpp
    console.cpp  
    console.hpp
    console_session.cpp
    console_session.h
    client.cpp
    lua_script.cpp
    lua_script.h
    protected_vector.cpp
    protected_vector.h
    session.hpp 
    Settings.cpp
    Settings.h
    stdafx.cpp
    stdafx.h
)

target_link_libraries(
  client
  PRIVATE project_options
          project_warnings
          fmt::fmt
          spdlog::spdlog
          sol2::sol2
          glad::glad
          glfw::glfw
          imgui::imgui
          yaml-cpp::yaml-cpp
          -lstdc++)

target_include_directories(client PRIVATE ${CMAKE_BINARY_DIR}/bindings)
target_compile_definitions(client PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

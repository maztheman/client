find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(client 
    commands.cpp
    commands.hpp
    console.cpp  
    console.hpp
    console_session.cpp
    console_session.h
    client.cpp
    protected_vector.cpp
    protected_vector.h
    session.hpp 
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
          -lstdc++)

target_include_directories(client PRIVATE ${CMAKE_BINARY_DIR}/bindings)
target_compile_definitions(client PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

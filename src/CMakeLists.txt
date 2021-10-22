find_package(fmt)
find_package(spdlog)
find_package(docopt)
find_package(lua)
find_package(ImGui-SFML REQUIRED)

add_executable(client client.cpp commands.cpp lua_script.cpp Settings.cpp stdafx.cpp)
target_link_libraries(
  client
  PRIVATE project_options
          project_warnings
          docopt::docopt
          fmt::fmt
          spdlog::spdlog
          lua::lua
          ImGui-SFML::ImGui-SFML
          -lstdc++)

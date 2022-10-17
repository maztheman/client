==================
# client 2.1
==================

Console based telnet client with full color support.

#TODO
- Use imgui or similar tech to create a custom tty, this should allow the same code to be cross platform.
- Add other supported event types for scripting current there is only alias (commands you type as input) or actions (respoding to text coming from the server)
- use sol2 instead of raw lua


Release 2.2
- Rearchitect the whole project using ImGui for the interface
  * Basically the console is the main program, and the mud session is controlled by it, instead of the other way around


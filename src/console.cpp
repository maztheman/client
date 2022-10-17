#include "stdafx.h"
#include "console.hpp"

#include "session.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include <mutex>
#include <condition_variable>

#include <imgui_impl_glfw.cpp>
#include <imgui_impl_opengl3.cpp>

using write_lock = std::unique_lock<std::shared_mutex>;
using read_lock = std::shared_lock<std::shared_mutex>;

namespace kms {


struct HostNameFilter
{
    static int FilterInvalidHostname(ImGuiInputTextCallbackData* data)
    {
        if (data->EventChar < 256 && ':' == static_cast<char>(data->EventChar))
        {
            return 1;
        }
        return 0;
    }
};



console_t::console_t()
{
   
}

console_t::~console_t()
{
    
}

//session should probably own console_session_t, and we have an vector of sessions, and a thread that 

static void startSession(session_t* session, std::string serverAddr, int port, std::function<void(session_t*)> endCallback) 
{
    session->play();
    fmt::print(stderr, "session closed [{}:{}]\n", serverAddr, port);
    endCallback(session);
}


void console_t::run()
{
    glfwInit();
    const char* glsl_version = "#version 330";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);		

    m_window = glfwCreateWindow(800, 600, "Mud Client", nullptr, nullptr);
    glfwMakeContextCurrent(m_window);
    gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool show_demo_window = false;
    bool show_another_window = false;
    bool show_connect = false;
    bool connect_to_server = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    //for connecting
    char server[256] = {0};
    int server_port = 3000;

    while(!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Connect", nullptr, &show_connect))
                {
                    memset(server, 0, 256);
                    server_port = 3000;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (show_connect)
        {
            ImGui::Begin("Connect to server", &show_connect);

            bool reclaimFocus = false;

            if (ImGui::InputText("Host", server, 256, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter, HostNameFilter::FilterInvalidHostname))
            {
                writeText("pressed enter on host");
                if (strlen(server) > 0)
                {
                    show_connect = false;
                    connect_to_server = true;
                }
                else
                {
                    reclaimFocus = true;
                }
            }

            ImGui::SetItemDefaultFocus();
            if (reclaimFocus)
            {
                ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
            }

            ImGui::InputInt("Port", &server_port);

            if (ImGui::Button("Connect"))
            {
                show_connect = false;
                connect_to_server = true;
            }

            if (connect_to_server)
            {
                //this is where we spawn new session t
                auto pSession = std::make_unique<session_t>(fmt::format("{}:{}", server, server_port));
                writeText(fmt::format("Connecting to {}", pSession->getSessionName()));

                if (pSession->connect(server, server_port))
                {
                    m_SessionThreads.emplace_back(startSession, pSession.get(), server, server_port, [&cs = m_ConsoleSessions](session_t* session) {
                        cs.remove(session);
                    });
                    m_ConsoleSessions.add(std::move(pSession));
                    writeText("Connected");
                }
                
                connect_to_server = false;
            }
            ImGui::End();
        }

        ImGuiWindowFlags outputFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysVerticalScrollbar;

        if (ImGui::Begin("Output", nullptr, outputFlags))
        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            {
                //get a read lock for reading the current texts
                for(ro_strings serverTexts; const auto* text : m_texts.get(serverTexts))
                {
                    ImGui::Text("%s", text->c_str());
                }
            }

            ImGui::End();
        }

        for(std::vector<session_t*> consoleSessions; auto* cs : m_ConsoleSessions.get(consoleSessions))
        {
            ro_strings consoleText;
            cs->readText(consoleText);
            //TODO: add the open boolean to the console_session_t class, so we can close it ?, should execute a "exit" input
            if (ImGui::Begin(fmt::format("##{:p}", (void*)cs).c_str(), nullptr, ImGuiWindowFlags_NoTitleBar))
            {
                if (ImGui::BeginChild(1U, ImVec2(0.0f, -1.0f * (ImGui::GetTextLineHeightWithSpacing() + 6.0f)), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysVerticalScrollbar))
                {
                    for(const auto* line : consoleText)
                    {
                        ImGui::Text("%s", line->c_str());
                    }
                    ImGui::SetScrollHereY();
                    ImGui::EndChild();
                }

                auto& inputBuf = cs->getInputBuffer();

                if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
                    ImGui::SetKeyboardFocusHere(0);

                ImGui::PushItemWidth(-1.0f);
                if (ImGui::InputText("##mudinput", inputBuf.data(), inputBuf.size(), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    cs->addInput(inputBuf.data());
                }
                ImGui::PopItemWidth();
                ImGui::End();
            }
        }

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        //we need a "connect to this address option"

        //swap buffer
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);        
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);

    glfwTerminate();

    for(std::vector<session_t*> consoleSessions; auto* ses : m_ConsoleSessions.get(consoleSessions))
    {
        fmt::print(stderr, "trying to quit {}\n", ses->getSessionName());
        ses->close();
    }

    for(auto& th : m_SessionThreads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }
}

void console_t::writeText(std::string sText)
{
    m_texts.add(std::move(sText));
}

}
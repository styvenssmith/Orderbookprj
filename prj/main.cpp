
#include "imgui.h"
#include <map>
#include "order.h"
//#include "functions.h"
#include <vector>
#include <random>
#include <iostream>
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#ifdef __APPLE__
#define GLSL_VERSION "#version 150"
#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 2
#define OPENGL_PROFILE GLFW_OPENGL_CORE_PROFILE
#define OPENGL_FORWARD_COMPAT GL_TRUE
#elif defined(IMGUI_IMPL_OPENGL_ES2)
#define GLSL_VERSION "#version 100"
#define OPENGL_MAJOR_VERSION 2
#define OPENGL_MINOR_VERSION 0
#define OPENGL_PROFILE GLFW_OPENGL_ES_API
#else
#define GLSL_VERSION "#version 130"
#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 0
#endif

using namespace std;

double current_price = 50.0;

int main()
{
    OrderBook ord;

    
  
    // Initialize GLFW
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions and set window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, OPENGL_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, OPENGL_FORWARD_COMPAT);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "OrderBook Simulation", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);


    vector<double> prices, bidQuantities, askQuantities;
    bool dataUpdated = false;
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        
    // Generate new orders and update the order book
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> orders(1, 2);
    int o_rders = orders(gen);
    gen_erate(ord, o_rders, current_price);

    // Get the depth chart data
    vector<double> tempPrices, tempBidQuantities, tempAskQuantities;
    ord.getDepthChartLevels(tempPrices, tempBidQuantities, tempAskQuantities);

    // Check if the data has changed
    if (tempPrices != prices || tempBidQuantities != bidQuantities || tempAskQuantities != askQuantities) {
        prices = tempPrices;
        bidQuantities = tempBidQuantities;
        askQuantities = tempAskQuantities;
        dataUpdated = true;
    } else {
        dataUpdated = false;
    }

    // Show the depth chart using ImPlot if the data has changed
    ImGui::Begin("Depth Chart");
    //ImPlot::SetNextAxesLimits(0, 1000, 0, 2000, ImGuiCond_Always);
    if (dataUpdated && !prices.empty() && !bidQuantities.empty() && !askQuantities.empty()) {
        if (ImPlot::BeginPlot("Depth Chart", "Price", "Quantity")) {
            ImPlot::PlotBars("Bids", &prices[0], &bidQuantities[0], prices.size(), 0.5f);
            ImPlot::PlotBars("Asks", &prices[0], &askQuantities[0], prices.size(), 0.5f);
            ImPlot::EndPlot();
        }
    }
    ImGui::End();
    

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

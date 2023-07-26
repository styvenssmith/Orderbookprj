#include "imgui.h"
#include <map>
#include "order.h"
#include <vector>
#include <random>
#include <string>
#include <iostream>
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <atomic>
#include <thread>
#include <mutex>
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
std::string executionMessage;
std::mutex executionMutex; // Mutex for synchronization

// Function to generate orders
void generateOrders(OrderBook& ord, double& current_price, std::atomic<bool>& runFlag) {
    while (runFlag) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> orders(1, 2);
        int o_rders = orders(gen);
        gen_erate(ord, o_rders, current_price);
    }
}

int main() {
    OrderBook ord;

    //used to store historical prices
    vector<double> historicalPrices;
    // Create a flag to control the generate orders thread
    std::atomic<bool> runGenerateThread(true);

    // Create the generate orders thread and run it in the background
    std::thread generateOrdersThread(generateOrders, std::ref(ord), std::ref(current_price), std::ref(runGenerateThread));

    // Initialize GLFW
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions and set window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, OPENGL_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, OPENGL_FORWARD_COMPAT);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(800, 600, "OrderBook Simulation", nullptr, nullptr);
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

    const int DESIRED_FPS = 60;
    const double FRAME_TIME_MS = 1000.0 / DESIRED_FPS;

    // Main loop
    while (!glfwWindowShouldClose(window) && current_price > 0) {
        // Calculate the time taken for the previous frame
        static double lastFrameTime = glfwGetTime();
        double currentFrameTime = glfwGetTime();
        double deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Skip rendering if the frame was too fast (limit the frame rate)
        if (deltaTime < FRAME_TIME_MS / 1000.0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(FRAME_TIME_MS - deltaTime * 1000)));
            continue;
        }

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Get the depth chart data
        vector<double> tempPrices, tempBidQuantities, tempAskQuantities;
        ord.getDepthChartLevels(tempPrices, tempBidQuantities, tempAskQuantities);

        // Sort the bid and ask data based on price and time priority
        sort(tempPrices.begin(), tempPrices.end(), greater<double>()); // Sort bid prices in descending order
        sort(tempAskQuantities.begin(), tempAskQuantities.end(), greater<double>()); // Sort ask quantities in descending order

        // Check if the data has changed
        if (tempPrices != prices || tempBidQuantities != bidQuantities || tempAskQuantities != askQuantities) {
            prices = tempPrices;
            bidQuantities = tempBidQuantities;
            askQuantities = tempAskQuantities;
        }

        // Get the latest execution message from the OrderBook
        std::lock_guard<std::mutex> lock(executionMutex);
        if (!ord.executionMessages.empty()) {
            executionMessage = ord.executionMessages.back();
        }

        // Show the depth chart using ImPlot if the data has changed
        ImGui::Begin("Depth Chart");
        ImVec2 plotsize = ImVec2(400, 600);
        if (!prices.empty() && !bidQuantities.empty() && !askQuantities.empty()) {
            if (ImPlot::BeginPlot("Depth Chart", "Price", "Quantity", plotsize)) {
                ImPlot::PlotBars("Bids", &prices[0], &bidQuantities[0], prices.size(), 0.5f);
                ImPlot::PlotBars("Asks", &prices[0], &askQuantities[0], prices.size(), 0.5f);
                ImPlot::EndPlot();
            }
        }
        ImGui::TextWrapped("%s", executionMessage.c_str());
        ImGui::End();

        // Add current price to historical prices and plot the line chart using ImPlot
        historicalPrices.push_back(current_price);
        if (historicalPrices.size() > 1000) {
            // Keep only the last 100 data points to avoid cluttering the chart
            historicalPrices.erase(historicalPrices.begin());
        }
        ImGui::Begin("Stock Price");
        if (ImPlot::BeginPlot("Stock Price", "Time", "Price", ImVec2(400, 200))) {
            ImPlot::PlotLine("Price", &historicalPrices[0], historicalPrices.size());
            ImPlot::EndPlot();
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

        // Execute orders to update the depth chart
        ord.execute_order();
    }

    runGenerateThread = false;
    generateOrdersThread.join();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    ImPlot::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

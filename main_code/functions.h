#pragma once

#include <iostream>
#include <random>
#include <cmath>
#include <vector>
#include "imgui.h"
#include "order.h"
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

double current_price = 50.0;



void gen_erate(int orders, OrderBook& ord, float& timeSpeed) {
    //keep track of the order type
    vector<string>order_type{"ask", "bid"};
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> side(0, 1);  // buy or sell order
    uniform_int_distribution<int> sz(1, 20);   // size of the order
    uniform_int_distribution<int> i_d(1, 1000);// the person making the order
    
    // Imbalance factor for price adjustment based on buy/sell order imbalance
    double imbalance_factor = 0.0000001;
    // Volatility factor to control the magnitude of price changes
    double volatility_factor = 1;
    
    // Calculate order book depth
    int buy_depth = 0;
    int sell_depth = 0;
    vector<Order*> buy_orders = ord.getBuyOrder();
    vector<Order*> sell_orders = ord.getSellOrder();
    for (Order* order: buy_orders) {

        Order* temp = order;
        
        while(temp){
            buy_depth+=temp->quantity;
            temp = temp->next;
        }
    }
    for (Order* order : sell_orders) {
        
        Order* temp = order;
        while(temp){
            sell_depth+=temp->quantity;
            temp = temp->next;
        }
    }
    
    for (int i = 0; i < orders; i++) {
        // Side
        int sde = side(gen);
        // Size
        int szs = sz(gen) * 100;
        // ID
        int id_id = i_d(gen);

        // Calculate new price based on order book imbalance and volatility
        double imbalance = (sell_depth-buy_depth) * imbalance_factor;
        double volatility = (current_price * volatility_factor) * (static_cast<double>(rand()) / RAND_MAX - 0.5);
        //double n_price = current_price + imbalance + volatility;
        double n_price = current_price+volatility;

        // Check if the new price is negative and set it to 0 if necessary
        if (n_price < 0) {
            return;
        }

        // Create order
        Order* order = new Order(timeSpeed, order_type[sde], n_price, szs, id_id);
        if(order->side=="ask"){
            ord.addSellOrder(order);
        }
        else ord.addBuyOrder(order);

        delete order;

        current_price = n_price;
    }
        
}

void random_plot(float &timeSpeed, OrderBook& ord)
{   
    //generate the amount of orders
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> order(1, 10);
    int orders = order(gen)*100;

    static std::vector<double> xData;
    static std::vector<double> yData;

    static double lastMinute = 0.0;
    static double timeCounter = 0.0;

    double deltaTime = ImGui::GetIO().DeltaTime; // Get the time elapsed since the last frame
    timeCounter += deltaTime * static_cast<double>(timeSpeed);             // Increment the time counter based on the speed

    float currentMinute = floor(timeCounter / 60.0); // Get the current minute

    if (currentMinute > lastMinute)
    {
        double x = currentMinute;
        double y = gen_erate(orders,ord, currentMinute);
        
       
        xData.push_back(x);
        yData.push_back(y);

        lastMinute = currentMinute;
    }


    // Start the Dear ImGui frame
    ImGui::Begin("Plot");

    ImGui::SliderFloat("Time Speed", &timeSpeed, 0.1f, 100.0f); // Slider to control the speed of time
    // Plot the line chart
    if (ImPlot::BeginPlot("Stock Chart", "Time", "Value", ImVec2(-1, -1), ImPlotFlags_NoChild))
    {
        ImPlot::PlotLine("Line", xData.data(), yData.data(), static_cast<int>(xData.size()));

        ImPlot::EndPlot();
    }

    // End the Dear ImGui frame
    ImGui::End();
    
}

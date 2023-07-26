
#ifndef order_h
#define order_h
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <cmath>
#include <random>
#include <set>
#include <deque>

#include <list>

//extern std::string executionMessage;
using namespace std;

struct Order {
    int time;
    string side;
    double price;
    int quantity;
    int id;
    string ordType;
    Order* next;
    Order(int time, const string side, double price, int quantity, int id, string ordType)
    : time(time), side(side), price(price), quantity(quantity), id(id),ordType(ordType) {next = nullptr;}

};

struct sell{
    bool operator()(double a, double b)const{
        return a<b;
    }
};
struct buy{
    bool operator()(double a, double b)const{
        return a>b;
    }
};
class OrderBook{
    
private:
    
    map<int, double>id_price;
    map<double, Order*, buy>BuyOrder;
    map<double, Order*, sell>SellOrder;
    
public:
    std::deque<std::string> executionMessages;
    
    void executeMessage(string message){
        while(executionMessages.size()>=10){
            executionMessages.pop_front();
        }
        executionMessages.push_back(message);
    }
    void execute_order() {
        auto it = BuyOrder.begin();
        auto at = SellOrder.begin();
        
        string message;
        while (it != BuyOrder.end() && at != SellOrder.end()) {
            Order* buy = it->second;
            Order* sell = at->second;
            
            if (!buy || !sell) return;
            
            
            
            while (buy && sell) {
                
                
                // Safeguard to check if both buy and sell quantities are greater than 0
                if (buy->quantity <= 0) {
                    buy = buy->next;
                    continue; // Move to the next buy order at the same price level
                }
                if (sell->quantity <= 0) {
                    sell = sell->next;
                    continue; // Move to the next sell order at the same price level
                }
                
                //lmt, mkt
                if(buy->ordType=="mkt" || sell->ordType=="mkt"){
                    int mn = min(buy->quantity, sell->quantity);
                    if(buy->ordType=="mkt"){
                        message = to_string(buy->id)+" is buying "+to_string(mn)+" shares from "+to_string(sell->id)+" at: "+to_string(sell->price)+" mkt";

                    }
                    else message = to_string(sell->id)+" is selling "+to_string(mn)+" shares to "+to_string(buy->id)+" at: "+to_string(buy->price)+" mkt";
                    buy->quantity -= mn;
                    sell->quantity -= mn;
                    
                }
                else{
                    if(buy->price>=sell->price){
                        int mn = min(buy->quantity, sell->quantity);
                        message = to_string(buy->id)+" is buying "+ to_string(mn)+" shares from "+to_string(sell->id)+" at: "+to_string(sell->price)+" lmt ";
                        buy->quantity -= mn;
                        sell->quantity -= mn;
                    }
                    else return;
                }
                executeMessage(message);
                //executionMessages.push_back(message);
                
            }
            
            // Move to the next price level if there are no more orders at the current price level
            if (!buy) {
                it++;
            }
            if (!sell) {
                at++;
            }
        }
        
    }
    
    
    
    
    
    void removeBuyOrder(int id){
        double price = id_price[id];
        id_price.erase(id);
        auto it = BuyOrder.find(price);
        
        if (it != BuyOrder.end()) {
            Order* temp = it->second;
            Order* prev = nullptr;
            
            while (temp) {
                if (temp->id == id) {
                    if (prev) {
                        prev->next = temp->next;
                    } else {
                        it->second = temp->next;
                    }
                    delete temp;
                    if(BuyOrder[price]==nullptr) BuyOrder.erase(price);
                    return;
                }
                prev = temp;
                temp = temp->next;
            }
        }
    }
    
    void removeSellOrder(int id){
        double price = id_price[id];
        id_price.erase(id);
        auto it = SellOrder.find(price);
        
        if (it != SellOrder.end()) {
            Order* temp = it->second;
            Order* prev = nullptr;
            
            while (temp) {
                if (temp->id == id) {
                    if (prev) {
                        prev->next = temp->next;
                    } else {
                        it->second = temp->next;
                    }
                    delete temp;
                    if(SellOrder[price]==nullptr)SellOrder.erase(price);
                    return;
                }
                prev = temp;
                temp = temp->next;
            }
        }
    }
    
    void addBuyOrder(Order* ord) {
        if(ord->quantity<=0) return;
        id_price[ord->id] = ord->price;
        double val = ord->price;
        auto it = BuyOrder.find(val);
        
        if (it != BuyOrder.end()) {
            // If the price already exists in the map, append the order to the existing list
            
            Order* temp = it->second;
            if(temp){
                while(temp->next){
                    temp = temp->next;
                }
                temp->next = ord;
            }else{
                it->second = ord;
            }
        }else{
            BuyOrder[val] = ord;
        }
    }
    
    void addSellOrder(Order* ord) {
        if(ord->quantity<=0) return;
        id_price[ord->id] = ord->price;
        double val = ord->price;
        auto it = SellOrder.find(val);
        
        if (it != SellOrder.end()) {
            // If the price already exists in the map, append the order to the existing list
            
            Order* temp = it->second;
            if(temp){
                while(temp->next){
                    temp = temp->next;
                }
                temp->next = ord;
            }else{
                it->second = ord;
            }
        }else{
            SellOrder[val] = ord;
        }
    }
    
    vector<int> getBuyWithZero() {
        vector<int> ordersToDelete;
        for (const auto& entry : BuyOrder) {
            Order* order = entry.second;
            while (order) {
                if (order->quantity == 0) {
                    ordersToDelete.push_back(order->id);
                }
                order = order->next;
            }
        }
        return ordersToDelete;
    }
    
    void deleteBuyWithZero(const vector<int>& ordersToDelete) {
        for (int id : ordersToDelete) {
            removeBuyOrder(id);
        }
    }
    
    vector<int> getSellWithZero() {
        vector<int> ordersToDelete;
        for (const auto& entry : SellOrder) {
            Order* order = entry.second;
            while (order) {
                if (order->quantity == 0) {
                    ordersToDelete.push_back(order->id);
                }
                order = order->next;
            }
        }
        return ordersToDelete;
    }
    
    void deleteSellWithZero(const vector<int>& ordersToDelete) {
        for (int id : ordersToDelete) {
            removeSellOrder(id);
        }
    }

    void getDepthChartLevels(vector<double>& prices, vector<double>& bidQuantities, vector<double>& askQuantities) {
        prices.clear();
        bidQuantities.clear();
        askQuantities.clear();

        // Iterate through BuyOrder and SellOrder to get bid and ask levels
        for (const auto& entry : BuyOrder) {
            double price = entry.first;
            Order* temp = entry.second;
            int totalQuantity = 0;
            while (temp) {
                totalQuantity += temp->quantity;
                temp = temp->next;
            }
            prices.push_back(price);
            bidQuantities.push_back(totalQuantity);
        }

        for (const auto& entry : SellOrder) {
            double price = entry.first;
            Order* temp = entry.second;
            int totalQuantity = 0;
            while (temp) {
                totalQuantity += temp->quantity;
                temp = temp->next;
            }
            prices.push_back(price);
            askQuantities.push_back(totalQuantity);
        }
    }
    
    
};
    


vector<string>order_type{"bid", "ask"};
vector<string>lmtmkt{"lmt", "mkt"};
int timer = 1;
int id = 1;

void gen_erate(OrderBook& ord, int o_rders, double& current_price) {
    random_device rd;
    mt19937 gen(rd());

    uniform_int_distribution<int> side(0, 1);
    uniform_int_distribution<int> lm(0, 1);
    uniform_int_distribution<int> size(1, 10);
    uniform_real_distribution<double> price(current_price - 1.0, current_price + 1.0); // Limit price range

    while (o_rders--) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        int sde = side(gen);
        int sze = size(gen) * 100;
        double prices = (floor(price(gen) * 10) / 10); // Limit prices to 1 decimal place
        current_price = prices;
        if (current_price <= 0) return;
        int limmkt = lm(gen);
        Order* order = new Order(timer, order_type[sde], prices, sze, id, lmtmkt[limmkt]);
        ++id;

        if (order_type[sde] == "bid" && order->quantity > 0) {
            ord.addBuyOrder(order);
        }
        else if (order_type[sde] == "ask" && order->quantity > 0) {
            ord.addSellOrder(order);
        }
        else delete order;
    }
    timer++;
}



#endif

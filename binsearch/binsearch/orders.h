#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <random>
#include <set>

#include <list>

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
    
    void print(){
        
        auto it = BuyOrder.begin();
        int i = 0;
        while(it!=BuyOrder.end() && i<5){
            cout<<it->first<<"\n";
            Order* temp = it->second;
            
            while(temp){
                cout<<"("<<temp->quantity<<" "<<temp->id<<")";
                temp = temp->next;
            }
            cout<<"\n";
            i++;
            it++;
            
        }
        cout<<"\n";
        int a = 0;
        auto at = SellOrder.begin();
        while(at!=SellOrder.end() && a<5){
            cout<<at->first<<"\n";
            Order* temp2 = at->second;
            while(temp2){
                cout<<"("<<temp2->quantity<<" "<<temp2->id<<")";
                temp2 = temp2->next;
            }
            cout<<"\n";
            at++;
            a++;
        }
        cout<<"\n";
        
    }
    
    void execute_order() {
        auto it = BuyOrder.begin();
        auto at = SellOrder.begin();
        
        
        while (it != BuyOrder.end() && at != SellOrder.end()) {
            Order* buy = it->second;
            Order* sell = at->second;
            
            if (!buy || !sell) return;
            if (sell->price > buy->price) return;
            
            
            while (buy && sell && buy->price >= sell->price) {
                // Safeguard to check if both buy and sell quantities are greater than 0
                if (buy->quantity <= 0) {
                    buy = buy->next;
                    continue; // Move to the next buy order at the same price level
                }
                if (sell->quantity <= 0) {
                    sell = sell->next;
                    continue; // Move to the next sell order at the same price level
                }
                
                int mn = min(buy->quantity, sell->quantity);
                
                cout << buy->id << " is buying " << mn << " shares from " << sell->id << " at: " << sell->price << "\n";
                buy->quantity -= mn;
                sell->quantity -= mn;
                
                
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
    
    
};
    


vector<string>order_type{"bid", "ask"};
vector<string>lmtmkt{"lmt", "mkt"};
int timer = 1;
int id = 1;
double current_price = 50.0;
void gen_erate(OrderBook& ord, int o_rders){
    
    random_device rd;
    mt19937 gen(rd());
    
    //sided
    uniform_int_distribution<int> side(0,1);
    uniform_int_distribution<int> lm(0,1);
    uniform_int_distribution<int> size(1, 10);
    uniform_real_distribution<float> price(current_price-1.5, current_price+1.5);
    
    while(o_rders--){
        int sde = side(gen);
        int sze = size(gen)*100;
        double prices = (price(gen)*10)/10;
        current_price = prices;
        int limmkt = lm(gen);
        Order* order = new Order(timer,order_type[sde],prices, sze, id,lmtmkt[limmkt]);
        ++id;
        
        if(order_type[sde]=="bid" && order->quantity>0){
            ord.addBuyOrder(order);
        }
        else if(order_type[sde]=="ask" && order->quantity>0){
            ord.addSellOrder(order);
        }
        else delete order;
    }
    timer++;
    ord.print();
    cout<<"\n";
    cout<<"\n";
    ord.execute_order();
    cout<<"\n";
    cout<<"\n";
    
}
int main(){
    
    OrderBook ord;
    
    random_device rd;
    mt19937 gen(rd());
    
    //sided
    uniform_int_distribution<int> orders(1,1000);
    
   
    
    int i = 0;
    
    while(i<200){
        int o_rders = orders(gen);
        gen_erate(ord, o_rders);
        i++;
            
        vector<int> buystodelete = ord.getBuyWithZero();
        ord.deleteBuyWithZero(buystodelete);
        
        vector<int> sellstodelete = ord.getSellWithZero();
        ord.deleteSellWithZero(sellstodelete);
    }
    
    return 0;
    
}

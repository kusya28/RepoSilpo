#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <map>

using namespace std;

enum class OrderStatus {
    PENDING, 
    PAID, 
    COMPLETED, 
    CANCELLED 
};

struct CatalogItem {
    string unit; 
    double price;  
};

struct Product {
    string name;
    double price;
    double quantity;
    string unit;
};

class SilpoOrder {
private:
    vector<Product> basket;
    map<string, CatalogItem> catalog;
    OrderStatus status;
    string deliveryTime; 
    double discount; 

    string statusToString() const; 
public:
    SilpoOrder();

    void loadCatalog(string filename);
    void showCatalog() const;
    bool isProductInCatalog(string name) const;
    string getUnit(string name); 

    void addProduct(string name, double qty);
    void removeProduct(string name);
    void updateQuantity(string name, double newQty);
    void setStatus(OrderStatus newStatus);
    void setDeliveryTime(string dateTime);
    void applyPromoCode(string code);
    double calculateTotal() const;
    void saveReceipt(string filename) const;

    bool isBasketEmpty() const { return basket.empty(); }

    void saveToJsonHistory(string filename) const;
};
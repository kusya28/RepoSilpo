#include "Silpo.h"
#include "json.hpp"

using json = nlohmann::json;

SilpoOrder::SilpoOrder() : status(OrderStatus::PENDING), discount(0.0), deliveryTime("Not set") {}

void SilpoOrder::loadCatalog(string filename) {
    ifstream file(filename);
    string line, name, unit, priceStr;

    if (!file.is_open()) {
        cout << "Error: File " << filename << " not found!" << endl;
        return;
    }

    catalog.clear();
    while (getline(file, line)) {
        stringstream ss(line);
        if (getline(ss, name, ';') && getline(ss, unit, ';') && getline(ss, priceStr)) {
            catalog[name].unit = unit;
            catalog[name].price = stod(priceStr);
        }
    }
    file.close();
}

void SilpoOrder::showCatalog() const {
    cout << "\n" << setfill('=') << setw(45) << "" << setfill(' ') << endl;
    cout << left << setw(20) << "Product" << setw(12) << "Unit" << "Price" << endl;
    cout << setfill('-') << setw(45) << "" << setfill(' ') << endl;

    for (map<string, CatalogItem>::const_iterator it = catalog.begin(); it != catalog.end(); ++it) {
        cout << left << setw(20) << it->first
            << setw(12) << it->second.unit
            << it->second.price << " UAH" << endl;
    }
    cout << setfill('=') << setw(45) << "" << setfill(' ') << endl;
}

bool SilpoOrder::isProductInCatalog(string name) const {
    return catalog.find(name) != catalog.end();
}

string SilpoOrder::getUnit(string name) {
    if (isProductInCatalog(name)) {
        return catalog[name].unit;
    }
    return "";
}

void SilpoOrder::addProduct(string name, double qty) {
    if (status != OrderStatus::PENDING) {
        cout << ">>> Error: Cannot modify order. It is already paid or processed!" << endl;
        return;
    }
    
    if (isProductInCatalog(name)) {
        basket.push_back({
            name,
            catalog[name].price,
            qty,
            catalog[name].unit
            });

        cout << ">>> Added to basket." << endl;
    }
}

void SilpoOrder::removeProduct(string name) {
    for (vector<Product>::iterator it = basket.begin(); it != basket.end(); ++it) {
        if (it->name == name) {
            basket.erase(it);
            cout << ">>> Product removed." << endl;
            return;
        }
    }
    cout << ">>> Product not found in basket." << endl;
}

void SilpoOrder::updateQuantity(string name, double newQty) {
    for (size_t i = 0; i < basket.size(); i++) {
        if (basket[i].name == name) {
            basket[i].quantity = (double)newQty;
            cout << ">>> Quantity updated." << endl;
            return;
        }
    }
    cout << ">>> Product not found in basket." << endl;
}

double SilpoOrder::calculateTotal() const {
    double total = 0;
    for (size_t i = 0; i < basket.size(); i++) {
        total += basket[i].price * basket[i].quantity;
    }
    return total * (1.0 - discount);
}

void SilpoOrder::applyPromoCode(string code) {
    if (code == "SILPO2026") {
        discount = 0.10;
        cout << ">>> 10% discount applied!" << endl;
    }
    else {
        cout << ">>> Invalid promo code." << endl;
    }
}

void SilpoOrder::setDeliveryTime(string dateTime) {
    deliveryTime = dateTime;
}

void SilpoOrder::setStatus(OrderStatus newStatus) {
    // якщо замовленн€ скасоване або виконане, його статус уже н≥коли не можна зм≥нити
    if (status == OrderStatus::CANCELLED || status == OrderStatus::COMPLETED) {
        cout << ">>> Error: Cannot change status of a finalized order!" << endl;
        return;
    }

    // Ћог≥ка дозволених переход≥в
    if (newStatus == OrderStatus::PAID && status != OrderStatus::PENDING) {
        cout << ">>> Error: You can only pay for a pending order!" << endl;
        return;
    }

    if (newStatus == OrderStatus::COMPLETED && status != OrderStatus::PAID) {
        cout << ">>> Error: Cannot complete order before it is paid!" << endl;
        return;
    }

    // якщо все добре Ч зм≥нюЇмо
    status = newStatus;
    cout << ">>> Status updated to: " << statusToString() << endl;
}

string SilpoOrder::statusToString() const {
    switch (status) {
    case OrderStatus::PENDING: return "Unpaid";
    case OrderStatus::PAID: return "Paid";
    case OrderStatus::COMPLETED: return "Completed";
    case OrderStatus::CANCELLED: return "Cancelled";
    default: return "Unknown";
    }
}

void SilpoOrder::saveReceipt(string filename) const {
    ofstream file(filename);
    if (!file.is_open()) return;

    time_t now = time(0);
    char dt[26];
    ctime_s(dt, sizeof(dt), &now);

    file << "---------- SILPO RECEIPT ----------" << endl;
    file << "Date: " << dt;
    file << "Delivery: " << deliveryTime << endl;
    file << "Status: " << statusToString() << endl;
    file << "-----------------------------------" << endl;

    // —початку рахуЇмо чисту суму вс≥х товар≥в без знижки
    double subtotal = 0;
    for (size_t i = 0; i < basket.size(); i++) {
        file << left << setw(18) << basket[i].name
            << basket[i].quantity << " " << basket[i].unit
            << " x " << fixed << setprecision(2) << basket[i].price << endl;

        subtotal += basket[i].price * basket[i].quantity;
    }

    file << "-----------------------------------" << endl;

    // ¬иводимо пром≥жну суму (загальну варт≥сть товар≥в)
    file << fixed << setprecision(2);
    file << "Subtotal: " << subtotal << " UAH" << endl;

    // якщо застосовано промокод (discount > 0), виводимо розм≥р знижки
    if (discount > 0.0) {
        double discountAmount = subtotal * discount;
        file << "Discount (" << (discount * 100) << "%): -" << discountAmount << " UAH" << endl;
    }

    // ¬иводимо ф≥нальну суму до сплати
    file << "TOTAL: " << calculateTotal() << " UAH" << endl;
    file << "-----------------------------------" << endl;

    file.close();
}

void SilpoOrder::saveToJsonHistory(string filename) const {
    json historyArray = json::array(); // —творюЇмо порожн≥й масив JSON

    // —пробуЇмо прочитати вже на€вну ≥стор≥ю з файлу
    ifstream inFile(filename);
    if (inFile.is_open()) {
        try {
            inFile >> historyArray; // якщо файл Ї ≥ там правильний JSON, зчитуЇмо його
        }
        catch (...) {
            // якщо файл був порожн≥й або пошкоджений, починаЇмо з нового масиву
            historyArray = json::array();
        }
        inFile.close();
    }

    // —творюЇмо JSON-об'Їкт дл€ поточного чека
    json currentOrder;
    currentOrder["status"] = statusToString();
    currentOrder["delivery_time"] = deliveryTime;
    currentOrder["discount_percent"] = discount * 100;
    currentOrder["total_price"] = calculateTotal();

    // —творюЇмо масив товар≥в дл€ поточного чека
    json productsArray = json::array();
    for (size_t i = 0; i < basket.size(); i++) {
        json item;
        item["name"] = basket[i].name;
        item["quantity"] = basket[i].quantity;
        item["unit"] = basket[i].unit;
        item["price_per_unit"] = basket[i].price;
        item["subtotal"] = basket[i].price * basket[i].quantity;
        productsArray.push_back(item);
    }
    currentOrder["products"] = productsArray;

    // ƒодаЇмо поточний чек у загальну ≥стор≥ю
    historyArray.push_back(currentOrder);

    // «аписуЇмо оновлену ≥стор≥ю назад у файл 
    ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << historyArray.dump(4);
        outFile.close();
    }
}
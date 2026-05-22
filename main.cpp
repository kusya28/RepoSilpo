#pragma comment(lib, "winmm.lib")
#include "Silpo.h"
#include <windows.h>
#include <mmsystem.h>

int main() {
    // Запуск фонової музики по колу та асинхронно
    PlaySound(TEXT("C:\\Users\\User\\source\\repos\\NewRepo3\\background.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    cout << "------ SILPO ------\n";

    SilpoOrder myOrder;
    myOrder.loadCatalog("list_product.csv");

    int choice;
    string inputName, promo, timeStr;
    double qty;

    while (true) {
        cout << "\n1. Catalog | 2. Add | 3. Update Qty | 4. Remove | 5. Promo | 6. Time | 7. Pay & Save | 8. Exit\nChoice: ";
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        if (choice == 1) {
            myOrder.showCatalog();
        }
        else if (choice == 2) {
            cout << "Name: "; cin.ignore(); getline(cin, inputName);
            if (myOrder.isProductInCatalog(inputName)) {
                cout << "How many " << myOrder.getUnit(inputName) << "? "; cin >> qty;
                myOrder.addProduct(inputName, qty);
            }
            else {
                cout << "Not found!";
            }
        }
        else if (choice == 3) {
            cout << "Name: "; cin.ignore(); getline(cin, inputName);
            cout << "New quantity: "; cin >> qty;
            myOrder.updateQuantity(inputName, qty);
        }
        else if (choice == 4) {
            cout << "Name: "; cin.ignore(); getline(cin, inputName);
            myOrder.removeProduct(inputName);
        }
        else if (choice == 5) {
            cout << "Code: "; cin >> promo;
            myOrder.applyPromoCode(promo);
        }
        else if (choice == 6) {
            cout << "Time: ";
            cin.ignore();
            getline(cin, timeStr);
            myOrder.setDeliveryTime(timeStr);
        }
        else if (choice == 7) {
            if (myOrder.isBasketEmpty()) {
                cout << "Basket empty!";
            }
            else {
                myOrder.setStatus(OrderStatus::PAID);
                myOrder.saveReceipt("receipt.txt");

                // Зберігаємо цей чек у загальну базу даних JSON
                myOrder.saveToJsonHistory("all_receipts.json");

                cout << "Done! Saved to receipt.txt and all_receipts.json";
                break;
            }
        }
        else if (choice == 8) break;
    }
    PlaySound(NULL, 0, 0);
    return 0;
}
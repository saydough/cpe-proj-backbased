#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include "Order.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

// Function for converting the duration time of food from string to integer (coming from the csv file)
int stringToInt(const string& str) {
    istringstream iss(str);
    int result;
    if (!(iss >> result)) {
        throw invalid_argument("Invalid integer: " + str);
    }
    return result;
}

// Function to display an animation while user waits for order to be served
void displayLoadingAnimation(int seconds) {
    const int progressBarWidth = 50;

    cout << "Preparing your order" << endl;
    cout << "|";
    for (int i = 0; i < progressBarWidth; ++i) {
        float progress = static_cast<float>(i) / progressBarWidth;
        int barWidth = static_cast<int>(progress * progressBarWidth);

        cout << "=";

        cout << " " << int(progress * 100.0) << "% [";
        for (int j = 0; j < barWidth; ++j) {
            cout << "=";
        }
        for (int j = barWidth; j < progressBarWidth; ++j) {
            cout << " ";
        }
        cout << "]";
        cout << "\r";
        cout.flush();

#ifdef _WIN32                                                            // Include real-time duration of food
        Sleep(seconds * 1000 / progressBarWidth);                        // Ex: it takes 20 seconds to prepare burger,
#else                                                                    //     so user will have to wait 20 "actual" 
        usleep(seconds * 1000000 / progressBarWidth);                    //     seconds                     -Abads  
#endif
    }

    cout << " 100% [==================================================]" << endl;
}

// Function to basically display selected orders
void displaySelectedOrders(const vector<Order>& selectedOrders) {
    cout << "Your current order:" << endl;
    for (size_t i = 0; i < selectedOrders.size(); ++i) {
        cout << i + 1 << ". " << selectedOrders[i].item << " (Cooking time: " << selectedOrders[i].cookingTime << " seconds)" << endl;
    }
}

int main() {
    string userName;

    cout << "Enter your name: ";
    getline(cin, userName);

    vector<Order> menu;
    ifstream menuFile("menu.csv");

    if (!menuFile.is_open()) {
        cerr << "Failed to open the menu CSV file." << endl;
        return 1;
    }

    string line;
    getline(menuFile, line);                                     // This while loop receives data from a database (menu.csv)
    while (getline(menuFile, line)) {                            // and stores it in a vector called "menu" with the help of
        vector<string> tokens;                                   // the vector "tokens" since the "tokens" vector validates
        stringstream ss(line);                                   // if the database contains the duration of the food and food
        string token;                                            // itself                                           -Abads
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() == 2) {
            string itemName = tokens[0];
            int cookingTime = stringToInt(tokens[1]);
            menu.push_back(Order(itemName, cookingTime));
        }
    }

    menuFile.close();

/*=====================================================================================================================================
                                                            ORDERING OF FOOD
=====================================================================================================================================*/

    vector<Order> selectedOrders;

    while (true) {
        cout << "Menu:" << endl;
        for (size_t i = 0; i < menu.size(); ++i) {
            cout << i + 1 << ". " << menu[i].item << " (Cooking time: " << menu[i].cookingTime << " seconds)" << endl;
        }

        if (selectedOrders.size() != 0){
            displaySelectedOrders(selectedOrders);
        }

        // Asks the user for orders
        cout << "Select items from the menu (enter item numbers separated by spaces, '0' to finish, 'R' to remove): ";
        string input;
        getline(cin, input);
        istringstream iss(input);
        int itemNumber;

        // Lets the user order as much as he/she wants as long as it is within the range (1-6) since the menu does not exceed 6 orders
        while (iss >> itemNumber) {                                                            // >> operator parang iniinput niya to that certain variable
            if (itemNumber == 0) {                                                             // in this case, lahat ng nasa "iss" variable ay papasok sa
                break;                                                                         // itemNumber, also, this line of code only executes pag
            }                                                                                  // number ung ininput                              -Abads
            else if (itemNumber >= 1 && static_cast<size_t>(itemNumber) <= menu.size()) {
                selectedOrders.push_back(menu[itemNumber - 1]);
            } 
            else {
                cerr << "Invalid menu item number: " << itemNumber << ". Skipping." << endl;
            }
        }

        char option;
        cout << "Do you want to add more (A) or remove (R) items, or finish (F) your order? ";
        cin >> option;

        if (option == 'F' || option == 'f') {
            break;
        } 
        else if (option == 'R' || option == 'r') {
            displaySelectedOrders(selectedOrders);
            cout << "Enter the item number to remove: ";
            int removeNumber;
            cin >> removeNumber;
            if (removeNumber >= 1 && static_cast<size_t>(removeNumber) <= selectedOrders.size()) {
                selectedOrders.erase(selectedOrders.begin() + removeNumber - 1);
            } else {
                cerr << "Invalid item number to remove: " << removeNumber << ". Skipping." << endl;
            }
        } 
        else if (option != 'A' && option != 'a') {
            cerr << "Invalid option: " << option << ". Skipping." << endl;
        }
        cin.ignore(); 
    }

    int maxCookingTime = 0;                                           // Since the "selectedOrders" vector's elements can be accessed using iterators,
    for (const Order& order : selectedOrders) {                       // "order" with datatype "Order" serves as an iterator which determines the
        if (order.cookingTime > maxCookingTime) {                     // maximum cooking time among the list of selected Orders.
            maxCookingTime = order.cookingTime;                       // Ex.: I orderred Burger (20 seconds), Fries (13 seconds), the prepartion time 
        }                                                             //      would be 20 seconds.                                         -Abads 
    }

    ofstream outputFile("completed_orders.csv", ios::app);

    if (!outputFile.is_open()) {
        cerr << "Failed to open the CSV file for writing." << endl;
        return 1;
    }

    outputFile << "Customer: " << userName << endl;

    displayLoadingAnimation(maxCookingTime);

    // Outputs the completed orders to the completed_orders CSV file
    for (const Order& order : selectedOrders) {
        cout << "Completed " << order.item << endl;
        outputFile << order.item << endl;
    }

    outputFile << endl;

    outputFile.close();

    return 0;
}

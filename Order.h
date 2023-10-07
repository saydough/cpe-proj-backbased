#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <iostream>

struct Order {
    std::string item;
    int cookingTime;

    Order(const std::string& item, int cookingTime) : item(item), cookingTime(cookingTime) {}

    bool operator>(const Order& other) const {
        return cookingTime > other.cookingTime;
    }
};

#endif // ORDER_H
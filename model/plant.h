#pragma once
#include <string>

using namespace std;

//
class Plant {
private:
    string name;
    string species;
    int quantity;
    double price;
public:
    // Constructor
    Plant(const string &name, const string &species, int quantity, double price) :
        name(name), species(species), quantity(quantity), price(price) {}
    // Getters
    string getName() const { return name; }
    string getSpecies() const { return species; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    // Setters
    void setPrice(double newPrice) { price = newPrice; }
    void setQuantity(int newQuantity) { quantity = newQuantity; }
};

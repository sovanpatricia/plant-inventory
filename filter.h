#pragma once
#include "../Model/plant.h"

#include <string>
#include <memory>
#include <vector>

using namespace std;

// Abstract base class for plant filters
class PlantFilter {
public:
    // Checks if the given plant matches the filter criteria
    virtual bool matches(const Plant &plant) const = 0;

    virtual ~PlantFilter() = default; // Destructor
};

// Filter for price range
class PricePlantFilter : public PlantFilter {
private:
    double minPrice;
    double maxPrice;
public:
    PricePlantFilter(double min, double max) : minPrice(min), maxPrice(max) {}

    // Returns true if plant price is within the range [minPrice, maxPrice]
    bool matches(const Plant &plant) const override {
        return plant.getPrice() >= minPrice && plant.getPrice() <= maxPrice;
    }
};

// Filter for substring match in plant name
class NamePlantFilter : public PlantFilter {
private:
    string substring;
public:
    explicit NamePlantFilter(const string &substring) : substring(substring) {}

    // Returns true if the plant's name contains the substring
    bool matches(const Plant &plant) const override {
        return plant.getName().find(substring) != string::npos;
    }
};

// Filter for exact species
class SpeciesPlantFilter : public PlantFilter {
private:
    string species;
public:
    explicit SpeciesPlantFilter(const string &species) : species(species) {}

    // Returns true if the plant's species matches the filter species
    bool matches(const Plant &plant) const override {
        return plant.getSpecies() == species;
    }
};

// Filter for stock availability (in stock / out of stock)
class StockAvailabilityPlantFilter : public PlantFilter {
private:
    bool shouldBeInStock;
public:
    explicit StockAvailabilityPlantFilter(bool shouldBeInStock) : shouldBeInStock(shouldBeInStock) {}

    // Returns true if the stock status matches the filter (quantity > 0 for in stock)
    bool matches(const Plant &plant) const override {
        return (plant.getQuantity() > 0) == shouldBeInStock;
    }
};

// Filter for minimum quantity
class MinQuantityPlantFilter : public PlantFilter {
private:
    int minQuantity;
public:
    explicit MinQuantityPlantFilter(int minQuantity) : minQuantity(minQuantity) {}

    // Returns true if the plant's quantity is at least minQuantity
    bool matches(const Plant &plant) const override {
        return plant.getQuantity() >= minQuantity;
    }
};

// Composite filter for logical AND of multiple filters
class AndPlantFilter : public PlantFilter {
private:
    vector<shared_ptr<PlantFilter>> filters;
public:
    explicit AndPlantFilter(const vector<shared_ptr<PlantFilter>>& filters)
        : filters(filters) {}

    // Returns true only if the plant matches all subfilter
    bool matches(const Plant& plant) const override {
        for (const auto& filter : filters)
            if (!filter->matches(plant)) return false;
        return true;
    }
};

// Composite filter for logical OR of multiple filters
class OrPlantFilter : public PlantFilter {
private:
    vector<shared_ptr<PlantFilter>> filters;
public:
    explicit OrPlantFilter(const vector<shared_ptr<PlantFilter>>& filters)
        : filters(filters) {}

    // Returns true if the plant matches at least one subfilter
    bool matches(const Plant& plant) const override {
        for (const auto& filter : filters)
            if (filter->matches(plant)) return true;
        return false;
    }
};

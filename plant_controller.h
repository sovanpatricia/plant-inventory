#pragma once
#include "../Model/plant.h"
#include "../Repository/plant_repository.h"
#include "command.h"
#include "filter.h"

#include <memory>
#include <vector>
#include <stack>
#include <string>
#include <stdexcept>

using namespace std;

class PlantController {
private:
    // The repository (CSV, JSON, etc.) where the plants are stored
    unique_ptr<PlantRepository> repository;

    // Undo/Redo stacks
    stack<unique_ptr<Command>> undoStack;
    stack<unique_ptr<Command>> redoStack;

    // Validators
    void validateQuantity(int quantity) const;
    void validatePrice(double price) const;

public:
    // Constructor
    explicit PlantController(unique_ptr<PlantRepository> repository);

    // Core CRUD operations
    void addPlant(const string &name, const string &species, int quantity, double price);
    void removePlant(const string &name);
    void updatePlant(const string &name, const string &species, int quantity, double price);

    // Undo/Redo
    void undo();
    void redo();

    // Returns a vector with all plants
    vector<Plant> getAllPlants() const;

    // Finds a plant by name
    Plant getPlantByName(const string &name) const;

    // Returns all plants where name or species contains the searchTerm
    vector<Plant> searchPlants(const string &searchTerm) const;

    // Statistics
    double getTotalInventoryValue() const;
    int getTotalQuantity() const;
    int getTotalUniquePlants() const;

    // Returns plants that match all (AND) or any (OR) of the given filters
    // By default, uses AND combination
    vector<Plant> filterPlants(const vector<shared_ptr<PlantFilter>> &filters, bool useAnd = true) const;
};
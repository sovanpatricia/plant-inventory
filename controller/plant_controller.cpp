#include "plant_controller.h"
#include "../repository/plant_repository.h"
#include "command.h"
#include "filter.h"

#include <algorithm>
#include <stdexcept>
#include <stack>
#include <string>

using namespace std;

// Constructor
PlantController::PlantController(unique_ptr<PlantRepository> repository)
    : repository(move(repository)) {
}

// Throws if quantity is negative
void PlantController::validateQuantity(int quantity) const {
    if (quantity < 0) {
        throw invalid_argument("Quantity must be greater than 0");
    }
}

// Throws if price is negative
void PlantController::validatePrice(double price) const {
    if (price < 0) {
        throw invalid_argument("Price must be greater than 0");
    }
}

// Adds a new plant using Command Pattern for undo/redo support
void PlantController::addPlant(const string &name, const string &species, int quantity, double price) {
    validateQuantity(quantity);
    validatePrice(price);

    Plant plant(name, species, quantity, price);
    auto cmd = make_unique<AddPlantCommand>(repository.get(), plant);
    cmd->execute();
    undoStack.push(move(cmd));
    while (!redoStack.empty()) redoStack.pop(); // Clear redo stack on new operation
}

// Removes a plant by name using Command Pattern for undo/redo support
void PlantController::removePlant(const string &name) {
    Plant toRemove = repository->getPlantByName(name);
    auto cmd = make_unique<RemovePlantCommand>(repository.get(), toRemove);
    cmd->execute();
    undoStack.push(move(cmd));
    while (!redoStack.empty()) redoStack.pop(); // Clear redo stack on new operation
}

// Updates a plant by name using Command Pattern for undo/redo support
void PlantController::updatePlant(const string &name, const string &species, int quantity, double price) {
    validateQuantity(quantity);
    validatePrice(price);

    Plant oldPlant = repository->getPlantByName(name);
    Plant newPlant(name, species, quantity, price);
    auto cmd = make_unique<UpdatePlantCommand>(repository.get(), oldPlant, newPlant);
    cmd->execute();
    undoStack.push(move(cmd));
    while (!redoStack.empty()) redoStack.pop(); // Clear redo stack on new operation
}

// Undoes the last command, if available. Moves it to the redo stack
void PlantController::undo() {
    if (undoStack.empty()) { throw runtime_error("Nothing to undo"); }
    auto cmd = move(undoStack.top());
    undoStack.pop();
    cmd->undo();
    redoStack.push(move(cmd));
}

// Redoes the last undone command, if available. Moves it back to the undo stack
void PlantController::redo() {
    if (redoStack.empty()) { throw runtime_error("Nothing to redo"); }
    auto cmd = move(redoStack.top());
    redoStack.pop();
    cmd->execute();
    undoStack.push(move(cmd));
}

// Returns a vector with all plants from the repository
vector<Plant> PlantController::getAllPlants() const { return repository->getAllPlants(); }

// Returns a specific plant by name or throws if not found
Plant PlantController::getPlantByName(const string &name) const { return repository->getPlantByName(name); }

// Returns plants where either name or species contains the search term
vector<Plant> PlantController::searchPlants(const string &searchTerm) const {
    vector<Plant> allPlants = repository->getAllPlants();
    vector<Plant> matchingPlants;

    for (const auto &plant : allPlants) {
        if (plant.getName().find(searchTerm) != string::npos ||
            plant.getSpecies().find(searchTerm) != string::npos) {
            matchingPlants.push_back(plant);
        }
    }
    return matchingPlants;
}

// Returns the total value of inventory
double PlantController::getTotalInventoryValue() const {
    double totalValue = 0;
    for (const auto &plant : repository->getAllPlants()) {
        totalValue += plant.getQuantity() * plant.getPrice();
    }
    return totalValue;
}

// Returns the sum of all plant quantities
int PlantController::getTotalQuantity() const {
    int totalQuantity = 0;
    for (const auto &plant : repository->getAllPlants()) {
        totalQuantity += plant.getQuantity();
    }
    return totalQuantity;
}

// Returns the number of unique plants
int PlantController::getTotalUniquePlants() const { return repository->getAllPlants().size(); }

// Filters plants using a vector of PlantFilters
// If filters is empty, returns all plants
// If useAnd is true, combines filters with logical AND, else with OR
vector<Plant> PlantController::filterPlants(
    const vector<shared_ptr<PlantFilter>>& filters, bool useAnd) const
{
    vector<Plant> allPlants = repository->getAllPlants();
    vector<Plant> result;

    if (filters.empty())
        return allPlants;

    // Combine filters using AND/OR composite filter
    shared_ptr<PlantFilter> combined;
    if (useAnd)
        combined = make_shared<AndPlantFilter>(filters);
    else
        combined = make_shared<OrPlantFilter>(filters);

    // Collect only plants that match the combined filter
    for (const auto& plant : allPlants)
        if (combined->matches(plant))
            result.push_back(plant);

    return result;
}






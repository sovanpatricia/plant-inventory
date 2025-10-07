#pragma once
#include "../Model/plant.h"

#include <vector>
#include <string>

using namespace std;

// Abstract base class for plant repositories
class PlantRepository {
public:
    // Exception thrown when adding a plant with a duplicate name
    class DuplicatePlantException : public runtime_error {
    public:
        explicit DuplicatePlantException(const string &name)
            : runtime_error("Plant with name '" + name + "' already exists") {}
    };
    // Exception thrown when a plant is not found by name
    class PlantNotFoundException : public runtime_error {
    public:
        explicit PlantNotFoundException(const string &name)
            : runtime_error("Plant with name '" + name + "' does not exist") {}
    };

    // Adds a new plant to the repository
    virtual void addPlant(const Plant&) = 0;

    // Removes a plant by name from the repository
    virtual void removePlant(const string &name) = 0;

    // Updates a plant (by name) in the repository
    virtual void updatePlant(const Plant&) = 0;

    // Retrieves a plant by name
    virtual Plant getPlantByName(const string &name) const = 0;

    // Returns a vector with all plants in the repository
    virtual vector<Plant> getAllPlants() const = 0;

    // Checks if a plant with the given name exists in the repository
    virtual bool exists(const string &name) const = 0;

    // Virtual destructor for proper cleanup of derived classes
    virtual ~PlantRepository() = default;
};

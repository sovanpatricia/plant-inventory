#pragma once

#include "plant_repository.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <string>
#include <vector>

using namespace std;

// Concrete implementation of PlantRepository that stores plants in a JSON file
class JSONPlantRepository : public PlantRepository {
private:
    string filename;
    vector<Plant> plants;

    // Loads all plants from the JSON file into the 'plants' vector.
    void loadFromFile();

    // Saves all plants from the 'plants' vector back to the JSON file.
    void saveToFile() const;

    // Converts a QJsonObject to a Plant object.
    static Plant plantFromJson(const QJsonObject &json);

    // Converts a Plant object to a QJsonObject.
    static QJsonObject plantToJson(const Plant &plant);

public:
    // Constructor
    explicit JSONPlantRepository(string filename);

    // Adds a new plant to the repository
    void addPlant(const Plant& plant) override;

    // Removes a plant by name from the repository
    void removePlant(const string& name) override;

    // Updates a plant (by name) in the repository
    void updatePlant(const Plant& plant) override;

    // Retrieves a plant by name
    Plant getPlantByName(const string &name) const override;

    // Returns a vector with all plants in the repository
    vector<Plant> getAllPlants() const override;

    // Checks if a plant with the given name exists in the repository
    bool exists(const string& name) const override;

    // Destructor
    ~JSONPlantRepository() override = default;
};
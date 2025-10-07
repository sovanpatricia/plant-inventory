#pragma once
#include "plant_repository.h"

#include <string>
#include <fstream>
#include <vector>

using namespace std;

// Concrete implementation of PlantRepository that stores plants in a CSV file
class CSVPlantRepository : public PlantRepository {
private:
       string filename;
       vector<Plant> plants;

       // Loads all plants from the CSV file into the 'plants' vector
       void loadFromFile();

       // Writes all plants from the 'plants' vector back to the CSV file
       void saveToFile() const;

       // Parses a single line from the CSV file and adds the plant to 'plants'
       void parseCSVLine(const string &line);

       // Converts a Plant object to a CSV-formatted line
       string plantToCSVLine(const Plant &plant) const;

public:
       // Constructor
       explicit CSVPlantRepository(string filename);

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
       ~CSVPlantRepository() override = default;
};
#include "csv_plant_repository.h"

#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// Constructor
CSVPlantRepository::CSVPlantRepository(string filename)
    : filename(move(filename)) { loadFromFile(); }

// Loads all plants from the CSV file into the 'plants' vector
void CSVPlantRepository::loadFromFile() {
    plants.clear();
    ifstream file(filename);

    // Open file for reading
    if (!file.is_open()) {
        throw runtime_error("Could not open file " + filename);
    }

    string line;
    bool isFirstLine = true;
    while (getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false; // Skip header line
            continue;
        }
        if (!line.empty()) parseCSVLine(line);
    }
    file.close();
}

// Parses a single line from the CSV file and adds a Plant object to the vector
void CSVPlantRepository::parseCSVLine(const string &line) {
    stringstream ss(line);
    string name, species, quantityStr, priceStr;

    // Extract each field separated by commas
    getline(ss, name, ',');
    getline(ss, species, ',');
    getline(ss, quantityStr, ',');
    getline(ss, priceStr, ',');

    int quantity = stoi(quantityStr);
    double price = stod(priceStr);

    plants.emplace_back(name, species, quantity, price);
}

// Saves all plants from the vector into the CSV file
// Overwrites the file and writes a header line first
void CSVPlantRepository::saveToFile() const {
    ofstream file(filename);
    if (!file.is_open()) { throw runtime_error("Could not open file " + filename); }

    file << "Name, Species, Quantity, Price\n";
    for (const auto &plant : plants) {
        file << plantToCSVLine(plant) << "\n";
    }
    file.close();
}

// Converts a Plant object into a CSV-formatted string line
string CSVPlantRepository::plantToCSVLine(const Plant &plant) const {
    return plant.getName() + "," +
           plant.getSpecies() + "," +
           to_string(plant.getQuantity()) + "," +
           to_string(plant.getPrice());
}

// Adds a new plant to the repository and saves to file
void CSVPlantRepository::addPlant(const Plant& plant) {
    if (exists(plant.getName())) { throw DuplicatePlantException(plant.getName()); }
    plants.push_back(plant);
    saveToFile();
}

// Removes a plant by name and saves to file
void CSVPlantRepository::removePlant(const string& name) {
    auto it = ranges::find_if(plants, [&name](const Plant& p) {
        return p.getName() == name;
    });
    if (it == plants.end()) { throw PlantNotFoundException(name); }

    plants.erase(it);
    saveToFile();
}

// Updates a plant by name and saves to file
void CSVPlantRepository::updatePlant(const Plant& plant) {
    auto it = ranges::find_if(plants, [&plant](const Plant& p) {
        return p.getName() == plant.getName();
    });
    if (it == plants.end()) { throw PlantNotFoundException(plant.getName()); }

    *it = plant;
    saveToFile();
}

// Returns the Plant object with the given name
Plant CSVPlantRepository::getPlantByName(const string &name) const {
    auto it = ranges::find_if(plants, [&name](const Plant& p) {
        return p.getName() == name;
    });
    if (it == plants.end()) {
        throw PlantNotFoundException(name);
    }
    return *it;
}

// Returns a vector of all plants in the repository
vector<Plant> CSVPlantRepository::getAllPlants() const { return plants; }

// Checks if a plant with the given name exists in the repository
bool CSVPlantRepository::exists(const string& name) const {
    return ranges::any_of(plants,[&name](const Plant& p)
        { return p.getName() == name; });
}


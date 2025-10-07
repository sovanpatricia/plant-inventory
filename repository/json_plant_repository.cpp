#include "json_plant_repository.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <algorithm>
#include <stdexcept>

using namespace std;

// Constructor
JSONPlantRepository::JSONPlantRepository(string filename)
    : filename(move(filename)) { loadFromFile(); }

// Loads all plants from the JSON file into the 'plants' vector
void JSONPlantRepository::loadFromFile() {
    QFile file(QString::fromStdString(filename));

    // Open file for reading
    if (!file.open(QIODevice::ReadOnly)) {
        throw runtime_error("Could not open file " + filename);
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    // Check if document is valid
    if (doc.isNull()) {
        file.close();
        throw runtime_error("File " + filename + " is not a valid JSON document");
    }

    // Check if root is a JSON object
    if (!doc.isObject()) {
        file.close();
        throw runtime_error("File " + filename + " is not a valid JSON object");
    }

    QJsonObject json = doc.object();
    QJsonArray plantsArray = json["plants"].toArray();
    plants.clear();

    // For each plant in the array, convert it from JSON and add to vector
    for (const QJsonValue &plantJson : plantsArray) {
        if (plantJson.isObject()) {
            QJsonObject plantObject = plantJson.toObject();
            plants.push_back(plantFromJson(plantObject));
        }
    }
    file.close();
}

// Saves all plants from the 'plants' vector into the JSON file
void JSONPlantRepository::saveToFile() const {
    QFile file(QString::fromStdString(filename));

    if (!file.open(QIODevice::WriteOnly)) {
        throw runtime_error("Could not open file " + filename);
    }
    QJsonArray plantsArray;

    // Convert each Plant to JSON and add to array
    for (const auto &plant : plants) {
        plantsArray.append(plantToJson(plant));
    }
    QJsonObject root;
    root["plants"] = plantsArray;

    // Create a JSON document and write it in indented form
    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

// Converts a QJsonObject to a Plant object
Plant JSONPlantRepository::plantFromJson(const QJsonObject &json) {
    return Plant(
        json["name"].toString().toStdString(),
        json["species"].toString().toStdString(),
        json["quantity"].toInt(),
        json["price"].toDouble());
}

// Converts a Plant object to a QJsonObject
QJsonObject JSONPlantRepository::plantToJson(const Plant& plant) {
    QJsonObject plantJson;
    plantJson["name"] = QString::fromStdString(plant.getName());
    plantJson["species"] = QString::fromStdString(plant.getSpecies());
    plantJson["quantity"] = plant.getQuantity();
    plantJson["price"] = plant.getPrice();
    return plantJson;
}

// Adds a new plant to the repository and saves to file
void JSONPlantRepository::addPlant(const Plant& plant) {
    if (exists(plant.getName())) { throw DuplicatePlantException(plant.getName()); }
    plants.push_back(plant);
    saveToFile();
}

// Removes a plant by name and saves to file
void JSONPlantRepository::removePlant(const string& name) {
    auto it = ranges::find_if(plants, [&name](const Plant& p) {
        return p.getName() == name;
    });
    if (it == plants.end()) { throw PlantNotFoundException(name); }
    plants.erase(it);
    saveToFile();
}

// Updates a plant by name and saves to file
void JSONPlantRepository::updatePlant(const Plant& plant) {
    auto it = ranges::find_if(plants, [&plant](const Plant& p) {
        return p.getName() == plant.getName();
    });
    if (it == plants.end()) { throw PlantNotFoundException(plant.getName()); }
    *it = plant;
    saveToFile();
}

// Returns the Plant object with the given name
Plant JSONPlantRepository::getPlantByName(const string &name) const {
    auto it = ranges::find_if(plants, [&name](const Plant& p) {
        return p.getName() == name;
    });
    if (it == plants.end()) { throw PlantNotFoundException(name); }
    return *it;
}

// Returns a vector of all plants in the repository
vector<Plant> JSONPlantRepository::getAllPlants() const { return plants; }

// Checks if a plant with the given name exists in the repository
bool JSONPlantRepository::exists(const string& name) const {
    return ranges::any_of(plants, [&name](const Plant& p)
        { return p.getName() == name; });
}


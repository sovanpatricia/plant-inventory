#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>

#include "../Model/plant.h"
#include "../Repository/plant_repository.h"
#include "../Repository/csv_plant_repository.h"
#include "../Repository/json_plant_repository.h"
#include "../Controller/plant_controller.h"
#include  "../Controller/filter.h"

using namespace std;

// Helper to delete test files
void deleteTestFiles(const string &filename) {
    remove(filename.c_str());
}

TEST(CSVPlantRepositoryTest, AddUpdateRemoveCRUD) {
    const string testFile = "test_plants.csv";
    deleteTestFiles(testFile); // Delete the file if it exists

    // Create file with header to avoid parsing errors
    ofstream out(testFile, ios::out);
    out << "Name,Species,Quantity,Price\n";
    out.close();

    {
        // Create a new CSVPlantRepository
        CSVPlantRepository repo(testFile);
        repo.addPlant(Plant("Aloe", "Succulent", 5, 15.5));
        repo.addPlant(Plant("Rose", "Flower", 10, 8.9));

        // Check if the plants were added correctly
        auto all = repo.getAllPlants();
        ASSERT_EQ(all.size(), 2);
        ASSERT_EQ(all[0].getName(), "Aloe");
        ASSERT_EQ(all[1].getName(), "Rose");

        // Check if the plants were updated correctly
        repo.updatePlant(Plant("Aloe", "Succulent", 7, 17.5));
        Plant aloe = repo.getPlantByName("Aloe");
        ASSERT_EQ(aloe.getQuantity(), 7);
        ASSERT_DOUBLE_EQ(aloe.getPrice(), 17.5);

        // Check if the plants were removed correctly
        repo.removePlant("Rose");
        ASSERT_EQ(repo.getAllPlants().size(), 1);

        EXPECT_THROW(repo.addPlant(Plant("Aloe", "Succulent", 1, 13)), PlantRepository::DuplicatePlantException);
        EXPECT_THROW(repo.removePlant("NoSuchPlant"), PlantRepository::PlantNotFoundException);
    }
    deleteTestFiles(testFile);
}

TEST(JSONPlantRepository, AddUpdateRemoveCRUD) {
    const string testFile = "test_plants.json";
    deleteTestFiles(testFile); // Delete the file if it exists
    ofstream touch(testFile, ios::out);
    touch << "{\"plants\":[]}";
    touch.close();
    {
        // Create a new JSONPlantRepository
        JSONPlantRepository repo(testFile);
        repo.addPlant(Plant("Aloe", "Succulent", 5, 15.5));
        repo.addPlant(Plant("Rose", "Flower", 10, 8.9));

        // Check if the plants were added correctly
        auto all = repo.getAllPlants();
        ASSERT_EQ(all.size(), 2);
        ASSERT_EQ(all[0].getName(), "Aloe");
        ASSERT_EQ(all[1].getName(), "Rose");

        // Check if the plants were updated correctly
        repo.updatePlant(Plant("Aloe", "Succulent", 7, 17.5));
        Plant aloe = repo.getPlantByName("Aloe");
        ASSERT_EQ(aloe.getQuantity(), 7);
        ASSERT_DOUBLE_EQ(aloe.getPrice(), 17.5);

        // Check if the plants were removed correctly
        repo.removePlant("Rose");
        ASSERT_EQ(repo.getAllPlants().size(), 1);

        EXPECT_THROW(repo.addPlant(Plant("Aloe", "Succulent", 1, 13)), PlantRepository::DuplicatePlantException);
        EXPECT_THROW(repo.removePlant("NoSuchPlant"), PlantRepository::PlantNotFoundException);
    }
    deleteTestFiles(testFile);
}

TEST(PlantControllerTest, AddUpdateRemoveUndoRedo) {
    const string testFile = "test_plants.csv";
    deleteTestFiles(testFile); // Delete the file if it exists

    ofstream out(testFile, ios::out);
    out << "Name,Species,Quantity,Price\n";
    out.close();

    {
        // Create a new CSVPlantRepository
        auto repo = make_unique<CSVPlantRepository>(testFile);
        PlantController controller(move(repo));

        controller.addPlant("Lily", "Flower", 8, 4.5);
        controller.addPlant("Bamboo", "Grass", 15, 20.0);

        ASSERT_EQ(controller.getAllPlants().size(), 2);

        // Checks if the plants were removed correctly
        controller.removePlant("Lily");
        ASSERT_EQ(controller.getAllPlants().size(), 1);

        // Undo the last operation
        controller.undo();
        ASSERT_EQ(controller.getAllPlants().size(), 2);

        // Redo the last operation
        controller.redo();
        ASSERT_EQ(controller.getAllPlants().size(), 1);

        // Checks if the plants were updated correctly
        controller.updatePlant("Bamboo", "Grass", 30, 18.5);
        Plant bamboo = controller.getPlantByName("Bamboo");
        ASSERT_EQ(bamboo.getQuantity(), 30);
        ASSERT_DOUBLE_EQ(bamboo.getPrice(), 18.5);

        // Undo the last operation
        controller.undo();
        bamboo = controller.getPlantByName("Bamboo");
        ASSERT_EQ(bamboo.getQuantity(), 15);
        ASSERT_DOUBLE_EQ(bamboo.getPrice(), 20.0);

        // Test undo / redo limits
        controller.undo(); // Redo remove
        controller.undo(); // Redo add "Bamboo"
        controller.undo(); // Redo add "Lily"

        EXPECT_THROW(controller.undo(), runtime_error); // Nothing to undo
        controller.redo();
        controller.redo();
        controller.redo();
        controller.redo();
        EXPECT_THROW(controller.redo(), runtime_error); // Nothing to redo
    }
    deleteTestFiles(testFile);
}

TEST(PlantControllerTest, FilteringAndSearch) {
    const string testFile = "test_plants.csv";
    deleteTestFiles(testFile); // Delete the file if it exists

    ofstream out(testFile, ios::out);
    out << "Name,Species,Quantity,Price\n";
    out.close();

    {
        // Create a new CSVPlantRepository
        auto repo = make_unique<CSVPlantRepository>(testFile);
        PlantController controller(move(repo));

        // Add some plants
        controller.addPlant("Lily", "Flower", 8, 4.5);
        controller.addPlant("Bamboo", "Grass", 15, 20.0);
        controller.addPlant("Rose", "Flower", 10, 8.9);
        controller.addPlant("Aloe", "Succulent", 5, 15.5);
        controller.addPlant("Snake Plant", "Succulent", 0, 10.0);

        // Search (by partial)
        auto search = controller.searchPlants("Aloe");
        ASSERT_EQ(search.size(), 1);
        ASSERT_EQ(search[0].getName(), "Aloe");

        // Filtering (In Stock)
        vector<shared_ptr<PlantFilter>> filters;
        filters.push_back(make_shared<StockAvailabilityPlantFilter>(true));
        auto inStock = controller.filterPlants(filters);
        ASSERT_EQ(inStock.size(), 4);
        vector<string> expectedNames{"Lily", "Bamboo", "Rose", "Aloe"};
        for (size_t i = 0; i < expectedNames.size(); ++i)
            ASSERT_EQ(inStock[i].getName(), expectedNames[i]);

        // Filtering (Species = Succulent)
        filters.clear();
        filters.push_back(make_shared<SpeciesPlantFilter>("Succulent"));
        auto succulents = controller.filterPlants(filters);
        ASSERT_EQ(succulents.size(), 2);
        ASSERT_EQ(succulents[0].getName(), "Aloe");
        ASSERT_EQ(succulents[1].getName(), "Snake Plant");

        // Filtering (In Stock AND Succulent)
        filters.clear();
        filters.push_back(make_shared<StockAvailabilityPlantFilter>(true));
        filters.push_back(make_shared<SpeciesPlantFilter>("Succulent"));
        auto inStockSucculents = controller.filterPlants(filters);
        ASSERT_EQ(inStockSucculents.size(), 1);
        ASSERT_EQ(inStockSucculents[0].getName(), "Aloe");

        // Filtering (In Stock OR Succulent)
        filters.clear();
        filters.push_back(make_shared<StockAvailabilityPlantFilter>(true));
        filters.push_back(make_shared<SpeciesPlantFilter>("Succulent"));
        auto inStockSucculentsOr = controller.filterPlants(filters, false);
        ASSERT_EQ(inStockSucculentsOr.size(), 5);
    }
    deleteTestFiles(testFile);
}

TEST(PlantControllerTest, Statistics) {
    const string testFile = "test_plants.csv";
    deleteTestFiles(testFile); // Delete the file if it exists

    ofstream out(testFile, ios::out);
    out << "Name,Species,Quantity,Price\n";
    out.close();

    {
        // Create a new CSVPlantRepository
        auto repo = make_unique<CSVPlantRepository>(testFile);
        PlantController controller(move(repo));

        // Add some plants
        controller.addPlant("Lily", "Flower", 8, 4.5);
        controller.addPlant("Bamboo", "Grass", 15, 20.0);
        controller.addPlant("Rose", "Flower", 10, 8.9);
        controller.addPlant("Aloe", "Succulent", 5, 15.5);
        controller.addPlant("Snake Plant", "Succulent", 0, 10.0);

        ASSERT_EQ(controller.getTotalUniquePlants(), 5);
        ASSERT_EQ(controller.getTotalQuantity(), 38);
        ASSERT_DOUBLE_EQ(controller.getTotalInventoryValue(), 8*4.5 + 15*20.0 + 10*8.9 + 5*15.5 + 0*10.0);
    }
    deleteTestFiles(testFile);
}
#pragma once
#include "../Model/plant.h"
#include "../Repository/plant_repository.h"

// Abstract base class for Command pattern
class Command {
public:
    virtual void execute() = 0; // Executes the command
    virtual void undo() = 0; // Undoes the command

    virtual ~Command() = default; // Destructor
};

// Command for adding a plant
class AddPlantCommand : public Command {
    PlantRepository* repository;
    Plant plant;
public:
    // Constructor
    AddPlantCommand(PlantRepository* repository, const Plant &plant)
        : repository(repository), plant(plant) {}

    // Executes the addition of the plant
    void execute() override { repository->addPlant(plant); }

    // Undoes the addition by removing the plant
    void undo() override { repository->removePlant(plant.getName()); }

    // Destructor
    ~AddPlantCommand() override = default;
};

// Command for removing a plant
class RemovePlantCommand : public Command {
    PlantRepository* repository;
    Plant plant;
public:
    // Constructor
    RemovePlantCommand(PlantRepository* repository, const Plant &plant)
        : repository(repository), plant(plant) {}

    // Executes the removal of the plant
    void execute() override { repository->removePlant(plant.getName()); }

    // Undoes the removal by re-adding the plant
    void undo() override { repository->addPlant(plant); }

    // Destructor
    ~RemovePlantCommand() override = default;
};

// Command for updating a plant
class UpdatePlantCommand : public Command {
    PlantRepository* repository;
    Plant oldPlant;
    Plant newPlant;
public:
    // Constructor
    UpdatePlantCommand(PlantRepository* repository, const Plant &oldPlant, const Plant &newPlant)
        : repository(repository), oldPlant(oldPlant), newPlant(newPlant) {}

    // Executes the updating (applies newPlant)
    void execute() override { repository->updatePlant(newPlant); }

    // Undoes the updating (restores oldPlant)
    void undo() override { repository->updatePlant(oldPlant); }

    // Destructor
    ~UpdatePlantCommand() override = default;
};
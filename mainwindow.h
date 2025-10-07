#pragma once

#include <QWidget>
#include <QMainWindow>
#include <memory>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "../Controller/plant_controller.h"

// The main GUI window for the Plant Inventory application
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr); // Constructor
    ~MainWindow(); // Destructor
    void setupRepoSelection(); // Sets up the repository selection screen (CSV/JSON)
    void startApp(); // Called after repo type is chosen to start the main application UI

private:
    QWidget *centralWidget; // Central widget holding all layouts and UI elements
    QTableWidget *tableWidget;  // Displays the plant list in a table
    QLineEdit *nameEdit, *speciesEdit, *quantityEdit, *priceEdit; // Text fields for plant name, species, quantity & price
    QLineEdit *searchEdit; // Text field for quick search

    QComboBox *filterCombo; // Dropdown for stock status filter
    QComboBox *speciesFilterCombo; // Dropdown for species filter
    QComboBox *repoTypeCombo;  // Dropdown to select repository type (CSV/JSON)

    QPushButton *addButton; // Add a new plant
    QPushButton *updateButton; // Update the selected plant
    QPushButton *removeButton; // Remove the selected plant
    QPushButton *undoButton; // Undo last operation
    QPushButton *redoButton; // Redo last undone operation
    QPushButton *filterButton;  // Apply filters
    QPushButton *clearFilterButton; // Clear all filters
    QPushButton *startButton; // Start application after repo selection

    QLabel *statusLabel, *statsLabel; // Label for general status messages & for displaying statistics

    bool appStarted = false;

    std::unique_ptr<PlantController> controller;

    void setupUI(); // Internal helper to set up the main UI widgets and layout
    void loadTable(const std::vector<Plant>& plants); // Populates the table with the provided list of plants
    void clearInputs(); // Clears all input fields
    void showError(const QString &msg); // Shows an error message dialog

private slots:
    void onAdd();
    void onUpdate();
    void onRemove();
    void onUndo();
    void onRedo();
    void onTableSelect();
    void onFilter();
    void onClearFilter();
    void updateSpeciesCombo();
    void updateStats();
};

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../Repository/csv_plant_repository.h"
#include "../Repository/json_plant_repository.h"

#include <QMessageBox>
#include <QString>
#include <QHeaderView>
#include <vector>
#include <memory>

using namespace std;

// Constructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setupRepoSelection();
}


// Destructor
MainWindow::~MainWindow() { delete centralWidget; }

// Sets up the first screen where the user chooses between CSV and JSON repository
void MainWindow::setupRepoSelection() {
    QWidget *selectionWidget = new QWidget(this);
    QVBoxLayout *selectLayout = new QVBoxLayout();

    QLabel *chooseLabel = new QLabel("Choose repository type");
    repoTypeCombo = new QComboBox();
    repoTypeCombo->setObjectName("repoTypeCombo");
    repoTypeCombo->addItems({"CSV", "JSON"});
    startButton = new QPushButton("Start");
    startButton->setObjectName("startButton");

    selectLayout->addWidget(chooseLabel);
    selectLayout->addWidget(repoTypeCombo);
    selectLayout->addWidget(startButton);

    selectionWidget->setLayout(selectLayout);
    setCentralWidget(selectionWidget);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::startApp);
}

// Initializes the controller based on user repo selection and shows main UI
void MainWindow::startApp() {
    QString repoType = repoTypeCombo->currentText();
    if (repoType == "CSV") {
        controller = make_unique<PlantController>(make_unique<CSVPlantRepository>("plants.csv"));
    } else {
        controller = make_unique<PlantController>(make_unique<JSONPlantRepository>("plants.json"));
    }
    appStarted = true;
    setupUI();
    loadTable(controller->getAllPlants());
}

// Sets up the main application interface
void MainWindow::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout();

    // Table for plant data
    tableWidget = new QTableWidget(0, 4, this);
    tableWidget->setObjectName("tableWidget");
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Name" << "Species" << "Quantity" << "Price");
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(tableWidget, &QTableWidget::cellClicked, this, &MainWindow::onTableSelect);
    mainLayout->addWidget(tableWidget);

    // Input form (name, species, quantity, price)
    QHBoxLayout *formLayout = new QHBoxLayout();
    nameEdit = new QLineEdit();
    nameEdit->setObjectName("nameEdit");
    formLayout->addWidget(new QLabel("Name:"));
    formLayout->addWidget(nameEdit);
    speciesEdit = new QLineEdit();
    speciesEdit->setObjectName("speciesEdit");
    formLayout->addWidget(new QLabel("Species:"));
    formLayout->addWidget(speciesEdit);
    quantityEdit = new QLineEdit();
    quantityEdit->setObjectName("quantityEdit");
    formLayout->addWidget(new QLabel("Qty:"));
    formLayout->addWidget(quantityEdit);
    priceEdit = new QLineEdit();
    priceEdit->setObjectName("priceEdit");
    formLayout->addWidget(new QLabel("Price:"));
    formLayout->addWidget(priceEdit);
    mainLayout->addLayout(formLayout);

    // CRUD + Undo/Redo buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    addButton = new QPushButton("Add");
    addButton->setObjectName("addButton");
    buttonLayout->addWidget(addButton);
    updateButton = new QPushButton("Update");
    updateButton->setObjectName("updateButton");
    buttonLayout->addWidget(updateButton);
    removeButton = new QPushButton("Remove");
    removeButton->setObjectName("removeButton");
    buttonLayout->addWidget(removeButton);
    undoButton = new QPushButton("Undo");
    undoButton->setObjectName("undoButton");
    buttonLayout->addWidget(undoButton);
    redoButton = new QPushButton("Redo");
    redoButton->setObjectName("redoButton");
    buttonLayout->addWidget(redoButton);
    mainLayout->addLayout(buttonLayout);

    // Filtering area
    QHBoxLayout *filterLayout = new QHBoxLayout();
    filterCombo = new QComboBox();
    filterCombo->setObjectName("filterCombo");
    filterCombo->addItems({"All", "In Stock", "Out of Stock"});
    filterLayout->addWidget(new QLabel("Stock:"));
    filterLayout->addWidget(filterCombo);
    speciesFilterCombo = new QComboBox();
    speciesFilterCombo->setObjectName("speciesFilterCombo");
    speciesFilterCombo->addItem("All");

    // Populate with unique species from current data
    for (const auto& p : controller->getAllPlants())
        if (speciesFilterCombo->findText(QString::fromStdString(p.getSpecies())) == -1)
            speciesFilterCombo->addItem(QString::fromStdString(p.getSpecies()));
    filterLayout->addWidget(new QLabel("Species:"));
    filterLayout->addWidget(speciesFilterCombo);

    filterButton = new QPushButton("Filter");
    filterButton->setObjectName("filterButton");
    filterLayout->addWidget(filterButton);
    clearFilterButton = new QPushButton("Clear");
    clearFilterButton->setObjectName("clearFilterButton");
    filterLayout->addWidget(clearFilterButton);
    mainLayout->addLayout(filterLayout);

    // Quick search area
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchEdit = new QLineEdit();
    searchEdit->setObjectName("searchEdit");
    searchLayout->addWidget(new QLabel("Search:"));
    searchLayout->addWidget(searchEdit);
    mainLayout->addLayout(searchLayout);

    // Status message label
    statusLabel = new QLabel();
    statusLabel->setObjectName("statusLabel");
    mainLayout->addWidget(statusLabel);

    // Statistics label: shows total unique plants, total quantity, and total value
    statsLabel = new QLabel();
    statsLabel->setObjectName("statsLabel");
    mainLayout->addWidget(statsLabel);
    updateStats();

    // Style and spacing for better UI look
    mainLayout->setContentsMargins(24, 16, 24, 16);
    mainLayout->setSpacing(12);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setShowGrid(false);
    tableWidget->setStyleSheet("QTableWidget { border-radius: 10px; }");
    statusLabel->setStyleSheet("color: #227a45; font-size: 16px; font-weight: bold; background: #e3fbe0; border-radius: 8px; padding: 4px 12px;");
    statusLabel->setText("🌱 Welcome to Plant Shop Inventory!");
    this->setWindowTitle("Plant Shop Inventory 🌿");

    // Connect all button signals to their slots
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAdd);
    connect(updateButton, &QPushButton::clicked, this, &MainWindow::onUpdate);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::onRemove);
    connect(undoButton, &QPushButton::clicked, this, &MainWindow::onUndo);
    connect(redoButton, &QPushButton::clicked, this, &MainWindow::onRedo);
    connect(filterButton, &QPushButton::clicked, this, &MainWindow::onFilter);
    connect(clearFilterButton, &QPushButton::clicked, this, &MainWindow::onClearFilter);
    connect(searchEdit, &QLineEdit::textChanged, [this](const QString &text) {
        loadTable(controller->searchPlants(text.toStdString()));
    });

    centralWidget->setLayout(mainLayout);
}


// Populates the table with plant data
void MainWindow::loadTable(const vector<Plant> &plants) {
    tableWidget->setRowCount(0);
    for (const auto &p : plants) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(p.getName())));
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(p.getSpecies())));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(p.getQuantity())));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(p.getPrice(), 'f', 2)));
    }
}

// Clears all input fields in the form
void MainWindow::clearInputs() {
    nameEdit->clear();
    speciesEdit->clear();
    quantityEdit->clear();
    priceEdit->clear();
}

// Displays an error dialog with the given message
void MainWindow::showError(const QString &msg) {
    QMessageBox::warning(this, "Error", msg);
}

// Slot for Add button
void MainWindow::onAdd() {
    try {
        controller->addPlant(
            nameEdit->text().toStdString(),
            speciesEdit->text().toStdString(),
            quantityEdit->text().toInt(),
            priceEdit->text().toDouble());
        loadTable(controller->getAllPlants());
        clearInputs();
        updateSpeciesCombo();
        updateStats();
    } catch (const exception &e) { showError(e.what()); }
}

// Slot for Update button
void MainWindow::onUpdate() {
    try {
        controller->updatePlant(
            nameEdit->text().toStdString(),
            speciesEdit->text().toStdString(),
            quantityEdit->text().toInt(),
            priceEdit->text().toDouble());
        loadTable(controller->getAllPlants());
        clearInputs();
        updateSpeciesCombo();
        updateStats();
    } catch (const exception &e) { showError(e.what()); }
}

// Slot for Remove button
void MainWindow::onRemove() {
    try {
        controller->removePlant(nameEdit->text().toStdString());
        loadTable(controller->getAllPlants());
        clearInputs();
        updateSpeciesCombo();
        updateStats();
    } catch (const exception &e) { showError(e.what()); }
}

// Slot for Undo button
void MainWindow::onUndo() {
    try {
        controller->undo();
        loadTable(controller->getAllPlants());
    } catch (const exception &e) { showError(e.what()); }
}

// Slot for Redo button
void MainWindow::onRedo() {
    try {
        controller->redo();
        loadTable(controller->getAllPlants());
    } catch (const exception &e) { showError(e.what()); }
}

// Slot for table row selection
void MainWindow::onTableSelect() {
    int row = tableWidget->currentRow();
    if (row < 0) return;
    nameEdit->setText(tableWidget->item(row, 0)->text());
    speciesEdit->setText(tableWidget->item(row, 1)->text());
    quantityEdit->setText(tableWidget->item(row, 2)->text());
    priceEdit->setText(tableWidget->item(row, 3)->text());
}

// Slot for Filter button
void MainWindow::onFilter() {
    vector<shared_ptr<PlantFilter>> filters;
    // Stock filter
    if (filterCombo->currentText() == "In Stock")
        filters.push_back(make_shared<StockAvailabilityPlantFilter>(true));
    else if (filterCombo->currentText() == "Out of Stock")
        filters.push_back(make_shared<StockAvailabilityPlantFilter>(false));
    // Species filter
    if (speciesFilterCombo->currentText() != "All")
        filters.push_back(make_shared<SpeciesPlantFilter>(speciesFilterCombo->currentText().toStdString()));
    loadTable(controller->filterPlants(filters, true));
}

// Slot for Clear Filter button
void MainWindow::onClearFilter() {
    filterCombo->setCurrentIndex(0);
    speciesFilterCombo->setCurrentIndex(0);
    loadTable(controller->getAllPlants());
}

// Updates the species combo box with all unique species from the repository
void MainWindow::updateSpeciesCombo() {
    // Save current selection
    QString current = speciesFilterCombo->currentText();
    speciesFilterCombo->blockSignals(true);
    speciesFilterCombo->clear();
    speciesFilterCombo->addItem("All");

    set<QString> uniqueSpecies;
    for (const auto &p : controller->getAllPlants())
        uniqueSpecies.insert(QString::fromStdString(p.getSpecies()));
    for (const auto &s : uniqueSpecies)
        speciesFilterCombo->addItem(s);

    // Restore selection if possible
    int idx = speciesFilterCombo->findText(current);
    if (idx >= 0)
        speciesFilterCombo->setCurrentIndex(idx);

    speciesFilterCombo->blockSignals(false);
}

// Updates the statistics label with the current inventory summary
void MainWindow::updateStats() {
    int totalPlants = controller->getTotalUniquePlants();
    int totalQty = controller->getTotalQuantity();
    double totalVal = controller->getTotalInventoryValue();
    statsLabel->setText(
    QString("Unique plants: %1 | Total quantity: %2 | Total value: %3 RON")
        .arg(totalPlants).arg(totalQty).arg(totalVal, 0, 'f', 2));
}

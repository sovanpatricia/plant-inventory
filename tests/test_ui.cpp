#include <QtTest/QtTest>
#include "../UI/mainwindow.h"
#include "../Repository/csv_plant_repository.h"
#include "../Controller/plant_controller.h"
#include <utility>
#include <memory>
#include <fstream>

class MainWindowTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void testAddPlant();
    void testUpdatePlant();
    void testRemovePlant();
    void testUndoRedo();
    void testValidationErrors();
    void testFiltering();
    void testSearch();
    void testStatsLabel();
    void resetApp();
private:
    MainWindow* window = nullptr;
};

void MainWindowTest::initTestCase() {
    ofstream("plants.csv").close();
    window = new MainWindow();
    window->show();

    // Simulate repository selection: CSV
    auto repoType = window->findChild<QComboBox*>("repoTypeCombo");
    auto startButton = window->findChild<QPushButton*>("startButton");
    QVERIFY(repoType && startButton);

    repoType->setCurrentText("CSV");
    QTest::mouseClick(startButton, Qt::LeftButton);
    QTest::qWait(100);
    QApplication::processEvents();

    auto nameEdit = window->findChild<QLineEdit*>("nameEdit");
    QVERIFY(nameEdit);
}

void MainWindowTest::cleanupTestCase() {
    if (window) {
        window->close();
        delete window;
        window = nullptr;
    }
}

void MainWindowTest::testAddPlant() {
    auto nameEdit = window->findChild<QLineEdit*>("nameEdit");
    auto speciesEdit = window->findChild<QLineEdit*>("speciesEdit");
    auto quantityEdit = window->findChild<QLineEdit*>("quantityEdit");
    auto priceEdit = window->findChild<QLineEdit*>("priceEdit");
    auto addButton = window->findChild<QPushButton*>("addButton");
    auto table = window->findChild<QTableWidget*>("tableWidget");

    QVERIFY(nameEdit && speciesEdit && quantityEdit && priceEdit && addButton && table);

    nameEdit->setText("TestPlant");
    speciesEdit->setText("TestSpecies");
    quantityEdit->setText("10");
    priceEdit->setText("10.00");
    QTest::mouseClick(addButton, Qt::LeftButton);

    // Confirm in table
    QCOMPARE(table->rowCount(), 1);
    QCOMPARE(table->item(0, 0)->text(), "TestPlant");
    QCOMPARE(table->item(0, 1)->text(), "TestSpecies");
    QCOMPARE(table->item(0, 2)->text(), "10");
    QCOMPARE(table->item(0, 3)->text(), "10.00");
}

void MainWindowTest::testUpdatePlant() {
    auto table = window->findChild<QTableWidget*>("tableWidget");
    auto speciesEdit = window->findChild<QLineEdit*>("speciesEdit");
    auto updateButton = window->findChild<QPushButton*>("updateButton");

    QVERIFY(table && speciesEdit && updateButton);

    QTest::mouseClick(table->viewport(), Qt::LeftButton, Qt::NoModifier, table->visualItemRect(table->item(0, 0)).center());
    speciesEdit->setText("UpdatedSpecies");
    QTest::mouseClick(updateButton, Qt::LeftButton);

    QCOMPARE(table->item(0, 1)->text(), "UpdatedSpecies");
}

void MainWindowTest::testRemovePlant() {
    auto table = window->findChild<QTableWidget*>("tableWidget");
    auto removeButton = window->findChild<QPushButton*>("removeButton");

    QVERIFY(table && removeButton);

    QTest::mouseClick(
        table->viewport(),
        Qt::LeftButton,
        Qt::NoModifier,
        table->visualItemRect(table->item(0, 0)).center()
    );
    QTest::mouseClick(removeButton, Qt::LeftButton);

    QCOMPARE(table->rowCount(), 0);
}

void MainWindowTest::testUndoRedo() {
    resetApp();

    auto undoButton = window->findChild<QPushButton*>("undoButton");
    auto redoButton = window->findChild<QPushButton*>("redoButton");
    auto table = window->findChild<QTableWidget*>("tableWidget");
    auto addButton = window->findChild<QPushButton*>("addButton");
    auto nameEdit = window->findChild<QLineEdit*>("nameEdit");
    auto speciesEdit = window->findChild<QLineEdit*>("speciesEdit");
    auto quantityEdit = window->findChild<QLineEdit*>("quantityEdit");
    auto priceEdit = window->findChild<QLineEdit*>("priceEdit");

    nameEdit->setText("UndoPlant");
    speciesEdit->setText("TypeA");
    quantityEdit->setText("3");
    priceEdit->setText("12.5");
    QTest::mouseClick(addButton, Qt::LeftButton);

    QCOMPARE(table->rowCount(), 1);
    QTest::mouseClick(undoButton, Qt::LeftButton);
    QCOMPARE(table->rowCount(), 0);
    QTest::mouseClick(redoButton, Qt::LeftButton);
    QCOMPARE(table->rowCount(), 1);
}

void MainWindowTest::testValidationErrors() {
    auto addButton = window->findChild<QPushButton*>("addButton");
    auto nameEdit = window->findChild<QLineEdit*>("nameEdit");
    auto quantityEdit = window->findChild<QLineEdit*>("quantityEdit");
    auto priceEdit = window->findChild<QLineEdit*>("priceEdit");

    // Invalid quantity
    nameEdit->setText("InvalidPlant");
    quantityEdit->setText("-7");
    priceEdit->setText("10.00");
    QSignalSpy spy(QApplication::instance(), &QApplication::aboutToQuit);
    QTest::mouseClick(addButton, Qt::LeftButton);

    // Invalid price
    quantityEdit->setText("5");
    priceEdit->setText("-10");
    QTest::mouseClick(addButton, Qt::LeftButton);
}

void MainWindowTest::testFiltering() {
    resetApp();
    auto addButton = window->findChild<QPushButton*>("addButton");
    auto nameEdit = window->findChild<QLineEdit*>("nameEdit");
    auto speciesEdit = window->findChild<QLineEdit*>("speciesEdit");
    auto quantityEdit = window->findChild<QLineEdit*>("quantityEdit");
    auto priceEdit = window->findChild<QLineEdit*>("priceEdit");
    auto filterCombo = window->findChild<QComboBox*>("filterCombo");
    auto filterButton = window->findChild<QPushButton*>("filterButton");
    auto table = window->findChild<QTableWidget*>("tableWidget");

    // Add in-stock and out-of-stock plants
    nameEdit->setText("Stocky");
    speciesEdit->setText("Herb");
    quantityEdit->setText("0");
    priceEdit->setText("2.5");
    QTest::mouseClick(addButton, Qt::LeftButton);

    nameEdit->setText("InStocky");
    speciesEdit->setText("Herb");
    quantityEdit->setText("5");
    priceEdit->setText("2.5");
    QTest::mouseClick(addButton, Qt::LeftButton);

    filterCombo->setCurrentText("In Stock");
    QTest::mouseClick(filterButton, Qt::LeftButton);
    QCOMPARE(table->rowCount(), 1);
    QCOMPARE(table->item(0, 0)->text(), "InStocky");
    filterCombo->setCurrentText("Out of Stock");
    QTest::mouseClick(filterButton, Qt::LeftButton);
    QCOMPARE(table->rowCount(), 1);
    QCOMPARE(table->item(0, 0)->text(), "Stocky");
}

void MainWindowTest::testSearch() {
    auto searchEdit = window->findChild<QLineEdit*>("searchEdit");
    auto table = window->findChild<QTableWidget*>("tableWidget");

    QVERIFY(searchEdit && table);

    searchEdit->setText("InStocky");
    QCOMPARE(table->rowCount(), 1);
    QCOMPARE(table->item(0, 0)->text(), "InStocky");

    searchEdit->setText("NO_MATCH");
    QCOMPARE(table->rowCount(), 0);
}

void MainWindowTest::testStatsLabel() {
    auto statsLabel = window->findChild<QLabel*>("statsLabel");
    QVERIFY(statsLabel);
    QVERIFY(statsLabel->text().contains("Unique plants:"));
}

void MainWindowTest::resetApp() {
    if (window) {
        window->close();
        delete window;
        window = nullptr;
    }
    std::ofstream("plants.csv").close();
    window = new MainWindow();
    window->show();

    auto repoType = window->findChild<QComboBox*>("repoTypeCombo");
    auto startButton = window->findChild<QPushButton*>("startButton");
    QTest::qWait(10);
    repoType->setCurrentText("CSV");
    QTest::mouseClick(startButton, Qt::LeftButton);
    QTest::qWait(10);
    QApplication::processEvents();
}

QTEST_MAIN(MainWindowTest)
#include "test_ui.moc"

/*------------------------------------------------------------------------------------------------------
 * Matrix Data Editor (MaDE)
 *
 * Copyright (c) 2012-2013, Alan R.T. Spencer
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License along with this program. If not,
 * see http://www.gnu.org/licenses/.
 *-----------------------------------------------------------------------------------------------------*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainwindow = this;

    //setDockOptions(QMainWindow::VerticalTabs);
    tabifyDockWidget(ui->infoDockWidget, ui->taxaListDockWidget);
    tabifyDockWidget(ui->taxaListDockWidget, ui->characterListDockWidget);
    ui->infoDockWidget->raise();

    // Start Log
    logAppend("MaDE","Started");

    // Get settings from .ini file
    settings = new Settings();
    settings->mw = this;
    settings->initialize();

    enabledColor.setRgba(settings->getSetting("enabledColor").toUInt());
    disabledColor.setRgba(settings->getSetting("disabledColor").toUInt());

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

    // MainWindow Menu and Docks Setup
    initializeMainMenu();
    initializeInformationDock();
    initializeTaxaDock();
    initializeCharacterDock();
    initializeDataDock();

    // Contect Matrix Mdi Child to MainWindow to catch change in focus
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMainWindow()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*------------------------------------------------------------------------------------/
 * Log
 *-----------------------------------------------------------------------------------*/

void MainWindow::logAppend(const QString &strTitle, const QString &strMessage)
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString();
    ui->logTextBrowser->append(tr("<div><span>%1</span> - <span><b>%2:</b> <i>%3</i></span></div>")
                               .arg(dateTimeString)
                               .arg(strTitle)
                               .arg(strMessage));
}

/*------------------------------------------------------------------------------------/
 * MDI Functions
 *-----------------------------------------------------------------------------------*/

//---- Create a new Matrix Mdi Child
Matrix *MainWindow::createMatrix()
{
    Matrix *child = new Matrix;
    child->mw = mainwindow;
    child->settings = settings;
    ui->mdiArea->addSubWindow(child);
    logAppend("Matrix (Mdi Child)","initialized.");
    return child;
}

QMdiSubWindow *MainWindow::findMatrix(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, ui->mdiArea->subWindowList()) {
        Matrix *matrix = qobject_cast<Matrix *>(window->widget());
        if (matrix->returnCurrentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

//--- Get Active Matrix Mdi Child
Matrix *MainWindow::getActiveMatrix()
{
     if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<Matrix *>(activeSubWindow->widget());
     return 0;
}

//---- Set Active Matrix Mdi Child
void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

//---- Update Dock on change in Matrix Mdi Child focus
void MainWindow::updateMainWindow()
{
    activeMatrix = getActiveMatrix();
    //---- Check to see if there is an active Mdi child
    bool hasMatrix = (activeMatrix != 0);
    if (hasMatrix) {
        logAppend("Main Window","an active Matrix (Mdi Child) has been found...");        
        updateWindowMenu();
        ui->menuWindows->setEnabled(true);
        ui->menuData->setEnabled(true);        
        updateInformationDock();
        updateTaxaDock();
        updateCharacterDock();
        updateDataDock();
        ui->addEditTaxonToolButton->setEnabled(true);
        ui->addEditCharacterToolButton->setEnabled(true);
    } else {
        //---- No active Mdi Child. Reset docks to default.
        logAppend("Main Window","an active Matrix (Mdi Child) has NOT been found...");
        updateWindowMenu();
        ui->menuWindows->setEnabled(false);
        ui->menuData->setEnabled(false);
        initializeInformationDock();
        initializeTaxaDock();
        initializeCharacterDock();
        initializeDataDock();
        ui->addEditTaxonToolButton->setEnabled(false);
        ui->addEditCharacterToolButton->setEnabled(false);
    }
}

/*------------------------------------------------------------------------------------/
 * Main Menu
 *-----------------------------------------------------------------------------------*/

// MainWindow Main Menu Setup and Actions
void MainWindow::initializeMainMenu()
{
    actionTile = new QAction(tr("&Tile"), this);
    actionTile->setStatusTip(tr("Tile the windows"));
    actionCascade = new QAction(tr("&Cascade"), this);
    actionCascade->setStatusTip(tr("Cascade the windows"));
    actionSeparator = new QAction(this);
    actionSeparator->setSeparator(true);

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    connect(ui->actionImportNEXUS, SIGNAL(triggered()), this, SLOT(importNexus()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(settingsDialogOpen()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionTile, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));
    connect(actionCascade, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));
    connect(ui->actionMatrixSettings, SIGNAL(triggered()), this, SLOT(matrixSettingsDialogOpen()));
    connect(ui->actionAddEditTaxa, SIGNAL(triggered()), this, SLOT(matrixTaxaDialogOpen()));
    connect(ui->actionAddEditCharacters, SIGNAL(triggered()), this, SLOT(matrixCharactersDialogOpen()));
    connect(ui->editMatrixSettingsToolButton, SIGNAL(clicked()), this, SLOT(matrixSettingsDialogOpen()));
}

//---- Update "Windows" Main Menu on open
void MainWindow::updateWindowMenu()
{
     ui->menuWindows->clear();
     ui->menuWindows->addAction(actionTile);
     ui->menuWindows->addAction(actionCascade);
     ui->menuWindows->addAction(actionSeparator);

     QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
     actionSeparator->setVisible(!windows.isEmpty());

     for (int i = 0; i < windows.size(); ++i) {
         Matrix *child = qobject_cast<Matrix *>(windows.at(i)->widget());

         QString text;
         if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                                .arg(child->userFriendlyCurrentFile());
         } else {
            text = tr("%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
         }
         QAction *action  = ui->menuWindows->addAction(text);
         action->setCheckable(true);
         action->setChecked(child == getActiveMatrix());
         connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
         windowMapper->setMapping(action, windows.at(i));
     }
}

/*------------------------------------------------------------------------------------/
 * Information Dock
 *-----------------------------------------------------------------------------------*/

void MainWindow::initializeInformationDock()
{
    ui->filenameText->setText("Undefined");
    ui->matrixNameText->setText("Undefined");
    ui->matrixTypeText->setText("Undefined");
    ui->matrixDescriptionTextBrowser->clear();
    ui->charactersNumberText->setText("Undefined");
    ui->gapCharacterText->setText("Undefined");
    ui->taxaNumberText->setText("Undefined");
    ui->unknownCharacterText->setText("Undefined");

    ui->editMatrixSettingsToolButton->setEnabled(false);
}

void MainWindow::updateInformationDock()
{
    int taxaNumber = activeMatrix->taxaCount();
    int characterNumber = activeMatrix->charactersCount();

    ui->filenameText->setText(activeMatrix->userFriendlyCurrentFile());
    ui->matrixNameText->setText(activeMatrix->getMatrixName());
    ui->matrixTypeText->setText(activeMatrix->getMatrixTypeName());
    ui->matrixDescriptionTextBrowser->setHtml(activeMatrix->getMatrixDescription());
    ui->charactersNumberText->setText(QString("%1").arg(characterNumber));
    ui->gapCharacterText->setText(QString("%1").arg(activeMatrix->getGapCharacter()));
    ui->taxaNumberText->setText(QString("%1").arg(taxaNumber));
    ui->unknownCharacterText->setText(activeMatrix->getMissingCharacter());

    ui->editMatrixSettingsToolButton->setEnabled(true);
}

/*------------------------------------------------------------------------------------/
 * Taxa List Dock
 *-----------------------------------------------------------------------------------*/

void MainWindow::initializeTaxaDock()
{
    //---- Set up default table text when there is no data
    ui->taxaTableWidget->setRowCount(1);
    ui->taxaTableWidget->setColumnCount(1);
    ui->taxaTableWidget->horizontalHeader()->hide();
    ui->taxaTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QTableWidgetItem *newItem = new QTableWidgetItem(tr("[You need to create/open a Matrix]"));
    newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    newItem->setForeground(Qt::gray);
    ui->taxaTableWidget->setItem(0, 0, newItem);

    connect(ui->addEditTaxonToolButton, SIGNAL(clicked()), this, SLOT(matrixTaxaDialogOpen()));
}

void MainWindow::updateTaxaDock()
{
    int taxaNumber = activeMatrix->taxaCount();

    ui->taxaTableWidget->setRowCount(1);
    ui->taxaTableWidget->setColumnCount(1);
    ui->taxaTableWidget->horizontalHeader()->hide();
    ui->taxaTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Reload Taxa List
    if (taxaNumber == 0) {
        //---- Set up default text when there is no data
        QTableWidgetItem *newItem = new QTableWidgetItem(tr("[You need to add one or more Taxon]"));
        newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        newItem->setForeground(Qt::gray);
        ui->taxaTableWidget->setItem(0, 0, newItem);
    } else {
        //---- Update the table view
        for(int i = 0; i < taxaNumber; i++)
        {
            QString taxonName = activeMatrix->taxonList[i].getName();
            int isEnabled = activeMatrix->taxonList[i].getIsEnabled();
            //---- Add Row
            if(i != 0) {
                ui->taxaTableWidget->insertRow(i);
            }
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(taxonName));
            newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            ui->taxaTableWidget->setItem(i, 0, newItem);

            updateTaxaDockTableColor(i, isEnabled);

            newItem = new QTableWidgetItem(tr("T%1").arg(i+1));
            newItem->setFlags(Qt::ItemIsEnabled);
            ui->taxaTableWidget->setVerticalHeaderItem(i,newItem);
        }
        taxonListSelect(0);
    }
}

void MainWindow::taxonListSelect(int row)
{
    ui->taxaTableWidget->selectionModel()->clear();
    QModelIndex index = ui->taxaTableWidget->model()->index(row,0);
    ui->taxaTableWidget->selectionModel()->select(index, QItemSelectionModel::Select);
}

void MainWindow::updateTaxaDockTableText(int row, QString text)
{
    ui->taxaTableWidget->item(row, 0)->setText(text);
}

void MainWindow::updateTaxaDockTableColor(int row, bool isEnabled)
{
    if (isEnabled){
        ui->taxaTableWidget->item(row,0)->setForeground(QBrush(enabledColor));
    } else{
        ui->taxaTableWidget->item(row,0)->setForeground(QBrush(disabledColor));
    }
}


void MainWindow::moveTaxaDockTableRow(int row, bool up)
{
    int sourceRow = row;
    int destRow = (up ? sourceRow-1 : sourceRow+1);

    // Take whole rows
    QList<QTableWidgetItem*> sourceItems = getTaxaDockTableRow(sourceRow);
    QList<QTableWidgetItem*> destItems = getTaxaDockTableRow(destRow);

    // Set back in reverse order
    setTaxaDockTableRow(sourceRow, destItems);
    setTaxaDockTableRow(destRow, sourceItems);
}

// Takes and returns the whole row
QList<QTableWidgetItem*> MainWindow::getTaxaDockTableRow(int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int column = 0; column < ui->taxaTableWidget->columnCount(); ++column)
    {
        rowItems << ui->taxaTableWidget->takeItem(row, column);
    }
    return rowItems;
}

// Sets the whole row
void MainWindow::setTaxaDockTableRow(int row, const QList<QTableWidgetItem*>& rowItems)
{
    for (int column = 0; column < ui->taxaTableWidget->columnCount(); ++column)
    {
        ui->taxaTableWidget->setItem(row, column, rowItems.at(column));
    }
}

/*------------------------------------------------------------------------------------/
 * Character List Dock
 *-----------------------------------------------------------------------------------*/

void MainWindow::initializeCharacterDock()
{
    ui->charactersTableWidget->setRowCount(1);
    ui->charactersTableWidget->setColumnCount(1);
    ui->charactersTableWidget->horizontalHeader()->hide();
    ui->charactersTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QTableWidgetItem *newItem = new QTableWidgetItem(tr("[You need to create/open a Matrix]"));
    newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    newItem->setForeground(Qt::gray);
    ui->charactersTableWidget->setItem(0, 0, newItem);

    connect(ui->addEditCharacterToolButton, SIGNAL(clicked()), this, SLOT(matrixCharactersDialogOpen()));
}

void MainWindow::updateCharacterDock()
{
    int characterNumber = activeMatrix->charactersCount();

    ui->charactersTableWidget->setRowCount(1);
    ui->charactersTableWidget->setColumnCount(1);
    ui->charactersTableWidget->horizontalHeader()->hide();
    ui->charactersTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Reload Character List
    if (characterNumber == 0) {
        //---- Set up default text when there is no data
        QTableWidgetItem *newItem = new QTableWidgetItem(tr("[You need to add one or more Character]"));
        newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        newItem->setForeground(Qt::gray);
        ui->charactersTableWidget->setItem(0, 0, newItem);
    } else {
        //---- Update the table view
        for(int i = 0; i < characterNumber; i++)
        {
            QString characterName = activeMatrix->characterList[i].getName();
            int isEnabled = activeMatrix->characterList[i].getIsEnabled();

            if(i != 0) {
                ui->charactersTableWidget->insertRow(i);
            }

            QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(characterName));
            newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            ui->charactersTableWidget->setItem(i, 0, newItem);

            updateCharacterDockTableColor(i, isEnabled);

            newItem = new QTableWidgetItem(tr("C%1").arg(i+1));
            newItem->setFlags(Qt::ItemIsEnabled);
            ui->charactersTableWidget->setVerticalHeaderItem(i,newItem);
        }
    }
}

void MainWindow::characterListSelect(int row)
{
    ui->charactersTableWidget->selectionModel()->clear();
    QModelIndex index = ui->charactersTableWidget->model()->index(row,0);
    ui->charactersTableWidget->selectionModel()->select(index, QItemSelectionModel::Select);
}

void MainWindow::updateCharacterDockTableText(int row, QString text)
{
    ui->charactersTableWidget->item(row, 0)->setText(text);
}

void MainWindow::updateCharacterDockTableColor(int row, bool isEnabled)
{
    if (isEnabled){
        ui->charactersTableWidget->item(row,0)->setForeground(QBrush(enabledColor));
    } else{
        ui->charactersTableWidget->item(row,0)->setForeground(QBrush(disabledColor));
    }
}

void MainWindow::moveCharacterDockTableRow(int row, bool up)
{
    int sourceRow = row;
    int destRow = (up ? sourceRow-1 : sourceRow+1);

    // Take whole rows
    QList<QTableWidgetItem*> sourceItems = getCharacterDockTableRow(sourceRow);
    QList<QTableWidgetItem*> destItems = getCharacterDockTableRow(destRow);

    // Set back in reverse order
    setCharacterDockTableRow(sourceRow, destItems);
    setCharacterDockTableRow(destRow, sourceItems);
}

// Takes and returns the whole row
QList<QTableWidgetItem*> MainWindow::getCharacterDockTableRow(int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int column = 0; column < ui->charactersTableWidget->columnCount(); ++column)
    {
        rowItems << ui->charactersTableWidget->takeItem(row, column);
    }
    return rowItems;
}

// Sets the whole row
void MainWindow::setCharacterDockTableRow(int row, const QList<QTableWidgetItem*>& rowItems)
{
    for (int column = 0; column < ui->charactersTableWidget->columnCount(); ++column)
    {
        ui->charactersTableWidget->setItem(row, column, rowItems.at(column));
    }
}

/*------------------------------------------------------------------------------------/
 * Data Dock
 *-----------------------------------------------------------------------------------*/

void MainWindow::initializeDataDock()
{
    ui->dataTaxonText->setText("No Data Selected");
    ui->dataCharacterText->setText("No Data Selected");

    // Default States Table
    ui->dataStatesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->dataStatesTableWidget->setRowCount(1);
    ui->dataStatesTableWidget->setColumnCount(1);
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("[You need to select a data cell]"));
    newItem->setFlags(Qt::ItemIsEnabled);
    newItem->setForeground(Qt::gray);
    ui->dataStatesTableWidget->setItem(0,0,newItem);
}

void MainWindow::updateDataDock()
{    
    int row = activeMatrix->currentSelectedCell->first;
    int column = activeMatrix->currentSelectedCell->second;
    int taxonID = activeMatrix->taxonList[row].getID();
    int characterID = activeMatrix->characterList[column].getID();

    //qDebug() << "Selected Row = " << row << " Column = " << column << " | Selected TaxonID = " << taxonID << " CharacterID = " << characterID;

    ui->dataTaxonText->setText(QString("T%1 - %2").arg(row+1).arg(activeMatrix->taxonList[row].getName()));
    ui->dataCharacterText->setText(QString("C%1 - %2").arg(column+1).arg(activeMatrix->characterList[column].getName()));

    // Update States Table
    QString symbol;

    QFont selectedFont;
    selectedFont.setBold(true);

    int i = 0;
    int statesNumber = activeMatrix->characterList[column].countStates();
    ui->dataStatesTableWidget->setRowCount(statesNumber+2);

    if (statesNumber != 0) {
        for (int n = 0; n < statesNumber; n++)
        {
            symbol = activeMatrix->characterList[column].getState(n).getSymbol();
            if (activeMatrix->isSymbolSelected(symbol,taxonID,characterID)) {
                QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(symbol));
                newItem->setFont(selectedFont);
                newItem->setFlags(Qt::ItemIsEnabled);
                ui->dataStatesTableWidget->setVerticalHeaderItem(n,newItem);

                newItem = new QTableWidgetItem(tr("%1").arg(activeMatrix->characterList[column].getState(n).getName()));
                newItem->setFont(selectedFont);
                newItem->setFlags(Qt::ItemIsEnabled);
                ui->dataStatesTableWidget->setItem(n,0,newItem);
            } else {
                QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(symbol));
                newItem->setFlags(Qt::ItemIsEnabled);
                ui->dataStatesTableWidget->setVerticalHeaderItem(n,newItem);

                newItem = new QTableWidgetItem(tr("%1").arg(activeMatrix->characterList[column].getState(n).getName()));
                newItem->setFlags(Qt::ItemIsEnabled);
                ui->dataStatesTableWidget->setItem(n,0,newItem);
            }
            i++;
        }
    }

    symbol = activeMatrix->getMissingCharacter();
    if (activeMatrix->isSymbolSelected(symbol,taxonID,characterID)) {
        QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(symbol));
        newItem->setFont(selectedFont);
        newItem->setFlags(Qt::ItemIsEnabled);
        ui->dataStatesTableWidget->setVerticalHeaderItem(i,newItem);
        newItem = new QTableWidgetItem(tr("[Unknown]"));
        newItem->setFont(selectedFont);
        newItem->setFlags(Qt::ItemIsEnabled);
        ui->dataStatesTableWidget->setItem(i,0,newItem);
    } else {
        QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(symbol));
        newItem->setFlags(Qt::ItemIsEnabled);
        ui->dataStatesTableWidget->setVerticalHeaderItem(i,newItem);
        newItem = new QTableWidgetItem(tr("[Unknown]"));
        newItem->setFlags(Qt::ItemIsEnabled);
        ui->dataStatesTableWidget->setItem(i,0,newItem);
    }

    i++;
    symbol = activeMatrix->getGapCharacter();
    if (activeMatrix->isSymbolSelected(symbol,taxonID,characterID)) {
        QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(symbol));
        newItem->setFont(selectedFont);
        newItem->setFlags(Qt::ItemIsEnabled);
        ui->dataStatesTableWidget->setVerticalHeaderItem(i,newItem);      
        newItem = new QTableWidgetItem(tr("[Gap/Blank]"));
        newItem->setFont(selectedFont);
        newItem->setFlags(Qt::ItemIsEnabled);
        ui->dataStatesTableWidget->setItem(i,0,newItem);
    } else {
        QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(symbol));
        newItem->setFlags(Qt::ItemIsEnabled);
        ui->dataStatesTableWidget->setVerticalHeaderItem(i,newItem);
        newItem = new QTableWidgetItem(tr("[Gap/Blank]"));
        newItem->setFlags(Qt::ItemIsEnabled);
        ui->dataStatesTableWidget->setItem(i,0,newItem);
    }
}

/*------------------------------------------------------------------------------------/
 * Actions
 *-----------------------------------------------------------------------------------*/

//---- Action to Call new Matrix Mdi Child
void MainWindow::newFile()
{
    logAppend("Action","new file...");
    Matrix *child = createMatrix();
    child->newFile();
    child->show();
}

//---- Action to Open File
void MainWindow::openFile()
{
    logAppend("Action","open file...");

    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter("MaDE (*.made)");
    dialog.setViewMode(QFileDialog::Detail);
    if (dialog.exec()) {
        QStringList fileNames = dialog.selectedFiles();
        QString fileName = fileNames[0];
        logAppend("Action","open file: "+fileName);
        if (!fileName.isEmpty()) {
            QMdiSubWindow *existing = findMatrix(fileName);
            if (existing) {
                ui->mdiArea->setActiveSubWindow(existing);
                return;
            }

            Matrix *child = createMatrix();
            if (child->loadFile(fileName)) {
                statusBar()->showMessage(tr("Loading file..."), 2000);
                child->show();
            } else {
                child->close();
            }
        }
    } else {
        logAppend("Action","open file canceled.");
    }
}

//---- Save File
void MainWindow::saveFile()
 {
    logAppend("Action","save file...");
    if (getActiveMatrix() && getActiveMatrix()->saveCheck())
         statusBar()->showMessage(tr("File saved!"), 2000);
 }

//---- Save File As
void MainWindow::saveFileAs()
{
    logAppend("Action","save file as...");
    if (getActiveMatrix() && getActiveMatrix()->saveFileAs())
        statusBar()->showMessage(tr("File saved!"), 2000);
}

// Import NEXUS file
void MainWindow::importNexus()
{
    logAppend("Action","import NEXUS file...");
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter("NEXUS (*.nex)");
    dialog.setViewMode(QFileDialog::Detail);
    if (dialog.exec()) {
        QStringList filenames = dialog.selectedFiles();
        QString filename = filenames[0];
        if (!filename.isEmpty()) {
            QFile file(filename);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                logAppend("Action","Unable to open .nex file in readonly text mode.");
            } else {
                QTextStream iStream(&file);

                // New nexusReader(mainwindow, settings)
                NxsReader nexusReader(mainwindow, settings);

                // New NxsToken token(QTextStream);
                NxsToken token(iStream);

                // Add Block Reader
                nexusReader.addBlock("TAXA");
                //nexusReader.addBlock("ASSUMPTIONS");
                nexusReader.addBlock("CHARACTERS");
                //nexusReader.addBlock("NOTES");

                if(nexusReader.execute(token)) {
                    // Create New Matrix from data
                    if (nexusReader.getBlockCount("TAXA") != 0) {
                        QMap<QString, QVariant> taxaData = nexusReader.getBlockData("TAXA", 0);

                        // Available data keys: "NTAX"; "TAXLABELS".
                        int ntax = taxaData.value("NTAX").toInt();
                        QList<QVariant> taxonLabels = taxaData.value("TAXLABELS").toList();

                        qDebug() << "ntax =" << ntax;
                        for(int i = 0; i < taxonLabels.count(); i++){
                            qDebug() << "T" << i << "=" << taxonLabels[i].toString();
                        }
                    }
                    if (nexusReader.getBlockCount("CHARACTERS") != 0) {
                        QMap<QString, QVariant> charactersData = nexusReader.getBlockData("CHARACTERS", 0);

                        // Available data keys: "NCHAR".
                        int nchar = charactersData.value("NCHAR").toInt();

                        qDebug() << "nchar =" << nchar;
                    }

                } else {
                    logAppend("Action","import NEXUS file aborted by NEXUS Reader.");
                }
            file.close();
            }
        }
    } else {
        logAppend("Action","import NEXUS file canceled.");
    }
}


//---- Settings:
void MainWindow::settingsDialogOpen()
{
    SettingsDialog *dialog = new SettingsDialog;
    dialog->mw = mainwindow;
    dialog->settings = settings;
    dialog->initalize();
    dialog->exec();
}

//---- Matrix Settings
void MainWindow::matrixSettingsDialogOpen()
{
    activeMatrix->settingsDialog();
}

//---- Matrix Taxa Dialog
void MainWindow::matrixTaxaDialogOpen()
{
    activeMatrix->taxaDialog(0);
}

//---- Matrix Characters Dialog
void MainWindow::matrixCharactersDialogOpen()
{
    activeMatrix->charactersDialog(0);
}

//---- About MaDE:
void MainWindow::about()
{
    QMessageBox msgBox;
    QSpacerItem* horizontalSpacer = new QSpacerItem(600, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setWindowTitle(tr("About Matrix Data Editor (MaDE)"));
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(
                tr("<b>Ma</b>trix <b>D</b>ata <b>E</b>ditor (MaDE) has been programmed to open, edit, save, import and export various types of NEXUS data files.")
                +"<br><br>"
                +tr("MaDE Version: v%1").arg( QString::number(APP_VERSION, 'f', 1) )
                +"<br><br>"
                +tr("Created by: %1").arg("Alan R.T. Spencer")
                +"<hr>"
                +"<b>References:</b>"
                +"<ul>"
                +"<li>Maddison, D.R., Swofford, D.L., Maddison, W.P., 1997. Nexus: An Extensible File Format for Systematic Information. Systematic Biology 46, 590-621. DOI: <a href=\"http://dx.doi.org/10.1093/sysbio/46.4.590\">10.1093/sysbio/46.4.590</a>.</li>"
                +"<li>Nixon, K.C., Carpenter, J.M., Borgardt, S.J., 2001. Beyond NEXUS: Universal Cladistic Data Objects. Cladistics 17, S53–S59. DOI: <a href=\"http://dx.doi.org/10.1111/j.1096-0031.2001.tb00104.x\">10.1111/j.1096-0031.2001.tb00104.x</a>.</li>"
                +"</ul>"
                );
    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
    msgBox.exec();
}

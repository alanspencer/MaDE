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

#include <QtWidgets>
#include <QtGui>

#include "matrix.h"
#include "matrixsettingsdialog.h"
#include "taxadialog.h"
#include "charactersdialog.h"

Matrix::Matrix()
{
    setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    matrix = this;

    // Defaults
    matrixType = 0; /* 0 = STANDARD; 1 = DNA; 2 = RNA; 3 = NUCLEOTIDE; 4 = PROTINE*/
    isUntitled = true;
    isModified = false;
    isSelected = false;
    nextCharacterID = 0;
    nextTaxonID = 0;
    previousSelectedCell = currentSelectedCell = new QPair<int,int>(0,0);
    previousSelectedCellColor = QBrush(Qt::transparent);
    currentSelectedCellColor = QBrush(QColor(176,196,222));
    currentSelectedCellData = missingCharacter;

    //---- Get Settings
    missingCharacter = settings->getSetting("defaultMissingCharacter").toString();
    gapCharacter = settings->getSetting("defaultGapCharacter").toString();
    numTaxaToAdd = settings->getSetting("defaultNumberTaxa").toInt();
    numCharatersToAdd = settings->getSetting("defaultNumberCharacter").toInt();
    matrixName = settings->getSetting("defaultName").toString();
    disallowedCharactersList = settings->getSetting("defaultDisallowedCharacters").toList();
    matrixTypesList = settings->getSetting("defaultMatrixTypes").toList();
    stateSetList = settings->getSetting("defaultStandardStateSet").toList();

    initializeMatrixTable();
}


/*------------------------------------------------------------------------------------/
 * Matrix Table Functions
 *-----------------------------------------------------------------------------------*/

void Matrix::initializeMatrixTable()
{
    matrix->matrixRightTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // Add Line to Splitter
    QSplitterHandle *handle = matrix->matricTableSplitter->handle(1);
    QHBoxLayout *layout = new QHBoxLayout(handle);
    layout->setSpacing(2);
    layout->setMargin(0);
    QFrame *line = new QFrame(handle);
    line->setFrameShape(QFrame::VLine);
    layout->addWidget(line);

    // Interconnect tables with scrollbars
    connect(matrix->matrixTableHorizontalScrollBar, SIGNAL(valueChanged(int)), matrix->matrixRightTableWidget->horizontalScrollBar(), SLOT(setValue(int)));
    connect(matrix->matrixRightTableWidget->horizontalScrollBar(), SIGNAL(valueChanged(int)), matrix->matrixTableHorizontalScrollBar, SLOT(setValue(int)));
    connect(matrix->matrixRightTableWidget->horizontalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(updateHorizontalScrollbarRange(int, int)));
    connect(matrix->matrixTableVerticalScrollBar, SIGNAL(valueChanged(int)), matrix->matrixLeftTableWidget->verticalScrollBar(), SLOT(setValue(int)));
    connect(matrix->matrixLeftTableWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), matrix->matrixTableVerticalScrollBar, SLOT(setValue(int)));

    connect(matrix->matrixTableVerticalScrollBar, SIGNAL(valueChanged(int)), matrix->matrixRightTableWidget->verticalScrollBar(), SLOT(setValue(int)));
    connect(matrix->matrixRightTableWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), matrix->matrixTableVerticalScrollBar, SLOT(setValue(int)));
    connect(matrix->matrixRightTableWidget->verticalScrollBar(), SIGNAL(rangeChanged(int, int)), this, SLOT(updateVerticalScrollbarRange(int, int)));

    connect(matrix->matricTableSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(updateSplitter(int, int)));

    matrix->matrixRightTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    matrix->matrixRightTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    matrix->matrixLeftTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    matrix->matrixLeftTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Fix header hieght
    matrix->matrixRightTableWidget->horizontalHeader()->setFixedHeight(20);
    matrix->matrixLeftTableWidget->horizontalHeader()->setFixedHeight(20);
    // Fix row hieghts
    matrix->matrixRightTableWidget->setRowHeight(0,20);
    matrix->matrixLeftTableWidget->setRowHeight(0,20);
    // Fix column widths
    matrix->matrixRightTableWidget->setColumnWidth(0,20);

    // Mouse actions
    connect(matrix->matrixRightTableWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(rightTableContexMenu(const QPoint&)));
    initializeSelection();

    //Edit actions
    connect(matrix->matrixRightTableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(updateRightTableCellChanged(QTableWidgetItem *)));
    connect(matrix->matrixRightTableWidget->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(horizontalHeaderRightTableDoubleClick(int)));
    connect(matrix->matrixLeftTableWidget->verticalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(verticalHeaderLeftTableDoubleClick(int)));
    connect(matrix->matrixRightTableWidget->verticalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(verticalHeaderRightTableDoubleClick(int)));

}

void Matrix::setupMatrixTable()
{   
    int taxaNumber = taxaCount();
    int characterNumber = charactersCount();

    // Add Taxa List
    if (taxaNumber != 0) {
        //---- Update the table view
        for(int t = 0; t < taxaNumber; t++)
        {
            QString taxonName = taxonList[t].getLabel();
            //---- Add Row
            matrix->matrixLeftTableWidget->insertRow(t);
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(taxonName));
            newItem->setFlags(Qt::ItemIsEnabled);
            matrix->matrixLeftTableWidget->setItem(t, 0, newItem);           

            totalNumberProcessed++;
            progress->setValue(totalNumberProcessed);
        }        
    }

    updateVerticalHeadersLeftTable();

    // Add Character Headers
    if (characterNumber != 0) {
        //---- Update the table view
        for(int c = 0; c < characterNumber; c++)
        {
            //---- Add Column
            matrix->matrixRightTableWidget->insertColumn(c);
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("C%1").arg((c+1)));
            newItem->setFlags(Qt::ItemIsEnabled);
            matrix->matrixRightTableWidget->setHorizontalHeaderItem(c,newItem);

            totalNumberProcessed++;
            progress->setValue(totalNumberProcessed);
        }
        if (taxaNumber != 0) {
            matrix->matrixRightTableWidget->setRowCount(taxaNumber);
            // Add Cell Values
            for(int t = 0; t < taxaNumber; t++)
            {
                // Get Taxa ID
                int taxonID = taxonList[t].getID();

                // Set Cell Data
                for(int c = 0; c < characterNumber; c++)
                {
                    // Get Character ID
                    int characterID = characterList[c].getID();

                    // Lookup State Data from matrixGrid
                    Cell *currentCell = matrixGrid.value(returnLocator(taxonID, characterID));
                    QString currentData = currentCell->getState();

                    QTableWidgetItem *newItem = new QTableWidgetItem(currentData);
                    newItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
                    matrix->matrixRightTableWidget->setItem(t, c, newItem);
                    matrix->matrixRightTableWidget->item(t,c)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
                }

                totalNumberProcessed++;
                progress->setValue(totalNumberProcessed);
            }
        }
    }
    // Set Vertical Header
    updateVerticalHeadersRightTable();

    matrix->matrixRightTableWidget->item(0,0)->setBackground(currentSelectedCellColor);
    isSelected = true;
}

/*------------------------------------------------------------------------------------/
 * Matrix Table Selection Functions
 *-----------------------------------------------------------------------------------*/
void Matrix::updateRightTableSelectionChanged(const QModelIndex & current, const QModelIndex & previous)
{
    isSelected = true;
    if (previous.row() > -1 && previous.column() > -1) {
        matrixRightTableWidget->item(previous.row(),previous.column())->setBackground(previousSelectedCellColor);
    } else {
        matrixRightTableWidget->item(previousSelectedCell->first, previousSelectedCell->second)->setBackground(previousSelectedCellColor);
    }
    matrixRightTableWidget->item(current.row(),current.column())->setBackground(currentSelectedCellColor);

    if (previous.row() > -1 && previous.column() > -1) {
        previousSelectedCell = new QPair<int,int>(previous.row(),previous.column());
    }
    currentSelectedCell = new QPair<int,int>(current.row(),current.column());
    currentSelectedCellData = matrix->matrixRightTableWidget->item(current.row(),current.column())->text();

    mw->taxonListSelect(current.row());
    mw->characterListSelect(current.column());
    mw->updateDataDock();
}

void Matrix::verticalHeaderLeftTableDoubleClick(int row)
{
    mw->logAppend("Matrix",QString("vertical header at position '%1' double clicked.").arg(row));
    taxaDialog(row);
}

void Matrix::verticalHeaderRightTableDoubleClick(int row)
{
    mw->logAppend("Matrix",QString("vertical header at position '%1' double clicked.").arg(row));
    taxaDialog(row);
}

void Matrix::horizontalHeaderRightTableDoubleClick(int column)
{
    mw->logAppend("Matrix",QString("horizontal header at position '%1' double clicked.").arg(column));
    charactersDialog(column);
}

void Matrix::initializeSelection()
{
    connect(matrixRightTableWidget->selectionModel(), SIGNAL(currentChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(updateRightTableSelectionChanged(const QModelIndex &,const QModelIndex &)));
    // Set selection cell vars to 0,0
    currentSelectedCell = new QPair<int,int>(0,0);
    previousSelectedCell = new QPair<int,int>(0,0);

    // Set selection
    QModelIndex index = matrixRightTableWidget->model()->index(0, 0);
    matrixRightTableWidget->selectionModel()->select(index, QItemSelectionModel::Select);
}

void Matrix::resetSelection()
{
    disconnect(matrixRightTableWidget->selectionModel(), SIGNAL(currentChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(updateRightTableSelectionChanged(const QModelIndex &,const QModelIndex &)));

    matrixRightTableWidget->item(currentSelectedCell->first, currentSelectedCell->second)->setBackground(previousSelectedCellColor);

    // Set selection none
    currentSelectedCell = new QPair<int,int>(0,0);
    previousSelectedCell = new QPair<int,int>(0,0);

    // Set selection
    matrixRightTableWidget->selectionModel()->clear();
}

/*------------------------------------------------------------------------------------/
 * Matrix Table Splitter Function
 *-----------------------------------------------------------------------------------*/
void Matrix::updateSplitter(int min, int max)
{
    if (min == 0 && max != -1) {
        // Show Vertical Headers on Right Table
        matrixRightTableWidget->verticalHeader()->show();
    } else {
        // Hide Vertical Headers on Right Table
        matrixRightTableWidget->verticalHeader()->hide();
    }
}

/*------------------------------------------------------------------------------------/
 * Matrix Table Scrollbar Functions
 *-----------------------------------------------------------------------------------*/
void Matrix::updateHorizontalScrollbarRange(int min, int max)
{
    matrixTableHorizontalScrollBar->setRange(min, max);
}

void Matrix::updateVerticalScrollbarRange(int min, int max)
{
    matrixTableVerticalScrollBar->setRange(min, max);
}


/*------------------------------------------------------------------------------------/
 * Matrix Right Table Context Menu Function
 *-----------------------------------------------------------------------------------*/
void Matrix::rightTableContexMenu(const QPoint& pos)
{
    QMenu contextMenu(tr("Quick Edit:"), this);

    // List states and mark as checked/not checked
    contextMenu.addAction(new QAction(tr("View/Edit Notes"), this));

    contextMenu.exec(matrixRightTableWidget->mapToGlobal(pos));
}

/*------------------------------------------------------------------------------------/
 * Matrix Right Table Edit Function
 *-----------------------------------------------------------------------------------*/
void Matrix::updateRightTableCellChanged(QTableWidgetItem * item)
{
    QString input = item->text();
    input.simplified();
    input.replace(" ","");

    // Look up Cell Data
    int taxonID = taxonList[item->row()].getID();
    int characterID = characterList[item->column()].getID();

    Cell *currentCell = matrixGrid.value(returnLocator(taxonID, characterID));
    QString currentState = currentCell->getState();
    QString currentNotes = currentCell->getNotes();

    // Check new value agaist stored value
    if (currentState != item->text()) {
        bool isError = false;
        // Do error checking here... must be a registered state and only contain the correct symbols.
        if(input.size() == 0) {
            mw->logAppend("Matrix Edit","error detected, data reset. Attempt to set the 'Character State' to 'Gap' without using the 'Gap' symbol.");
            isError = true;
        }
        if (input.size() == 1) {
            // Check against allowed states
            if(!isSymbolAllowed(input, item->column())){
                mw->logAppend("Matrix Edit","error detected, data reset. Attempt to set the 'Character State' for the taxon to an illegal symbol.");
                isError = true;
            }
        }
        if (input.size() > 1) {
            // Remove any duplicate text characters
            QString cleansedString;
            for (int i = 0; i < input.size(); ++i) {
                bool addToString = true;
                for (int n = 0; n < cleansedString.size(); ++n) {
                    if (cleansedString.at(n) == input.at(i)) {
                        addToString = false;
                    }
                }
                if (addToString) {
                    cleansedString.append(input.at(i));
                }
            }
            input.clear();
            input.append(cleansedString);

            // Search for a gap or unknown character
            for (int i = 1; i < input.size()-1; ++i) {
                if(input.at(i) == gapCharacter || input.at(i) == missingCharacter){
                    mw->logAppend("Matrix Edit","error detected, data reset. Attempt to set a polymorphic or uncertain 'Character State' for the taxon with a 'Gap' and/or 'Unknown' symbol included.");
                    isError = true;
                }
            }

            if (!isError) {
                // Loop through all text characters
                if (input.startsWith("(") && input.endsWith(")")) {
                    // It is a polymorphic state
                    for (int i = 1; i < input.size()-1; ++i) {
                        if(!isSymbolAllowed(input.at(i), item->column())){
                            mw->logAppend("Matrix Edit","error detected, data reset. Attempt to set a polymorphic 'Character State' for the taxon with an illegal symbol.");
                            isError = true;
                        }
                    }
                } else if (input.startsWith("{") && input.endsWith("}")) {
                    // It is a state with uncertainty
                    for (int i = 1; i < input.size()-1; ++i) {
                        if(!isSymbolAllowed(input.at(i), item->column())){
                            mw->logAppend("Matrix Edit","error detected, data reset. Attempt to set an uncertain 'Character State' for the taxon with an illegal symbol.");
                            isError = true;
                        }
                    }
                } else {
                    // Assume that it is a polymorphic state and format accordingly
                    for (int i = 0; i < input.size(); ++i) {
                        if(!isSymbolAllowed(input.at(i), item->column())){
                            mw->logAppend("Matrix Edit","error detected, data reset. Attempt to set a polymorphic 'Character State' for the taxon with an illegal symbol.");
                            isError = true;
                        }
                    }
                    input.prepend("(");
                    input.append(")");
                }
            }
        }


        if (!isError) {
            // Has changed therefore update stored value and data dock
            item->setText(input);
            cellEdit(taxonID, characterID, item->text(), currentNotes);
            mw->updateDataDock();
            mw->logAppend("Matrix Edit","data updated.");
        } else {
            // There is an error replace cell value with stored
            item->setText(currentState);
        }
    }
}

/*------------------------------------------------------------------------------------/
 * Matrix Left Table Text Update Function
 *-----------------------------------------------------------------------------------*/
void Matrix::updateLeftTableText(int row, QString text)
{
    matrixLeftTableWidget->item(row, 0)->setText(text);
}

/*------------------------------------------------------------------------------------/
 * Matrix Table Move Row (i.e. Taxon) Up/Down Functions
 *-----------------------------------------------------------------------------------*/
void Matrix::moveRow(int row, bool up)
{
    resetSelection();
    moveRowLeftTable(row, up);
    moveRowRightTable(row, up);
    initializeSelection();
}

void Matrix::moveRowLeftTable(int row, bool up)
{
    int sourceRow = row;
    int destRow = (up ? sourceRow-1 : sourceRow+1);

    // Take whole rows
    QList<QTableWidgetItem*> sourceItems = getRowLeftTable(sourceRow);
    QList<QTableWidgetItem*> destItems = getRowLeftTable(destRow);

    // Set back in reverse order
    setRowLeftTable(sourceRow, destItems);
    setRowLeftTable(destRow, sourceItems);
}

void Matrix::moveRowRightTable(int row, bool up)
{
    int sourceRow = row;
    int destRow = (up ? sourceRow-1 : sourceRow+1);

    // Take whole rows
    QList<QTableWidgetItem*> sourceItems = getRowRightTable(sourceRow);
    QList<QTableWidgetItem*> destItems = getRowRightTable(destRow);

    // Set back in reverse order
    setRowRightTable(sourceRow, destItems);
    setRowRightTable(destRow, sourceItems);
}


// Takes and returns the whole row
QList<QTableWidgetItem*> Matrix::getRowLeftTable(int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int column = 0; column < matrixLeftTableWidget->columnCount(); ++column)
    {
        rowItems << matrixLeftTableWidget->takeItem(row, column);
    }
    return rowItems;
}

// Sets the whole row
void Matrix::setRowLeftTable(int row, const QList<QTableWidgetItem*>& rowItems)
{
    for (int column = 0; column < matrixLeftTableWidget->columnCount(); ++column)
    {
        matrixLeftTableWidget->setItem(row, column, rowItems.at(column));
    }
}

// Takes and returns the whole row
QList<QTableWidgetItem*> Matrix::getRowRightTable(int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int column = 0; column < matrixRightTableWidget->columnCount(); ++column)
    {
        rowItems << matrixRightTableWidget->takeItem(row, column);
    }
    return rowItems;
}

// Sets the whole row
void Matrix::setRowRightTable(int row, const QList<QTableWidgetItem*>& rowItems)
{
    for (int column = 0; column < matrixRightTableWidget->columnCount(); ++column)
    {
        matrixRightTableWidget->setItem(row, column, rowItems.at(column));
    }
}

/*------------------------------------------------------------------------------------/
 * Matrix Table Delete Row (i.e. Taxon) Functions
 *-----------------------------------------------------------------------------------*/
void Matrix::deleteRow(int row)
{
    if (taxaCount() > 0) {
        resetSelection();

        int taxonID = taxonList[row].getID();
        for(int i = 0; i < charactersCount(); i++)
        {
            int characterID = characterList[i].getID();
            cellRemove(taxonID, characterID);
        }

        taxonRemove(row);

        deleteRowLeftTable(row);
        deleteRowRightTable(row);

        updateVerticalHeadersLeftTable();
        updateVerticalHeadersRightTable();

        initializeSelection();
    }
}

void Matrix::deleteRowLeftTable(int row)
{
    matrixLeftTableWidget->removeRow(row);
}

void Matrix::deleteRowRightTable(int row)
{
    matrixRightTableWidget->removeRow(row);
}

/*------------------------------------------------------------------------------------/
 * Matrix Table Insert Row (i.e. Taxon) Functions
 *-----------------------------------------------------------------------------------*/
void Matrix::insertRow(int row)
{
    resetSelection();
    insertRowLeftTable(row);
    insertRowRightTable(row);
    updateVerticalHeadersLeftTable();
    updateVerticalHeadersRightTable();
    initializeSelection();
}

void Matrix::insertRowLeftTable(int row)
{
    QString taxonName = taxonList[row].getLabel();
    //---- Add Row
    matrixLeftTableWidget->insertRow(row);
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(taxonName));
    newItem->setFlags(Qt::ItemIsEnabled);
    matrixLeftTableWidget->setItem(row, 0, newItem);
}

void Matrix::insertRowRightTable(int row)
{
    matrixRightTableWidget->insertRow(row);
    // Get Taxa ID
    int taxonID = taxonList[row].getID();
    int characterNumber = charactersCount();

    // Set Cell Data
    for(int c = 0; c < characterNumber; c++)
    {
        // Get Character ID
        int characterID = characterList[c].getID();

        // Create Cell data
        cellAdd(taxonID, characterID, missingCharacter, "");

        // Lookup State Data from matrixGrid
        Cell *currentCell = matrixGrid.value(returnLocator(taxonID, characterID));
        QString currentData = currentCell->getState();

        QTableWidgetItem *newItem = new QTableWidgetItem(currentData);
        newItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
        matrixRightTableWidget->setItem(row, c, newItem);
        matrixRightTableWidget->item(row, c)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

/*------------------------------------------------------------------------------------/
 * Matrix Table Update Vertical Headers (i.e. Tx) Functions
 *-----------------------------------------------------------------------------------*/
void Matrix::updateVerticalHeadersLeftTable()
{
    int taxaNumber = taxaCount();
    if (taxaNumber != 0) {
        //---- Update the table view
        for(int t = 0; t < taxaNumber; t++)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("T%1").arg(t+1));
            newItem->setFlags(Qt::ItemIsEnabled);
            matrixLeftTableWidget->setVerticalHeaderItem(t,newItem);
        }
    }

}

void Matrix::updateVerticalHeadersRightTable()
{
    int taxaNumber = taxaCount();
    if (taxaNumber != 0) {
        //---- Update the table view
        for(int t = 0; t < taxaNumber; t++)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("T%1").arg(t+1));
            newItem->setFlags(Qt::ItemIsEnabled);
            matrixRightTableWidget->setVerticalHeaderItem(t,newItem);
        }
    }
}

/*------------------------------------------------------------------------------------/
 * Matrix Table Move Column (i.e. Character) Left/Right Functions
 *-----------------------------------------------------------------------------------*/
void Matrix::moveColumn(int column, bool left)
{
    int sourceColumn = column;
    int destColumn = (left ? sourceColumn-1 : sourceColumn+1);

    // Take whole rows
    QList<QTableWidgetItem*> sourceItems = getColumn(sourceColumn);
    QList<QTableWidgetItem*> destItems = getColumn(destColumn);

    // Set back in reverse order
    setColumn(sourceColumn, destItems);
    setColumn(destColumn, sourceItems);
}

// Takes and returns the whole row
QList<QTableWidgetItem*> Matrix::getColumn(int column)
{
    QList<QTableWidgetItem*> columnItems;
    for (int row = 0; row < matrixRightTableWidget->rowCount(); ++row)
    {
        columnItems << matrixRightTableWidget->takeItem(row, column);
    }
    return columnItems;
}

// Sets the whole row
void Matrix::setColumn(int column, const QList<QTableWidgetItem*>& columnItems)
{
    for (int row = 0; row < matrixRightTableWidget->rowCount(); ++row)
    {
        matrixRightTableWidget->setItem(row, column, columnItems.at(row));
    }
}

/*------------------------------------------------------------------------------------/
 * Matrix Table Insert Column (i.e. Character) Function
 *-----------------------------------------------------------------------------------*/
void Matrix::insertColumn(int column)
{
    resetSelection();

    // Insert Column data - set all to unknown sysmbol
    matrixRightTableWidget->insertColumn(column);
    // Get Taxa ID
    int characterID = characterList[column].getID();

    // Set Cell Data
    for(int t = 0; t < taxaCount(); t++)
    {
        // Get Character ID
        int taxonID = taxonList[t].getID();

        // Create Cell data
        cellAdd(taxonID, characterID, missingCharacter, "");

        // Lookup State Data from matrixGrid
        Cell *currentCell = matrixGrid.value(returnLocator(taxonID, characterID));
        QString currentData = currentCell->getState();

        QTableWidgetItem *newItem = new QTableWidgetItem(currentData);
        newItem->setFlags(Qt::ItemIsEnabled|Qt::ItemIsEditable);
        matrixRightTableWidget->setItem(t, column, newItem);
        matrixRightTableWidget->item(t, column)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }

    updateHorizontalHeadersRightTable();
    initializeSelection();
}

/*------------------------------------------------------------------------------------/
 * Matrix Table Delete Column (i.e. Character) Function
 *-----------------------------------------------------------------------------------*/
void Matrix::deleteColumn(int column)
{
    if (charactersCount() > 0) {
        resetSelection();

        matrixRightTableWidget->removeColumn(column);

        int characterID = characterList[column].getID();
        for(int t = 0; t < taxaCount(); t++)
        {
            int taxonID = taxonList[t].getID();
            cellRemove(taxonID, characterID);
        }

        charactersRemove(column);

        updateHorizontalHeadersRightTable();

        initializeSelection();
    }
}

/*------------------------------------------------------------------------------------/
 * Matrix Right Table Update Horizontal Headers (i.e. Cx) Function
 *-----------------------------------------------------------------------------------*/
void Matrix::updateHorizontalHeadersRightTable()
{
    int characterNumber = charactersCount();
    if (characterNumber != 0) {
        //---- Update the table view
        for(int c = 0; c < characterNumber; c++)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("C%1").arg(c+1));
            newItem->setFlags(Qt::ItemIsEnabled);
            matrixRightTableWidget->setHorizontalHeaderItem(c,newItem);
        }
    }
}
/*------------------------------------------------------------------------------------/
 * Filename Functions
 *-----------------------------------------------------------------------------------*/
QString Matrix::userFriendlyCurrentFile()
{
    return strippedName(currentFile);
}

void Matrix::setCurrentFile(QString fileName)
{
    currentFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    isModified = false;
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString Matrix::strippedName(QString fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

/*------------------------------------------------------------------------------------/
 * General Setters/Getters
 *-----------------------------------------------------------------------------------*/

// Undefined Discription Functions
void Matrix::setMatrixDescription(QString description)
{
    isModified = true;
    matrixDescription = description;
};

QString Matrix::getMatrixDescription()
{
    return  matrixDescription;
};

// Undefined Character Functions
void Matrix::setMissingCharacter(QString character)
{
    isModified = true;
    missingCharacter = character;
};

QString Matrix::getMissingCharacter()
{
    return missingCharacter;
};

// Matrix Type Functions
void Matrix::setMatrixType(int type)
{
    isModified = true;
    matrixType = type;
};

int Matrix::getMatrixType()
{
    return matrixType;
};

QString Matrix::getMatrixTypeName()
{
    return matrixTypesList[matrixType].toString();
};

// Gap Character Functions
void Matrix::setGapCharacter(QString character)
{
    isModified = true;
    gapCharacter = character;
};

QString Matrix::getGapCharacter()
{
    return gapCharacter;
};

// Matrix Name Functions
void Matrix::setMatrixName(QString name)
{
    isModified = true;
    matrixName = name;
};

QString Matrix::getMatrixName()
{
    return matrixName;
};


/*------------------------------------------------------------------------------------/
 * On Close
 *-----------------------------------------------------------------------------------*/

//-- Close Event:
void Matrix::closeEvent(QCloseEvent *event)
{
    if (maybeSaveCheck()) {
        mw->logAppend("Matrix","closing window containing matrix file \""+currentFile+"\".");
        mw->logAppend("Matrix (Mdi Child)","destroyed.");        
        event->accept();
    } else {
        event->ignore();
    }    
}


//-- Is Modified Check:
bool Matrix::maybeSaveCheck()
{
    if (isModified) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("MaDE - Do you want to save?"),
                  tr("'%1' has been modified.\n"
                     "Do you want to save your changes?")
                  .arg(userFriendlyCurrentFile()),
                  QMessageBox::Save | QMessageBox::Discard
                  | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return saveCheck();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

/*------------------------------------------------------------------------------------/
 * Actions
 *-----------------------------------------------------------------------------------*/

//---- New File
void Matrix::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    isModified = false;

    //---- Create File
    currentFile = tr("document%1.made").arg(sequenceNumber++);
    setWindowTitle(currentFile + "[*]");
    mw->logAppend("Matrix","new matrix file called \""+currentFile+"\" has been created.");

    totalNumberToProcess = ((numTaxaToAdd+numTaxaToAdd+numCharatersToAdd)*2);
    totalNumberProcessed = 0;
    progress = new QProgressDialog("Setting up the Matrix...", "Abort", 0, totalNumberToProcess, mw);
    progress->setCancelButton(0);
    progress->setMinimumDuration(0);
    progress->setWindowModality(Qt::WindowModal);

    // Only show if the number of cells is above...
    if ((numTaxaToAdd*numCharatersToAdd) > 6400) {
        progress->show();
    }

    // Taxa
    for (int n=1; n<(numTaxaToAdd+1); n++) {
        taxonAdd(QString("Taxon %1").arg(n), "");
        totalNumberProcessed++;
        progress->setValue(totalNumberProcessed);
        qApp->processEvents();
    }

    // Characters
    for (int n=1; n<(numCharatersToAdd+1); n++) {
        characterAdd(QString("Character %1").arg(n), "");
        totalNumberProcessed++;
        progress->setValue(totalNumberProcessed);
    }

    // Data Cells
    for (int t=0; t<(numTaxaToAdd); t++) {
        for (int c=0; c<(numCharatersToAdd); c++) {
            cellAdd(t, c, missingCharacter, "");
        }
        totalNumberProcessed++;
        progress->setValue(totalNumberProcessed);
    }

    setupMatrixTable();

    mw->logAppend("Matrix",
                  QString("\""+currentFile+"\" has %1 'Taxa' and %2 'Characters'. States set to unknown ("+missingCharacter+") symbol.")
                  .arg(taxaCount())
                  .arg(charactersCount()));
}

//---- Load File
bool Matrix::loadFile(QString fileName)
{
    // Load file as sqlLite DB

    setCurrentFile(fileName);

    return true;
}

//---- Save File Check
bool Matrix::saveCheck()
{
    if (isUntitled) {
        return saveFileAs();
    } else {
        return saveFile(currentFile);
    }
}

//---- Save File As
bool Matrix::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), currentFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

//---- Save File
bool Matrix::saveFile(QString fileName)
{
    // Save file as SQLite DB

    setCurrentFile(fileName);

    return true;
}

//---- Edit Matrix Settings
void Matrix::settingsDialog()
{
    MatrixSettingsDialog *dialog = new MatrixSettingsDialog;
    dialog->mw = mw;
    dialog->matrix = matrix;
    dialog->initalize();
    dialog->exec();
}

/*------------------------------------------------------------------------------------/
 * Taxa Functions
 *-----------------------------------------------------------------------------------*/

//---- Add Taxon
bool Matrix::taxonAdd(QString name, QString notes) {   
    taxonList.append(Taxon(nextTaxonID++, name, notes));
    isModified = true;
    return true;
}

//---- Edit Taxon
bool Matrix::taxonEdit(int key, QString name, QString notes) {
    taxonList[key].setLabel(name);
    taxonList[key].setNotes(notes);
    isModified = true;
    return true;
}

//---- Remove Taxon
bool Matrix::taxonRemove(int row)
{
    taxonList.removeAt(row);
    isModified = true;
    return true;
}

int Matrix::taxaCount()
{
    return taxonList.count();
}

void Matrix::taxaDialog(int row)
{
    TaxaDialog *dialog = new TaxaDialog;
    dialog->mw = mw;
    dialog->matrix = matrix;
    dialog->settings = settings;
    dialog->initalize(row);
    dialog->exec();
}

/*------------------------------------------------------------------------------------/
 * Character Functions
 *-----------------------------------------------------------------------------------*/

//---- Add Character
bool Matrix::characterAdd(QString name, QString notes) {
    Character character(nextCharacterID++, name, notes);

    // Add default states
    switch(matrixType) {
    // STANDARD Matrix
    case 0:
        /*
         * 0 = Absent
         * 1 = Present
        */        
        character.addState("0","Absent", "A default '<font color=\"red\">Absent</font>' state.");
        character.addState("1","Present", "A default '<font color=\"green\">Present</font>' state.");
        break;

    // DNA
    case 1:

        break;

    // RNA
    case 2:

        break;

    // NUCLEOTIDE
    case 3:

        break;

    // PROTINE
    case 4:

        break;
    }

    characterList.append(character);
    isModified = true;
    return true;
}

//---- Edit Character
bool Matrix::characterEdit(int key, QString name, QString notes) {
    characterList[key].setLabel(name);
    characterList[key].setNotes(notes);
    isModified = true;
    return true;
}

int Matrix::charactersCount()
{
    return characterList.count();
}

//---- Remove Taxon
bool Matrix::charactersRemove(int column)
{
    characterList.removeAt(column);
    isModified = true;
    return true;
}

void Matrix::charactersDialog(int column = 0)
{
    CharactersDialog *dialog = new CharactersDialog;
    dialog->mw = mw;
    dialog->matrix = matrix;
    dialog->settings = settings;
    dialog->initalize(column);
    dialog->exec();
}

/*------------------------------------------------------------------------------------/
 * Matrix Character State Functions
 *-----------------------------------------------------------------------------------*/

// Return State Count for selected Character
int Matrix::stateCount(int characterKey)
{
    return characterList[characterKey].countStates();
}

// Return State List for selected Character
State Matrix::getState(int characterKey, int stateKey)
{
    return characterList[characterKey].getState(stateKey);
}


/*------------------------------------------------------------------------------------/
 * Matrix Grid Data Functions
 *-----------------------------------------------------------------------------------*/

//---- Add Data Cell
bool Matrix::cellAdd(int taxonID, int characterID, QString state, QString notes)
{
    Cell *cellData = new Cell(state, notes);

    matrixGrid.insert(returnLocator(taxonID, characterID), cellData);

    isModified = true;
    return true;
}

//---- Edit Data Cell
bool Matrix::cellEdit(int taxonID, int characterID, QString state, QString notes)
{
    Cell *cellData = new Cell(state, notes);

    matrixGrid.insert(returnLocator(taxonID, characterID), cellData);

    isModified = true;
    return true;
}

//---- Remove Data Cell
bool Matrix::cellRemove(int taxonID, int characterID)
{
    matrixGrid.remove(returnLocator(taxonID, characterID));

    isModified = true;
    return true;
}

int Matrix::cellCount()
{
    return matrixGrid.count();
}

// Create Cell Locator
QPair<int,int> Matrix::returnLocator(int taxonID, int characterID)
{
    QPair<int,int> locator;
    locator.first = taxonID;
    locator.second = characterID;
    return locator;
}

/*------------------------------------------------------------------------------------/
 * Symbol Testing Functions
 *-----------------------------------------------------------------------------------*/

// Check symbol against data in cell
bool Matrix::isSymbolSelected(QString symbol, int taxonID, int characterID)
{
    Cell *currentCell = matrixGrid.value(returnLocator(taxonID, characterID));
    QString input = currentCell->getState();

    if (input.size() == 1) {
        if (symbol == input) {
            return true;
        } else {
            return false;
        }
    } else {
        bool returnValue = false;
        if ((input.startsWith("{") && input.endsWith("}")) || (input.startsWith("(") && input.endsWith(")"))) {
            for (int i = 1; i < input.size()-1; ++i) {
                if(input.at(i) == symbol){
                    returnValue = true;
                }
            }
        }
        return returnValue;
    }
}

// Check symbol against allowed states
bool Matrix::isSymbolAllowed(QString symbol, int characterID)
{
    // Create list of all symbols
    int stateNumber = characterList[characterID].stateList.count();
    QList<QString> allowedSymbols;
    for(int i=0; i<stateNumber; i++)
    {
        allowedSymbols.append(characterList[characterID].stateList[i].getSymbol());
    }
    allowedSymbols.append(missingCharacter);
    allowedSymbols.append(gapCharacter);

    bool isAllowed = false;
    for(int i=0; i<allowedSymbols.count(); i++)
    {
        if (allowedSymbols[i] == symbol) {
            isAllowed = true;
        }
    }

    return isAllowed;
}

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

#ifndef MATRIX_H
#define MATRIX_H

#include "ui_matrixTable.h"

#include <QtGui>
#include <QWidget>

#include <taxon.h>
#include <character.h>
#include <cell.h>
#include <mainwindow.h>
#include <settings.h>

class MainWindow;
class Settings;
class Cell;

class Matrix : public QWidget, Ui::matrixTableForm
{
    Q_OBJECT

public:
    Matrix();

    Matrix *matrix;
    MainWindow *mw;
    Settings *settings;

    QString returnCurrentFile() { return currentFile; }
    QString userFriendlyCurrentFile();

    int matrixType;

    void settingsDialog();
    void taxaDialog(int row);
    void charactersDialog(int column);

    void newFile();
    bool loadFile(QString fileName);
    bool saveCheck();
    bool saveFileAs();
    bool saveFile(QString fileName);

    void moveRow(int row, bool up);
    void deleteRow(int row);
    void insertRow(int row);

    void moveColumn(int column, bool left);
    void insertColumn(int column);
    void deleteColumn(int column);

    int nextTaxonID;
    QList <Taxon> taxonList;
    bool taxonAdd(QString name, QString notes);
    bool taxonEdit(int key, QString name, QString notes);
    bool taxonRemove(int row);
    int taxaCount();
    void updateLeftTableText(int row, QString text);

    int nextCharacterID;
    QList <Character> characterList;
    bool characterAdd(QString name, QString notes);
    bool characterEdit(int key, QString name, QString notes);
    bool charactersRemove(int column);
    int charactersCount();

    int stateCount(int characterKey);
    State getState(int characterKey, int stateKey);

    void setMatrixName(QString name);
    QString getMatrixName();

    void setMatrixType(int type);
    int getMatrixType();
    QString getMatrixTypeName();

    void setUnknownCharacter(QString character);
    QString getUnknownCharacter();

    void setGapCharacter(QString character);
    QString getGapCharacter();

    void setMatrixDescription(QString description);
    QString getMatrixDescription();

    QList<QVariant> stateSetList;
    bool isSymbolSelected(QString symbol, int taxonID, int characterID);
    bool isSymbolAllowed(QString symbol, int characterID);

    QHash<QPair<int, int>, Cell*> matrixGrid;
    bool cellAdd(int taxonID, int characterID, QString state, QString notes);
    bool cellEdit(int taxonID, int characterID, QString state, QString notes);
    bool cellRemove(int taxonID, int characterID);
    int cellCount();

    QPair<int,int> returnLocator(int taxonID, int characterID);
    QPair<int,int> *currentSelectedCell;
    QPair<int,int> *previousSelectedCell;
    QBrush currentSelectedCellColor;
    QBrush previousSelectedCellColor;
    QString currentSelectedCellData;

protected:
    void closeEvent(QCloseEvent *event);

private:    
    bool isUntitled;
    bool isModified;
    bool isSelected;
    QString currentFile;
    QString unknownCharacter;
    int numTaxaToAdd;
    int numCharatersToAdd;
    QString matrixName;
    QString matrixDescription;
    QString gapCharacter;
    QList<QVariant> disallowedCharactersList;
    QList<QVariant> matrixTypesList;

    int totalNumberProcessed;
    int totalNumberToProcess;
    bool wasCanceled;
    QProgressDialog *progress;

    void initializeMatrixTable ();
    void setupMatrixTable();
    bool maybeSaveCheck();
    void setCurrentFile(QString fileName);
    QString strippedName(QString fullFileName);

    void initializeSelection();
    void resetSelection();

    void moveRowLeftTable(int row, bool up);
    void moveRowRightTable(int row, bool up);
    void deleteRowLeftTable(int row);
    void deleteRowRightTable(int row);
    void insertRowLeftTable(int row);
    void insertRowRightTable(int row);
    void updateVerticalHeadersRightTable();
    void updateVerticalHeadersLeftTable();
    QList<QTableWidgetItem*> getRowLeftTable(int row);
    void setRowLeftTable(int row, const QList<QTableWidgetItem*>& rowItems);
    QList<QTableWidgetItem*> getRowRightTable(int row);
    void setRowRightTable(int row, const QList<QTableWidgetItem*>& rowItems);

    void updateHorizontalHeadersRightTable();
    QList<QTableWidgetItem*> getColumn(int column);
    void setColumn(int column, const QList<QTableWidgetItem*>& columnItems);

private slots:
    void updateHorizontalScrollbarRange(int min, int max);
    void updateVerticalScrollbarRange(int min, int max);
    void updateSplitter(int min, int max);
    void updateRightTableSelectionChanged(const QModelIndex & current, const QModelIndex & previous);
    void rightTableContexMenu(const QPoint& pos);
    void updateRightTableCellChanged(QTableWidgetItem * item);
    void horizontalHeaderRightTableDoubleClick(int column);
    void verticalHeaderLeftTableDoubleClick(int row);
    void verticalHeaderRightTableDoubleClick(int row);
};
#endif // MATRIX_H

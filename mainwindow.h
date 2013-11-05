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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractButton>
#include <QtWidgets>
#include <QMessageBox>


#include "settings.h"
#include "matrix.h"
#include "nexusreader.h"
#include "settingsdialog.h"

class Matrix;
class QAction;
class QMdiSubWindow;
class QSignalMapper;
class Settings;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Ui::MainWindow *ui;

    MainWindow *mainwindow;
    Settings *settings;

    void logAppend(const QString &strTitle, const QString &strMessage);

    void updateInformationDock();

    void updateTaxaDock();
    void updateTaxaDockTableText(int row, QString text);
    void moveTaxaDockTableRow(int row, bool up);
    void taxonListSelect(int row);
    void updateTaxaDockTableColor(int row, bool isEnabled);

    void updateCharacterDock();
    void updateCharacterDockTableText(int row, QString text);
    void updateCharacterDockTableColor(int row, bool isEnabled);
    void characterListSelect(int row);
    void moveCharacterDockTableRow(int row, bool up);

    void updateDataDock();

private:

    QSignalMapper *windowMapper;
    Matrix *activeMatrix;

    Matrix *getActiveMatrix();
    QMdiSubWindow *findMatrix(const QString &matrixID);

    QAction *actionTile;
    QAction *actionCascade;
    QAction *actionSeparator;

    QColor enabledColor;
    QColor disabledColor;

    void initializeMainMenu();
    void initializeInformationDock();
    void initializeDataDock();

    void initializeTaxaDock();
    QList<QTableWidgetItem*> getTaxaDockTableRow(int row);
    void setTaxaDockTableRow(int row, const QList<QTableWidgetItem*>& rowItems);

    void initializeCharacterDock();
    QList<QTableWidgetItem*> getCharacterDockTableRow(int row);
    void setCharacterDockTableRow(int row, const QList<QTableWidgetItem*>& rowItems);

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void importNexus();
    void settingsDialogOpen();
    void matrixSettingsDialogOpen();
    void matrixTaxaDialogOpen();    
    void matrixCharactersDialogOpen();
    void about();
    void updateWindowMenu();
    void updateMainWindow();
    void setActiveSubWindow(QWidget *window);
    Matrix *createMatrix();
};

#endif // MAINWINDOW_H

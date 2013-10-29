#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractButton>
#include <QtWidgets>
#include <QMessageBox>

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

    MainWindow *mw;   
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

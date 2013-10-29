#ifndef TAXADIALOG_H
#define TAXADIALOG_H

#include "ui_taxadialog.h"

#include <QtGui>
#include <QWidget>

#include <mainwindow.h>
#include <matrix.h>
#include <settings.h>

class TaxaDialog : public QDialog, Ui::TaxaDialog
{
    Q_OBJECT

public:
    TaxaDialog(QWidget *parent = 0);

    void initalize(int row);

    MainWindow *mw;
    Matrix *matrix;
    Settings *settings;

    int selectedRow;
    int previousRow;

private:
    void initializeTaxaDialog();
    void initializeTaxonList();
    void updateTaxaDialog();

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);

    void saveData(int row);

    void selectTaxon(int row);    
    QList<QTableWidgetItem*> getRow(int row);
    void setRow(int row, const QList<QTableWidgetItem*>& rowItems);
    void moveRow(bool up);

    void updateButtons(int row);
    void updateTaxaTableColor(int row, bool isEnabled);
    void updateVerticalHeader();

    bool isModified;
    bool doNotSave;

    QColor enabledColor;
    QColor disabledColor;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void buttonActions(QAbstractButton *button);
    void actionSave();
    void actionUpArrow();
    void actionDownArrow();
    void actionAddTaxon();
    void actionDeleteTaxon();

    void updateTableSelectionChanged(const QModelIndex & current,const QModelIndex & previous);
    void updateTableTaxonName(QString text);
    void isEnabledChanged(bool isEnabled);

    void textBold();
    void textUnderline();
    void textItalic();
    void textColor();
    void currentCharFormatChanged(const QTextCharFormat &format);
};

#endif // TAXADIALOG_H

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

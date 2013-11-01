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

#ifndef MATRIXSETTINGSDIALOG_H
#define MATRIXSETTINGSDIALOG_H

#include "ui_matrixsettingsdialog.h"

#include <QtGui>
#include <QWidget>

#include <mainwindow.h>
#include <matrix.h>

class MatrixSettingsDialog : public QDialog, Ui::MatrixSettingsDialog
{
    Q_OBJECT

public:
    MatrixSettingsDialog(QWidget *parent = 0);

    void initalize();

    MainWindow *mw;
    Matrix *matrix;

private:
    void loadValues();
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);

private slots:
    void buttonActions(QAbstractButton *button);
    void actionSave();
    void actionClose();
    void textBold();
    void textUnderline();
    void textItalic();
    void textColor();
    void currentCharFormatChanged(const QTextCharFormat &format);
};

#endif // MATRIXSETTINGSDIALOG_H

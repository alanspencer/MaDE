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

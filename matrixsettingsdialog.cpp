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

#include "matrixsettingsdialog.h"

MatrixSettingsDialog::MatrixSettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
}

void MatrixSettingsDialog::initalize()
{
    mw->logAppend("Matrix Settings Dialog","dialog opened.");

    loadValues();

    fontChanged(this->matrixDescriptionTextEdit->font());

    // Color Selector
    QPixmap pix(14, 14);
    pix.fill(Qt::black);
    this->colorToolButton->setIcon(pix);


    // Set up actions
    connect(this->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonActions(QAbstractButton*)));
    connect(this->boldToolButton, SIGNAL(clicked()), this, SLOT(textBold()));
    connect(this->italicToolButton, SIGNAL(clicked()), this, SLOT(textItalic()));
    connect(this->underlineToolButton, SIGNAL(clicked()), this, SLOT(textUnderline()));
    connect(this->colorToolButton, SIGNAL(clicked()), this, SLOT(textColor()));
    connect(this->matrixDescriptionTextEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),this, SLOT(currentCharFormatChanged(QTextCharFormat)));
}

void MatrixSettingsDialog::buttonActions(QAbstractButton *button)
{
    if (button->text() == tr("OK")) {
        mw->logAppend("Matrix Settings Dialog","'OK' pressed, saving new settings values to matrix and closing dialog...");
        actionSave();
    } else if (button->text() == tr("Cancel")){
        mw->logAppend("Matrix Settings Dialog","'Cancel' pressed, closing dialog without saving...");
        actionClose();
    }
}

void MatrixSettingsDialog::actionSave()
{
    // Save value to matrix
    matrix->setMatrixName(this->matrixNameLineEdit->text());
    matrix->setUnknownCharacter(this->unknownCharacterLineEdit->text());
    matrix->setMatrixDescription(this->matrixDescriptionTextEdit->toHtml());
    mw->updateInformationDock();
    actionClose();
}

void MatrixSettingsDialog::actionClose()
{
    mw->logAppend("Matrix Settings Dialog","dialog closed.");
    this->close();
}

void MatrixSettingsDialog::loadValues()
{
    // Get values from matrix
    this->matrixNameLineEdit->setText(matrix->getMatrixName());
    this->unknownCharacterLineEdit->setText(matrix->getUnknownCharacter());
    this->matrixDescriptionTextEdit->setHtml(matrix->getMatrixDescription());
}



void MatrixSettingsDialog::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(this->boldToolButton->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
    mw->logAppend("Matrix Settings Dialog","'Bold' button pressed.");
}

void MatrixSettingsDialog::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(this->underlineToolButton->isChecked());
    mergeFormatOnWordOrSelection(fmt);
    mw->logAppend("Matrix Settings Dialog","'Underline' button pressed.");
}

void MatrixSettingsDialog::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(this->italicToolButton->isChecked());
    mergeFormatOnWordOrSelection(fmt);
    mw->logAppend("Matrix Settings Dialog","'Italic' button pressed.");
}

void MatrixSettingsDialog::textColor()
{
    QColor col = QColorDialog::getColor(this->matrixDescriptionTextEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void MatrixSettingsDialog::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = this->matrixDescriptionTextEdit->textCursor();
    cursor.mergeCharFormat(format);
    this->matrixDescriptionTextEdit->mergeCurrentCharFormat(format);
}

void MatrixSettingsDialog::fontChanged(const QFont &f)
{
    this->boldToolButton->setChecked(f.bold());
    this->italicToolButton->setChecked(f.italic());
    this->underlineToolButton->setChecked(f.underline());
}

void MatrixSettingsDialog::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void MatrixSettingsDialog::colorChanged(const QColor &c)
{
    QPixmap pix(14, 14);
    pix.fill(c);
    this->colorToolButton->setIcon(pix);
}


#ifndef CHARACTERSDIALOG_H
#define CHARACTERSDIALOG_H

#include "ui_charactersdialog.h"

#include <QtGui>
#include <QWidget>

#include <mainwindow.h>
#include <matrix.h>
#include <settings.h>

class CharactersDialog : public QDialog, Ui::CharactersDialog
{
    Q_OBJECT

public:
    CharactersDialog(QWidget *parent = 0);

    void initalize(int column);

    MainWindow *mw;
    Matrix *matrix;
    Settings *settings;

    int selectedRow;
    int previousRow;
    int selectedStateRow;
    int previousStateRow;

    QString nextAvailableSymbol();
    void selectState(int row);

private:
    void initializeCharactersDialog();
    void initializeCharactersList();
    void updateCharactersDialog();
    void initializeStatesList();

    void updateButtons(int row);
    void updateCharactersTableColor(int row, bool isEnabled);
    void updateVerticalHeader();

    bool isModified;
    bool doNotSave;

    void saveData(int row);

    void selectCharacter(int row);
    QList<QTableWidgetItem*> getRow(int row);
    void setRow(int row, const QList<QTableWidgetItem*>& rowItems);
    void moveRow(bool up);

    QList<QVariant> availableSymbols();

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);

    void stateMergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void stateFontChanged(const QFont &f);
    void stateColorChanged(const QColor &c);

    QColor enabledColor;
    QColor disabledColor;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void buttonActions(QAbstractButton *button);
    void orderRadioActions(QAbstractButton *button);

    void actionSave();
    void actionUpArrow();
    void actionDownArrow();
    void actionAddCharacter();
    void actionDeleteCharacter();

    void textBold();
    void textUnderline();
    void textItalic();
    void textColor();
    void currentCharFormatChanged(const QTextCharFormat &format);

    void stateTextBold();
    void stateTextUnderline();
    void stateTextItalic();
    void stateTextColor();
    void stateCurrentCharFormatChanged(const QTextCharFormat &format);

    void updateTableSelectionChanged(const QModelIndex & current,const QModelIndex & previous);
    void updateTableCharactersName(QString text);
    void isEnabledChanged(bool isEnabled);

    void updateTableStateName(QString text);
};

#endif // CHARACTERSDIALOG_H

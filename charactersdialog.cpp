#include "charactersdialog.h"

CharactersDialog::CharactersDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
}

void CharactersDialog::initalize(int column = 0)
{
    selectedRow = 0;
    previousRow = 0;
    isModified = false;
    doNotSave = false;

    mw->logAppend("Characters Dialog","dialog opened.");

    enabledColor.setRgba(settings->getSetting("enabledColor").toUInt());
    disabledColor.setRgba(settings->getSetting("disabledColor").toUInt());

    fontChanged(notesTextEdit->font());

    // Color Selector
    QPixmap pix(14, 14);
    pix.fill(Qt::black);
    colorToolButton->setIcon(pix);
    stateColorToolButton->setIcon(pix);

    initializeCharactersDialog();

    // Set up actions
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonActions(QAbstractButton*)));

    // Character Action
    connect(boldToolButton, SIGNAL(clicked()), this, SLOT(textBold()));
    connect(italicToolButton, SIGNAL(clicked()), this, SLOT(textItalic()));
    connect(underlineToolButton, SIGNAL(clicked()), this, SLOT(textUnderline()));
    connect(colorToolButton, SIGNAL(clicked()), this, SLOT(textColor()));
    connect(notesTextEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(characterListTableWidget->selectionModel(), SIGNAL(currentChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(updateTableSelectionChanged(const QModelIndex &,const QModelIndex &)));
    connect(upToolButton, SIGNAL(clicked()),this, SLOT(actionUpArrow()));
    connect(downToolButton, SIGNAL(clicked()),this, SLOT(actionDownArrow()));
    connect(addPushButton, SIGNAL(clicked()),this, SLOT(actionAddCharacter()));
    connect(deletePushButton, SIGNAL(clicked()),this, SLOT(actionDeleteCharacter()));
    connect(isEnabledCheckBox, SIGNAL(toggled(bool)), this, SLOT(isEnabledChanged(bool)));

    // State Actions
    connect(orderButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(orderRadioActions(QAbstractButton*)));
    connect(stateBoldToolButton, SIGNAL(clicked()), this, SLOT(stateTextBold()));
    connect(stateItalicToolButton, SIGNAL(clicked()), this, SLOT(stateTextItalic()));
    connect(stateUnderlineToolButton, SIGNAL(clicked()), this, SLOT(stateTextUnderline()));
    connect(stateColorToolButton, SIGNAL(clicked()), this, SLOT(stateTextColor()));
    connect(stateNotesTextEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(stateCurrentCharFormatChanged(QTextCharFormat)));

    if(column != 0) {
        selectCharacter(column);
        updateCharactersDialog();
    }
}

void CharactersDialog::initializeCharactersDialog()
{
    nameLineEdit->setText("Undefined");
    nameLineEdit->setEnabled(false);
    notesTextEdit->setEnabled(false);
    isEnabledCheckBox->setEnabled(false);
    colorToolButton->setEnabled(false);
    boldToolButton->setEnabled(false);
    italicToolButton->setEnabled(false);
    underlineToolButton->setEnabled(false);

    stateSymbolLabel->setText("Undefined");
    stateNameLineEdit->setText("Undefined");
    stateNameLineEdit->setEnabled(false);
    stateNotesTextEdit->setEnabled(false);
    stateColorToolButton->setEnabled(false);
    stateBoldToolButton->setEnabled(false);
    stateItalicToolButton->setEnabled(false);
    stateUnderlineToolButton->setEnabled(false);

    unorderedRadioButton->setChecked(true);
    orderedRadioButton->setChecked(false);
    unorderedRadioButton->setEnabled(false);
    orderedRadioButton->setEnabled(false);

    initializeCharactersList();
    initializeStatesList();
}

void CharactersDialog::updateCharactersDialog()
{
    if (matrix->charactersCount() != 0) {
        disconnect(nameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(updateTableCharactersName(QString)));
        disconnect(stateNameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(updateTableStateName(QString)));

        // Enable Widgets
        isEnabledCheckBox->setEnabled(true);
        nameLineEdit->setEnabled(true);
        notesTextEdit->setEnabled(true);
        colorToolButton->setEnabled(true);
        boldToolButton->setEnabled(true);
        italicToolButton->setEnabled(true);
        underlineToolButton->setEnabled(true);
        unorderedRadioButton->setEnabled(true);
        orderedRadioButton->setEnabled(true);
        stateNameLineEdit->setEnabled(true);
        stateNotesTextEdit->setEnabled(true);
        stateColorToolButton->setEnabled(true);
        stateBoldToolButton->setEnabled(true);
        stateItalicToolButton->setEnabled(true);
        stateUnderlineToolButton->setEnabled(true);

        // Get Data from Matrix
        nameLineEdit->setText(matrix->characterList[selectedRow].getName());        
        notesTextEdit->setHtml(matrix->characterList[selectedRow].getNotes());        
        isEnabledCheckBox->setChecked(matrix->characterList[selectedRow].getIsEnabled());

        if (matrix->characterList[selectedRow].getIsOrdered()){
            unorderedRadioButton->setChecked(false);
            orderedRadioButton->setChecked(true);
        } else {
            unorderedRadioButton->setChecked(true);
            orderedRadioButton->setChecked(false);
        }
        stateSymbolLabel->setText(matrix->characterList[selectedRow].getState(selectedStateRow).getSymbol());
        stateNameLineEdit->setText(matrix->characterList[selectedRow].getState(selectedStateRow).getName());
        stateNotesTextEdit->setHtml(matrix->characterList[selectedRow].getState(selectedStateRow).getNotes());

        initializeStatesList();

        connect(nameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(updateTableCharactersName(QString)));
        connect(stateNameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(updateTableStateName(QString)));

        deletePushButton->setEnabled(true);
    } else {
        deletePushButton->setEnabled(false);
    }
}

void CharactersDialog::initializeCharactersList()
{
    int characterNumber = matrix->charactersCount();
    characterListTableWidget->setRowCount(1);
    if(characterNumber == 0) {
        //---- Set up default table text when there is no data
        characterListTableWidget->setColumnCount(1);
        characterListTableWidget->horizontalHeader()->hide();
        characterListTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        QTableWidgetItem *newItem = new QTableWidgetItem(tr("[You need to add a Character]"));
        newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        newItem->setForeground(Qt::gray);
        characterListTableWidget->setItem(0, 0, newItem);

        deletePushButton->setEnabled(false);
    } else {
        //---- Update the table view
        for(int i = 0; i < characterNumber; i++)
        {
            QString characterName = matrix->characterList[i].getName();
            bool isEnabled = matrix->characterList[i].getIsEnabled();
            //---- Add Row
            if(i != 0) {
                characterListTableWidget->insertRow(i);
            }
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(characterName));
            newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            characterListTableWidget->setItem(i, 0, newItem);

            newItem = new QTableWidgetItem(tr("C%1").arg(i+1));
            newItem->setFlags(Qt::ItemIsEnabled);
            characterListTableWidget->setVerticalHeaderItem(i,newItem);

            updateCharactersTableColor(i, isEnabled);
        }
        deletePushButton->setEnabled(true);
        selectCharacter(0);
    }
}

/*------------------------------------------------------------------------------------/
 * State List Table
 *-----------------------------------------------------------------------------------*/

void CharactersDialog::initializeStatesList()
{
    selectedStateRow = 0;
    previousStateRow = 0;

    int statesNumber = matrix->stateCount(selectedRow);
    statesListTableWidget->setRowCount(1);
    if(statesNumber == 0) {
        //---- Set up default table text when there is no data
        statesListTableWidget->setColumnCount(1);
        statesListTableWidget->horizontalHeader()->hide();
        statesListTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        QTableWidgetItem *newItem = new QTableWidgetItem(tr("[You need to add a State]"));
        newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        newItem->setForeground(Qt::gray);
        statesListTableWidget->setItem(0, 0, newItem);

        deleteStatePushButton->setEnabled(false);
    } else {
        //---- Update the table view
        for(int i = 0; i < statesNumber; i++)
        {
            State state = matrix->getState(selectedRow, i);
            //---- Add Row
            if(i != 0) {
                statesListTableWidget->insertRow(i);
            }
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(state.getName()));
            newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            statesListTableWidget->setItem(i, 0, newItem);

            newItem = new QTableWidgetItem(tr("%1").arg(state.getSymbol()));
            newItem->setFlags(Qt::ItemIsEnabled);
            statesListTableWidget->setVerticalHeaderItem(i,newItem);
        }
        deleteStatePushButton->setEnabled(true);
        selectState(0);
    }
}


/*------------------------------------------------------------------------------------/
 * Save Data Function
 *-----------------------------------------------------------------------------------*/

void CharactersDialog::saveData(int row)
{
    if (matrix->charactersCount() != 0 && !doNotSave){
        QString name = nameLineEdit->text();
        name = name.trimmed();
        if (name.size() == 0) {
            name = "Undefined";
        }

        matrix->characterList[row].setName(name);
        characterListTableWidget->item(row, 0)->setText(name);
        mw->updateCharacterDock();

        matrix->characterList[row].setNotes(notesTextEdit->toHtml());
        matrix->characterList[row].setIsEnabled(isEnabledCheckBox->isChecked());

        if (unorderedRadioButton->isChecked()) {
            matrix->characterList[row].setIsOrdered(false);
        } else {
            matrix->characterList[row].setIsOrdered(true);
        }

        mw->updateDataDock();
    } else {
        doNotSave = false;
    }
}

/*------------------------------------------------------------------------------------/
 * Select a Character Row Function
 *-----------------------------------------------------------------------------------*/

void CharactersDialog::selectCharacter(int row)
{
    // Select column
    if (matrix->charactersCount() != 0) {
        selectedRow = row;
        characterListTableWidget->selectionModel()->clearSelection();
        QModelIndex index = characterListTableWidget->model()->index(row, 0);
        characterListTableWidget->selectionModel()->select(index, QItemSelectionModel::Select);       
    }
}

void CharactersDialog::moveRow(bool up)
{
    int sourceRow = characterListTableWidget->row(characterListTableWidget->selectedItems().at(0));
    QString characterName = matrix->characterList[sourceRow].getName();
    int destRow = (up ? sourceRow-1 : sourceRow+1);

    // Take whole rows
    QList<QTableWidgetItem*> sourceItems = getRow(sourceRow);
    QList<QTableWidgetItem*> destItems = getRow(destRow);

    // Set back in reverse order
    setRow(sourceRow, destItems);
    setRow(destRow, sourceItems);

    // Update Table selection
    selectCharacter(destRow);

    if (up) {
        mw->logAppend("Matrix Edit",QString("moving character \"%1\" at position %2 up to %3.").arg(characterName).arg(sourceRow).arg(destRow));
    } else {
        mw->logAppend("Matrix Edit",QString("moving character \"%1\" at position %2 down to %3.").arg(characterName).arg(sourceRow).arg(destRow));
    }
}

// Takes and returns the whole row
QList<QTableWidgetItem*> CharactersDialog::getRow(int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int column = 0; column < characterListTableWidget->columnCount(); ++column)
    {
        rowItems << characterListTableWidget->takeItem(row, column);
    }
    return rowItems;
}

// Sets the whole row
void CharactersDialog::setRow(int row, const QList<QTableWidgetItem*>& rowItems)
{
    for (int column = 0; column < characterListTableWidget->columnCount(); ++column)
    {
        characterListTableWidget->setItem(row, column, rowItems.at(column));
    }
}

/*------------------------------------------------------------------------------------/
 * Select a State Row Function
 *-----------------------------------------------------------------------------------*/

void CharactersDialog::selectState(int row)
{
    // Select top row
    if (matrix->stateCount(selectedRow) != 0) {
        statesListTableWidget->selectionModel()->clearSelection();
        QModelIndex index = statesListTableWidget->model()->index(row,0);
        statesListTableWidget->selectionModel()->select(index, QItemSelectionModel::Select);
        selectedStateRow = row;
    }
}

/*------------------------------------------------------------------------------------/
 * Action Funtions
 *-----------------------------------------------------------------------------------*/

void CharactersDialog::buttonActions(QAbstractButton *button)
{
    if (button->text() == tr("OK")) {
        mw->logAppend("Characters Dialog","'OK' pressed, saving values to matrix and closing dialog...");
        actionSave();
    }
}

void CharactersDialog::updateButtons(int row)
{
    // Update buttons
    if (row == 0) {
        upToolButton->setEnabled(false);
    } else {
        upToolButton->setEnabled(true);
    }
    if (row == characterListTableWidget->rowCount()-1) {
        downToolButton->setEnabled(false);
    } else {
        downToolButton->setEnabled(true);
    }
}

void CharactersDialog::orderRadioActions(QAbstractButton *button)
{
    if (button->text() == tr("Unordered")) {
        mw->logAppend("Characters Dialog","'Unordered' pressed.");
        matrix->characterList[selectedRow].setIsOrdered(false);
    } else if (button->text() == tr("Ordered")) {
        mw->logAppend("Characters Dialog","'Ordered' pressed.");
        matrix->characterList[selectedRow].setIsOrdered(true);
    }
}

void CharactersDialog::closeEvent(QCloseEvent *event)
{
    // Save any change to last upadted dialog form
    saveData(selectedRow);

    mw->logAppend("Characters Dialog","dialog closed.");
    event->accept();
}

void CharactersDialog::actionSave()
{
    close();
}

void CharactersDialog::actionUpArrow()
{
    int currentSelectedRow = selectedRow; // === selected column
    if (currentSelectedRow != 0) {
        moveRow(true);
        matrix->moveColumn(currentSelectedRow, true);
        mw->moveCharacterDockTableRow(currentSelectedRow, true);
        matrix->characterList.move(currentSelectedRow,currentSelectedRow-1);
        updateButtons(currentSelectedRow-1);
    }
}

void CharactersDialog::actionDownArrow()
{
    int currentSelectedRow = selectedRow; // === selected column
    if (currentSelectedRow != characterListTableWidget->rowCount()-1) {
        moveRow(false);
        matrix->moveColumn(currentSelectedRow, false);
        mw->moveCharacterDockTableRow(currentSelectedRow, false);
        matrix->characterList.move(currentSelectedRow,currentSelectedRow+1);
        updateButtons(currentSelectedRow+1);
    }
}

void CharactersDialog::actionAddCharacter()
{
    addPushButton->setEnabled(false);

    matrix->characterAdd("temp","");
    int row = matrix->charactersCount()-1;

    QString characterName = QString("Character %1").arg(matrix->characterList[matrix->charactersCount()-1].getID()+1);
    matrix->characterList[row].setName(characterName);
    bool isEnabled = matrix->characterList[row].getIsEnabled();
    matrix->insertColumn(row);

    characterListTableWidget->insertRow(row); // === new column

    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(characterName));
    newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    characterListTableWidget->setItem(row, 0, newItem);

    newItem = new QTableWidgetItem(tr("C%1").arg(row+1));
    newItem->setFlags(Qt::ItemIsEnabled);
    characterListTableWidget->setVerticalHeaderItem(row, newItem);

    updateCharactersTableColor(row, isEnabled);
    selectCharacter(row);
    updateCharactersDialog();
    updateButtons(row);
    characterListTableWidget->scrollToBottom();

    mw->updateCharacterDock();
    mw->updateInformationDock();

    addPushButton->setEnabled(true);
}

void CharactersDialog::actionDeleteCharacter()
{
    deletePushButton->setEnabled(false);

    doNotSave = true;
    int removeAt = selectedRow; // === column to remove

    QString characterName = matrix->characterList[removeAt].getName();

    matrix->deleteColumn(removeAt);

    initializeCharactersDialog();
    mw->updateCharacterDock();
    mw->updateInformationDock();

    if (matrix->charactersCount() > 0) {
        if (removeAt == 0) {
            selectCharacter(0);
        } else {
            selectCharacter(removeAt-1);
            QModelIndex index = characterListTableWidget->model()->index(removeAt-1,0);
            characterListTableWidget->scrollTo(index);
        }
        updateCharactersDialog();
    }

    mw->logAppend("Matrix Edit",QString("deleting character \"%1\" at position %2.").arg(characterName).arg(removeAt));

    if (matrix->charactersCount() == 1) {
        deletePushButton->setEnabled(false);
        upToolButton->setEnabled(false);
        downToolButton->setEnabled(false);
        mw->logAppend("Matrix Edit","reached the minimum number of characters allowed.");
    } else {
        deletePushButton->setEnabled(true);
    }
}

void CharactersDialog::updateTableSelectionChanged(const QModelIndex & current,const QModelIndex & previous)
{
    selectedRow = current.row();
    if (previous.row() > -1) {
        previousRow = previous.row();

        // Save any changes back to the Matrix
        saveData(previousRow);
    }

    updateButtons(selectedRow);

    // Update Dialog Info
    updateCharactersDialog();
}

void CharactersDialog::updateTableCharactersName(QString text)
{
    characterListTableWidget->item(selectedRow, 0)->setText(text);
    mw->updateCharacterDockTableText(selectedRow, text);
}

void CharactersDialog::updateCharactersTableColor(int row, bool isEnabled)
{
    if (isEnabled){
        characterListTableWidget->item(row,0)->setForeground(QBrush(enabledColor));
    } else {
        characterListTableWidget->item(row,0)->setForeground(QBrush(disabledColor));
    }
}

void CharactersDialog::isEnabledChanged(bool isEnabled)
{
    matrix->characterList[selectedRow].setIsEnabled(isEnabled);
    updateCharactersTableColor(selectedRow, isEnabled);
    mw->updateCharacterDockTableColor(selectedRow, isEnabled);
}

void CharactersDialog::updateTableStateName(QString text)
{
    statesListTableWidget->item(selectedRow, 0)->setText(text);
    // Update Information Dock?...
}

/*------------------------------------------------------------------------------------/
 * Character Text/Font Editing Funtions
 *-----------------------------------------------------------------------------------*/

void CharactersDialog::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(boldToolButton->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
    mw->logAppend("Characters Dialog","'Bold' button pressed.");
}

void CharactersDialog::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(underlineToolButton->isChecked());
    mergeFormatOnWordOrSelection(fmt);
    mw->logAppend("Characters Dialog","'Underline' button pressed.");
}

void CharactersDialog::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(italicToolButton->isChecked());
    mergeFormatOnWordOrSelection(fmt);
    mw->logAppend("Characters Dialog","'Italic' button pressed.");
}

void CharactersDialog::textColor()
{
    QColor col = QColorDialog::getColor(notesTextEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void CharactersDialog::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = notesTextEdit->textCursor();
    cursor.mergeCharFormat(format);
    notesTextEdit->mergeCurrentCharFormat(format);
}

void CharactersDialog::fontChanged(const QFont &f)
{
    boldToolButton->setChecked(f.bold());
    italicToolButton->setChecked(f.italic());
    underlineToolButton->setChecked(f.underline());
}

void CharactersDialog::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void CharactersDialog::colorChanged(const QColor &c)
{
    QPixmap pix(14, 14);
    pix.fill(c);
    colorToolButton->setIcon(pix);
}

/*------------------------------------------------------------------------------------/
 * State Text/Font Editing Funtions
 *-----------------------------------------------------------------------------------*/

void CharactersDialog::stateTextBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(stateBoldToolButton->isChecked() ? QFont::Bold : QFont::Normal);
    stateMergeFormatOnWordOrSelection(fmt);
    mw->logAppend("Characters Dialog","'Bold' button pressed.");
}

void CharactersDialog::stateTextUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(stateUnderlineToolButton->isChecked());
    stateMergeFormatOnWordOrSelection(fmt);
    mw->logAppend("Characters Dialog","'Underline' button pressed.");
}

void CharactersDialog::stateTextItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(stateItalicToolButton->isChecked());
    stateMergeFormatOnWordOrSelection(fmt);
    mw->logAppend("Characters Dialog","'Italic' button pressed.");
}

void CharactersDialog::stateTextColor()
{
    QColor col = QColorDialog::getColor(stateNotesTextEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    stateMergeFormatOnWordOrSelection(fmt);
    stateColorChanged(col);
}

void CharactersDialog::stateMergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = stateNotesTextEdit->textCursor();
    cursor.mergeCharFormat(format);
    stateNotesTextEdit->mergeCurrentCharFormat(format);
}

void CharactersDialog::stateFontChanged(const QFont &f)
{
    stateBoldToolButton->setChecked(f.bold());
    stateItalicToolButton->setChecked(f.italic());
    stateUnderlineToolButton->setChecked(f.underline());
}

void CharactersDialog::stateCurrentCharFormatChanged(const QTextCharFormat &format)
{
    stateFontChanged(format.font());
    stateColorChanged(format.foreground().color());
}

void CharactersDialog::stateColorChanged(const QColor &c)
{
    QPixmap pix(14, 14);
    pix.fill(c);
    stateColorToolButton->setIcon(pix);
}

/*------------------------------------------------------------------------------------/
 * Symbols
 *-----------------------------------------------------------------------------------*/

QList<QVariant> CharactersDialog::availableSymbols()
{
    QList<QVariant> defaultStates = matrix->stateSetList;
    QList<QVariant> available;

    for(int i=0; i>defaultStates.count(); i++)
    {
        bool addToList = true;
        QString symbol = defaultStates[i].toString();

        for (int s=0; s>matrix->characterList[selectedRow].countStates(); s++)
        {
            if(matrix->characterList[selectedRow].getState(s).getSymbol() == symbol){
                addToList = false;
            }
        }

        if (addToList) {
            available[i] = symbol;
        }
    }

    return available;
}

QString CharactersDialog::nextAvailableSymbol()
{
    QList<QVariant> available = availableSymbols();
    return available[0].toString();
}

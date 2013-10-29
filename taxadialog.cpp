#include "taxadialog.h"

TaxaDialog::TaxaDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
}

/*------------------------------------------------------------------------------------/
 * Initialization/Update Funtions
 *-----------------------------------------------------------------------------------*/

void TaxaDialog::initalize(int row = 0)
{
    selectedRow = 0;
    previousRow = 0;
    isModified = false;
    doNotSave = false;

    mw->logAppend("Taxa Dialog","dialog opened.");

    enabledColor.setRgba(settings->getSetting("enabledColor").toUInt());
    disabledColor.setRgba(settings->getSetting("disabledColor").toUInt());

    fontChanged(notesTextEdit->font());

    initializeTaxaDialog();

    // Color Selector
    QPixmap pix(14, 14);
    pix.fill(Qt::black);
    colorToolButton->setIcon(pix);

    // Set up actions
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonActions(QAbstractButton*)));
    connect(boldToolButton, SIGNAL(clicked()), this, SLOT(textBold()));
    connect(italicToolButton, SIGNAL(clicked()), this, SLOT(textItalic()));
    connect(underlineToolButton, SIGNAL(clicked()), this, SLOT(textUnderline()));
    connect(colorToolButton, SIGNAL(clicked()), this, SLOT(textColor()));
    connect(notesTextEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(taxonListTableWidget->selectionModel(), SIGNAL(currentChanged(const QModelIndex &,const QModelIndex &)), this, SLOT(updateTableSelectionChanged(const QModelIndex &,const QModelIndex &)));
    connect(upToolButton, SIGNAL(clicked()),this, SLOT(actionUpArrow()));
    connect(downToolButton, SIGNAL(clicked()),this, SLOT(actionDownArrow()));
    connect(addPushButton, SIGNAL(clicked()),this, SLOT(actionAddTaxon()));
    connect(deletePushButton, SIGNAL(clicked()),this, SLOT(actionDeleteTaxon()));
    connect(isEnabledCheckBox, SIGNAL(toggled(bool)), this, SLOT(isEnabledChanged(bool)));

    if(row != 0) {
        selectTaxon(row);
        updateTaxaDialog();
    }
}

void TaxaDialog::initializeTaxaDialog()
{
    nameLineEdit->setText("Undefined");
    nameLineEdit->setEnabled(false);
    notesTextEdit->setEnabled(false);
    isEnabledCheckBox->setEnabled(false);
    colorToolButton->setEnabled(false);
    boldToolButton->setEnabled(false);
    italicToolButton->setEnabled(false);
    underlineToolButton->setEnabled(false);

    initializeTaxonList();
}

void TaxaDialog::updateTaxaDialog()
{
    if (matrix->taxaCount() != 0) {
        disconnect(nameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(updateTableTaxonName(QString)));

        // Get Data from Matrix
        nameLineEdit->setText(matrix->taxonList[selectedRow].getName());
        nameLineEdit->setEnabled(true);

        notesTextEdit->setHtml(matrix->taxonList[selectedRow].getNotes());
        notesTextEdit->setEnabled(true);

        isEnabledCheckBox->setChecked(matrix->taxonList[selectedRow].getIsEnabled());
        isEnabledCheckBox->setEnabled(true);

        colorToolButton->setEnabled(true);
        boldToolButton->setEnabled(true);
        italicToolButton->setEnabled(true);
        underlineToolButton->setEnabled(true);

        connect(nameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(updateTableTaxonName(QString)));

        deletePushButton->setEnabled(true);
    } else {
        deletePushButton->setEnabled(false);
    }
}

void TaxaDialog::initializeTaxonList()
{
    int taxaNumber = matrix->taxaCount();
    taxonListTableWidget->setRowCount(1);
    if(taxaNumber == 0) {
        //---- Set up default table text when there is no data
        taxonListTableWidget->setColumnCount(1);
        taxonListTableWidget->horizontalHeader()->hide();
        taxonListTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        QTableWidgetItem *newItem = new QTableWidgetItem(tr("[You need to add a Taxon]"));
        newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        newItem->setForeground(Qt::gray);
        taxonListTableWidget->setItem(0, 0, newItem);

        deletePushButton->setEnabled(false);
    } else {
        //---- Update the table view
        for(int i = 0; i < taxaNumber; i++)
        {
            QString taxonName = matrix->taxonList[i].getName();
            bool isEnabled = matrix->taxonList[i].getIsEnabled();
            //---- Add Row
            if(i != 0) {
                taxonListTableWidget->insertRow(i);
            }
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(taxonName));
            newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            taxonListTableWidget->setItem(i, 0, newItem);

            newItem = new QTableWidgetItem(tr("T%1").arg(i+1));
            newItem->setFlags(Qt::ItemIsEnabled);
            taxonListTableWidget->setVerticalHeaderItem(i,newItem);

            updateTaxaTableColor(i, isEnabled);
        }
        deletePushButton->setEnabled(true);
        selectTaxon(0);
    }
}

/*------------------------------------------------------------------------------------/
 * Save Data Function
 *-----------------------------------------------------------------------------------*/

void TaxaDialog::saveData(int row)
{
    if (matrix->taxaCount() != 0 && !doNotSave){
        QString name = nameLineEdit->text();
        name = name.trimmed();
        if (name.size() == 0) {
            name = "Undefined";
        }

        matrix->taxonList[row].setName(name);
        taxonListTableWidget->item(row, 0)->setText(name);
        mw->updateTaxaDock();

        matrix->taxonList[row].setNotes(notesTextEdit->toHtml());
        matrix->taxonList[row].setIsEnabled(isEnabledCheckBox->isChecked());

        mw->updateDataDock();
    } else {
        doNotSave = false;
    }
}

/*------------------------------------------------------------------------------------/
 * Select a Row Function
 *-----------------------------------------------------------------------------------*/

void TaxaDialog::selectTaxon(int row)
{
    // Select row
    if (matrix->taxaCount() != 0) {
        selectedRow = row;
        taxonListTableWidget->selectionModel()->clearSelection();
        QModelIndex index = taxonListTableWidget->model()->index(row, 0);
        taxonListTableWidget->selectionModel()->select(index, QItemSelectionModel::Select);       
    }
}

void TaxaDialog::moveRow(bool up)
{
    int sourceRow = taxonListTableWidget->row(taxonListTableWidget->selectedItems().at(0));
    QString taxonName = matrix->taxonList[sourceRow].getName();
    int destRow = (up ? sourceRow-1 : sourceRow+1);

    // Take whole rows
    QList<QTableWidgetItem*> sourceItems = getRow(sourceRow);
    QList<QTableWidgetItem*> destItems = getRow(destRow);

    // Set back in reverse order
    setRow(sourceRow, destItems);
    setRow(destRow, sourceItems);

    // Update Table selection
    selectTaxon(destRow);

    if (up) {
        mw->logAppend("Matrix Edit",QString("moving taxon \"%1\" at position %2 up to %3.").arg(taxonName).arg(sourceRow).arg(destRow));
    } else {
        mw->logAppend("Matrix Edit",QString("moving taxon \"%1\" at position %2 down to %3.").arg(taxonName).arg(sourceRow).arg(destRow));
    }
}

// Takes and returns the whole row
QList<QTableWidgetItem*> TaxaDialog::getRow(int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int column = 0; column < taxonListTableWidget->columnCount(); ++column)
    {
        rowItems << taxonListTableWidget->takeItem(row, column);
    }
    return rowItems;
}

// Sets the whole row
void TaxaDialog::setRow(int row, const QList<QTableWidgetItem*>& rowItems)
{
    for (int column = 0; column < taxonListTableWidget->columnCount(); ++column)
    {
        taxonListTableWidget->setItem(row, column, rowItems.at(column));
    }
}

/*------------------------------------------------------------------------------------/
 * Action Funtions
 *-----------------------------------------------------------------------------------*/

void TaxaDialog::buttonActions(QAbstractButton *button)
{
    if (button->text() == tr("OK")) {
        mw->logAppend("Taxa Dialog","'OK' pressed, saving values to matrix and closing dialog...");
        actionSave();
    }
}

void TaxaDialog::actionSave()
{
    close();
}


void TaxaDialog::closeEvent(QCloseEvent *event)
{
    // Save any change to last upadted dialog form
    saveData(selectedRow);

    mw->logAppend("Taxa Dialog","dialog closed.");
    event->accept();
}

void TaxaDialog::actionUpArrow()
{
    int currentSelectedRow = selectedRow;
    if (currentSelectedRow != 0) {
        moveRow(true);
        matrix->moveRow(currentSelectedRow, true);
        mw->moveTaxaDockTableRow(currentSelectedRow, true);
        matrix->taxonList.move(currentSelectedRow,currentSelectedRow-1);
        updateButtons(currentSelectedRow-1);
    }
}

void TaxaDialog::actionDownArrow()
{
    int currentSelectedRow = selectedRow;
    if (currentSelectedRow != taxonListTableWidget->rowCount()-1) {
        moveRow(false);
        matrix->moveRow(currentSelectedRow, false);
        mw->moveTaxaDockTableRow(currentSelectedRow, false);
        matrix->taxonList.move(currentSelectedRow,currentSelectedRow+1);
        updateButtons(currentSelectedRow+1);
    }
}

void TaxaDialog::actionAddTaxon()
{
    addPushButton->setEnabled(false);

    matrix->taxonAdd("temp","");
    QString taxonName = QString("Taxon %1").arg(matrix->taxonList[matrix->taxaCount()-1].getID()+1);
    matrix->taxonList[matrix->taxaCount()-1].setName(taxonName);

    int row = matrix->taxaCount()-1;
    bool isEnabled = matrix->taxonList[row].getIsEnabled();

    taxonListTableWidget->insertRow(row);
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(taxonName));
    newItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    taxonListTableWidget->setItem(row, 0, newItem);

    newItem = new QTableWidgetItem(tr("T%1").arg(row+1));
    newItem->setFlags(Qt::ItemIsEnabled);
    taxonListTableWidget->setVerticalHeaderItem(row, newItem);

    updateTaxaTableColor(row, isEnabled);
    selectTaxon(row);
    updateTaxaDialog();
    updateButtons(row);
    taxonListTableWidget->scrollToBottom();

    matrix->insertRow(row);

    mw->updateTaxaDock();
    mw->updateInformationDock();

    addPushButton->setEnabled(true);
}

void TaxaDialog::actionDeleteTaxon()
{   
    deletePushButton->setEnabled(false);

    doNotSave = true;
    int row = selectedRow;
    QString taxonName = matrix->taxonList[row].getName();

    matrix->deleteRow(row);

    initializeTaxaDialog();
    mw->updateTaxaDock();
    mw->updateInformationDock();

    if (matrix->taxaCount() > 0) {
        if (row == 0) {
            selectTaxon(0);
        } else {
            selectTaxon(row-1);
        }
        updateTaxaDialog();
    }

    mw->logAppend("Matrix Edit",QString("deleting taxon \"%1\" at position %2.").arg(taxonName).arg(row));

    if (matrix->taxaCount() == 1) {
        deletePushButton->setEnabled(false);
        upToolButton->setEnabled(false);
        downToolButton->setEnabled(false);
        mw->logAppend("Matrix Edit","reached the minimum number of taxa allowed.");
    } else {
        deletePushButton->setEnabled(true);
    }
}

void TaxaDialog::updateTableSelectionChanged(const QModelIndex & current,const QModelIndex & previous)
{
    selectedRow = current.row();
    if (previous.row() > -1) {
        previousRow = previous.row();

        // Save any changes back to the Matrix
        saveData(previousRow);
    }

    updateButtons(selectedRow);

    // Update Dialog Info
    updateTaxaDialog();
}

void TaxaDialog::updateButtons(int row)
{
    // Update buttons
    if (row == 0) {
        upToolButton->setEnabled(false);
    } else {
        upToolButton->setEnabled(true);
    }
    if (row == taxonListTableWidget->rowCount()-1) {
        downToolButton->setEnabled(false);
    } else {
        downToolButton->setEnabled(true);
    }
}

void TaxaDialog::updateTableTaxonName(QString text)
{
    taxonListTableWidget->item(selectedRow, 0)->setText(text);
    mw->updateTaxaDockTableText(selectedRow, text);
    matrix->updateLeftTableText(selectedRow, text);
}

void TaxaDialog::isEnabledChanged(bool isEnabled)
{
    matrix->taxonList[selectedRow].setIsEnabled(isEnabled);
    updateTaxaTableColor(selectedRow, isEnabled);
    mw->updateTaxaDockTableColor(selectedRow, isEnabled);
}

void TaxaDialog::updateTaxaTableColor(int row, bool isEnabled)
{
    if (isEnabled){
        taxonListTableWidget->item(row, 0)->setForeground(QBrush(enabledColor));
    } else {
        taxonListTableWidget->item(row, 0)->setForeground(QBrush(disabledColor));
    }
}

/*------------------------------------------------------------------------------------/
 * Text/Font Editing Funtions
 *-----------------------------------------------------------------------------------*/

void TaxaDialog::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(boldToolButton->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
    mw->logAppend("Taxa Dialog","'Bold' button pressed.");
}

void TaxaDialog::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(underlineToolButton->isChecked());
    mergeFormatOnWordOrSelection(fmt);
    mw->logAppend("Taxa Dialog","'Underline' button pressed.");
}

void TaxaDialog::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(italicToolButton->isChecked());
    mergeFormatOnWordOrSelection(fmt);
    mw->logAppend("Taxa Dialog","'Italic' button pressed.");
}

void TaxaDialog::textColor()
{
    QColor col = QColorDialog::getColor(notesTextEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void TaxaDialog::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = notesTextEdit->textCursor();
    cursor.mergeCharFormat(format);
    notesTextEdit->mergeCurrentCharFormat(format);
}

void TaxaDialog::fontChanged(const QFont &f)
{
    boldToolButton->setChecked(f.bold());
    italicToolButton->setChecked(f.italic());
    underlineToolButton->setChecked(f.underline());
}

void TaxaDialog::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
    colorChanged(format.foreground().color());
}

void TaxaDialog::colorChanged(const QColor &c)
{
    QPixmap pix(14, 14);
    pix.fill(c);
    colorToolButton->setIcon(pix);
}

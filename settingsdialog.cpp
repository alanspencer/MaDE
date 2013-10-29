#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
}

void SettingsDialog::initalize()
{
        mw->logAppend("Settings Dialog","dialog opened.");

        loadValues();

        // Set up actions
        connect(this->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonActions(QAbstractButton*)));
}


void SettingsDialog::buttonActions(QAbstractButton *button)
{
    if (button->text() == tr("Restore Defaults")){
        mw->logAppend("Settings Dialog","'Restore Defaults' pressed, restoring .ini setting values to application defaults.");
        actionRestoreDefaults();
    } else if (button->text() == tr("OK")) {
        mw->logAppend("Settings Dialog","'OK' pressed, saving new setting values to .ini file and closing dialog...");
        actionSave();
    } else if (button->text() == tr("Cancel")){
        mw->logAppend("Settings Dialog","'Cancel' pressed, closing dialog without saving...");
        actionClose();
    }
}

void SettingsDialog::actionRestoreDefaults()
{
    settings->setDefaultSettings();
    loadValues();
}

void SettingsDialog::actionSave()
{
    settings->setSetting("defaultUnknownCharacter",this->unknownCharacterLineEdit->text());
    settings->setSetting("defaultName",this->defaultNameLineEdit->text());
    settings->setSetting("defaultNumberTaxa",this->numberTaxaSpinBox->value());
    settings->setSetting("defaultNumberCharacter",this->numberCharactersSpinBox->value());

    actionClose();
}

void SettingsDialog::actionClose()
{
    mw->logAppend("Settings Dialog","dialog closed.");
    this->close();
}

void SettingsDialog::loadValues()
{
    // Get values from settings
    this->unknownCharacterLineEdit->setText(settings->getSetting("defaultUnknownCharacter").toString());
    this->defaultNameLineEdit->setText(settings->getSetting("defaultName").toString());
    this->numberTaxaSpinBox->setValue(settings->getSetting("defaultNumberTaxa").toInt());
    this->numberCharactersSpinBox->setValue(settings->getSetting("defaultNumberCharacter").toInt());
}
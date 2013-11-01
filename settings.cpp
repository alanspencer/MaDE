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

#include "settings.h"

//-- Contructor:
Settings::Settings()
{
    defaultSettingsList.insert("appVersion",QString::number(APP_VERSION, 'f', 1));

    defaultSettingsList.insert("defaultNumberTaxa","10");
    defaultSettingsList.insert("defaultNumberCharacter","10");
    defaultSettingsList.insert("defaultName","Undefined");
    defaultSettingsList.insert("defaultUnknownCharacter","?");
    defaultSettingsList.insert("defaultGapCharacter","-");
    defaultSettingsList.insert("defaultDisallowedCharacters",
                               QStringList() << "(" << ")" << "[" << "]" << "{" << "}" << "/" << "\\" << "," << ";" << ":"
                               << "=" << "*" << "\"" << "'" << "`" << "<" << ">" << "~");
    defaultSettingsList.insert("defaultMatrixTypes",QStringList() << "STANDARD" << "DNA" << "RNA" << "NUCLEOTIDE" << "PROTINE");
    defaultSettingsList.insert("defaultStandardStateSet",
                               QStringList() << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9"
                               << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" << "I" << "J" << "K" << "L"
                               << "M" << "N" << "O" << "P" << "Q" << "R" << "S" << "T" << "U" << "V" << "W" << "X"
                               << "Y" << "Z");

    //----- Not Currently Used....
    defaultSettingsList.insert("defaultDNAStateSet",QStringList() << "A" << "C" << "G" << "T");
    defaultSettingsList.insert("defaultRNAStateSet",QStringList() << "A" << "C" << "G" << "U");
    defaultSettingsList.insert("defaultNucleotideStateSet",QStringList() << "A" << "C" << "G" << "T");
    defaultSettingsList.insert("defaultRNAStateSet",
                               QStringList() << "A" << "C" << "D" << "E" << "F" << "G" << "H" << "I" << "K" << "L"
                               << "M" << "N" << "P" << "Q" << "R" << "S" << "T" << "V" << "W" << "Y" << "*");
    defaultSettingsList.insert("defaultRNAStateNames",
                               QStringList() << "ala [alanine]" << "cys [cysteien]" << "asp [aspartic acid]" << "glu [glutamic acid]"
                               << "phe [phenylalanine]" << "gly [glycine]" << "his [histidne]" << "ile [isoleucine]" << "lys [lysine]"
                               << "leu [leucine]" << "met [methionine]" << "asn [asparagine]" << "pro [proline]" << "gln [glutamine]"
                               << "[arg arginie]" << "ser [serine]" << "thr [threonine]" << "val [valine]" << "trp [tryptophan]"
                               << "try [tyrosine]" << "stop [chain termination]");
    defaultSettingsList.insert("enabledColor",QColor(0,153,0).rgba());
    defaultSettingsList.insert("disabledColor",QColor(153,0,0).rgba());
}

void Settings::initialize()
{
    mw->logAppend("Application Settings","retrieving application settings.");
    // Create default settings
    if (isFileEmpty() == true)
    {
        setDefaultSettings();
    }
    mw->logAppend("Application Settings","finished loading all application settings.");
}

bool Settings::isFileEmpty()
{
    QSettings settingsFile("settings.ini", QSettings::IniFormat);
    if(settingsFile.contains("appVersion") == true)
    {       
        mw->logAppend("Application Settings","settings.ini found.");
        return false;
    } else {        
        mw->logAppend("Application Settings","settings.ini not found.");
        return true;
    }
}

void Settings::setDefaultSettings()
{
    mw->logAppend("Application Settings","creating/restoring default .ini file.");
    QSettings settingsFile("settings.ini", QSettings::IniFormat);

    QHash<QString, QVariant>::const_iterator i;
    for (i = defaultSettingsList.constBegin(); i != defaultSettingsList.constEnd(); ++i)
        settingsFile.setValue(i.key(), i.value());
}

void Settings::setSetting(QString key, QVariant value)
{
    QSettings settingsFile("settings.ini", QSettings::IniFormat);
    settingsFile.setValue(key, value);
}

QVariant Settings::getSetting(QString key)
{
    QSettings settingsFile("settings.ini", QSettings::IniFormat);
    return settingsFile.value(key);
}

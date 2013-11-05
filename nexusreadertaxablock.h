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

/*------------------------------------------------------------------------------------------------------
 * Modified from work by Paul O. Lewis and Mark T. Holder. Available from http://sourceforge.net/p/ncl/
 * under a GNU General Public License version 2.0 (GPLv2) Licence.
 *
 * Nexus Class Libray (NCL)
 *
 * Copyright (c) 2003-2012, Paul O. Lewis
 * Copyright (c) 2007-2012, Mark T. Holder
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *-----------------------------------------------------------------------------------------------------*/

#ifndef NEXUSREADERTAXABLOCK_H
#define NEXUSREADERTAXABLOCK_H

#include <QWidget>

#include "nexusreader.h"

class NexusReaderBlock;
class NexusReaderException;

class NexusReaderTaxaBlock : public NexusReaderBlock
{
public:
    NexusReaderTaxaBlock();
    virtual ~NexusReaderTaxaBlock();

    virtual int addTaxonLabel(QString taxonLabel);
    virtual QMap<QString,QVariant> getData();
    virtual void reset();

protected:
    virtual void read(NexusReaderToken *&token);

    int taxaNumber; // == ntax, number of taxa found
    QList<QVariant> taxonLabels; // storage for list of taxon labels
    QList<bool> needsQuotes; // needsQuotes[i] true if label i needs to be quoted when output
};

#endif // NEXUSREADERTAXABLOCK_H

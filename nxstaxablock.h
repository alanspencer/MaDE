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

#ifndef NXSTAXABLOCK_H
#define NXSTAXABLOCK_H

#include<QtWidgets>

class NxsReader;
class NxsBlock;
class NxsException;
class Taxon;

class NxsTaxaBlock : public NxsBlock
{
public:
    NxsTaxaBlock(NxsReader *pointer);
    virtual ~NxsTaxaBlock();
    virtual void reset();

    int taxonAdd(QString taxonLabel);
    QList<Taxon> getTaxonList();
    int getNumTaxonLabels();
    int taxonFind(QString & str);
    int taxonIDFind(QString & str);

    class NxsX_NoSuchTaxon {};	// thrown if findTaxon cannot locate a supplied taxon label in the taxonLabels vector

protected:
    virtual void read(NxsToken &token);
    int handleDimensions(NxsToken &token, QString ntaxLabel);

    int nextTaxonID;
    QList<Taxon> taxonList;

    int ntax; // == ntax, number of taxa found
};

#endif // NXSTAXABLOCK_H

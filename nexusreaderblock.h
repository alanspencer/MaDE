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

#ifndef NEXUSREADERBLOCK_H
#define NEXUSREADERBLOCK_H

#include <QWidget>

#include "nexusreader.h"

class NexusReader;

class NexusReaderBlock
{

friend class NexusReader;

public:
    NexusReaderBlock();

    QString getID();
    void setNexusReader(NexusReader *pointer);

    enum NexusCommandResult
    {
        STOP_PARSING_BLOCK,
        HANDLED_COMMAND,
        UNKNOWN_COMMAND
    };

    QString errorMessage;

private:
    NexusReader *nexusReader;
    QString blockID;

};

#endif // NEXUSREADERBLOCK_H

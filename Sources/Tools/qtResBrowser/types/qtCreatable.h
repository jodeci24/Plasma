/*==LICENSE==*

CyanWorlds.com Engine - MMOG client, server and tools
Copyright (C) 2011  Cyan Worlds, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Additional permissions under GNU GPL version 3 section 7

If you modify this Program, or any covered work, by linking or
combining it with any of RAD Game Tools Bink SDK, Autodesk 3ds Max SDK,
NVIDIA PhysX SDK, Microsoft DirectX SDK, OpenSSL library, Independent
JPEG Group JPEG library, Microsoft Windows Media SDK, or Apple QuickTime SDK
(or a modified version of those libraries),
containing parts covered by the terms of the Bink SDK EULA, 3ds Max EULA,
PhysX SDK EULA, DirectX SDK EULA, OpenSSL and SSLeay licenses, IJG
JPEG Library README, Windows Media SDK EULA, or QuickTime SDK EULA, the
licensors of this Program grant you additional
permission to convey the resulting work. Corresponding Source for a
non-source form of such a combination shall include the source code for
the parts of OpenSSL and IJG JPEG Library used as well as that of the covered
work.

You can contact Cyan Worlds, Inc. by email legal@cyan.com
 or by snail mail at:
      Cyan Worlds, Inc.
      14617 N Newport Hwy
      Mead, WA   99021

*==LICENSE==*/

/* This file is based heavily on PlasmaShop's PrpShop. */
#ifndef _qtCreatable_inc_
#define _qtCreatable_inc_

#include <QWidget>
#include "qtPlasma.h"

class plCreatable;
class plLocation;
class plCreatableIndex;

#define kNullCreatableIndex 0x8000

// Pull in all of plCreatableIndex :(
#include "plClassIndexMacros.h"
#include "plCreatableIndex.h"

class qtCreatable : public QWidget {
    Q_OBJECT

protected:
    plCreatable*    fCreatable;
    uint16_t        fForceType;

public:
    qtCreatable(plCreatable* pCre, uint16_t type, QWidget* parent = nullptr);
    virtual ~qtCreatable() { }

    bool isMatch(plCreatable* pCre, uint16_t type);
    bool compareLocation(const plLocation& loc);
    //virtual void saveDamage() = 0;

protected:
    virtual void closeEvent(QCloseEvent* evt);
};

qtCreatable* qtMakeCreatableForm(plCreatable* pCre, QWidget* parent, uint16_t forceType = kNullCreatableIndex);

#endif //_qtCreatable_inc_

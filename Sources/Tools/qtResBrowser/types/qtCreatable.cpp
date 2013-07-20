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
#include "qtCreatable.h"
#include "pnKeyedObject/hsKeyedObject.h"
#include "pnKeyedObject/plUoid.h"

qtCreatable::qtCreatable(plCreatable* pCre, uint16_t type, QWidget* parent)
    : QWidget(parent), fCreatable(pCre), fForceType(type)
{
    setAttribute(Qt::WA_DeleteOnClose);

    plString title = plString::Null;
    hsKeyedObject* ko = hsKeyedObject::ConvertNoRef(pCre);
    if (ko != nullptr) {
        title = plString::Format("%s: %s", plFactory::GetNameOfClass(type), ko->GetKey()->GetName().c_str());
    } else {
        title = plFactory::GetNameOfClass(type);
    }
    setWindowTitle(~title);
}

bool qtCreatable::isMatch(plCreatable* pCre, uint16_t type)
{
    if (fCreatable == nullptr)
        return false;

    if (type == kNullCreatableIndex)
        type = pCre->ClassIndex();

    return (fCreatable == pCre) && (fForceType == type);
}

bool qtCreatable::compareLocation(const plLocation& loc)
{
    if (fCreatable == nullptr)
        return false;

    hsKeyedObject* ko = hsKeyedObject::ConvertNoRef(fCreatable);
    if (ko == nullptr)
        return false;

    return ko->GetKey()->GetUoid().GetLocation() == loc;
}

void qtCreatable::closeEvent(QCloseEvent*)
{
    //saveDamage();

    delete fCreatable;
    fCreatable = nullptr;
}

#include "qtMipmap.h"

qtCreatable* qtMakeCreatableForm(plCreatable* pCre, QWidget* parent, uint16_t forceType)
{
    uint16_t type = (forceType == kNullCreatableIndex) ? pCre->ClassIndex() : forceType;

    switch (type) {
        case CLASS_INDEX_SCOPED(plMipmap):
            return new qtMipmap(pCre, parent);

        default:
            delete pCre;
            return nullptr;
    }
}

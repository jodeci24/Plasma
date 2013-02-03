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

#ifndef _qtTreeIterator_inc_
#define _qtTreeIterator_inc_

#include "qtPlasma.h"
#include "qtTreeItem.h"

#include "plResMgr/plResManager.h"
#include "plResMgr/plRegistryHelpers.h"
#include "plResMgr/plRegistryNode.h"
#include "plResMgr/plPageInfo.h"
#include "pnKeyedObject/plUoid.h"
#include "pnKeyedObject/plKey.h"
#include "pnKeyedObject/plKeyImp.h"
#include "pnFactory/plFactory.h"

class qtTreeIterator : public plRegistryPageIterator, public plRegistryKeyIterator
{
private:
    QHash<uint32_t, qtTreeItem*> fFolders;
    QTreeWidget* fWidget;
    qtTreeItem* fRoot;

public:
    qtTreeIterator(QTreeWidget* widget) : fWidget(widget), fRoot(nullptr) { }
    qtTreeIterator(qtTreeItem* root) : fWidget(nullptr), fRoot(root) { }

    virtual bool EatPage(plRegistryPageNode* page)
    {
        // See if the page is already loaded -- return that if so
        qtTreeItem* parent = nullptr;
        qtTreeItem* item = fFolders.value(page->GetPageInfo().GetLocation().GetSequenceNumber(), nullptr);
        if (item != nullptr) {
            item->setFilename(page->GetPagePath());
            return true;
        }

        // Find or create the Age folder
        QString ageName = ~page->GetPageInfo().GetAge();
        for (int i = 0; i < fWidget->topLevelItemCount(); i++) {
            if (fWidget->topLevelItem(i)->text(0) == ageName) {
                parent = static_cast<qtTreeItem*>(fWidget->topLevelItem(i));
                break;
            }
        }
        if (parent == NULL)
            parent = new qtTreeItem(fWidget, ageName);

        // And now the Page entry
        item = new qtTreeItem(parent, page);

        qtTreeIterator iter(item);
        page->LoadKeys();
        page->IterateKeys(&iter);

        item->setFilename(page->GetPagePath());

        fFolders[page->GetPageInfo().GetLocation().GetSequenceNumber()] = item;
        return true;
    }

    virtual bool EatKey(const plKey& key)
    {
        uint16_t type = key->GetUoid().GetClassType();

        qtTreeItem* folder = fFolders.value(type, nullptr);
        if (folder == nullptr) {
            folder = new qtTreeItem(fRoot);
            folder->setText(0, plFactory::GetNameOfClass(type));

            fFolders[type] = folder;
        }

        qtTreeItem* item = new qtTreeItem(folder, key);

        return true;
    }
};

#endif // _qtTreeIterator_inc_

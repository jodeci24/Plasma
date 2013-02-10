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
#ifndef _qtResBrowser_inc_
#define _qtResBrowser_inc_

#include <QMainWindow>
#include <QMdiArea>
#include <QTreeWidget>
#include <QDockWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QAction>

#include "qtTreeItem.h"
#include "qtTreeIterator.h"
#include "plResMgr/plResManager.h"
#include "pnKeyedObject/plUoid.h"

class plRegistryPageNode;

class qtResBrowser : public QMainWindow
{
    Q_OBJECT

private:
    QString         fDialogDir;
    QMdiArea*       fMdiArea;
    QDockWidget*    fBrowserDock;
    QTreeWidget*    fBrowserTree;
    QDockWidget*    fPropertyDock;
    QWidget*        fPropertyContainer;

    // Property Panel stuff
    enum PropWhich {
        kPropsNone,
        kPropsAge,
        kPropsPage,
        kPropsKO
    };
    QLineEdit*      fAgeName;
    QLineEdit*      fPageName;
    QSpinBox*       fReleaseVersion;
    QLineEdit*      fSeqPrefix;
    QLineEdit*      fObjName;
    QLabel*         fObjType;
    QSpinBox*       fLoadMaskQ[2];
    QGroupBox*      fCloneIdBox;
    QSpinBox*       fCloneId;
    QSpinBox*       fClonePlayerId;

    enum {
        kLocLocalOnly,
        kLocVolatile,
        kLocItinerant,
        kLocReserved,
        kLocBuiltIn,
        kLocNumFlags
    };
    QCheckBox*      fLocationFlags[kLocNumFlags];

    // Menu actions
    enum {
        // Main Menu
        kFileNewPage, kFileOpen, kFileSave, kFileSaveAs, kFileExit,
        kToolsProperties, kToolsNewObject, kWindowPrev, kWindowNext,
        kWindowTile, kWindowCascade, kWindowClose, kWindowCloseAll,

        // Tree Context Menu
        kTreeClose, kTreeEdit, kTreeEditPRC, kTreePreview, kTreeDelete,
        kTreeImport, kTreeExport,

        kNumActions
    };
    QAction* fActions[kNumActions];

    // Plasma stuff
    qtTreeIterator* fTreeIter;

    // Magic for Creatable loading
    static qtResBrowser* sInstance;

public:
    static qtResBrowser* Instance();

    qtResBrowser();
    void setPropertyPage(PropWhich which);
    void loadFile(QString filename);
//    void saveFile(plPageInfo* page, QString filename);
//    void saveProps(QPlasmaTreeItem* item);
//    void editCreatable(plCreatable* pCre, short forceType = -1);

protected:
    virtual void closeEvent(QCloseEvent* evt);
    virtual void dragEnterEvent(QDragEnterEvent* evt);
    virtual void dropEvent(QDropEvent* evt);
//    QPlasmaTreeItem* findCurrentPageItem(bool isSave = false);
//    QPlasmaTreeItem* ensurePath(const plLocation& loc, short objType);

public slots:
//    void newPage();
    void openFiles();
//    void performSave();
//    void performSaveAs();
    void treeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
//    void treeItemActivated(QTreeWidgetItem* item, int column);
    void treeContextMenu(const QPoint& pos);
//    void createNewObject();

//    void treeClose();
//    void treeEdit();
//    void treeEditPRC();
//    void treePreview();
//    void treeDelete();
//    void treeImport();
    void treeExport();
};

#endif //_qtResBrowser_inc_

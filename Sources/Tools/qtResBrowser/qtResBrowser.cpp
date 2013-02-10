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
#include <QApplication>
#include <QSettings>
#include <QDesktopServices>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QMdiSubWindow>
#include <QDropEvent>
#include <QUrl>

#include "qtPlasma.h"
#include "qtTreeIterator.h"
#include "qtResBrowser.h"
#include "pnAllCreatables.h"
#include "plResMgr/plResMgrCreatable.h"
#include "plResMgr/plResMgrSettings.h"
#include "plResMgr/plRegistryNode.h"
#include "plResMgr/plPageInfo.h"
#include "pnKeyedObject/plUoid.h"
#include "pnKeyedObject/plKey.h"
#include "pnKeyedObject/plKeyImp.h"
#include "plMessage/plResMgrHelperMsg.h"
REGISTER_CREATABLE(plResMgrHelperMsg);


qtResBrowser* qtResBrowser::sInstance = nullptr;
qtResBrowser* qtResBrowser::Instance() { return sInstance; }


qtResBrowser::qtResBrowser() {
    // Set up the "Magic" instance
    hsAssert(sInstance == nullptr, "qtResBrowser broke!");
    sInstance = this;

    plResMgrSettings::Get().SetFilterNewerPageVersions(false);
    plResMgrSettings::Get().SetFilterOlderPageVersions(false);
    plResMgrSettings::Get().SetLoadPagesOnInit(false);

    // Basic Form Settings
    setWindowTitle("Plasma Resource Browser");
    setWindowIcon(QIcon(":/icons/qtResBrowser.svg"));
    setDockOptions(QMainWindow::AnimatedDocks);

    // Set up actions
    fActions[kFileNewPage] = new QAction(QIcon::fromTheme("document-new"), tr("New &Page"), this);
    fActions[kFileOpen] = new QAction(QIcon::fromTheme("document-open"), tr("&Open..."), this);
    fActions[kFileSave] = new QAction(QIcon::fromTheme("document-save"), tr("&Save"), this);
    fActions[kFileSaveAs] = new QAction(tr("Sa&ve As..."), this);
    fActions[kFileExit] = new QAction(tr("E&xit"), this);
    fActions[kToolsProperties] = new QAction(tr("Show &Properties Pane"), this);
    fActions[kToolsNewObject] = new QAction(tr("&New Object..."), this);
    fActions[kWindowPrev] = new QAction(tr("&Previous"), this);
    fActions[kWindowNext] = new QAction(tr("&Next"), this);
    fActions[kWindowTile] = new QAction(tr("&Tile"), this);
    fActions[kWindowCascade] = new QAction(tr("&Cascade"), this);
    fActions[kWindowClose] = new QAction(tr("C&lose"), this);
    fActions[kWindowCloseAll] = new QAction(tr("Cl&ose All"), this);

    fActions[kTreeClose] = new QAction(tr("&Close"), this);
    fActions[kTreeEdit] = new QAction(tr("&Edit"), this);
    fActions[kTreeEditPRC] = new QAction(tr("Edit P&RC"), this);
    fActions[kTreePreview] = new QAction(tr("&Preview"), this);
    fActions[kTreeDelete] = new QAction(tr("&Delete"), this);
    fActions[kTreeImport] = new QAction(tr("&Import..."), this);
    fActions[kTreeExport] = new QAction(tr("E&xport..."), this);

    fActions[kFileOpen]->setShortcut(Qt::CTRL + Qt::Key_O);
    fActions[kFileSave]->setShortcut(Qt::CTRL + Qt::Key_S);
    fActions[kFileExit]->setShortcut(Qt::ALT + Qt::Key_F4);
    fActions[kWindowClose]->setShortcut(Qt::CTRL + Qt::Key_W);
    fActions[kToolsProperties]->setCheckable(true);
    fActions[kToolsProperties]->setChecked(true);

    // TODO: Main Menus
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    //fileMenu->addAction(fActions[kFileNewPage]);
    //fileMenu->addSeparator();
    fileMenu->addAction(fActions[kFileOpen]);
    //fileMenu->addAction(fActions[kFileSave]);
    //fileMenu->addAction(fActions[kFileSaveAs]);
    fileMenu->addSeparator();
    fileMenu->addAction(fActions[kFileExit]);

    QMenu* viewMenu = menuBar()->addMenu(tr("&Tools"));
    viewMenu->addAction(fActions[kToolsProperties]);
    //viewMenu->addSeparator();
    //viewMenu->addAction(fActions[kToolsNewObject]);

    QMenu* wndMenu = menuBar()->addMenu(tr("&Window"));
    wndMenu->addAction(fActions[kWindowPrev]);
    wndMenu->addAction(fActions[kWindowNext]);
    wndMenu->addSeparator();
    wndMenu->addAction(fActions[kWindowTile]);
    wndMenu->addAction(fActions[kWindowCascade]);
    wndMenu->addSeparator();
    wndMenu->addAction(fActions[kWindowClose]);
    wndMenu->addAction(fActions[kWindowCloseAll]);

    // TODO: Toolbars
    QToolBar* fileTbar = addToolBar(tr("File Toolbar"));
    fileTbar->setObjectName("FileToolBar");
    //fileTbar->addAction(fActions[kFileNewPage]);
    fileTbar->addAction(fActions[kFileOpen]);
    //fileTbar->addAction(fActions[kFileSave]);
    fileTbar->setMovable(false);

    // MDI Area for child editors
    fMdiArea = new QMdiArea(this);
    fMdiArea->setFrameShadow(QFrame::Sunken);
    fMdiArea->setFrameShape(QFrame::StyledPanel);
    fMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    fMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    fMdiArea->setActivationOrder(QMdiArea::ActivationHistoryOrder);
    setCentralWidget(fMdiArea);

    // Object Browser
    fBrowserDock = new QDockWidget(tr("Object Browser"), this);
    fBrowserDock->setObjectName("BrowserDock");
    fBrowserTree = new QTreeWidget(fBrowserDock);
    fBrowserDock->setWidget(fBrowserTree);
    fBrowserDock->setAllowedAreas(Qt::LeftDockWidgetArea |
                                  Qt::RightDockWidgetArea);
    fBrowserDock->setFeatures(QDockWidget::DockWidgetMovable |
                              QDockWidget::DockWidgetFloatable);
    fBrowserTree->setUniformRowHeights(true);
    fBrowserTree->setHeaderHidden(true);
    fBrowserTree->setContextMenuPolicy(Qt::CustomContextMenu);
    addDockWidget(Qt::LeftDockWidgetArea, fBrowserDock);

    // Registry Iterator
    fTreeIter = new qtTreeIterator(fBrowserTree);

    // Property Pane
    fPropertyDock = new QDockWidget(tr("Properties"), this);
    fPropertyDock->setObjectName("PropertyDock");
    fPropertyDock->setAllowedAreas(Qt::LeftDockWidgetArea |
                                   Qt::RightDockWidgetArea);
    fPropertyContainer = new QWidget(fPropertyDock);
    QVBoxLayout* propLayout = new QVBoxLayout(fPropertyContainer);
    propLayout->setContentsMargins(4, 4, 4, 4);
    fPropertyDock->setWidget(fPropertyContainer);
    setPropertyPage(kPropsNone);
    addDockWidget(Qt::LeftDockWidgetArea, fPropertyDock);

    // TODO: Global UI Signals
    //connect(fActions[kFileNewPage], SIGNAL(triggered()), this, SLOT(newPage()));
    connect(fActions[kFileExit], SIGNAL(triggered()), this, SLOT(close()));
    connect(fActions[kFileOpen], SIGNAL(triggered()), this, SLOT(openFiles()));
    //connect(fActions[kFileSave], SIGNAL(triggered()), this, SLOT(performSave()));
    connect(fActions[kFileSaveAs], SIGNAL(triggered()), this, SLOT(performSaveAs()));

    connect(fActions[kToolsProperties], SIGNAL(toggled(bool)),
            fPropertyDock, SLOT(setVisible(bool)));
    connect(fPropertyDock, SIGNAL(visibilityChanged(bool)),
            fActions[kToolsProperties], SLOT(setChecked(bool)));
    //connect(fActions[kToolsNewObject], SIGNAL(triggered()),
    //        this, SLOT(createNewObject()));

    connect(fActions[kWindowPrev], SIGNAL(triggered()),
            fMdiArea, SLOT(activatePreviousSubWindow()));
    connect(fActions[kWindowNext], SIGNAL(triggered()),
            fMdiArea, SLOT(activateNextSubWindow()));
    connect(fActions[kWindowTile], SIGNAL(triggered()),
            fMdiArea, SLOT(tileSubWindows()));
    connect(fActions[kWindowCascade], SIGNAL(triggered()),
            fMdiArea, SLOT(cascadeSubWindows()));
    connect(fActions[kWindowClose], SIGNAL(triggered()),
            fMdiArea, SLOT(closeActiveSubWindow()));
    connect(fActions[kWindowCloseAll], SIGNAL(triggered()),
            fMdiArea, SLOT(closeAllSubWindows()));

    //connect(fActions[kTreeClose], SIGNAL(triggered()), this, SLOT(treeClose()));
    //connect(fActions[kTreeEdit], SIGNAL(triggered()), this, SLOT(treeEdit()));
    //connect(fActions[kTreeEditPRC], SIGNAL(triggered()), this, SLOT(treeEditPRC()));
    //connect(fActions[kTreePreview], SIGNAL(triggered()), this, SLOT(treePreview()));
    //connect(fActions[kTreeDelete], SIGNAL(triggered()), this, SLOT(treeDelete()));
    //connect(fActions[kTreeImport], SIGNAL(triggered()), this, SLOT(treeImport()));
    connect(fActions[kTreeExport], SIGNAL(triggered()), this, SLOT(treeExport()));

    connect(fBrowserTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(treeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
    //connect(fBrowserTree, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
    //        this, SLOT(treeItemActivated(QTreeWidgetItem*, int)));
    connect(fBrowserTree, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(treeContextMenu(const QPoint&)));

    // TODO: Load UI Settings
}

void qtResBrowser::closeEvent(QCloseEvent*)
{
    // Free all the tree items
    for (int i = fBrowserTree->topLevelItemCount(); i >= 0; i--) {
        qtTreeItem* item = static_cast<qtTreeItem*>(fBrowserTree->takeTopLevelItem(i));

        if (item)
            item->deleteRecur();

        delete item;
    }

    // TODO: Save UI Settings
}

void qtResBrowser::dragEnterEvent(QDragEnterEvent* evt)
{
    if (evt->mimeData()->hasUrls())
        evt->acceptProposedAction();
}

void qtResBrowser::dropEvent(QDropEvent* evt)
{
    if (evt->mimeData()->hasUrls()) {
        foreach (QUrl url, evt->mimeData()->urls()) {
            QString filename = url.toLocalFile();
            if (!filename.isEmpty())
                loadFile(filename);
        }
    }
}


qtTreeItem* qtResBrowser::findCurrentPageItem(bool isSave)
{
    qtTreeItem* item = static_cast<qtTreeItem*>(fBrowserTree->currentItem());
    if (item == nullptr)
        return nullptr;

    if (item->type() == qtTreeItem::kTypeAge)
    {
        if (isSave) {
            // TODO: Save all pages
        }

        return nullptr;
    }
    else if (item->type() == qtTreeItem::kTypePage)
    {
        return item;
    }
    else if (item->type() == qtTreeItem::kTypeKO)
    {
        return fTreeIter->GetPageItem(item->key()->GetUoid().GetLocation());
    }
    else
    {
        // Type folder
        qtTreeItem* pageItem = static_cast<qtTreeItem*>(item->parent());
        hsAssert(pageItem->type() == qtTreeItem::kTypePage, "Got non-page parent");

        return pageItem;
    }
}


void qtResBrowser::setPropertyPage(PropWhich which)
{
    QWidget* group = nullptr;
    QLayoutItem* item = fPropertyContainer->layout()->itemAt(0);
    if (item != nullptr) {
        // Clear the old property page
        fPropertyContainer->layout()->removeItem(item);
        delete item->widget();
        delete item;
    }

    switch (which) {
    case kPropsAge:
        {
            // An age is selected -- not much to do here for now
            group = new QGroupBox(tr("Age Properties"), fPropertyContainer);
            QGridLayout* layout = new QGridLayout(group);
            fAgeName = new QLineEdit(group);
            fAgeName->setReadOnly(true);
            layout->addWidget(new QLabel(tr("Name:"), group), 0, 0);
            layout->addWidget(fAgeName, 0, 1);
        }
        break;
    case kPropsPage:
        {
            // PRP file's properties.  Changes to the Location should propagate
            // to all other keys in this PRP.
            group = new QGroupBox(tr("Page Properties"), fPropertyContainer);
            QGridLayout* layout = new QGridLayout(group);
            fAgeName = new QLineEdit(group);
            fPageName = new QLineEdit(group);
            fReleaseVersion = new QSpinBox(group);
            fReleaseVersion->setRange(0, 0x7FFFFFFF);

            QGroupBox* locationGrp = new QGroupBox(tr("Location"), group);
            QGridLayout* locationLayout = new QGridLayout(locationGrp);
            fSeqPrefix = new QLineEdit(locationGrp);
            fLocationFlags[kLocLocalOnly] = new QCheckBox(tr("Local Only"), locationGrp);
            fLocationFlags[kLocVolatile] = new QCheckBox(tr("Volatile"), locationGrp);
            fLocationFlags[kLocItinerant] = new QCheckBox(tr("Itinerant"), locationGrp);
            fLocationFlags[kLocReserved] = new QCheckBox(tr("Reserved"), locationGrp);
            fLocationFlags[kLocBuiltIn] = new QCheckBox(tr("Built-In"), locationGrp);
            locationLayout->addWidget(new QLabel(tr("Page ID:"), locationGrp), 0, 0);
            locationLayout->addWidget(fSeqPrefix, 0, 1);
            locationLayout->addWidget(new QLabel(tr("Flags:"), locationGrp), 1, 0);
            locationLayout->addWidget(fLocationFlags[kLocLocalOnly], 1, 1);
            locationLayout->addWidget(fLocationFlags[kLocReserved], 1, 2);
            locationLayout->addWidget(fLocationFlags[kLocVolatile], 2, 1);
            locationLayout->addWidget(fLocationFlags[kLocBuiltIn], 2, 2);
            locationLayout->addWidget(fLocationFlags[kLocItinerant], 3, 1);

            layout->addWidget(new QLabel(tr("Age:"), group), 0, 0);
            layout->addWidget(fAgeName, 0, 1);
            layout->addWidget(new QLabel(tr("Page:"), group), 1, 0);
            layout->addWidget(fPageName, 1, 1);
            layout->addWidget(new QLabel(tr("Data Version:"), group), 2, 0);
            layout->addWidget(fReleaseVersion, 2, 1);
            layout->addWidget(locationGrp, 3, 0, 1, 2);
        }
        break;
    case kPropsKO:
        {
            // A KeyedObject -- just edit the object's key (much easier than
            // throwing Key editors everywhere else they appear)
            group = new QGroupBox(tr("Object Properties"), fPropertyContainer);
            QGridLayout* layout = new QGridLayout(group);
            fObjName = new QLineEdit(group);
            fObjType = new QLabel(group);
            fLoadMaskQ[0] = new QSpinBox(group);
            fLoadMaskQ[1] = new QSpinBox(group);
            fLoadMaskQ[0]->setRange(0, 255);
            fLoadMaskQ[1]->setRange(0, 255);

            fCloneIdBox = new QGroupBox(tr("Clone IDs"), group);
            fCloneIdBox->setCheckable(true);
            QGridLayout* cloneLayout = new QGridLayout(fCloneIdBox);
            fCloneId = new QSpinBox(fCloneIdBox);
            fCloneId->setRange(0, 0x7FFFFFFF);
            fClonePlayerId = new QSpinBox(fCloneIdBox);
            fClonePlayerId->setRange(0, 0x7FFFFFFF);
            cloneLayout->addWidget(new QLabel(tr("Clone ID:"), fCloneIdBox), 0, 0);
            cloneLayout->addWidget(fCloneId, 0, 1);
            cloneLayout->addWidget(new QLabel(tr("Clone Player ID:"), fCloneIdBox), 1, 0);
            cloneLayout->addWidget(fClonePlayerId, 1, 1);

            layout->addWidget(new QLabel(tr("Name:"), group), 0, 0);
            layout->addWidget(fObjName, 0, 1, 1, 2);
            layout->addWidget(new QLabel(tr("Type:"), group), 1, 0);
            layout->addWidget(fObjType, 1, 1, 1, 2);
            layout->addWidget(new QLabel(tr("Load Mask:"), group), 2, 0);
            layout->addWidget(fLoadMaskQ[0], 2, 1);
            layout->addWidget(fLoadMaskQ[1], 2, 2);
            layout->addWidget(fCloneIdBox, 3, 0, 1, 3);
        }
        break;
    default:
        {
            group = new QWidget(fPropertyContainer);
            QGridLayout* layout = new QGridLayout(group);
            layout->addWidget(new QLabel(tr("No Object Selected"), group), 0, 0);
        }
    }
    fPropertyContainer->layout()->addWidget(group);
    fPropertyContainer->setFixedHeight(group->sizeHint().height() + 8);
}

void qtResBrowser::treeItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    //saveProps((QPlasmaTreeItem*)previous);

    qtTreeItem* item = static_cast<qtTreeItem*>(current);
    if (item == nullptr)
    {
        setPropertyPage(kPropsNone);
    }
    else if (item->type() == qtTreeItem::kTypeAge)
    {
        setPropertyPage(kPropsAge);

        fAgeName->setText(item->age());
    }
    else if (item->type() == qtTreeItem::kTypePage)
    {
        setPropertyPage(kPropsPage);

        const plPageInfo& pageInfo = item->page()->GetPageInfo();

        fAgeName->setText(~pageInfo.GetAge());
        fPageName->setText(~pageInfo.GetPage());
        fReleaseVersion->setValue(pageInfo.GetMajorVersion());

        plLocation loc = pageInfo.GetLocation();
        fSeqPrefix->setText(~loc.StringIze());

        fLocationFlags[kLocLocalOnly]->setChecked(loc.GetFlags() & plLocation::kLocalOnly);
        fLocationFlags[kLocVolatile] ->setChecked(loc.GetFlags() & plLocation::kVolatile);
        fLocationFlags[kLocItinerant]->setChecked(loc.GetFlags() & plLocation::kItinerant);
        fLocationFlags[kLocReserved] ->setChecked(loc.GetFlags() & plLocation::kReserved);
        fLocationFlags[kLocBuiltIn]  ->setChecked(loc.GetFlags() & plLocation::kBuiltIn);
    }
    else if (item->type() == qtTreeItem::kTypeKO)
    {
        setPropertyPage(kPropsKO);

        fObjName->setText(~item->key()->GetName());
        fObjType->setText(plFactory::GetNameOfClass(item->key()->GetUoid().GetClassType()));

        plLoadMask mask = item->key()->GetUoid().GetLoadMask();
        fLoadMaskQ[0]->setValue(mask.MatchesQuality(0));
        fLoadMaskQ[1]->setValue(mask.MatchesQuality(1));

        fCloneId->setValue(item->key()->GetUoid().GetCloneID());
        fClonePlayerId->setValue(item->key()->GetUoid().GetClonePlayerID());
        fCloneIdBox->setChecked(fCloneId->value() != 0 || fClonePlayerId->value() != 0);
    }
    else
    {
        setPropertyPage(kPropsNone);
    }
}

void qtResBrowser::treeContextMenu(const QPoint& pos)
{
    fBrowserTree->setCurrentItem(fBrowserTree->itemAt(pos));
    qtTreeItem* item = static_cast<qtTreeItem*>(fBrowserTree->currentItem());
    if (item == nullptr)
        return;

    QMenu menu(this);
    if (item->type() == qtTreeItem::kTypeAge)
    {
        //menu.addAction(fActions[kTreeClose]);
    }
    else if (item->type() == qtTreeItem::kTypePage)
    {
        //menu.addAction(fActions[kTreeClose]);
        //menu.addAction(fActions[kTreeImport]);
    }
    else if (item->type() == qtTreeItem::kTypeKO)
    {
        //menu.addAction(fActions[kTreeEdit]);
        //menu.addAction(fActions[kTreeEditPRC]);
        //menu.addAction(fActions[kTreePreview]);
        //menu.addSeparator();
        //menu.addAction(fActions[kTreeDelete]);
        //menu.addAction(fActions[kTreeImport]);
        menu.addAction(fActions[kTreeExport]);
        //menu.setDefaultAction(fActions[kTreeEdit]);
        //fActions[kTreePreview]->setEnabled(pqCanPreviewType(item->obj()->ClassIndex()));
    }
    else
    {
        //menu.addAction(fActions[kTreeImport]);
    }
    menu.exec(fBrowserTree->viewport()->mapToGlobal(pos));
}

void qtResBrowser::treeExport()
{
    qtTreeItem* item = static_cast<qtTreeItem*>(fBrowserTree->currentItem());
    if (item == nullptr || item->key() == nullptr)
        return;

    plKeyImp* keyImp = static_cast<plKeyImp*>(item->key());

    if (keyImp->GetDataLen() <= 0)
        return;

    QString fnfix = (~keyImp->GetName()).replace(QRegExp("[?:/\\*\"<>|]"), "_");
    QString genPath = tr("%1/[%2]%3.po").arg(fDialogDir)
                                        .arg(plFactory::GetNameOfClass(keyImp->GetUoid().GetClassType()))
                                        .arg(fnfix);
    QString filename = QFileDialog::getSaveFileName(this,
                            tr("Export Raw Object"), genPath,
                            "Plasma Objects (*.po *.mof *.uof)");

    if (!filename.isEmpty()) {
        plResManager* resMgr = static_cast<plResManager*>(hsgResMgr::ResMgr());
        plRegistryPageNode* page = resMgr->FindPage(keyImp->GetUoid().GetLocation());

        hsStream* dataStream = page->OpenStream();
        uint8_t* buffer = new uint8_t[keyImp->GetDataLen()];

        if (buffer) {
            dataStream->SetPosition(keyImp->GetStartPos());
            dataStream->Read(keyImp->GetDataLen(), buffer);
        }

        page->CloseStream();

        if (buffer == nullptr)
            return;

        hsUNIXStream outStream;
        outStream.Open((~filename).c_str(), "wb");
        outStream.Write(keyImp->GetDataLen(), buffer);
        outStream.Close();

        delete[] buffer;

        QDir dir = QDir(filename);
        dir.cdUp();
        fDialogDir = dir.absolutePath();
    }
}


void qtResBrowser::openFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(this,
                            tr("Open File(s)"), fDialogDir,
                            "All supported types (*.age *.prp);;"
                            "Age Files (*.age);;"
                            "Page Files (*.prp)");

    QStringList filesIt = files;
    for (QStringList::Iterator it = filesIt.begin(); it != filesIt.end(); it++) {
        loadFile(*it);
        QDir dir = QDir(*it);
        dir.cdUp();
        fDialogDir = dir.absolutePath();
    }
}

void qtResBrowser::loadFile(QString filename)
{
    if (filename.endsWith(".age", Qt::CaseInsensitive))
    {
        // TODO
    }
    else if (filename.endsWith(".prp", Qt::CaseInsensitive))
    {
        // Load that source
        plResManager* mgr = (plResManager*)hsgResMgr::ResMgr();
        mgr->AddSinglePage(~filename);

        plRegistryPageNode* page = mgr->FindSinglePage(~filename);
        fTreeIter->EatPage(page);
    }
    else
    {
        QMessageBox msgBox(QMessageBox::Warning, tr("Error"),
                           tr("%1: Unsupported File Type").arg(filename),
                           QMessageBox::Ok, this);
        msgBox.exec();
    }

    fBrowserTree->sortItems(0, Qt::AscendingOrder);
}


void qtResBrowser::performSaveAs()
{
    qtTreeItem* pageItem = this->findCurrentPageItem(false);
    if (pageItem == nullptr)
        return;

    QString saveDir = !pageItem->filename().IsValid()
                    ? fDialogDir
                    : ~pageItem->filename().AsString();

    QString filename = QFileDialog::getSaveFileName(this,
                            tr("Save PRP"), saveDir,
                            "Page file (*.prp)");

    if (!filename.isEmpty()) {
        this->saveFile(pageItem->page(), filename);
        QDir dir = QDir(filename);
        dir.cdUp();
        fDialogDir = dir.absolutePath();
    }
}

void qtResBrowser::saveFile(plRegistryPageNode* page, QString filename)
{
    hsUNIXStream outStream;
    outStream.Open((~filename).c_str(), "wb");

    // NOTE: This causes the page to recalculate things internally, which means
    // loading and immediately saving a page might result in different object
    // ordering or stuff.
    // TODO: Find out why this doesn't seem to write any objects :(
    page->Write(&outStream);

    outStream.Close();
}


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    qtResBrowser mainWnd;

    plResManager* resMgr = new plResManager();
    hsgResMgr::Init(resMgr);

    mainWnd.show();

    for (int i = 1; i < argc; i++) {
        mainWnd.loadFile(argv[i]);
    }

    mainWnd.setAcceptDrops(true);

    int ret = app.exec();

    hsgResMgr::Shutdown();

    return ret;
}

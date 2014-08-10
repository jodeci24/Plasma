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

#include "HeadSpin.h"
#include "plNetCore.h"
#include "plNetCoreDns.h"
#include "plNetCoreThreadInfo.h"

#include "pnNetCommon/plNetApp.h"

#include <thread> // This should be pulled in by plNetCoreThreadInfo eventually

#ifdef HS_BUILD_FOR_WIN32
#   include "hsWindows.h"
#endif


#define LOCK(mtx) \
    std::unique_lock<std::mutex> lock_##mtx; \
    if (fThreadInfo) { \
        lock_##mtx = std::unique_lock<std::mutex>(fThreadInfo->mtx); \
    }

#define UNLOCK(mtx) \
    if (fThreadInfo) { \
        lock_##mtx.unlock(); \
    }

plNetCore* plNetCore::gInstance = nullptr;

void plNetCore::Initialize()
{
    hsAssert(!gInstance, "NetCore already initialized!");

#ifdef HS_BUILD_FOR_WIN32
    // Windows network init
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    // Initialize the subsystems
    plNetCoreDns::Initialize();

    gInstance = new plNetCore();
}


void plNetCore::Shutdown()
{
    hsAssert(gInstance, "NetCore was never initialized!");

    delete gInstance;
    gInstance = nullptr;

    // Shutdown the subsystems
    plNetCoreDns::Shutdown();

#ifdef HS_BUILD_FOR_WIN32
    // Cleanup for Win32
    WSACleanup();
#endif
}


plNetCore::plNetCore()
{
    for (int i = 0; i < plNetCoreConnInfo::kNumConnTypes; i++) {
        fConnections[i] = nullptr;
    }

    plNetApp* netapp = plNetApp::GetInstance();
    if (netapp && netapp->GetFlagsBit(plNetApp::kNetCoreSingleThreaded)) {
        fThreadInfo = nullptr;
    } else {
        fThreadInfo = new plNetCoreThreadInfo(this);
    }
}

plNetCore::~plNetCore()
{
    if (fThreadInfo)
    {
        delete fThreadInfo;
    }
}


int32_t plNetCore::Connect(plNetCoreConnInfo::ConnType type, const plNetAddress& addr)
{
    hsAssert(type != plNetCoreConnInfo::kInvalid, "Invalid connection type");

    if (fConnections[type]) {
        // TODO: Figure out connection closing
    }

    switch (type) {
        case plNetCoreConnInfo::kGate:
            fConnections[type] = new plGateConnInfo(this);
            break;
        default:
            return kErrInvalidPeer;
    }

    int32_t ret = fConnections[type]->Connect(addr);

    if (ret != plNetCore::kNetOK) {
        plPrintf("Got socket error: {}\n", strerror(ret));
        return ret;
    }

    LOCK(fSetMutex)

    // Add the socket to the read FD set
    fReads.SetForSocket(static_cast<plSocket&>(*fConnections[type]));

    UNLOCK(fSetMutex)

    return ret;
}


void plNetCore::ISend(size_t& count) {
    count = 0;
}

void plNetCore::Send(size_t& count) {
    if (fThreadInfo) {
        std::this_thread::yield();
    } else {
        ISend(count);
    }
}


void plNetCore::IRecv(size_t& count) {
    count = 0;

    LOCK(fSetMutex)
    plFdSet reads = fReads;
    UNLOCK(fSetMutex)

    char* buf = new char[1024];
    if (reads.WaitForRead(false, 1000) > 0)
    {
        for (int i = 0; i < plNetCoreConnInfo::kNumConnTypes; i++)
        {
            memset(buf, '\0', 1024);

            if (fConnections[i] && reads.IsSetFor(static_cast<plSocket&>(*fConnections[i])))
            {
                plNet::Read(static_cast<plSocket&>(*fConnections[i]).GetSocket(), buf, arrsize(buf));

                plPrintf("{}", buf);
            }
        }
    }
}

void plNetCore::Recv(size_t& count) {
    if (fThreadInfo) {
        std::this_thread::yield();
    } else {
        IRecv(count);
    }
}

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
#include <iostream>

#ifdef HS_BUILD_FOR_WIN32
#    include "hsWindows.h"
#else
#    include <netdb.h>
#endif

#include <ares.h>
#include "hsThread.h"
#include "plString.h"
#include "pnNetCommon/plNetAddress.h"

// *grumble*
#include "pnNetCommon/pnNetCommonCreatable.h"
#include "pnKeyedObject/pnKeyedObjectCreatable.h"
#include "pnMessage/pnMessageCreatable.h"

using namespace std;

class plResolver : public hsThread
{
private:
    ares_channel fChannel;
    ares_host_callback fCallback;
    plString fHostname;
    hsSemaphore fSemaphore;

public:
    plResolver(plString& hostname, ares_host_callback callback) : hsThread(0) {
        fHostname = hostname;
        fCallback = callback;
    }

    hsSemaphore& GetSemaphore() { return fSemaphore; }

    virtual hsError Run() {
        int status = ares_init(&fChannel);
        if (status != ARES_SUCCESS) {
            cerr << "ares_init: " << ares_strerror(status) << endl;
            return -1;
        }

        ares_gethostbyname(fChannel, fHostname.c_str(), AF_INET, fCallback, NULL);

        for (;;) {
            struct timeval *tvp, tv;
            fd_set read_fds, write_fds;
            int nfds;

            FD_ZERO(&read_fds);
            FD_ZERO(&write_fds);
            nfds = ares_fds(fChannel, &read_fds, &write_fds);
            if (nfds == 0) {
                break;
            }
            tvp = ares_timeout(fChannel, NULL, &tv);
            select(nfds, &read_fds, &write_fds, NULL, tvp);
            ares_process(fChannel, &read_fds, &write_fds);
        }

        ares_destroy(fChannel);

        fSemaphore.Signal();
        return hsOK;
    }
};

void callback(void *arg, int status, int timeouts, struct hostent *host)
{
    if (!host || status != ARES_SUCCESS) {
        cerr << "ares_gethostbyname: " << ares_strerror(status) << endl;
        return;
    }

    cout << endl << "Found address name " << host->h_name << endl;

    in_addr const * const * const inAddr = (in_addr**)host->h_addr_list;

    size_t count = arrsize(inAddr);
    plNetAddress* addrs = new plNetAddress[count];

    /* Fill in the address data */
    for (size_t i = 0; i < count; ++i) {
        addrs[i].SetHost(inAddr[i]->s_addr);
        addrs[i].SetAnyPort();

        cout << addrs[i].GetHostString().c_str() << endl;
    }
}


int main(int argc, char *argv[])
{
#ifdef HS_BUILD_FOR_WIN32
    WSADATA p;
    WSAStartup((2 << 8) | 2, &p);
#endif

    if (argc < 2) {
        cout << "USAGE: plDNS [HOSTNAME]" << endl;
        return -1;
    }

    plString hostname = argv[1];

    int status = ares_library_init(ARES_LIB_INIT_ALL);
    if (status != ARES_SUCCESS) {
        cerr << "ares_library_init: " << ares_strerror(status) << endl;
        return -2;
    }

    plResolver* thr = new plResolver(hostname, &callback);
    thr->Start();

    while (!thr->GetSemaphore().TryWait()) {
        cout << '.';
        hsSleep::Sleep(1);
    }

    ares_library_cleanup();
    return 0;
}

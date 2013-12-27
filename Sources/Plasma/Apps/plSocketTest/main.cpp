#include "HeadSpin.h"
#include "pnNetCommon/plNetAddress.h"
#include "plSockets/plTcpSocket.h"

// *grumble* plNetAddress drags these in :(
#include "pnNetCommon/pnNetCommonCreatable.h"
#include "pnKeyedObject/pnKeyedObjectCreatable.h"
#include "pnMessage/pnMessageCreatable.h"

int main(int argc, char* argv[])
{
    plTcpSocket* fSock = new plTcpSocket();
    plNetAddress addr("cho.cyan.com", 1813);
    bool running = false;

    running = fSock->ActiveOpen(addr);

    if (running) {
        fSock->SendData("40|1\r\n", 6);
    }

    while (running) {
        int nread = 0;
        char* buf = new char[1024];
        char* bp = buf;
        int bsize = 1024;

        while (bsize && (nread = fSock->RecvData(bp, bsize)) > 0) {
            bp += nread;
            bsize -= nread;

            if (*(bp-1) == '\n' || bsize == 0) {
                *bp = '\0';
                break;
            }
        }

        if (nread == 0) {
        } else if (nread > 0) {
            char* saveptr = nullptr;
            char* line = strtok_r(buf, "\n", &saveptr);

            while (line != nullptr) {
                printf("%s\n", line);
                line = strtok_r(saveptr, "\n", &saveptr);
            }
        }

        delete[] buf;
    }

    return 0;
}

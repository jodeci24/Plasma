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

You can contact Cyan Worlds, Inc. by email legal@cyan.com
 or by snail mail at:
      Cyan Worlds, Inc.
      14617 N Newport Hwy
      Mead, WA   99021

*==LICENSE==*/
/*****************************************************************************
*
*   $/Plasma20/Sources/Plasma/NucleusLib/pnNetProtocol/Private/Protocols/Srv2Vault/pnNpSrv2Vault.cpp
*   
***/

#ifdef SERVER

#define USES_PROTOCOL_SRV2VAULT
#include "../../../Pch.h"
#pragma hdrstop


/*****************************************************************************
*
*   Exports
*
***/

//===========================================================================
bool Srv2VaultValidateConnect (
    AsyncNotifySocketListen *   listen,
    Srv2Vault_ConnData *        connectPtr
) {
    // Ensure that there are enough bytes for the header
    const Srv2Vault_ConnData & connect = * (const Srv2Vault_ConnData *) listen->buffer;

    // Validate message size
    const unsigned kMinStructSize = sizeof(dword) * 3;
    if (listen->bytes < kMinStructSize)
        return false;
    if (listen->bytes < connect.dataBytes)
        return false;

    // Validate connect server type
    if (!(connect.srvType == kSrvTypeAuth || connect.srvType == kSrvTypeGame || connect.srvType == kSrvTypeMcp))
        return false;

    ZEROPTR(connectPtr);
    MemCopy(connectPtr, &connect, min(sizeof(*connectPtr), connect.dataBytes));

    listen->bytesProcessed += connect.dataBytes;
    return true;
}

#endif // SERVER

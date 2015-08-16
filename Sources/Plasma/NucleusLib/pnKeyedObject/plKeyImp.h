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
#ifndef plKeyImp_inc
#define plKeyImp_inc

#include "plKey.h"
#include "hsTemplates.h"
#include "plUoid.h"  
#include "hsBitVector.h"
#include "plRefFlags.h"

//------------------------------------
// plKey is a handle to a keyedObject
//------------------------------------
class plKeyImp : public plKeyData
{
private:
    static hsKeyedObject* SafeGetObject(const plKeyImp* key);

public:
    plKeyImp();
    plKeyImp(plUoid, uint32_t pos, uint32_t len);
    virtual ~plKeyImp();


    /**
     * Advances the stream position by the size of a key without really reading
     * the key data.
     */
    static void SkipRead(hsStream* s);


    const plUoid&   GetUoid() const override { return fUoid; }
    const plString& GetName() const override;

    /**
     * Returns a direct pointer to the object.
     *
     * You probably want to use ObjectIsLoaded unless you're 100% sure the
     * object is loaded.
     */
    hsKeyedObject* GetObjectPtr() override;

    /**
     * Returns the object if it is loaded, null otherwise.
     */
    hsKeyedObject* ObjectIsLoaded() const override;

    /**
     * Forces the object to be read and loaded by the res manager, and returns
     * the loaded object.
     */
    hsKeyedObject* VerifyLoaded() override;


    /**
     * Sets the integer object ID of this object.
     *
     * This is called before writing to disk so that static keys can have
     * faster lookups in the res manager (integer comparison rather than
     * name-based string comparison).
     */
    void SetObjectID(uint32_t id) { fUoid.SetObjectID(id); }

    /**
     * Sets the object pointer directly.
     *
     * This should only be used by hsKeyedObject!
     */
    hsKeyedObject* SetObjectPtr(hsKeyedObject* p);


    //----------------------
    // I/O
    // ResMgr performs read, so it can search for an existing instance....
    //----------------------
    void Read(hsStream* s);
    void Write(hsStream* s);
    void WriteObject(hsStream* s);

    /**
     * Returns the start position of this object in the data file, for the
     * res manager to read the object.
     */
    uint32_t GetStartPos() const { return fStartPos; }

    /**
     * Returns the length of this object's data in the data file, for the res
     * manager to read the object.
     */
    uint32_t GetDataLen() const { return fDataLen;  }

    //----------------------
    // Allow a keyed object to behave as if it has an active ref when in fact the object
    // should only be active ref'ed by a non-keyed parent.  Essentially just bumps/decs
    // the active ref count to facilitate normal object creation/destruction
    //----------------------
    hsKeyedObject* RefObject(plRefFlags::Type flags = plRefFlags::kActiveRef) override;
    void UnRefObject(plRefFlags::Type flags = plRefFlags::kActiveRef) override;

    /**
     * Release this object's reference on the target key.
     *
     * Release has two behaviours, depending on whether the ref is active or
     * passive:
     *
     * - Active: Decrements the target object's active ref count. If it gets to
     *           zero, the target object will be deleted.
     *
     * - Passive: Unregisters this object's interest in when the target object
     *            is created or destroyed.
     */
    void Release(plKey targetKey) override;

    void UnRegister();
    void SetUoid(const plUoid& uoid);
    void SetupNotify(plRefMsg* msg, plRefFlags::Type flags);

    ////////////////////////////////////////////////////////////////////////////
    // ResManager/Registry use only!
    //

    //----------------------
    // Clone access
    //----------------------
    void    AddClone(plKeyImp* c);
    void    RemoveClone(plKeyImp* c) const;
    plKey   GetClone(uint32_t playerID, uint32_t cloneID) const;
    void    CopyForClone(const plKeyImp* p, uint32_t playerID, uint32_t cloneID);    // Copy the contents of p for cloning process

    uint32_t  GetNumClones();
    plKey   GetCloneByIdx(uint32_t idx);
    plKey   GetCloneOwner() { return fCloneOwner; }

    void NotifyCreated();
    void ISetupNotify(plRefMsg* msg, plRefFlags::Type flags); // Setup notifcations for reference, don't send anything.

    void        AddRef(plKeyImp* key) const;
    uint16_t      GetNumRefs() const { return fRefs.GetCount(); }
    plKeyImp*   GetRef(int i) const { return fRefs[i]; }
    void        RemoveRef(plKeyImp *key) const;

    virtual uint16_t      GetActiveRefs() const           { return fNumActiveRefs; }
    virtual uint16_t      GetNumNotifyCreated() const     { return fNotifyCreated.GetCount(); }
    virtual plRefMsg*   GetNotifyCreated(int i) const   { return fNotifyCreated[i]; }
    const hsBitVector& GetActiveBits() const override { return fActiveRefs; }

protected:
    void        AddNotifyCreated(plRefMsg* msg, plRefFlags::Type flags);
    void        ClearNotifyCreated();
    uint16_t      GetNumNotifyCreated() { return fNotifyCreated.GetCount(); }
    plRefMsg*   GetNotifyCreated(int i) { return fNotifyCreated[i]; }
    void        RemoveNotifyCreated(int i);

    uint16_t      IncActiveRefs() { return ++fNumActiveRefs; }
    uint16_t      DecActiveRefs() { return fNumActiveRefs ? --fNumActiveRefs : 0; }

    bool    IsActiveRef(int i) const            { return fActiveRefs.IsBitSet(i) != 0; }
    void    SetActiveRef(int i, bool on=true) { fActiveRefs.SetBit(i, on); }
    bool    IsNotified(int i) const             { return fNotified.IsBitSet(i) != 0; }
    void    SetNotified(int i, bool on=true)  { fNotified.SetBit(i, on); }

    void SatisfyPending(plRefMsg* msg) const;
    void SatisfyPending() const;

    void INotifySelf(hsKeyedObject* ko);
    void INotifyDestroyed();
    void IClearRefs();

    void IRelease(plKeyImp* keyImp);


    /**
     * The actual object pointer for this object.
     */
    hsKeyedObject* fObjectPtr;


    // These fields are the ones actually saved to disk:

    /**
     * The unique object identifier for this object.
     */
    plUoid fUoid;

    /**
     * The start position of this object in the data file.
     */
    uint32_t fStartPos;

    /**
     * The length of the object data in the data file.
     */
    uint32_t fDataLen;


    // Following used by hsResMgr to notify on defered load or when a passive ref is destroyed:

    /**
     * The number of active references to this object from plKeys.
     */
    uint16_t fNumActiveRefs;

    /**
     * A list of messages to send for notifying other objects when this object
     * has been created or destroyed.
     */
    hsTArray<plRefMsg*> fNotifyCreated;

    /**
     * Boolean vector indicating which of the fNotifyCreated messages are
     * active refs (and thus need to be notified on destruction).
     */
    hsBitVector fActiveRefs;

    /**
     * Boolean vector indicating which of the fNotifyCreated messages have
     * already been notified.
     */
    hsBitVector fNotified;

    mutable hsTArray<plKeyImp*> fRefs;          // refs I've made (to be released when I'm unregistered).
    mutable int16_t               fPendingRefs;   // Outstanding requests I have out.
    mutable hsTArray<plKeyImp*> fClones;        // clones of me
    mutable plKey               fCloneOwner;    // pointer for clones back to the owning key
};

#endif // hsRegistry_inc

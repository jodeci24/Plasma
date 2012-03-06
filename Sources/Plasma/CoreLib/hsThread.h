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
#ifndef hsThread_Defined
#define hsThread_Defined

#include "HeadSpin.h"

/**
 * Define hsMilliseconds as an unsigned integer.
 * @remark Should this be uint64_t?
 */
typedef uint32_t hsMilliseconds;


#ifdef HS_BUILD_FOR_UNIX
    #include <pthread.h>
    #include <semaphore.h>
    //  We can't wait with a timeout with semas
    #define USE_SEMA
    // Linux kernel 2.4 w/ NTPL threading patch and O(1) scheduler
    // seems to have a problem in it's cond_t implementation that
    // causes a hang under heavy load. This is a workaround that
    // uses select() and pipes.
//  #define PSEUDO_EVENT
#endif

/**
 * A base class for threaded execution.
 *
 * This class is implemented as a wrapper over the threading functionality of
 * the native operating system. On Windows, this uses Win32 threads; on *nix
 * it uses POSIX threads (pthreads).
 *
 * To run a thread, you should derive your own class, overriding the Run
 * method to do your work. You can start the thread by calling the Start
 * method, and stop the thread's execution with the Stop method.
 */
class hsThread
{
public:
#if HS_BUILD_FOR_WIN32
    /** ThreadId is a DWORD value in Win32. */
    typedef DWORD ThreadId;
#elif HS_BUILD_FOR_UNIX
    /** ThreadID is a pthread_t type in *nix. */
    typedef pthread_t ThreadId;
#endif

private:
    /** Boolean indicating if the thread is quitting. */
    bool        fQuit;

    /** The thread stack size, used only in Win32 threads. */
    uint32_t    fStackSize;

#if HS_BUILD_FOR_WIN32
    /** The Win32 thread ID. */
    ThreadId    fThreadId;

    /** The Win32 HANDLE to the thread. */
    HANDLE      fThreadH;

    /** The Win32 HANDLE to the quit semaphore. */
    HANDLE      fQuitSemaH;
#elif HS_BUILD_FOR_UNIX
    /** The pthread thread ID. */
    ThreadId    fPThread;

    /** Boolean indicating if the thread is valid. */
    bool        fIsValid;

    /** A pthread mutex used during initialization. */
    pthread_mutex_t fMutex;
#endif

protected:
    /**
     * Returns whether the thread is quitting.
     *
     * @return True if the thread is quitting, false otherwise.
     */
    bool GetQuit() const { return fQuit; }

    /**
     * Sets whether the thread is quitting.
     * Setting this with a value of true will result in the thread terminating.
     *
     * @param value Boolean whether to quit the thread.
     */
    void SetQuit(bool value) { fQuit = value; }

public:
    /**
     * Creates a new thread instance with an optional stack size.
     *
     * @param stackSize The stack size for the thread. This is unused and
     *                  ignored in *nix.
     */
    hsThread(uint32_t stackSize = 0);

    /**
     * Stops, destroys, and cleans up a thread instance.
     *
     * This will call the Stop method to terminate the thread if it is running.
     */
    virtual     ~hsThread();

#if HS_BUILD_FOR_WIN32
    /**
     * Gets the OS-specific ID for the thread instance.
     *
     * @return The thread ID.
     */
    ThreadId        GetThreadId() { return fThreadId; }

    /**
     * Gets the OS-specific ID for the currently executing thread.
     *
     * @return The ID of the current thread.
     */
    static ThreadId GetMyThreadId() { return GetCurrentThreadId(); }
#elif HS_BUILD_FOR_UNIX
    /**
     * Gets the OS-specific ID for the thread instance.
     *
     * @return The thread ID.
     */
    ThreadId            GetThreadId() { return fPThread; }

    /**
     * Gets the OS-specific ID for the currently executing thread.
     *
     * @return The ID of the current thread.
     */
    static ThreadId     GetMyThreadId() { return pthread_self(); }

    /**
     * Gets the mutex used during thread startup.
     *
     * @return The startup mutex.
     */
    pthread_mutex_t* GetStartupMutex() { return &fMutex;  }
#endif

    /**
     * Run the thread.
     *
     * This is a pure virtual method, which must be overridden in a derived
     * class to perform the threaded work.
     *
     * @return An hsError code: 0 for success.
     */
    virtual hsError Run() = 0;

    /**
     * Initializes and starts the thread.
     *
     * This sets up the thread for execution, and calls the Run method.
     */
    virtual void Start();

    /**
     * Signals the thread to stop execution.
     *
     * This sets the quitting value to true, and waits for the thread to
     * terminate.
     */
    virtual void Stop();

    //  Static functions

    /**
     * Allocates memory from the context of the current thread.
     * This does not call the new operator.
     *
     * @deprecated Do not use this method.
     * @param size The amount of memory to be allocated in bytes.
     * @return A pointer to the newly allocated memory, or NULL.
     */
    static void* Alloc(size_t size);

    /**
     * Frees memory from the context of the current thread.
     * This does not call the delete operator.
     *
     * @deprecated Do not use this method.
     * @param p The pointer to the memory to be freed.
     */
    static void Free(void* p);

    /**
     * Yield the current thread. UNIMPLEMENTED.
     *
     * @deprecated Do not use this method.
     */
    static void ThreadYield();
};

//////////////////////////////////////////////////////////////////////////////

class hsMutex {
#if HS_BUILD_FOR_WIN32
    HANDLE  fMutexH;
#elif HS_BUILD_FOR_UNIX
    pthread_mutex_t fPMutex;
#endif
public:
    hsMutex();
    virtual ~hsMutex();

#ifdef HS_BUILD_FOR_WIN32
    HANDLE GetHandle() const { return fMutexH; }
#endif

    void        Lock();
    bool        TryLock();
    void        Unlock();
};

class hsTempMutexLock {
    hsMutex*    fMutex;
public:
    hsTempMutexLock(hsMutex* mutex) : fMutex(mutex)
    {
        fMutex->Lock();
    }
    hsTempMutexLock(hsMutex& mutex) : fMutex(&mutex)
    {
        fMutex->Lock();
    }
    ~hsTempMutexLock()
    {
        fMutex->Unlock();
    }
};

//////////////////////////////////////////////////////////////////////////////

class hsSemaphore {
#if HS_BUILD_FOR_WIN32
    HANDLE  fSemaH;
#elif HS_BUILD_FOR_UNIX
#ifdef USE_SEMA
    sem_t*  fPSema;
    bool    fNamed;
#else
    pthread_mutex_t fPMutex;
    pthread_cond_t  fPCond;
    int32_t       fCounter;
#endif
#endif
public:
    hsSemaphore(int initialValue=0, const char* name=nil);
    ~hsSemaphore();

#ifdef HS_BUILD_FOR_WIN32
    HANDLE GetHandle() const { return fSemaH; }
#endif

    bool        TryWait();
    bool        Wait(hsMilliseconds timeToWait = kPosInfinity32);
    void        Signal();
};

//////////////////////////////////////////////////////////////////////////////
class hsEvent
{
#if HS_BUILD_FOR_UNIX
#ifndef PSEUDO_EVENT
    pthread_mutex_t fMutex;
    pthread_cond_t  fCond;
    bool  fTriggered;
#else
    enum { kRead, kWrite };
    int     fFds[2];
    hsMutex fWaitLock;
    hsMutex fSignalLock;
#endif // PSEUDO_EVENT
#elif HS_BUILD_FOR_WIN32
    HANDLE fEvent;
#endif
public:
    hsEvent();
    ~hsEvent();

#ifdef HS_BUILD_FOR_WIN32
    HANDLE GetHandle() const { return fEvent; }
#endif

    bool  Wait(hsMilliseconds timeToWait = kPosInfinity32);
    void  Signal();
};

//////////////////////////////////////////////////////////////////////////////
class hsSleep
{
public:
#if HS_BUILD_FOR_UNIX
    static void Sleep(uint32_t millis);

#elif HS_BUILD_FOR_WIN32
    static void Sleep(uint32_t millis) { ::Sleep(millis); }

#endif
};

//////////////////////////////////////////////////////////////////////////////
// Allows multiple readers, locks out readers for writing.

class hsReaderWriterLock
{
public:
    struct Callback
    {
        virtual void OnLockingForRead( hsReaderWriterLock * lock ) {}
        virtual void OnLockedForRead( hsReaderWriterLock * lock ) {}
        virtual void OnUnlockingForRead( hsReaderWriterLock * lock ) {}
        virtual void OnUnlockedForRead( hsReaderWriterLock * lock ) {}
        virtual void OnLockingForWrite( hsReaderWriterLock * lock ) {}
        virtual void OnLockedForWrite( hsReaderWriterLock * lock ) {}
        virtual void OnUnlockingForWrite( hsReaderWriterLock * lock ) {}
        virtual void OnUnlockedForWrite( hsReaderWriterLock * lock ) {}
    };
    hsReaderWriterLock( const char * name="<unnamed>", Callback * cb=nil );
    ~hsReaderWriterLock();
    void LockForReading();
    void UnlockForReading();
    void LockForWriting();
    void UnlockForWriting();
    const char * GetName() const { return fName; }

private:
    int     fReaderCount;
    hsMutex fReaderCountLock;
    hsMutex fReaderLock;
    hsSemaphore fWriterSema;
    Callback *  fCallback;
    char *  fName;
};

class hsLockForReading
{
    hsReaderWriterLock * fLock;
public:
    hsLockForReading( hsReaderWriterLock & lock ): fLock( &lock )
    {
        fLock->LockForReading();
    }
    hsLockForReading( hsReaderWriterLock * lock ): fLock( lock )
    {
        fLock->LockForReading();
    }
    ~hsLockForReading()
    {
        fLock->UnlockForReading();
    }
};

class hsLockForWriting
{
    hsReaderWriterLock * fLock;
public:
    hsLockForWriting( hsReaderWriterLock & lock ): fLock( &lock )
    {
        fLock->LockForWriting();
    }
    hsLockForWriting( hsReaderWriterLock * lock ): fLock( lock )
    {
        fLock->LockForWriting();
    }
    ~hsLockForWriting()
    {
        fLock->UnlockForWriting();
    }
};

#endif


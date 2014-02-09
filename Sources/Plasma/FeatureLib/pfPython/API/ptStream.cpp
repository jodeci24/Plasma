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
#include "ptStream.h"

#include "hsStream.h"
#include "plFileSystem.h"
#include "plFile/plEncryptedStream.h"

#include "pyHelpers.h"


ptStream::ptStream(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
: Py::PythonClass<ptStream>::PythonClass(self, args, kwds),
  fStream(nullptr)
{
}


ptStream::~ptStream()
{
    if (fStream) {
        fStream->Close();
        delete fStream;
    }
}


PYCXX_NOARGS_METHOD_DECL(ptStream, isOpen)
Py::Object ptStream::isOpen()
{
    if (fStream != nullptr) {
        return Py::True();
    }

    return Py::False();
}


PYCXX_VARARGS_METHOD_DECL(ptStream, open)
Py::Object ptStream::open(const Py::Tuple& args)
{
    plFileName filename;
    plString flags;

    try {
        args.verify_length(2);

        filename = PyString_plStringify(args[0]);
        flags = PyString_plStringify(args[1]);
    } catch (const Py::Exception&) {
        return Py::Object(nullptr);
    }

    if (fStream) {
        fStream->Close();
        delete fStream;
    }
    fStream = nullptr;

    if (filename.IsValid())
    {
        // Read
        bool r = flags.Find('r', plString::kCaseInsensitive) != -1;
        // Write
        bool w = flags.Find('w', plString::kCaseInsensitive) != -1;
        // Encrypt
        bool e = flags.Find('e', plString::kCaseInsensitive) != -1;

        // If there is a write flag, it takes priority over read
        if (w)
        {
            if (e)
            {
                fStream = new plEncryptedStream();
                fStream->Open(filename, "wb");
            }
            else
            {
                fStream = plEncryptedStream::OpenEncryptedFileWrite(filename);
            }
        }
        else
        {
            fStream = plEncryptedStream::OpenEncryptedFile(filename);
        }

        return Py::True();
    }

    return Py::False();
}


PYCXX_NOARGS_METHOD_DECL(ptStream, close)
Py::Object ptStream::close()
{
    if (fStream) {
        fStream->Close();
        delete fStream;
    }

    fStream = nullptr;

    return Py::None();
}


PYCXX_NOARGS_METHOD_DECL(ptStream, readlines)
Py::Object ptStream::readlines()
{
    Py::List lines;

    if (fStream)
    {
        char buffer[4096];

        while (!fStream->AtEnd())
        {
            if (fStream->ReadLn(buffer, sizeof(buffer), 0, 0))
            {
                Py::String str(buffer);
                lines.append(str);
            }
        }
    }

    return lines;
}


PYCXX_VARARGS_METHOD_DECL(ptStream, writelines)
Py::Object ptStream::writelines(const Py::Tuple& args)
{
    Py::List lines;

    try {
        args.verify_length(1);

        lines = args[0];
    } catch (Py::Exception&) {
        return Py::Object(nullptr);
    }

    if (!fStream) {
        return Py::False();
    }

    for (auto it = lines.begin(); it != lines.end(); ++it)
    {
        Py::String str(*it);
        plString line = PyString_plStringify(str);

        fStream->WriteString(line);
    }

    return Py::True();
}


void ptStream::init_type()
{
    behaviors().name("ptStream");
    behaviors().doc("A basic stream class");

    PYCXX_ADD_NOARGS_METHOD(isOpen, isOpen,
            "Returns whether the stream file is currently opened");

    PYCXX_ADD_VARARGS_METHOD(open, open,
            "Params: filename, flags"
            "\n"
            "Opens a stream file for reading or writing");

    PYCXX_ADD_NOARGS_METHOD(close, close,
            "Close the stream file");


    PYCXX_ADD_NOARGS_METHOD(readlines, readlines,
            "Reads a list of strings from the file");


    PYCXX_ADD_VARARGS_METHOD(writelines, writelines,
            "Params: lines"
            "\n"
            "Write a list of strings to the file");

    behaviors().readyType();
}

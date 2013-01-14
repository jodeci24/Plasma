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

#ifndef _MaxConvert_Pch_inc_
#define _MaxConvert_Pch_inc_

/** 
 * \file Pch.h
 * \brief Precompiled Header for MaxConvert
 */

// Standard Library
#include <algorithm>
#include <float.h>
#include <math.h>
#include <string.h>
#include <vector>

// Core Plasma
#include "HeadSpin.h"
#include "hsBitVector.h"
#include "hsColorRGBA.h"
#include "plgDispatch.h"
#include "hsExceptionStack.h"
#include "hsFastMath.h"
#include "hsGeometry3.h"
#include "hsMatrix44.h"
#include "hsResMgr.h"
#include "plString.h"
#include "hsStringTokenizer.h"
#include "hsTemplates.h"
#include "plTweak.h"

// Windows
#include "hsWindows.h"
#include <CommCtrl.h>
#include <commdlg.h>

// MaxComponent
#include "MaxComponent/plComponent.h"

// 3ds Max SDK
// This stuff should ALWAYS come after hsWindows.h
#include <bmmlib.h>
#include <dummy.h>
#include <keyreduc.h>
#include <INode.h>
#include <ISkin.h>
#include <istdplug.h>
#include <iparamm2.h>
#include <maxversion.h>
#include <meshdlib.h> 
#include <modstack.h>
#include <notify.h>
#include <stdmat.h>
#include <texutil.h>

#if MAX_VERSION_MAJOR >= 13
#    include <INamedSelectionSetManager.h>
#endif

#endif // _MaxConvert_Pch_inc_
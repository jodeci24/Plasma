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

#include "qtMipmap.h"
#include "plGImage/plMipmap.h"

#include <QGridLayout>


PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB = nullptr;

static QGLFormat s_format = QGL::DepthBuffer | QGL::StencilBuffer
                          | QGL::Rgba | QGL::AlphaChannel | QGL::DoubleBuffer;

qtMipmapRenderer::qtMipmapRenderer(QWidget* parent, plMipmap* tex)
    : QGLWidget(s_format, parent), fTexture(tex)
{
}

void qtMipmapRenderer::initializeGL()
{
    initializeGLFunctions();

    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    if (glCompressedTexImage2DARB == nullptr)
        glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)context()->getProcAddress("glCompressedTexImage2DARB");


    initializeShaders();

    glGenBuffers(1, &fVboID);

    float vertices[] = {
        // Position     UV Coords
        -1.0f,  1.0f,   0.0f, 1.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
        -1.0f, -1.0f,   0.0f, 0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, fVboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &fEboID);

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fEboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glGenTextures(1, &fTexID);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fTexID);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (fTexture->IsCompressed()) {
        GLuint dxCompression = 0;
        uint8_t compType = fTexture->fDirectXInfo.fCompressionType;

        if (compType == plBitmap::DirectXInfo::kDXT1)
            dxCompression = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        else if (compType == plBitmap::DirectXInfo::kDXT5)
            dxCompression = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

        if (glCompressedTexImage2DARB != nullptr) {
            for (uint8_t i = 0; i < fTexture->GetNumLevels(); i++) {
                fTexture->SetCurrLevel(i);

                glCompressedTexImage2DARB(GL_TEXTURE_2D, i, dxCompression,
                                        fTexture->GetCurrWidth(), fTexture->GetCurrHeight(),
                                        0, fTexture->GetCurrLevelSize(), fTexture->GetCurrLevelPtr());
            }
        } else {
            return;
        }
    } else {
        // TODO
        return;
    }
}

void qtMipmapRenderer::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}

void qtMipmapRenderer::initializeShaders()
{
    setlocale(LC_NUMERIC, "C");

    if (!program.addShaderFromSourceFile(QGLShader::Vertex, ":/shaders/qtMipmapRenderer_vshader.glsl"))
        close();

    if (!program.addShaderFromSourceFile(QGLShader::Fragment, ":/shaders/qtMipmapRenderer_fshader.glsl"))
        close();

    if (!program.link())
        close();

    if (!program.bind())
        close();

    setlocale(LC_ALL, "");
}

void qtMipmapRenderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBindBuffer(GL_ARRAY_BUFFER, fVboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fEboID);

    int vertexLocation = program.attributeLocation("aVertexPosition");
    program.enableAttributeArray(vertexLocation);
    glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)0);

    int texcoordLocation = program.attributeLocation("aTextureCoord");
    program.enableAttributeArray(texcoordLocation);
    glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)(2 * sizeof(float)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fTexID);
    glEnable(GL_BLEND);

    int texLocation = program.uniformLocation("uSampler");
    program.setUniformValue(texLocation, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}



qtMipmap::qtMipmap(plCreatable* pCre, QWidget* parent)
    : qtCreatable(pCre, CLASS_INDEX_SCOPED(plMipmap), parent)
{
    plMipmap* mipmap = plMipmap::ConvertNoRef(pCre);
    fRenderer = new qtMipmapRenderer(this, mipmap);

    QGridLayout* layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setVerticalSpacing(0);
    layout->addWidget(fRenderer, 0, 0);
}

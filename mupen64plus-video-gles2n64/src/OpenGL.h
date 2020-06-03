#ifndef OPENGL_H
#define OPENGL_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "gSP.h"

#ifdef USE_SDL
//    #include <EGL/egl.h>  // Android 2.3 only
//    #include <GLES2/gl2extimg.h>
    #include <SDL.h>
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define RS_NONE         0
#define RS_TRIANGLE     1
#define RS_RECT         2
#define RS_TEXTUREDRECT 3
#define RS_LINE         4


#define SCREEN_UPDATE_AT_VI_UPDATE              1
#define SCREEN_UPDATE_AT_VI_CHANGE              2
#define SCREEN_UPDATE_AT_CI_CHANGE              3
#define SCREEN_UPDATE_AT_1ST_CI_CHANGE          4
#define SCREEN_UPDATE_AT_1ST_PRIMITIVE          5
#define SCREEN_UPDATE_BEFORE_SCREEN_CLEAR       6
#define SCREEN_UPDATE_AT_VI_UPDATE_AND_DRAWN    7


#define BLEND_NOOP              0x0000
#define BLEND_NOOP5             0xcc48  // Fog * 0 + Mem * 1
#define BLEND_NOOP4             0xcc08  // Fog * 0 + In * 1
#define BLEND_FOG_ASHADE        0xc800
#define BLEND_FOG_3             0xc000  // Fog * AIn + In * 1-A
#define BLEND_FOG_MEM           0xc440  // Fog * AFog + Mem * 1-A
#define BLEND_FOG_APRIM         0xc400  // Fog * AFog + In * 1-A
#define BLEND_BLENDCOLOR        0x8c88
#define BLEND_BI_AFOG           0x8400  // Bl * AFog + In * 1-A
#define BLEND_BI_AIN            0x8040  // Bl * AIn + Mem * 1-A
#define BLEND_MEM               0x4c40  // Mem*0 + Mem*(1-0)?!
#define BLEND_FOG_MEM_3         0x44c0  // Mem * AFog + Fog * 1-A
#define BLEND_NOOP3             0x0c48  // In * 0 + Mem * 1
#define BLEND_PASS              0x0c08  // In * 0 + In * 1
#define BLEND_FOG_MEM_IN_MEM    0x0440  // In * AFog + Mem * 1-A
#define BLEND_FOG_MEM_FOG_MEM   0x04c0  // In * AFog + Fog * 1-A
#define BLEND_OPA               0x0044  //  In * AIn + Mem * AMem
#define BLEND_XLU               0x0040
#define BLEND_MEM_ALPHA_IN      0x4044  //  Mem * AIn + Mem * AMem


#define OGL_FRAMETIME_NUM       8

struct GLVertex
{
    float x, y, z, w;
    struct
    {
        float r, g, b, a;
    } color, secondaryColor;
    float s0, t0, s1, t1;
};

struct GLcolor
{
    float r, g, b, a;
};

struct GLInfo
{
#ifdef USE_SDL
// TODO: More EGL stuff, need to do this in Java
    SDL_Surface *hScreen;  // TODO: Do we really need this?  Only using it in one place AFAICT..
/*
    struct
    {
        EGLint		            version_major, version_minor;
        EGLDisplay              display;
        EGLContext              context;
        EGLConfig               config;
        EGLSurface              surface;
        EGLNativeDisplayType    device;
        EGLNativeWindowType     handle;
    } EGL;
*/
#endif

    bool    screenUpdate;

    struct
    {
        GLuint fb,depth_buffer, color_buffer;
    } framebuffer;


    int     frameSkipped;
    unsigned consecutiveSkips;
    unsigned frameTime[OGL_FRAMETIME_NUM];

    int     frame_vsync, frame_actual, frame_dl;
    int     frame_prevdl;
    int     mustRenderDlist;
    int     renderingToTexture;


    GLint   defaultProgram;
    GLint   defaultVertShader;
    GLint   defaultFragShader;

    float   scaleX, scaleY;

#define INDEXMAP_SIZE 64
#define VERTBUFF_SIZE 256
#define ELEMBUFF_SIZE 1024

    struct {
        SPVertex    vertices[VERTBUFF_SIZE];
        GLubyte     elements[ELEMBUFF_SIZE];
        int         num;

//#ifdef __TRIBUFFER_OPT

        u32     indexmap[INDEXMAP_SIZE];
        u32     indexmapinv[VERTBUFF_SIZE];
        u32     indexmap_prev;
        u32     indexmap_nomap;
//#endif

    } triangles;


    unsigned int    renderState;

    GLVertex rect[4];
};

extern GLInfo OGL;

bool OGL_Start();
void OGL_Stop();

void OGL_AddTriangle(int v0, int v1, int v2);
void OGL_DrawTriangles();
void OGL_DrawTriangle(SPVertex *vertices, int v0, int v1, int v2);
void OGL_DrawLine(int v0, int v1, float width);
void OGL_DrawRect(int ulx, int uly, int lrx, int lry, float *color);
void OGL_DrawTexturedRect(float ulx, float uly, float lrx, float lry, float uls, float ult, float lrs, float lrt, bool flip );

void OGL_UpdateFrameTime();
void OGL_UpdateScale();
void OGL_UpdateStates();
void OGL_UpdateViewport();
void OGL_UpdateScissor();
void OGL_UpdateCullFace();

void OGL_ClearDepthBuffer();
void OGL_ClearColorBuffer(float *color);
void OGL_ResizeWindow(int x, int y, int width, int height);
void OGL_SwapBuffers();
void OGL_ReadScreen( void *dest, int *width, int *height );

int  OGL_CheckError();
int  OGL_IsExtSupported( const char *extension );

#ifdef FAKE_RENDERING

#define glTexImage2D fake_glTexImage2D
static void fake_glTexImage2D(GLenum target, GLint level, GLint internalFormat,
                              GLsizei width, GLsizei height, GLint border,
                              GLenum format, GLenum type, const GLvoid * data)
            __attribute__((used));

static void fake_glTexImage2D(GLenum target, GLint level, GLint internalFormat,
                              GLsizei width, GLsizei height, GLint border,
                              GLenum format, GLenum type, const GLvoid * data)
{
}

#define glDrawElements fake_glDrawElements
static void fake_glDrawElements(GLenum mode, GLsizei count, GLenum type,
                                const GLvoid * indices)
            __attribute__((used));

static void fake_glDrawElements(GLenum mode, GLsizei count, GLenum type,
                                const GLvoid * indices)
{
}

#define glDrawArrays fake_glDrawArrays
static void fake_glDrawArrays(GLenum mode, GLint first, GLsizei count)
            __attribute__((used));

static void fake_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
}

#define glUniform1f fake_glUniform1f
static void fake_glUniform1f(GLint location, GLfloat v0)
            __attribute__((used));

static void fake_glUniform1f(GLint location, GLfloat v0)
{
}

#define glUniform2f fake_glUniform2f
static void fake_glUniform2f(GLint location, GLfloat v0, GLfloat v1)
            __attribute__((used));

static void fake_glUniform2f(GLint location, GLfloat v0, GLfloat v1)
{
}

#define glUniform1i fake_glUniform1i
static void fake_glUniform1i(GLint location, GLint v0)
            __attribute__((used));

static void fake_glUniform1i(GLint location, GLint v0)
{
}

#define glUniform2i fake_glUniform2i
static void fake_glUniform2i(GLint location, GLint v0, GLint v1)
            __attribute__((used));

static void fake_glUniform2i(GLint location, GLint v0, GLint v1)
{
}

#define glUniform4fv fake_glUniform4fv
static void fake_glUniform4fv(GLint location, GLsizei count,
                              const GLfloat *value)
            __attribute__((used));

static void fake_glUniform4fv(GLint location, GLsizei count,
                              const GLfloat *value)
{
}

#define glTexParameteri fake_glTexParameteri
static void fake_glTexParameteri(GLenum target, GLenum pname, GLint param)
            __attribute__((used));

static void fake_glTexParameteri(GLenum target, GLenum pname, GLint param)
{
}

#define glEnable fake_glEnable
static void fake_glEnable(GLenum cap)
            __attribute__((used));

static void fake_glEnable(GLenum cap)
{
}

#define glDisable fake_glDisable
static void fake_glDisable(GLenum cap)
            __attribute__((used));

static void fake_glDisable(GLenum cap)
{
}

#define glActiveTexture fake_glActiveTexture
static void fake_glActiveTexture(GLenum texunit)
            __attribute__((used));

static void fake_glActiveTexture(GLenum texunit)
{
}

#define glBindTexture fake_glBindTexture
static void fake_glBindTexture(GLenum texunit, GLuint texture)
            __attribute__((used));

static void fake_glBindTexture(GLenum texunit, GLuint texture)
{
}

#define glDepthFunc fake_glDepthFunc
static void fake_glDepthFunc(GLenum func)
            __attribute__((used));

static void fake_glDepthFunc(GLenum func)
{
}

#define glDepthMask fake_glDepthMask
static void fake_glDepthMask(GLboolean flag)
            __attribute__((used));

static void fake_glDepthMask(GLboolean flag)
{
}

#define glUseProgram fake_glUseProgram
static void fake_glUseProgram(GLuint program)
            __attribute__((used));

static void fake_glUseProgram(GLuint program)
{
}

#define glClearDepthf fake_glClearDepthf
static void fake_glClearDepthf(GLclampf depth)
            __attribute__((used));

static void fake_glClearDepthf(GLclampf depth)
{
}

#define glClearColor fake_glClearColor
static void fake_glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
            __attribute__((used));

static void fake_glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
}

#define glClear fake_glClear
static void fake_glClear(GLbitfield mask)
            __attribute__((used));

static void fake_glClear(GLbitfield mask)
{
}

#define glViewport fake_glViewport
static void fake_glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
            __attribute__((used));

static void fake_glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
}

#define glScissor fake_glScissor
static void fake_glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
            __attribute__((used));

static void fake_glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
}

#define glBlendFunc fake_glBlendFunc
static void fake_glBlendFunc(GLenum sfactor, GLenum dfactor)
            __attribute__((used));

static void fake_glBlendFunc(GLenum sfactor, GLenum dfactor)
{
}

#define glEnableVertexAttribArray fake_glEnableVertexAttribArray
static void fake_glEnableVertexAttribArray(GLuint index)
            __attribute__((used));

static void fake_glEnableVertexAttribArray(GLuint index)
{
}

#define glDisableVertexAttribArray fake_glDisableVertexAttribArray
static void fake_glDisableVertexAttribArray(GLuint index)
            __attribute__((used));

static void fake_glDisableVertexAttribArray(GLuint index)
{
}

#define glVertexAttrib4f fake_glVertexAttrib4f
static void fake_glVertexAttrib4f(GLuint index, GLfloat v0, GLfloat v1,
                                  GLfloat v2, GLfloat v3)
            __attribute__((used));

static void fake_glVertexAttrib4f(GLuint index, GLfloat v0, GLfloat v1,
                                  GLfloat v2, GLfloat v3)
{
}

#define glVertexAttrib4fv fake_glVertexAttrib4fv
static void fake_glVertexAttrib4fv(GLuint index, const GLfloat *v)
            __attribute__((used));

static void fake_glVertexAttrib4fv(GLuint index, const GLfloat *v)
{
}

#endif /* FAKE_RENDERING */

#endif


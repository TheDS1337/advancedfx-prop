// Project :  Half-Life Advanced Effects
// File    :  dll/supportrender.h

// Authors : last change / first change / name
// 2008-03-28 / 2008-03-27 / Dominik Tugend

// Comment: see supportrender.h

#include <windows.h>

#include <gl\gl.h>
#include <gl\glu.h>
#include "../shared\ogl\glext.h"

#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
//#include "r_efx.h"
//#include "com_model.h"
//#include "r_studioint.h"
//#include "pm_defs.h"
//#include "cvardef.h"
//#include "entity_types.h"

#include "supportrender.h"

extern cl_enginefuncs_s *pEngfuncs;

//#define ERROR_MESSAGE(errmsg) pEngfuncs->Con_Printf("SupportRender: " errmsg "\n");

char g_ErrorMessageBuffer[300];

#define ERROR_MESSAGE(errmsg) MessageBoxA(0, errmsg, "SupportRender:",MB_OK|MB_ICONERROR);
#define ERROR_MESSAGE_LE(errmsg) \
	{ \
		_snprintf(g_ErrorMessageBuffer,sizeof(g_ErrorMessageBuffer)-1,"SupportRender:" errmsg "\nGetLastError: %i",::GetLastError()); \
		g_ErrorMessageBuffer[sizeof(g_ErrorMessageBuffer)-1]=0; \
		MessageBoxA(0, g_ErrorMessageBuffer, "SupportRender:",MB_OK|MB_ICONERROR); \
	}


//
// EXT_framebuffer_object
//
//
// http://www.opengl.org/registry/specs/EXT/framebuffer_object.txt
//

bool g_bEXT_framebuffer_object=false;

PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT = NULL;
PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT = NULL;

CHlaeSupportRender::EFboSupport InstallFrameBufferExtentsion(void)
// ATTENTION:
//   do not install before a context has been created, because
//   in this case glGetString will not work (return 0)
{
	if (g_bEXT_framebuffer_object) return CHlaeSupportRender::FBOS_YES;

	CHlaeSupportRender::EFboSupport retTemp=CHlaeSupportRender::FBOS_NO;
	char *pExtStr = (char *)(glGetString( GL_EXTENSIONS ));
	if (!pExtStr)
	{
		retTemp = CHlaeSupportRender::FBOS_UNKNOWN;
		ERROR_MESSAGE("glGetString failed!\nInstallFrameBufferExtentsion called before context was currrent?")
	}
	else if (strstr( pExtStr, "EXT_framebuffer_object" ))
	{
		glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
		glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
		glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
		glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
		glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
		glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
		glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)wglGetProcAddress("glIsFramebufferEXT");
		glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
		glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
		glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
		glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
		glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
		glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
		glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
		glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
		glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
		glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");

		if (
			glIsRenderbufferEXT && glBindRenderbufferEXT && glDeleteRenderbuffersEXT && glGenRenderbuffersEXT
			&& glRenderbufferStorageEXT && glGetRenderbufferParameterivEXT && glIsFramebufferEXT && glBindFramebufferEXT
			&& glDeleteFramebuffersEXT && glGenFramebuffersEXT && glCheckFramebufferStatusEXT && glFramebufferTexture1DEXT
			&& glFramebufferTexture2DEXT && glFramebufferTexture3DEXT && glFramebufferRenderbufferEXT && glGetFramebufferAttachmentParameterivEXT
			&& glGenerateMipmapEXT
		) retTemp = CHlaeSupportRender::FBOS_YES;


	}

	g_bEXT_framebuffer_object = CHlaeSupportRender::FBOS_YES == retTemp;

	return retTemp;
}

//
//  CHlaeSupportRender
//

CHlaeSupportRender::CHlaeSupportRender(HWND hGameWindow, int iWidth, int iHeight)
{
	_hGameWindow = hGameWindow;
	_iWidth = iWidth;
	_iHeight = iHeight;

	_eRenderTarget = RT_NULL;
	_eFBOsupported = FBOS_UNKNOWN;

	_ownHGLRC = NULL;
}

CHlaeSupportRender::~CHlaeSupportRender()
{
	if (RT_NULL != _eRenderTarget) hlaeDeleteContext (_ownHGLRC);
	_eRenderTarget = RT_NULL;
}

CHlaeSupportRender::EFboSupport CHlaeSupportRender::Has_EXT_FrameBufferObject()
{
	return _eFBOsupported;
}

CHlaeSupportRender::ERenderTarget CHlaeSupportRender::GetRenderTarget()
{
	return _eRenderTarget;
}

HGLRC CHlaeSupportRender::GetHGLRC()
{
	return _ownHGLRC;
}

HDC	CHlaeSupportRender::GetInternalHDC()
{
	switch (_eRenderTarget)
	{
	case RT_MEMORYDC:
		return _MemoryDc_r.ownHDC;
	case RT_HIDDENWINDOW:
		return _HiddenWindow_r.ownHDC;
	}
	return NULL;
}

HWND CHlaeSupportRender::GetInternalHWND()
{
	if (_eRenderTarget == RT_HIDDENWINDOW)
		return _HiddenWindow_r.ownHWND;

	return NULL;
}

HGLRC CHlaeSupportRender::hlaeCreateContext (ERenderTarget eRenderTarget, HDC hGameWindowDC)
{
	if(RT_NULL != _eRenderTarget)
	{
		ERROR_MESSAGE("already using a target")
		return NULL;
	}

	switch (eRenderTarget)
	{
	case RT_NULL:
		ERROR_MESSAGE("cannot Create RT_NULL target")
		return NULL;
	case RT_GAMEWINDOW:
		return _Create_RT_GAMEWINDOW (hGameWindowDC);
	case RT_MEMORYDC:
		return _Create_RT_MEMORYDC (hGameWindowDC);
	case RT_FRAMEBUFFEROBJECT:
		return _Create_RT_FRAMEBUFFEROBJECT (hGameWindowDC);
	}

	ERROR_MESSAGE("cannot Create unknown target")
	return NULL;
}

BOOL CHlaeSupportRender::hlaeDeleteContext (HGLRC hGlRc)
{
	if (RT_NULL==_eRenderTarget)
	{
		ERROR_MESSAGE("cannot Delete RT_NULL target")
		return FALSE;
	}
	else if (hGlRc != _ownHGLRC)
	{
		ERROR_MESSAGE("cannot Delete, hGlRc is not managed by this class")
		return FALSE;
	}

	switch (_eRenderTarget)
	{
	case RT_GAMEWINDOW:
		return _Delete_RT_GAMEWINDOW ();
	case RT_MEMORYDC:
		return _Delete_RT_MEMORYDC ();
	case RT_FRAMEBUFFEROBJECT:
		return _Delete_RT_FRAMEBUFFEROBJECT ();
	}

	ERROR_MESSAGE("cannot delete unknown target")
	return FALSE;
}

BOOL CHlaeSupportRender::hlaeMakeCurrent(HDC hGameWindowDC, HGLRC hGlRc)
{
	if (RT_NULL==_eRenderTarget)
	{
		ERROR_MESSAGE("cannot MakeCurrent RT_NULL target")
		return FALSE;
	}
	else if (hGlRc != _ownHGLRC)
	{
		ERROR_MESSAGE("cannot MakeCurrent, hGlRc is not managed by this class")
		return FALSE;
	}

	switch (_eRenderTarget)
	{
	case RT_GAMEWINDOW:
		return wglMakeCurrent(hGameWindowDC,_ownHGLRC);
	case RT_MEMORYDC:
		return _MakeCurrent_RT_MEMORYDC (hGameWindowDC);
	case RT_FRAMEBUFFEROBJECT:
		return _MakeCurrent_RT_FRAMEBUFFEROBJECT (hGameWindowDC);
	}

	ERROR_MESSAGE("cannot MakeCurrent unknown target")
	return FALSE;
}

BOOL CHlaeSupportRender::hlaeSwapBuffers(HDC hGameWindowDC)
{
	if (RT_NULL==_eRenderTarget)
	{
		ERROR_MESSAGE("cannot SwapBuffers RT_NULL target")
		return FALSE;
	}

	switch (_eRenderTarget)
	{
	case RT_GAMEWINDOW:
		return SwapBuffers (hGameWindowDC);
	case RT_MEMORYDC:
		return _SwapBuffers_RT_MEMORYDC (hGameWindowDC);
	case RT_FRAMEBUFFEROBJECT:
		return SwapBuffers (hGameWindowDC);
	}

	ERROR_MESSAGE("cannot SwapBuffers unknown target")
	return FALSE;
}

HGLRC CHlaeSupportRender::_Create_RT_GAMEWINDOW (HDC hGameWindowDC)
{
	_ownHGLRC = wglCreateContext(hGameWindowDC);

	if(_ownHGLRC) _eRenderTarget=RT_GAMEWINDOW;

	return _ownHGLRC;
}

BOOL CHlaeSupportRender::_Delete_RT_GAMEWINDOW ()
{
	BOOL wbRet = wglDeleteContext(_ownHGLRC);

	if (TRUE != wbRet)
		return wbRet;

	_eRenderTarget=RT_NULL;
	_ownHGLRC=NULL;
	
	return wbRet;
}

HGLRC CHlaeSupportRender::_Create_RT_MEMORYDC (HDC hGameWindowDC)
{
	_MemoryDc_r.ownHDC = CreateCompatibleDC(hGameWindowDC);
	if (!_MemoryDc_r.ownHDC)
	{
		ERROR_MESSAGE_LE("could not create compatible context")
		return NULL;
	}

    _MemoryDc_r.ownHBITMAP = CreateCompatibleBitmap ( hGameWindowDC, _iWidth, _iHeight );
	if (!_MemoryDc_r.ownHBITMAP)
	{
		ERROR_MESSAGE_LE("could not create compatible bitmap")
		DeleteDC(_MemoryDc_r.ownHDC);
		return NULL;
	}

    HGDIOBJ tobj = SelectObject ( _MemoryDc_r.ownHDC, _MemoryDc_r.ownHBITMAP );
	if (!tobj || tobj == HGDI_ERROR)
	{
		ERROR_MESSAGE_LE("could not select bitmap")
		DeleteObject(_MemoryDc_r.ownHBITMAP);
		DeleteDC(_MemoryDc_r.ownHDC);
		return NULL;
	}

	int iPixelFormat = GetPixelFormat( hGameWindowDC );
	PIXELFORMATDESCRIPTOR *ppfd = new (PIXELFORMATDESCRIPTOR);
	DescribePixelFormat( hGameWindowDC, iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), ppfd );

	ppfd->dwFlags = (ppfd->dwFlags & !( (DWORD)PFD_DRAW_TO_WINDOW | (DWORD)PFD_DOUBLEBUFFER  )) | PFD_DRAW_TO_BITMAP;
	//ppfd->dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED;

	iPixelFormat = ChoosePixelFormat(_MemoryDc_r.ownHDC,ppfd);
	if (iPixelFormat == 0)
	{
		ERROR_MESSAGE_LE("could not choose PixelFormat")
		delete ppfd;
		DeleteObject(_MemoryDc_r.ownHBITMAP);
		DeleteDC(_MemoryDc_r.ownHDC);
		return NULL;
	}

	if (TRUE != SetPixelFormat(_MemoryDc_r.ownHDC,iPixelFormat,ppfd))
	{
		ERROR_MESSAGE_LE("could not Set PixelFormat")
		delete ppfd;
		DeleteObject(_MemoryDc_r.ownHBITMAP);
		DeleteDC(_MemoryDc_r.ownHDC);
		return NULL;
	}

	delete ppfd;

	_ownHGLRC = wglCreateContext(_MemoryDc_r.ownHDC);
	if (!_ownHGLRC)
	{
		ERROR_MESSAGE_LE("could not create own context")
		DeleteObject(_MemoryDc_r.ownHBITMAP);
		DeleteDC(_MemoryDc_r.ownHDC);
		return NULL;
	}

	_eRenderTarget=RT_MEMORYDC;

	return _ownHGLRC;
}

BOOL CHlaeSupportRender::_Delete_RT_MEMORYDC ()
{
	BOOL wbRet = wglDeleteContext(_ownHGLRC);
	
	if (TRUE != wbRet)
		return wbRet;

	_eRenderTarget=RT_NULL;
	_ownHGLRC=NULL;

	DeleteObject(_MemoryDc_r.ownHBITMAP);
	DeleteDC(_MemoryDc_r.ownHDC);

	return wbRet;
}

BOOL CHlaeSupportRender::_MakeCurrent_RT_MEMORYDC (HDC hGameWindowDC)
{
	return wglMakeCurrent( _MemoryDc_r.ownHDC, _ownHGLRC );
}

BOOL CHlaeSupportRender::_SwapBuffers_RT_MEMORYDC (HDC hGameWindowDC)
{
	BOOL bwRet=FALSE;
	bwRet=BitBlt(hGameWindowDC,0,0,_iWidth,_iHeight,_MemoryDc_r.ownHDC,0,0,SRCCOPY);
	//SwapBuffers(hGameWindowDC);
	return bwRet;
}

HGLRC CHlaeSupportRender::_Create_RT_FRAMEBUFFEROBJECT (HDC hGameWindowDC)
{
	_ownHGLRC = wglCreateContext(hGameWindowDC);
	
	if(_ownHGLRC) _eRenderTarget=RT_FRAMEBUFFEROBJECT;

	return _ownHGLRC;
}

BOOL CHlaeSupportRender::_Delete_RT_FRAMEBUFFEROBJECT ()
{
	_Delete_RT_FRAMEBUFFEROBJECT_onlyFBO ();

	BOOL wbRet = wglDeleteContext(_ownHGLRC);

	if (TRUE != wbRet)
		return wbRet;

	_eRenderTarget=RT_NULL;
	_ownHGLRC=NULL;
	
	return wbRet;
}

BOOL CHlaeSupportRender::_MakeCurrent_RT_FRAMEBUFFEROBJECT (HDC hGameWindowDC)
{
	BOOL bwResult = wglMakeCurrent( hGameWindowDC, _ownHGLRC );

	// since we have a current context we can also query the extensions now:
	// (WHY DOES THIS NOT WORK EARLIER BTW?, CAN WE DETECT THAT EARLIER?)
	_eFBOsupported = InstallFrameBufferExtentsion();

	if (FBOS_YES != _eFBOsupported)
	{
		if (FBOS_NO == _eFBOsupported)
			ERROR_MESSAGE("EXT_FrameBufferObject not supported!\nFalling back to RT_GAMEWINDOW ...")
		else
			ERROR_MESSAGE("EXT_FrameBufferObject support could not be evaluated!\nFalling back to RT_GAMEWINDOW ...")
		
		_eRenderTarget = RT_GAMEWINDOW;

		return bwResult;
	}

	// create FrameBufferObject:
	glGenFramebuffersEXT( 1, &_FrameBufferObject_r.FBOid );

	// create depthRenderBuffer:
	glGenRenderbuffersEXT( 1, &_FrameBufferObject_r.depthRenderBuffer );
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, _FrameBufferObject_r.depthRenderBuffer );
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, _iWidth, _iHeight );

	// create rgbaRenderBuffer:
	glGenRenderbuffersEXT( 1, &_FrameBufferObject_r.rgbaRenderBuffer );
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, _FrameBufferObject_r.rgbaRenderBuffer );
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_RGBA, _iWidth, _iHeight );

	// bind and setup FrameBufferObject (select buffers):
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, _FrameBufferObject_r.FBOid );
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _FrameBufferObject_r.depthRenderBuffer );
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, _FrameBufferObject_r.rgbaRenderBuffer );

	// check if FBO status is complete:
	if (GL_FRAMEBUFFER_COMPLETE_EXT != glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT ))
	{
		ERROR_MESSAGE("FrameBufferObject status not complete\nFalling back to RT_GAMEWINDOW ...")

		_Delete_RT_FRAMEBUFFEROBJECT_onlyFBO ();

		_eRenderTarget=RT_GAMEWINDOW;

		return bwResult;
	}
		

	return bwResult;
}

void CHlaeSupportRender::_Delete_RT_FRAMEBUFFEROBJECT_onlyFBO ()
{
	if (_FrameBufferObject_r.FBOid)
	{
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

		glDeleteFramebuffersEXT( 1, &_FrameBufferObject_r.FBOid );
		_FrameBufferObject_r.FBOid = 0;

		glDeleteRenderbuffersEXT( 1, &_FrameBufferObject_r.rgbaRenderBuffer );
		glDeleteRenderbuffersEXT( 1, &_FrameBufferObject_r.depthRenderBuffer );
	}
}
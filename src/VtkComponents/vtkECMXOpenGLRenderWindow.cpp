#ifndef VTK_8	// CP
#ifndef USE_OPENGL2_BACKEND	// CP
#include <vtkRenderingOpenGLConfigure.h>	// => VTK_USE_X, VTK_USE_OSMESA, ...
#ifdef VTK_USE_X	// CP
/**
 * \file		vtkECMXOpenGLRenderWindow.cpp
 * \author		Charles PIGNEROL (fortement guidé par Thierry CARRARD)
 * \date		21/06/2012
 */
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkECMXOpenGLRenderWindow.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "GL/gl.h"
#include "GL/glext.h"

#ifndef VTK_IMPLEMENT_MESA_CXX
#include "VtkComponents/vtkECMXOpenGLRenderWindow.h"
#include <vtkOpenGLRenderer.h>
#include <vtkOpenGLProperty.h>
#include <vtkOpenGLTexture.h>
#include <vtkOpenGLCamera.h>
#include <vtkOpenGLLight.h>
#include <vtkOpenGLActor.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkXRenderWindowInteractor.h>

#include <vtkOpenGL.h>

// define GLX_GLXEXT_LEGACY to prevent glx.h to include glxext.h provided by
// the system
//#define GLX_GLXEXT_LEGACY
#include "GL/glx.h"

#include "vtkgl.h"
#else
#include "MangleMesaInclude/osmesa.h"
#endif

#include "vtkToolkits.h"
#ifndef VTK_IMPLEMENT_MESA_CXX
#ifdef VTK_OPENGL_HAS_OSMESA
#include "GL/osmesa.h"
#endif
#endif

#include "vtkCommand.h"
#include "vtkIdList.h"
#include "vtkObjectFactory.h"
#include "vtkRendererCollection.h"
#include "vtkOpenGLExtensionManager.h"
#include "vtksys/SystemTools.hxx"

#ifndef VTK_5
#include <sstream>
#else	// VTK_5
#include <vtksys/ios/sstream>
#endif	// VTK_5

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

class vtkECMXOpenGLRenderWindow;
class vtkRenderWindow;
class vtkECMXOpenGLRenderWindowInternal
{
  friend class vtkECMXOpenGLRenderWindow;
private:
  vtkECMXOpenGLRenderWindowInternal(vtkRenderWindow*);

  GLXContext ContextId;

  // so we basically have 4 methods here for handling drawables
  // how about abstracting this a bit?

  // support for Pixmap based offscreen rendering
  Pixmap pixmap;
  GLXContext PixmapContextId;
  Window PixmapWindowId;


  // support for Pbuffer based offscreen rendering
  GLXContext PbufferContextId;
#ifndef VTK_IMPLEMENT_MESA_CXX
  vtkglX::GLXPbuffer Pbuffer;
#endif

  // store previous settings of on screen window
  int ScreenDoubleBuffer;
  int ScreenMapped;

#if defined( VTK_OPENGL_HAS_OSMESA )
  // OffScreen stuff
  OSMesaContext OffScreenContextId;
  void *OffScreenWindow;
#endif  
};

vtkECMXOpenGLRenderWindowInternal::vtkECMXOpenGLRenderWindowInternal(
  vtkRenderWindow *rw)
{
  this->ContextId = NULL;

  this->PixmapContextId = NULL;
  this->PixmapWindowId = 0;

  this->PbufferContextId = NULL;
#ifndef VTK_IMPLEMENT_MESA_CXX
  this->Pbuffer = 0;
#endif
  
  this->ScreenMapped = rw->GetMapped();
  this->ScreenDoubleBuffer = rw->GetDoubleBuffer();
  
  // OpenGL specific
#ifdef VTK_OPENGL_HAS_OSMESA
  this->OffScreenContextId = NULL;
  this->OffScreenWindow = NULL;
#endif
}


#ifndef VTK_IMPLEMENT_MESA_CXX
vtkStandardNewMacro(vtkECMXOpenGLRenderWindow);
#endif



#define MAX_LIGHTS 8

#ifdef VTK_OPENGL_HAS_OSMESA
// a couple of routines for offscreen rendering
//void vtkOSMesaDestroyWindow(void *Window) 	// CP
void vtkECMOSMesaDestroyWindow(void *Window) 	// CP
{
  free(Window);
}

//void *vtkOSMesaCreateWindow(int width, int height) 	// CP
void *vtkECMOSMesaCreateWindow(int width, int height) 	// CP
{
  return malloc(width*height*4);
}
#endif


#ifndef VTK_IMPLEMENT_MESA_CXX

vtkglX::GLXFBConfig* vtkECMXOpenGLRenderWindowTryForFBConfig(Display *DisplayId,
                                                          int drawable_type,
                                                          int doublebuff,
                                                          int stereo,
                                                          int multisamples,
                                                          int alphaBitPlanes,
                                                          int stencil)
{
  int           index;
  static int    attributes[50];

  // setup the default stuff we ask for
  index = 0;
  attributes[index++] = vtkglX::DRAWABLE_TYPE;
  attributes[index++] = drawable_type;
  attributes[index++] = vtkglX::RENDER_TYPE;
  attributes[index++] = vtkglX::RGBA_BIT;
  attributes[index++] = GLX_RED_SIZE;
  attributes[index++] = 1;
  attributes[index++] = GLX_GREEN_SIZE;
  attributes[index++] = 1;
  attributes[index++] = GLX_BLUE_SIZE;
  attributes[index++] = 1;
  attributes[index++] = GLX_DEPTH_SIZE;
  attributes[index++] = 1;
  if (alphaBitPlanes)
    {
    attributes[index++] = GLX_ALPHA_SIZE;
    attributes[index++] = 1;
    }
  if (doublebuff)
    {
    attributes[index++] = GLX_DOUBLEBUFFER;
    }
  if (stencil)
    {    
    attributes[index++] = GLX_STENCIL_SIZE;
    attributes[index++] = 8;
    }
  if (stereo)
    {
    // also try for STEREO
    attributes[index++] = GLX_STEREO;
    }
  if (multisamples)
    {
#ifdef GLX_SAMPLE_BUFFERS_SGIS
    attributes[index++] = GLX_SAMPLE_BUFFERS_SGIS;
    attributes[index++] = 1;
    attributes[index++] = GLX_SAMPLES_SGIS;
    attributes[index++] = multisamples;
#endif
    }
    
  attributes[index++] = None;
  int tmp;
  vtkglX::GLXFBConfig* fb = vtkglX::ChooseFBConfig(DisplayId,
                                                   XDefaultScreen(DisplayId),
                                                   attributes, &tmp);
  return fb;
}

#endif


XVisualInfo *vtkECMXOpenGLRenderWindowTryForVisual(Display *DisplayId,
                                                int doublebuff, int stereo,
                                                int multisamples,
                                                int alphaBitPlanes,
                                                int stencil)
{
  int           index;
  static int    attributes[50];

  // setup the default stuff we ask for
  index = 0;
  attributes[index++] = GLX_RGBA;
  attributes[index++] = GLX_RED_SIZE;
  attributes[index++] = 1;
  attributes[index++] = GLX_GREEN_SIZE;
  attributes[index++] = 1;
  attributes[index++] = GLX_BLUE_SIZE;
  attributes[index++] = 1;
  attributes[index++] = GLX_DEPTH_SIZE;
  attributes[index++] = 1;
  if (alphaBitPlanes)
    {
    attributes[index++] = GLX_ALPHA_SIZE;
    attributes[index++] = 1;
    }
  if (doublebuff)
    {
    attributes[index++] = GLX_DOUBLEBUFFER;
    }
  if (stencil)
    {
    attributes[index++] = GLX_STENCIL_SIZE;
    attributes[index++] = 8;
    }
  if (stereo)
    {
    // also try for STEREO
    attributes[index++] = GLX_STEREO;
    }
  if (multisamples)
    {
#ifdef GLX_SAMPLE_BUFFERS_SGIS
    attributes[index++] = GLX_SAMPLE_BUFFERS_SGIS;
    attributes[index++] = 1;
    attributes[index++] = GLX_SAMPLES_SGIS;
    attributes[index++] = multisamples;
#endif
    }
    
  attributes[index++] = None;

  return glXChooseVisual(DisplayId, XDefaultScreen(DisplayId), attributes );
}

#ifndef VTK_IMPLEMENT_MESA_CXX
vtkglX::GLXFBConfig *vtkECMXOpenGLRenderWindowGetDesiredFBConfig(
  Display *DisplayId,
  int &win_stereo,
  int &win_multisamples,
  int &win_doublebuffer,
  int &win_alphaplanes,
  int drawable_type,
  int &stencil)
{
  vtkglX::GLXFBConfig   *fbc = NULL;
  int           multi;
  int           stereo = 0;
  
  // try every possibility stoping when we find one that works
  for (stereo = win_stereo; !fbc && stereo >= 0; stereo--)
    {
    for (multi = win_multisamples; !fbc && multi >= 0; multi--)
      {
      if (fbc) 
        {
        XFree(fbc);
        }
      fbc = vtkECMXOpenGLRenderWindowTryForFBConfig(DisplayId,
                                                 drawable_type,
                                                 win_doublebuffer,
                                                 stereo, multi,
                                                 win_alphaplanes,
                                                 stencil);
      if (fbc && win_stereo && !stereo)
        {
        // requested a stereo capable window but we could not get one
        win_stereo = 0;
        }
      }
    }
  for (stereo = win_stereo; !fbc && stereo >= 0; stereo--)
    {
    for (multi = win_multisamples; !fbc && multi >= 0; multi--)
      {
      if (fbc) 
        {
        XFree(fbc);
        }
      fbc = vtkECMXOpenGLRenderWindowTryForFBConfig(DisplayId,
                                                 drawable_type,
                                                 !win_doublebuffer, 
                                                 stereo, multi,
                                                 win_alphaplanes,
                                                 stencil);
      if (fbc)
        {
        win_doublebuffer = !win_doublebuffer;
        }
      if (fbc && win_stereo && !stereo)
        {
        // requested a stereo capable window but we could not get one
        win_stereo = 0;
        }
      }
    }
  return ( fbc );
}
#endif

XVisualInfo *vtkECMXOpenGLRenderWindow::GetDesiredVisualInfo()
{
  XVisualInfo   *v = NULL;
  int           alpha;
  int           multi;
  int           stereo = 0;
  int           stencil;

  // get the default display connection 
  if (!this->DisplayId)
    {
    this->DisplayId = XOpenDisplay(static_cast<char *>(NULL)); 

    if (this->DisplayId == NULL)
      {
      vtkErrorMacro(<< "bad X server connection. DISPLAY="
        << vtksys::SystemTools::GetEnv("DISPLAY") << "\n");
      return NULL;
      }

    this->OwnDisplay = 1;
    }

  // try every possibility stoping when we find one that works
  for (stencil = this->StencilCapable; !v && stencil >= 0; stencil--)
    {
    for (alpha = this->AlphaBitPlanes; !v && alpha >= 0; alpha--)
      {
      for (stereo = this->StereoCapableWindow; !v && stereo >= 0; stereo--)
        {
        for (multi = this->MultiSamples; !v && multi >= 0; multi--)
          {
          if (v) 
            {
            XFree(v);
            }
          v = vtkECMXOpenGLRenderWindowTryForVisual(this->DisplayId,
                                                 this->DoubleBuffer, 
                                                 stereo, multi, alpha, 
                                                 stencil);
          if (v)
            {
            this->StereoCapableWindow = stereo;
            this->MultiSamples = multi;
            this->AlphaBitPlanes = alpha;
            this->StencilCapable = stencil;
            }
          }
        }
      }
    }
  for (stencil = this->StencilCapable; !v && stencil >= 0; stencil--)
    {
    for (alpha = this->AlphaBitPlanes; !v && alpha >= 0; alpha--)
      {
      for (stereo = this->StereoCapableWindow; !v && stereo >= 0; stereo--)
        {
        for (multi = this->MultiSamples; !v && multi >= 0; multi--)
          {
          v = vtkECMXOpenGLRenderWindowTryForVisual(this->DisplayId,
                                                 !this->DoubleBuffer, 
                                                 stereo, multi, alpha,
                                                 stencil);
          if (v)
            {
            this->DoubleBuffer = !this->DoubleBuffer;
            this->StereoCapableWindow = stereo;
            this->MultiSamples = multi;
            this->AlphaBitPlanes = alpha;
            this->StencilCapable = stencil;
            }
          }
        }
      }
    }
  if (!v) 
    {
    vtkErrorMacro(<< "Could not find a decent visual\n");
    }
  return ( v );
}

vtkECMXOpenGLRenderWindow::vtkECMXOpenGLRenderWindow()
{
  this->ParentId = static_cast<Window>(NULL);
  this->ScreenSize[0] = 0;
  this->ScreenSize[1] = 0;
  this->OwnDisplay = 0;
  this->CursorHidden = 0;
  this->ForceMakeCurrent = 0;
  this->UsingHardware = 0;
  this->DisplayId = static_cast<Display *>(NULL);
  this->WindowId = static_cast<Window>(NULL);
  this->NextWindowId = static_cast<Window>(NULL);
  this->ColorMap = static_cast<Colormap>(0);
  this->OwnWindow = 0;
 
  this->Internal = new vtkECMXOpenGLRenderWindowInternal(this);

  this->XCArrow =   0;
  this->XCSizeAll = 0;
  this->XCSizeNS =  0;
  this->XCSizeWE =  0;
  this->XCSizeNE =  0;
  this->XCSizeNW =  0;
  this->XCSizeSE =  0;
  this->XCSizeSW =  0;
  this->XCHand   =  0;

  this->Capabilities = 0;

}

// free up memory & close the window
vtkECMXOpenGLRenderWindow::~vtkECMXOpenGLRenderWindow()
{
  // close-down all system-specific drawing resources
  this->Finalize();
  
  vtkRenderer* ren;
/* CP : Code VTK 5.8.0
  this->Renderers->InitTraversal();
  for ( ren = vtkOpenGLRenderer::SafeDownCast(this->Renderers->GetNextItemAsObject());
        ren != NULL;
        ren = vtkOpenGLRenderer::SafeDownCast(this->Renderers->GetNextItemAsObject())  )
CP : Fin Code VTK 5.8.0 */
// CP : Code VTK 5.10.0
	vtkCollectionSimpleIterator rit;
	this->Renderers->InitTraversal(rit);
	while ( (ren = this->Renderers->GetNextRenderer(rit)) )
// CP : Fin Code VTK 5.10.0
    {
    ren->SetRenderWindow(NULL);
    }

  delete this->Internal;

}

// End the rendering process and display the image.
void vtkECMXOpenGLRenderWindow::Frame()
{
  this->MakeCurrent();
  if (!this->AbortRender && this->DoubleBuffer && this->SwapBuffers
      && this->WindowId!=0)
    {
    glXSwapBuffers(this->DisplayId, this->WindowId);
    vtkDebugMacro(<< " glXSwapBuffers\n");
    }
  else
    {
    glFlush();
    }
}

//
// Set the variable that indicates that we want a stereo capable window
// be created. This method can only be called before a window is realized.
//
void vtkECMXOpenGLRenderWindow::SetStereoCapableWindow(int capable)
{
  if (!this->Internal->ContextId && !this->Internal->PixmapContextId
      && !this->Internal->PbufferContextId
#if defined( VTK_OPENGL_HAS_OSMESA )
      && !this->Internal->OffScreenContextId
#endif
    )
    {
    vtkOpenGLRenderWindow::SetStereoCapableWindow(capable);
    }
  else
    {
    vtkWarningMacro(<< "Requesting a StereoCapableWindow must be performed "
                    << "before the window is realized, i.e. before a render.");
    }
}

#ifndef VTK_IMPLEMENT_MESA_CXX
static int PbufferAllocFail = 0;
extern "C"
{
//  int vtkXOGLPbufferErrorHandler(Display*, XErrorEvent*)	// CP
  int vtkECMXOGLPbufferErrorHandler(Display*, XErrorEvent*)	// CP
  {
    PbufferAllocFail = 1;
    return 1;
  }
}
#endif


void vtkECMXOpenGLRenderWindow::CreateAWindow()
{
  XVisualInfo  *v, matcher;
  XSetWindowAttributes  attr;
  int x, y, width, height, nItems;
  XWindowAttributes winattr;
  XSizeHints xsh;

  xsh.flags = USSize;
  if ((this->Position[0] >= 0)&&(this->Position[1] >= 0))
    {
    xsh.flags |= USPosition;
    xsh.x =  static_cast<int>(this->Position[0]);
    xsh.y =  static_cast<int>(this->Position[1]);
    }
  
  x = ((this->Position[0] >= 0) ? this->Position[0] : 5);
  y = ((this->Position[1] >= 0) ? this->Position[1] : 5);
  width = ((this->Size[0] > 0) ? this->Size[0] : 300);
  height = ((this->Size[1] > 0) ? this->Size[1] : 300);

  xsh.width  = width;
  xsh.height = height;

  // get the default display connection 
  if (!this->DisplayId)
    {
    this->DisplayId = XOpenDisplay(static_cast<char *>(NULL));
    if (this->DisplayId == NULL) 
      {
      vtkErrorMacro(<< "bad X server connection. DISPLAY="
        << vtksys::SystemTools::GetEnv("DISPLAY") << "\n");
      }
    this->OwnDisplay = 1;
    }

  attr.override_redirect = False;
  if (this->Borders == 0.0)
    {
    attr.override_redirect = True;
    }

  // create our own window ? 
  this->OwnWindow = 0;
  if (!this->WindowId)
    {
    v = this->GetDesiredVisualInfo();
    this->ColorMap = XCreateColormap(this->DisplayId,
                                     XRootWindow(this->DisplayId,v->screen),
                                     v->visual, AllocNone);
    
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = this->ColorMap;
    attr.event_mask = StructureNotifyMask | ExposureMask;
    
    // get a default parent if one has not been set.
    if (! this->ParentId)
      {
      this->ParentId = XRootWindow(this->DisplayId, v->screen);
      }
    this->WindowId = 
      XCreateWindow(this->DisplayId,
                    this->ParentId,
                    x, y, static_cast<unsigned int>(width),
                    static_cast<unsigned int>(height), 0, v->depth,
                    InputOutput, v->visual,
                    CWBackPixel | CWBorderPixel | CWColormap | 
                    CWOverrideRedirect | CWEventMask, 
                    &attr);
    XStoreName(this->DisplayId, this->WindowId, this->WindowName);
    XSetNormalHints(this->DisplayId,this->WindowId,&xsh);
    this->OwnWindow = 1;
    }
  else
    {
    XChangeWindowAttributes(this->DisplayId,this->WindowId,
                            CWOverrideRedirect, &attr);
    XGetWindowAttributes(this->DisplayId,
                         this->WindowId,&winattr);
    matcher.visualid = XVisualIDFromVisual(winattr.visual);
    matcher.screen = XDefaultScreen(DisplayId);
    v = XGetVisualInfo(this->DisplayId, VisualIDMask | VisualScreenMask,
                       &matcher, &nItems);
    }
  
  if (this->OwnWindow)
    {
    // RESIZE THE WINDOW TO THE DESIRED SIZE
    vtkDebugMacro(<< "Resizing the xwindow\n");
    XResizeWindow(this->DisplayId,this->WindowId,
                  ((this->Size[0] > 0) ? 
                   static_cast<unsigned int>(this->Size[0]) : 300),
                  ((this->Size[1] > 0) ? 
                   static_cast<unsigned int>(this->Size[1]) : 300));
    XSync(this->DisplayId,False);
    }

  // is GLX extension is supported?
  if(!glXQueryExtension(this->DisplayId, NULL, NULL)) 
    {
    vtkErrorMacro("GLX not found.  Aborting.");
    if (this->HasObserver(vtkCommand::ExitEvent))
      {
      this->InvokeEvent(vtkCommand::ExitEvent, NULL);
      return;
      }
    else
      {
      abort();
      }
    }

  if (!this->Internal->ContextId)
    {
    this->Internal->ContextId = 
      glXCreateContext(this->DisplayId, v, 0, GL_TRUE);
    }

  if(!this->Internal->ContextId)
    {
    vtkErrorMacro("Cannot create GLX context.  Aborting.");
    if (this->HasObserver(vtkCommand::ExitEvent))
      {
      this->InvokeEvent(vtkCommand::ExitEvent, NULL);
      return;
      }
    else
      {
      abort();
      }
    }

  if(this->OwnWindow)
    {
    vtkDebugMacro(" Mapping the xwindow\n");
    XMapWindow(this->DisplayId, this->WindowId);
    XSync(this->DisplayId,False);
    XGetWindowAttributes(this->DisplayId,
                         this->WindowId,&winattr);
    // guarantee that the window is mapped before the program continues 
    // on to do the OpenGL rendering.
    while (winattr.map_state == IsUnmapped)
      {
      XGetWindowAttributes(this->DisplayId,
                           this->WindowId,&winattr);
      }
    }
  // free the visual info
  if (v)
    {
    XFree(v);
    }
  this->Mapped = 1;
  this->Size[0] = width;
  this->Size[1] = height;

}

void vtkECMXOpenGLRenderWindow::DestroyWindow()
{
  // free the cursors
  if (this->DisplayId)
    {
    if (this->WindowId)
      {
      // we will only have a cursor defined if a CurrentCursor has been
      // set > 0 or if the cursor has been hidden... if we undefine without
      // checking, bad things can happen (BadWindow)
      if (this->GetCurrentCursor() || this->CursorHidden)
        {
        XUndefineCursor(this->DisplayId,this->WindowId);
        }
      }
    if (this->XCArrow)
      {
      XFreeCursor(this->DisplayId,this->XCArrow);
      }
    if (this->XCSizeAll)
      {
      XFreeCursor(this->DisplayId,this->XCSizeAll);
      }
    if (this->XCSizeNS)
      {
      XFreeCursor(this->DisplayId,this->XCSizeNS);
      }
    if (this->XCSizeWE)
      {
      XFreeCursor(this->DisplayId,this->XCSizeWE);
      }
    if (this->XCSizeNE)
      {
      XFreeCursor(this->DisplayId,this->XCSizeNE);
      }
    if (this->XCSizeNW)
      {
      XFreeCursor(this->DisplayId,this->XCSizeNW);
      }
    if (this->XCSizeSE)
      {
      XFreeCursor(this->DisplayId,this->XCSizeSE);
      }
    if (this->XCSizeSW)
      {
      XFreeCursor(this->DisplayId,this->XCSizeSW);
      }
    if (this->XCHand)
      {
      XFreeCursor(this->DisplayId,this->XCHand);
      }
    }

  this->XCArrow =   0;
  this->XCSizeAll = 0;
  this->XCSizeNS =  0;
  this->XCSizeWE =  0;
  this->XCSizeNE =  0;
  this->XCSizeNW =  0;
  this->XCSizeSE =  0;
  this->XCSizeSW =  0;
  this->XCHand   =  0;

  this->MakeCurrent();
  
  // tell each of the renderers that this render window/graphics context
  // is being removed (the RendererCollection is removed by vtkRenderWindow's
  // destructor)
  vtkRenderer* ren;
  this->Renderers->InitTraversal();
  for ( ren = vtkOpenGLRenderer::SafeDownCast(this->Renderers->GetNextItemAsObject());
        ren != NULL;
        ren = vtkOpenGLRenderer::SafeDownCast(this->Renderers->GetNextItemAsObject())  )
    {
    ren->SetRenderWindow(NULL);
    ren->SetRenderWindow(this);
    }


  if(this->Internal->ContextId)
    {
    
        
    /* first delete all the old lights */
    for (short cur_light = GL_LIGHT0; cur_light < GL_LIGHT0+MAX_LIGHTS; cur_light++)
      {
      glDisable(static_cast<GLenum>(cur_light));
      }

    /* now delete all textures */
    glDisable(GL_TEXTURE_2D);
    for (int i = 1; i < this->TextureResourceIds->GetNumberOfIds(); i++)
      {
      GLuint txId = static_cast<GLuint>(this->TextureResourceIds->GetId(i));
#ifdef GL_VERSION_1_1
      if (glIsTexture(txId))
        {
        glDeleteTextures(1, &txId);
        }
#else
      if (glIsList(txId))
        {
        glDeleteLists(txId,1);
        }
#endif
      }

    glFinish();


    glXDestroyContext(this->DisplayId, this->Internal->ContextId);
    this->Internal->ContextId = NULL;
    
    }
  
  // then close the old window if we own it
  if (this->OwnWindow && this->DisplayId && this->WindowId)
    {
    XDestroyWindow(this->DisplayId,this->WindowId);
    this->WindowId = static_cast<Window>(NULL);
    }

  // if we create the display, we'll delete it
  if (this->OwnDisplay && this->DisplayId)
    {
    XCloseDisplay(this->DisplayId);
    this->DisplayId = NULL;
    }

  if (this->Capabilities)
    {
    delete[] this->Capabilities;
    this->Capabilities = 0;
    }
    
  // make sure all other code knows we're not mapped anymore
  this->Mapped = 0;

}

void vtkECMXOpenGLRenderWindow::CreateOffScreenWindow(int width, int height)
{
  
  XVisualInfo  *v;
  
  this->DoubleBuffer = 0;

  // always prefer OSMESA if we built with it
#ifdef VTK_OPENGL_HAS_OSMESA
  if(1)
    {
    if (!this->Internal->OffScreenWindow)
      {
//      this->Internal->OffScreenWindow = vtkOSMesaCreateWindow(width,height);	// CP
      this->Internal->OffScreenWindow = vtkECMOSMesaCreateWindow(width,height);	// CP
      this->Size[0] = width;
      this->Size[1] = height;      
      this->OwnWindow = 1;
      }    
    if (!this->Internal->OffScreenContextId)
      {
      this->Internal->OffScreenContextId = OSMesaCreateContext(GL_RGBA, NULL);
      }
    this->MakeCurrent();
    }
  else
#endif
    {
		// Modifs CP, sur conseils de TC : que les fenêtres off-screen
		// n'apparaissent plus du tout à l'écran :
		int	FBOffScreenOk	= 0;	// CP
		if( this->Internal->PixmapContextId!=0 )	// CP
			FBOffScreenOk	= this->CreateHardwareOffScreenWindow(width,height);	// CP

//    if(!this->CreateHardwareOffScreenWindow(width,height))	// CP
		if( !FBOffScreenOk )	// CP
      {
      // get the default display connection 
      if (!this->DisplayId)
        {
        this->DisplayId = XOpenDisplay(static_cast<char *>(NULL));
        if (this->DisplayId == NULL) 
          {
          vtkErrorMacro(<< "bad X server connection. DISPLAY="
            << vtksys::SystemTools::GetEnv("DISPLAY") << "\n");
          }
        this->OwnDisplay = 1;
        }

#ifndef VTK_IMPLEMENT_MESA_CXX
      int v1, v2;
      glXQueryVersion(this->DisplayId, &v1, &v2);

      // check for GLX 1.3 or greater for Pbuffer offscreen support
      if(v1 > 1 || (v1 == 1 && v2 >= 3))
        {
        if(!this->Internal->PbufferContextId)
          {
          // Load GLX 1.3
          vtkOpenGLExtensionManager *manager=vtkOpenGLExtensionManager::New();
          int loaded = vtkgl::LoadExtension("GLX_VERSION_1_3", manager);
          manager->Delete();

          if(loaded)
            {
            // get FBConfig
            vtkglX::GLXFBConfig* fb = vtkECMXOpenGLRenderWindowGetDesiredFBConfig(
              this->DisplayId,this->StereoCapableWindow, this->MultiSamples,
              this->DoubleBuffer,this->AlphaBitPlanes, vtkglX::PBUFFER_BIT,
              this->StencilCapable);
            if(fb)
              {
//              XErrorHandler previousHandler = XSetErrorHandler(vtkXOGLPbufferErrorHandler);	// CP
              XErrorHandler previousHandler = XSetErrorHandler(vtkECMXOGLPbufferErrorHandler);	// CP
              this->Internal->PbufferContextId = 
                vtkglX::CreateNewContext(this->DisplayId, fb[0],
                                         vtkglX::RGBA_TYPE, NULL, true);
              int atts [] = 
                {
                  vtkglX::PBUFFER_WIDTH, width,
                  vtkglX::PBUFFER_HEIGHT, height,
                  0
                };
              this->Internal->Pbuffer = vtkglX::CreatePbuffer(this->DisplayId,
                                                              fb[0], atts);
              vtkglX::MakeContextCurrent( this->DisplayId,
                                          this->Internal->Pbuffer, 
                                          this->Internal->Pbuffer,
                                          this->Internal->PbufferContextId );
              XFree(fb);
              XSetErrorHandler(previousHandler);
            
              // failed to allocate Pbuffer, clean up
              if(PbufferAllocFail)
                {
                //vtkglX::DestroyPbuffer(this->DisplayId, this->Internal->Pbuffer);
                this->Internal->Pbuffer = 0;
                if(this->Internal->PbufferContextId)
                  glXDestroyContext(this->DisplayId,
                                    this->Internal->PbufferContextId);
                this->Internal->PbufferContextId = NULL;
                }
              PbufferAllocFail = 0;
              }
            }
          }
        }

#endif
    
      // GLX 1.3 doesn't exist or failed to allocate Pbuffer
      // fallback on GLX 1.0 GLXPixmap offscreen support
      if(!this->Internal->PbufferContextId && !this->Internal->PixmapContextId)
        {
        v = this->GetDesiredVisualInfo();
        this->Internal->PixmapContextId = glXCreateContext(this->DisplayId,
                                                           v, 0, GL_FALSE);
        this->Internal->pixmap=
          XCreatePixmap(this->DisplayId,
                        XRootWindow(this->DisplayId,v->screen),
                        static_cast<unsigned int>(width),
                        static_cast<unsigned int>(height),
                        static_cast<unsigned int>(v->depth));

        this->Internal->PixmapWindowId = glXCreateGLXPixmap(this->DisplayId, v, this->Internal->pixmap);
        glXMakeCurrent(this->DisplayId,this->Internal->PixmapWindowId,
                       this->Internal->PixmapContextId);

        if(v)
          {
          XFree(v);
          }
        }
      } // if not hardware offscreen
    }
  this->Mapped = 0;
  this->Size[0] = width;
  this->Size[1] = height;
  
  
  this->MakeCurrent();

  // tell our renderers about us
  vtkRenderer* ren;
  for (this->Renderers->InitTraversal(); 
       (ren = this->Renderers->GetNextItem());)
    {
    ren->SetRenderWindow(0);
    ren->SetRenderWindow(this);
    }

  this->OpenGLInit();
}

void vtkECMXOpenGLRenderWindow::DestroyOffScreenWindow()
{
  
  // release graphic resources.
  vtkRenderer *ren;
  vtkCollectionSimpleIterator rit;
  this->Renderers->InitTraversal(rit);
  while ( (ren = this->Renderers->GetNextRenderer(rit)) )
    {
    ren->SetRenderWindow(NULL);
    ren->SetRenderWindow(this);
    }


#ifdef VTK_OPENGL_HAS_OSMESA
  if (this->Internal->OffScreenContextId)
    {
    OSMesaDestroyContext(this->Internal->OffScreenContextId);
    this->Internal->OffScreenContextId = NULL;
//    vtkOSMesaDestroyWindow(this->Internal->OffScreenWindow);	// CP
    vtkECMOSMesaDestroyWindow(this->Internal->OffScreenWindow);	// CP
    this->Internal->OffScreenWindow = NULL;
    }
  else
#endif
    {
    if(this->OffScreenUseFrameBuffer)
      {
      this->DestroyHardwareOffScreenWindow();
      }
    else
      {
      if(this->Internal->PbufferContextId)
        {
#ifndef VTK_IMPLEMENT_MESA_CXX
        vtkglX::DestroyPbuffer(this->DisplayId, this->Internal->Pbuffer);
        this->Internal->Pbuffer = 0;
        glXDestroyContext(this->DisplayId, this->Internal->PbufferContextId);
        this->Internal->PbufferContextId = NULL;
#endif
        }
      else if(this->Internal->PixmapContextId)
        {
        glXDestroyGLXPixmap(this->DisplayId, this->Internal->PixmapWindowId);
        this->Internal->PixmapWindowId = 0;
        XFreePixmap(this->DisplayId, this->Internal->pixmap);
        glXDestroyContext( this->DisplayId, this->Internal->PixmapContextId);
        this->Internal->PixmapContextId = NULL;
        }
      }
    }
}

void vtkECMXOpenGLRenderWindow::ResizeOffScreenWindow(int width, int height)
{
  if(!this->OffScreenRendering)
    {
    return;
    }

// CP : Code VTK 5.10.0
#ifdef VTK_OPENGL_HAS_OSMESA
  if (this->Internal->OffScreenContextId && this->Internal->OffScreenWindow)
    {
    vtkOSMesaDestroyWindow(this->Internal->OffScreenWindow);
    this->Internal->OffScreenWindow = NULL;

    // allocate new one.
    this->Internal->OffScreenWindow = vtkOSMesaCreateWindow(width,height);
    this->Size[0] = width;
    this->Size[1] = height;      
    this->OwnWindow = 1;
    return;
    }
#endif
// CP : Fin Code VTK 5.10.0

  if(this->Internal->PixmapContextId ||
     this->Internal->PbufferContextId || 
     this->OffScreenUseFrameBuffer 
/* CP : Code VTK 5.8.0
#ifdef VTK_OPENGL_HAS_OSMESA
     || this->Internal->OffScreenContextId
#endif
CP : Fin Code VTK 5.8.0 */
    )
    {
    this->DestroyOffScreenWindow();
    this->CreateOffScreenWindow(width, height);
    }
}


// Initialize the window for rendering.
void vtkECMXOpenGLRenderWindow::WindowInitialize (void)
{
  this->CreateAWindow();

  this->MakeCurrent();

  // tell our renderers about us
  vtkRenderer* ren;
  for (this->Renderers->InitTraversal(); 
       (ren = this->Renderers->GetNextItem());)
    {
    ren->SetRenderWindow(0);
    ren->SetRenderWindow(this);
    }

  this->OpenGLInit();
}

// Initialize the rendering window.
void vtkECMXOpenGLRenderWindow::Initialize (void)
{
  if(!this->OffScreenRendering && !this->Internal->ContextId)
    {
    // initialize the window 
    this->WindowInitialize();
    }
  else if(this->OffScreenRendering && 
          ! (this->Internal->PixmapContextId ||
             this->Internal->PbufferContextId 
#ifdef VTK_OPENGL_HAS_OSMESA
             || this->Internal->OffScreenContextId
#endif
             || this->OffScreenUseFrameBuffer
            ))
    {
    // initialize offscreen window
    int width = ((this->Size[0] > 0) ? this->Size[0] : 300);
    int height = ((this->Size[1] > 0) ? this->Size[1] : 300);
    this->CreateOffScreenWindow(width, height);
    }
}

void vtkECMXOpenGLRenderWindow::Finalize (void)
{

  // clean up offscreen stuff
  this->SetOffScreenRendering(0);

  // clean and destroy window
  this->DestroyWindow();

}

// Change the window to fill the entire screen.
void vtkECMXOpenGLRenderWindow::SetFullScreen(int arg)
{
  int *temp;
  
  if (this->OffScreenRendering)
    {
    return;
    }
  
  if (this->FullScreen == arg) return;
  
  if (!this->Mapped)
    {
    this->PrefFullScreen();
    return;
    }

  // set the mode 
  this->FullScreen = arg;
  if (this->FullScreen <= 0)
    {
    this->Position[0] = this->OldScreen[0];
    this->Position[1] = this->OldScreen[1];
    this->Size[0] = this->OldScreen[2]; 
    this->Size[1] = this->OldScreen[3];
    this->Borders = this->OldScreen[4];
    }
  else
    {
    // if window already up get its values 
    if (this->WindowId)
      {
      XWindowAttributes attribs;
      
      //  Find the current window size 
      XGetWindowAttributes(this->DisplayId, 
                           this->WindowId, &attribs);
      
      this->OldScreen[2] = attribs.width;
      this->OldScreen[3] = attribs.height;;

      temp = this->GetPosition();      
      this->OldScreen[0] = temp[0];
      this->OldScreen[1] = temp[1];

      this->OldScreen[4] = this->Borders;
      this->PrefFullScreen();
      }
    }
  
  // remap the window 
  this->WindowRemap();

  // if full screen then grab the keyboard 
  if (this->FullScreen)
    {
    XGrabKeyboard(this->DisplayId,this->WindowId,
                  False,GrabModeAsync,GrabModeAsync,CurrentTime);
    }
  this->Modified();
}

// Set the preferred window size to full screen.
void vtkECMXOpenGLRenderWindow::PrefFullScreen()
{
  // use full screen 
  this->Position[0] = 0;
  this->Position[1] = 0;

  if (this->OffScreenRendering)
    {
    this->Size[0] = 1280;
    this->Size[1] = 1024;
    }
  else
    {
    int *size;
    size = this->GetScreenSize();
    this->Size[0] = size[0];
    this->Size[1] = size[1];
    }
  
  // don't show borders 
  this->Borders = 0;
}

// Resize the window.
void vtkECMXOpenGLRenderWindow::WindowRemap()
{
  // shut everything down
  this->Finalize();

  // set the default windowid 
  this->WindowId = this->NextWindowId;
  this->NextWindowId = static_cast<Window>(NULL);

  // set everything up again 
  this->Initialize();
}

// Begin the rendering process.
void vtkECMXOpenGLRenderWindow::Start(void)
{
  this->Initialize();

  // set the current window 
  this->MakeCurrent();
}


// Specify the size of the rendering window.
void vtkECMXOpenGLRenderWindow::SetSize(int width,int height)
{
  if ((this->Size[0] != width)||(this->Size[1] != height))
    {
    this->Size[0] = width;
    this->Size[1] = height;

    if (this->Interactor)
      {
      this->Interactor->SetSize( width, height );
      }

    if(this->OffScreenRendering)
      {
      this->ResizeOffScreenWindow(width,height);
      }
    else if(this->WindowId && this->Mapped)
      {
      XResizeWindow(this->DisplayId,this->WindowId,
                    static_cast<unsigned int>(width),
                    static_cast<unsigned int>(height));
      XSync(this->DisplayId,False);
      }

    this->Modified();
    }
}



int vtkECMXOpenGLRenderWindow::GetDesiredDepth()
{
  XVisualInfo *v;
  int depth = 0;
  
  // get the default visual to use 
  v = this->GetDesiredVisualInfo();
  
  if (v)
    {
    depth = v->depth;  
    XFree(v);
    }

  return depth;
}

// Get a visual from the windowing system.
Visual *vtkECMXOpenGLRenderWindow::GetDesiredVisual ()
{
  XVisualInfo *v;
  Visual *vis=0;
  
  // get the default visual to use 
  v = this->GetDesiredVisualInfo();

  if (v)
    {
    vis = v->visual;  
    XFree(v);
    }
  
  return vis;  
}


// Get a colormap from the windowing system.
Colormap vtkECMXOpenGLRenderWindow::GetDesiredColormap ()
{
  XVisualInfo *v;
  
  if (this->ColorMap) return this->ColorMap;
  
  // get the default visual to use 
  v = this->GetDesiredVisualInfo();

  if (v)
    {
    this->ColorMap = XCreateColormap(this->DisplayId,
                                     XRootWindow(this->DisplayId, v->screen),
                                     v->visual, AllocNone);
    XFree(v);
    }

  return this->ColorMap;  
}

void vtkECMXOpenGLRenderWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "ContextId: " << this->Internal->ContextId << "\n";
#ifdef VTK_OPENGL_HAS_OSMESA
  os << indent << "OffScreenContextId: " << this->Internal->OffScreenContextId << "\n";
#endif
  os << indent << "Color Map: " << this->ColorMap << "\n";
  os << indent << "Display Id: " << this->GetDisplayId() << "\n";
  os << indent << "Next Window Id: " << this->NextWindowId << "\n";
  os << indent << "Window Id: " << this->GetWindowId() << "\n";
}

// the following can be useful for debugging XErrors
// When uncommented (along with the lines in MakeCurrent) 
// it will cause a segfault upon an XError instead of 
// the normal XError handler
//  extern "C" {int vtkXError(Display *display, XErrorEvent *err)
//  {
//  // cause a segfault
//    *(float *)(0x01) = 1.0;
//    return 1;
//  }}

void vtkECMXOpenGLRenderWindow::MakeCurrent()
{
  // when debugging XErrors uncomment the following lines
//    if (this->DisplayId)
//      {
//        XSynchronize(this->DisplayId,1);
//      }
//     XSetErrorHandler(vtkXError);
#ifdef VTK_OPENGL_HAS_OSMESA

  // set the current window 
  if (this->OffScreenRendering && this->Internal->OffScreenContextId)
    {
    if (OSMesaMakeCurrent(this->Internal->OffScreenContextId, 
                          this->Internal->OffScreenWindow, GL_UNSIGNED_BYTE, 
                          this->Size[0], this->Size[1]) != GL_TRUE) 
      {
      vtkWarningMacro("failed call to OSMesaMakeCurrent");
      }
    }
  else
#endif
#ifndef VTK_IMPLEMENT_MESA_CXX
    if(this->OffScreenRendering && this->Internal->PbufferContextId)
      {
      if (((this->Internal->PbufferContextId != glXGetCurrentContext())
           || this->ForceMakeCurrent))
        {
        vtkglX::MakeContextCurrent(this->DisplayId, this->Internal->Pbuffer,
                                   this->Internal->Pbuffer,
                                   this->Internal->PbufferContextId);
        this->ForceMakeCurrent = 0;
        }
      }
    else 
#endif
      if(this->OffScreenRendering && this->Internal->PixmapContextId)
        {
        if (((this->Internal->PixmapContextId != glXGetCurrentContext())
             || this->ForceMakeCurrent))
          {
          glXMakeCurrent(this->DisplayId,this->Internal->PixmapWindowId,
                         this->Internal->PixmapContextId);
          this->ForceMakeCurrent = 0;
          }
        }
      else
        {
        if (this->Internal->ContextId &&
            ((this->Internal->ContextId != glXGetCurrentContext())
             || this->ForceMakeCurrent))
          {
          glXMakeCurrent(this->DisplayId,this->WindowId,
                         this->Internal->ContextId);
          this->ForceMakeCurrent = 0;
          }
        }
}

// ----------------------------------------------------------------------------
// Description:
// Tells if this window is the current OpenGL context for the calling thread.
bool vtkECMXOpenGLRenderWindow::IsCurrent()
{
  bool result=false;
#ifdef VTK_OPENGL_HAS_OSMESA
  if(this->OffScreenRendering && this->Internal->OffScreenContextId)
    {
    result=this->Internal->OffScreenContextId==OSMesaGetCurrentContext();
    }
  else
    {
#endif
#ifdef VTK_IMPLEMENT_MESA_CXX
    if(this->OffScreenRendering && this->Internal->PbufferContextId)
      {
      result=this->Internal->PbufferContextId==glXGetCurrentContext();
      }
    else
      {
#endif
      if(this->OffScreenRendering && this->Internal->PixmapContextId)
        {
        result=this->Internal->PixmapContextId==glXGetCurrentContext();
        }
      else
        {
        if(this->Internal->ContextId)
          {
          result=this->Internal->ContextId==glXGetCurrentContext();
          }
        }
#ifdef VTK_IMPLEMENT_MESA_CXX
      }
#endif
#ifdef VTK_OPENGL_HAS_OSMESA
    }
#endif
  return result;
}

void vtkECMXOpenGLRenderWindow::SetForceMakeCurrent()
{
  this->ForceMakeCurrent = 1;
}

int vtkECMXOpenGLRenderWindowFoundMatch;

extern "C"
{
  Bool vtkECMXOpenGLRenderWindowPredProc(Display *vtkNotUsed(disp), 
                                      XEvent *event, 
                                      char *arg)
  {
    Window win = (Window)arg;
    
    if (((reinterpret_cast<XAnyEvent *>(event))->window == win) &&
        ((event->type == ButtonPress)))
      {
      vtkECMXOpenGLRenderWindowFoundMatch = 1;
      }

    return 0; 
  }
}

void *vtkECMXOpenGLRenderWindow::GetGenericContext()
{
#if defined(MESA) && defined(VTK_OPENGL_HAS_OSMESA)
  if (this->OffScreenRendering && this->Internal->OffScreenContextId)
    {
    return (void *)this->Internal->OffScreenContextId;
    }
  else
#endif
    if(this->OffScreenRendering && this->Internal->PbufferContextId)
      {
      return static_cast<void *>(this->Internal->PbufferContextId);
      }
    else if(this->OffScreenRendering && this->Internal->PixmapContextId)
      {
      return static_cast<void *>(this->Internal->PixmapContextId);
      }
    else
      {
      static GC gc = static_cast<GC>(NULL);
      if (!gc)
        {
        gc = XCreateGC(this->DisplayId, this->WindowId, 0, 0);
        }
      return static_cast<void *>(gc);
      }

}

int vtkECMXOpenGLRenderWindow::GetEventPending()
{
  XEvent report;
  
  vtkECMXOpenGLRenderWindowFoundMatch = 0;
  if (this->OffScreenRendering)
    {
    return vtkECMXOpenGLRenderWindowFoundMatch;
    }
  XCheckIfEvent(this->DisplayId, &report, vtkECMXOpenGLRenderWindowPredProc, 
                reinterpret_cast<char *>(this->WindowId));
  return vtkECMXOpenGLRenderWindowFoundMatch;
}

// Get the size of the screen in pixels
int *vtkECMXOpenGLRenderWindow::GetScreenSize()
{
  // get the default display connection 
  if (!this->DisplayId)
    {
    this->DisplayId = XOpenDisplay(static_cast<char *>(NULL)); 
    if (this->DisplayId == NULL) 
      {
      vtkErrorMacro(<< "bad X server connection. DISPLAY="
        << vtksys::SystemTools::GetEnv("DISPLAY") << "\n");
      }
    else
      {
      this->OwnDisplay = 1;
      }
    }

  this->ScreenSize[0] = 
    XDisplayWidth(this->DisplayId, XDefaultScreen(this->DisplayId));
  this->ScreenSize[1] = 
    XDisplayHeight(this->DisplayId, XDefaultScreen(this->DisplayId));

  return this->ScreenSize;
}

// Get the position in screen coordinates (pixels) of the window.
int *vtkECMXOpenGLRenderWindow::GetPosition(void)
{
  XWindowAttributes attribs;
  int x,y;
  Window child;
  
  // if we aren't mapped then just return the ivar 
  if (!this->Mapped)
    {
    return this->Position;
    }

  //  Find the current window size 
  XGetWindowAttributes(this->DisplayId, this->WindowId, &attribs);
  x = attribs.x;
  y = attribs.y;

  XTranslateCoordinates(this->DisplayId,this->WindowId,
                        XRootWindowOfScreen(XScreenOfDisplay(
                                              this->DisplayId,0)),
                        x,y,&this->Position[0],&this->Position[1],&child);

  return this->Position;
}

// Get this RenderWindow's X display id.
Display *vtkECMXOpenGLRenderWindow::GetDisplayId()
{
  vtkDebugMacro(<< "Returning DisplayId of " << static_cast<void *>(this->DisplayId) << "\n"); 

  return this->DisplayId;
}

// Get this RenderWindow's parent X window id.
Window vtkECMXOpenGLRenderWindow::GetParentId()
{
  vtkDebugMacro(<< "Returning ParentId of " << reinterpret_cast<void *>(this->ParentId) << "\n");
  return this->ParentId;
}

// Get this RenderWindow's X window id.
Window vtkECMXOpenGLRenderWindow::GetWindowId()
{
  vtkDebugMacro(<< "Returning WindowId of " << reinterpret_cast<void *>(this->WindowId) << "\n");
  return this->WindowId;
}

// Move the window to a new position on the display.
void vtkECMXOpenGLRenderWindow::SetPosition(int x, int y)
{
  // if we aren't mapped then just set the ivars
  if (!this->Mapped)
    {
    if ((this->Position[0] != x)||(this->Position[1] != y))
      {
      this->Modified();
      }
    this->Position[0] = x;
    this->Position[1] = y;
    return;
    }

  XMoveWindow(this->DisplayId,this->WindowId,x,y);
  XSync(this->DisplayId,False);
}

// Sets the parent of the window that WILL BE created.
void vtkECMXOpenGLRenderWindow::SetParentId(Window arg)
{
//   if (this->ParentId)
//     {
//     vtkErrorMacro("ParentId is already set.");
//     return;
//     }
  
  vtkDebugMacro(<< "Setting ParentId to " << reinterpret_cast<void *>(arg) << "\n"); 

  this->ParentId = arg;
}

// Set this RenderWindow's X window id to a pre-existing window.
void vtkECMXOpenGLRenderWindow::SetWindowId(Window arg)
{
  vtkDebugMacro(<< "Setting WindowId to " << reinterpret_cast<void *>(arg) << "\n"); 

  this->WindowId = arg;

  if (this->CursorHidden)
    {
    this->CursorHidden = 0;
    this->HideCursor();
    } 
}

// Set this RenderWindow's X window id to a pre-existing window.
void vtkECMXOpenGLRenderWindow::SetWindowInfo(char *info)
{
  int tmp;
  
  // get the default display connection 
  if (!this->DisplayId)
    {
    this->DisplayId = XOpenDisplay(static_cast<char *>(NULL));
    if (this->DisplayId == NULL) 
      {
      vtkErrorMacro(<< "bad X server connection. DISPLAY="
        << vtksys::SystemTools::GetEnv("DISPLAY") << "\n");
      }
    else
      {
      this->OwnDisplay = 1;
      }
    }

  sscanf(info,"%i",&tmp);
 
  this->SetWindowId(static_cast<Window>(tmp));
}

// Set this RenderWindow's X window id to a pre-existing window.
void vtkECMXOpenGLRenderWindow::SetNextWindowInfo(char *info)
{
  int tmp;
  sscanf(info,"%i",&tmp);
 
  this->SetNextWindowId(static_cast<Window>(tmp));
}

// Sets the X window id of the window that WILL BE created.
void vtkECMXOpenGLRenderWindow::SetParentInfo(char *info)
{
  int tmp;
  
  // get the default display connection 
  if (!this->DisplayId)
    {
    this->DisplayId = XOpenDisplay(static_cast<char *>(NULL));
    if (this->DisplayId == NULL) 
      {
      vtkErrorMacro(<< "bad X server connection. DISPLAY="
        << vtksys::SystemTools::GetEnv("DISPLAY") << "\n");
      }
    else
      {
      this->OwnDisplay = 1;
      }
    }

  sscanf(info,"%i",&tmp);
 
  this->SetParentId(static_cast<Window>(tmp));
}

void vtkECMXOpenGLRenderWindow::SetWindowId(void *arg)
{
  this->SetWindowId(reinterpret_cast<Window>(arg));
}
void vtkECMXOpenGLRenderWindow::SetParentId(void *arg)
{
  this->SetParentId(reinterpret_cast<Window>(arg));
}

const char* vtkECMXOpenGLRenderWindow::ReportCapabilities()
{
  MakeCurrent();

  if (!this->DisplayId) 
    {
    return "display id not set";
    }

  int scrnum = XDefaultScreen(this->DisplayId);
  const char *serverVendor = glXQueryServerString(this->DisplayId, scrnum,
                                                  GLX_VENDOR);
  const char *serverVersion = glXQueryServerString(this->DisplayId, scrnum,
                                                   GLX_VERSION);
  const char *serverExtensions = glXQueryServerString(this->DisplayId, scrnum,
                                                      GLX_EXTENSIONS);
  const char *clientVendor = glXGetClientString(this->DisplayId, GLX_VENDOR);
  const char *clientVersion = glXGetClientString(this->DisplayId, GLX_VERSION);
  const char *clientExtensions = glXGetClientString(this->DisplayId,
                                                    GLX_EXTENSIONS);
  const char *glxExtensions = glXQueryExtensionsString(this->DisplayId,scrnum);
  const char *glVendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
  const char *glRenderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
  const char *glVersion = reinterpret_cast<const char *>(glGetString(GL_VERSION));
  const char *glExtensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

#ifndef VTK_5
	std::ostringstream strm;
#else	// VTK_5
  vtksys_ios::ostringstream strm;
#endif	// VTK_5
  strm << "server glx vendor string:  " << serverVendor << endl;
  strm << "server glx version string:  " << serverVersion << endl;
  strm << "server glx extensions:  " << serverExtensions << endl;
  strm << "client glx vendor string:  " << clientVendor << endl;
  strm << "client glx version string:  " << clientVersion << endl;
  strm << "client glx extensions:  " << clientExtensions << endl;
  strm << "glx extensions:  " << glxExtensions << endl;
  strm << "OpenGL vendor string:  " << glVendor << endl;
  strm << "OpenGL renderer string:  " << glRenderer << endl;
  strm << "OpenGL version string:  " << glVersion << endl;
  strm << "OpenGL extensions:  " << glExtensions << endl;
  strm << "X Extensions:  ";

  int n = 0;
  char **extlist = XListExtensions(this->DisplayId, &n);

  for (int i = 0; i < n; i++) 
    {
    if (i != n-1)
      {
      strm << extlist[i] << ", ";
      }
    else
      {
      strm << extlist[i] << endl;
      }
    }

  delete[] this->Capabilities;

  size_t len = strm.str().length();
  this->Capabilities = new char[len + 1];
  strncpy(this->Capabilities, strm.str().c_str(), len);
  this->Capabilities[len] = 0;

  return this->Capabilities;
}

int vtkECMXOpenGLRenderWindow::SupportsOpenGL()
{
  this->MakeCurrent();
  if (!this->DisplayId) 
    {
    return 0;
    }

  int value = 0;
  XVisualInfo *v = this->GetDesiredVisualInfo();
  if (v)
    {
    glXGetConfig(this->DisplayId, v, GLX_USE_GL, &value);
    XFree(v);
    }

  return value;
}


int vtkECMXOpenGLRenderWindow::IsDirect()
{
  this->MakeCurrent();
  this->UsingHardware = 0;
  if (this->OffScreenRendering && this->Internal->PbufferContextId)
    {
    this->UsingHardware = glXIsDirect(this->DisplayId,
                                      this->Internal->PbufferContextId) ? 1:0;
    }
  else if (this->OffScreenRendering && this->Internal->PixmapContextId)
    {
    this->UsingHardware = glXIsDirect(this->DisplayId,
                                      this->Internal->PixmapContextId) ? 1:0;
    }
  else if (this->DisplayId && this->Internal->ContextId)
    {
    this->UsingHardware = glXIsDirect(this->DisplayId, 
                                      this->Internal->ContextId) ? 1:0;
    }
  return this->UsingHardware;
}


void vtkECMXOpenGLRenderWindow::SetWindowName(const char * cname)
{
  char *name = new char[ strlen(cname)+1 ];
  strcpy(name, cname);
  XTextProperty win_name_text_prop;

  this->vtkOpenGLRenderWindow::SetWindowName( name );

  if (this->Mapped)
    {
    if( XStringListToTextProperty( &name, 1, &win_name_text_prop ) == 0 )
      {
      XFree (win_name_text_prop.value);
      vtkWarningMacro(<< "Can't rename window"); 
      delete [] name;
      return;
      }
    
    XSetWMName( this->DisplayId, this->WindowId, &win_name_text_prop );
    XSetWMIconName( this->DisplayId, this->WindowId, &win_name_text_prop );
    XFree (win_name_text_prop.value);
    }
  delete [] name;
}


// Specify the X window id to use if a WindowRemap is done.
void vtkECMXOpenGLRenderWindow::SetNextWindowId(Window arg)
{
  vtkDebugMacro(<< "Setting NextWindowId to " << reinterpret_cast<void *>(arg) << "\n"); 

  this->NextWindowId = arg;
}

void vtkECMXOpenGLRenderWindow::SetNextWindowId(void *arg)
{
  this->SetNextWindowId(reinterpret_cast<Window>(arg));
}


// Set the X display id for this RenderWindow to use to a pre-existing 
// X display id.
void vtkECMXOpenGLRenderWindow::SetDisplayId(Display  *arg)
{
  vtkDebugMacro(<< "Setting DisplayId to " << static_cast<void *>(arg) << "\n"); 

  this->DisplayId = arg;
  this->OwnDisplay = 0;

}
void vtkECMXOpenGLRenderWindow::SetDisplayId(void *arg)
{
  this->SetDisplayId(static_cast<Display *>(arg));
  this->OwnDisplay = 0;
}

void vtkECMXOpenGLRenderWindow::Render()
{
  XWindowAttributes attribs;

  // To avoid the expensive XGetWindowAttributes call,
  // compute size at the start of a render and use
  // the ivar other times.
  if (this->Mapped)
    {
    //  Find the current window size 
    XGetWindowAttributes(this->DisplayId, 
                         this->WindowId, &attribs);

    this->Size[0] = attribs.width;
    this->Size[1] = attribs.height;
    }

  // Now do the superclass stuff
  this->vtkOpenGLRenderWindow::Render();
}

//----------------------------------------------------------------------------
void vtkECMXOpenGLRenderWindow::HideCursor()
{
  static char blankBits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
  static XColor black = { 0, 0, 0, 0, 0, 0 };
 
  if (!this->DisplayId || !this->WindowId)
    {
    this->CursorHidden = 1;
    }
  else if (!this->CursorHidden)
    {
    Pixmap blankPixmap = XCreateBitmapFromData(this->DisplayId,
                                               this->WindowId,
                                               blankBits, 16, 16);
    
    Cursor blankCursor = XCreatePixmapCursor(this->DisplayId, blankPixmap,
                                             blankPixmap, &black, &black,
                                             7, 7);
    
    XDefineCursor(this->DisplayId, this->WindowId, blankCursor);
    
    XFreePixmap(this->DisplayId, blankPixmap);
    
    this->CursorHidden = 1;
    }
}

//----------------------------------------------------------------------------
void vtkECMXOpenGLRenderWindow::ShowCursor()
{
  if (!this->DisplayId || !this->WindowId)
    {
    this->CursorHidden = 0;
    }
  else if (this->CursorHidden)
    {
    XUndefineCursor(this->DisplayId, this->WindowId);
    this->CursorHidden = 0;
    }
}                                  


//============================================================================
// Stuff above this is almost a mirror of vtkECMXOpenGLRenderWindow.
// The code specific to OpenGL Off-Screen stuff may eventually be 
// put in a supper class so this whole file could just be included 
// (mangled) from vtkECMXOpenGLRenderWindow like the other OpenGL classes.
//============================================================================

void vtkECMXOpenGLRenderWindow::SetOffScreenRendering(int i)
{
  if (this->OffScreenRendering == i)
    {
    return;
    }

  // invoke super
  this->vtkRenderWindow::SetOffScreenRendering(i);

  if(this->OffScreenRendering)
    {
    this->Internal->ScreenDoubleBuffer = this->DoubleBuffer;
    this->DoubleBuffer = 0;
    if(this->Mapped)
      {
      this->DestroyWindow();
      }

    // delay initialization until Render
    }
  else
    {
    this->DestroyOffScreenWindow();
    
    this->DoubleBuffer = this->Internal->ScreenDoubleBuffer;
     
    // reset size based on screen window
    if(this->Mapped && this->WindowId)
      {
      XWindowAttributes a;
      XGetWindowAttributes(this->DisplayId, this->WindowId, &a);

      this->Size[0] = a.width;
      this->Size[1] = a.height;
      }
    // force context switch as we might be going from osmesa to onscreen
    this->SetForceMakeCurrent();
    }
}

// This probably has been moved to superclass.
void *vtkECMXOpenGLRenderWindow::GetGenericWindowId()
{
#ifdef VTK_OPENGL_HAS_OSMESA
  if (this->OffScreenRendering && this->Internal->OffScreenWindow)
    {
    return reinterpret_cast<void*>(this->Internal->OffScreenWindow);
    }
  else
#endif
    if(this->OffScreenRendering)
      {
      return reinterpret_cast<void*>(this->Internal->PixmapWindowId);
      }
  return reinterpret_cast<void*>(this->WindowId);
}

void vtkECMXOpenGLRenderWindow::SetCurrentCursor(int shape)
{
  if ( this->InvokeEvent(vtkCommand::CursorChangedEvent,&shape) )
    {
    return;
    }
  this->Superclass::SetCurrentCursor(shape);
  if (!this->DisplayId || !this->WindowId)
    {
    return;
    }
  
  if (shape == VTK_CURSOR_DEFAULT)
    {
    XUndefineCursor(this->DisplayId,this->WindowId);
    return;
    }
      
  switch (shape)
    {
    case VTK_CURSOR_CROSSHAIR: // XC_crosshair sucks on linux, default to arrow
    case VTK_CURSOR_ARROW:
      if (!this->XCArrow)
        {
        this->XCArrow = XCreateFontCursor(this->DisplayId, XC_top_left_arrow);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCArrow);
      break;
    case VTK_CURSOR_SIZEALL:
      if (!this->XCSizeAll)
        {
        this->XCSizeAll = XCreateFontCursor(this->DisplayId, XC_fleur);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeAll);
      break;
    case VTK_CURSOR_SIZENS:
      if (!this->XCSizeNS)
        {
        this->XCSizeNS = XCreateFontCursor(this->DisplayId,  
                                           XC_sb_v_double_arrow);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeNS);
      break;
    case VTK_CURSOR_SIZEWE:
      if (!this->XCSizeWE)
        {
        this->XCSizeWE = XCreateFontCursor(this->DisplayId, 
                                           XC_sb_h_double_arrow);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeWE);
      break;
    case VTK_CURSOR_SIZENE:
      if (!this->XCSizeNE)
        {
        this->XCSizeNE = XCreateFontCursor(this->DisplayId, 
                                           XC_top_right_corner);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeNE);
      break;
    case VTK_CURSOR_SIZENW:
      if (!this->XCSizeNW)
        {
        this->XCSizeNW = XCreateFontCursor(this->DisplayId, 
                                           XC_top_left_corner);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeNW);
      break;
    case VTK_CURSOR_SIZESE:
      if (!this->XCSizeSE)
        {
        this->XCSizeSE = XCreateFontCursor(this->DisplayId, 
                                           XC_bottom_right_corner);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeSE);
      break;
    case VTK_CURSOR_SIZESW:
      if (!this->XCSizeSW)
        {
        this->XCSizeSW = XCreateFontCursor(this->DisplayId, 
                                           XC_bottom_left_corner);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCSizeSW);
      break;
    case VTK_CURSOR_HAND:
      if (!this->XCHand)
        {
        this->XCHand = XCreateFontCursor(this->DisplayId, 
                                         XC_hand1);
        }
      XDefineCursor(this->DisplayId, this->WindowId, this->XCHand);
      break;            
    }
}
#endif	// VTK_USE_X, CP

#endif	// USE_OPENGL2_BACKEND	// CP
#endif	// VTK_8	// CP

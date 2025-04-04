/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <mutex>
#include "gpupixel_framebuffer_factory.h"
#include "gpupixel_macros.h"
#include "dispatch_queue.h"

#include "filter.h"
#include "gpupixel_program.h"

#if defined(GPUPIXEL_ANDROID)
  typedef struct _gpu_context_t {
    EGLDisplay egldisplay;
    EGLSurface eglsurface;
    EGLContext eglcontext;
  } _gpu_context_t;
#endif
 
 
namespace gpupixel {
class GPUPIXEL_API GPUPixelContext {
 public:
  static GPUPixelContext* getInstance();
  static void destroy();

  FramebufferFactory* getFramebufferFactory() const;
  //todo(zhaoyou)
  void setActiveShaderProgram(GPUPixelGLProgram* shaderProgram);
  void clean();

  void runSync(std::function<void(void)> func);
  void runAsync(std::function<void(void)> func);
  void useAsCurrent(void);
  void presentBufferForDisplay();
 
#if defined(GPUPIXEL_IOS)
  EAGLContext* getEglContext() const { return _eglContext; };
#elif defined(GPUPIXEL_MAC)
  NSOpenGLContext* getOpenGLContext() const { return imageProcessingContext; };
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  GLFWwindow* GetGLContext() const { return gl_context_; };
#endif
 

  // used for capturing a processed frame data
  bool isCapturingFrame;
  std::shared_ptr<Filter> captureUpToFilter;
  unsigned char* capturedFrameData;
  int captureWidth;
  int captureHeight;

 private:
  GPUPixelContext();
  ~GPUPixelContext();

  void init();

  void createContext();
  void releaseContext();
 private:
  static GPUPixelContext* _instance;
  static std::mutex _mutex;
  FramebufferFactory* _framebufferFactory;
  GPUPixelGLProgram* _curShaderProgram;
  std::shared_ptr<DispatchQueue> task_queue_;
  
#if defined(GPUPIXEL_ANDROID)
  bool context_inited = false;
  int m_surfacewidth;
  int m_surfaceheight;
  _gpu_context_t* m_gpu_context;
#elif defined(GPUPIXEL_IOS)
  EAGLContext* _eglContext;
#elif defined(GPUPIXEL_MAC)
  NSOpenGLContext* imageProcessingContext;
  NSOpenGLPixelFormat* _pixelFormat;
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  GLFWwindow* gl_context_ = nullptr;
#endif

};

}

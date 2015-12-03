/*=====================================================================================
  framebuffer_object.h
Description:  Framebuffer Object Class

Created by Chen Chen on 09/28/2015
=====================================================================================*/

#ifndef FRAMEBUFFER_OBJECT_H_
#define FRAMEBUFFER_OBJECT_H_

#include "headers.h"

//#define FRAMEBUFFER_DEBUG

/*=====================================================================================
      A framebuffer is COMPLETE when the following is satisfied:

      1) Have to attach at least one buffer (color, depth, or stencil buffer)
      2) There should be at least one color attachment (however, for shadow map,
         you can explicitly tell OpenGL you are not going to render any color 
         data by:

              glDrawBuffers(GL_NONE);
              glReadBuffer(GL_NONE);)

      3) All attachment should have reserved memory
      4) Each buffer should have the same number of samples

      There are two attachments you can attach to a framebuffer:

        - Texture attachment 
        
        - Renderbuffer object attachment
  =====================================================================================*/

class FrameBufferObject
{
    public:
        FrameBufferObject(GLuint width, GLuint height);
        ~FrameBufferObject();

        void bind();
        void release();
        void bindDefault();

        GLuint id() const;    

        // Texture Attachment
        void createTexAttachment(GLenum attachment, 
                                 GLuint iFormat, 
                                 GLuint format, 
                                 GLuint type,
                                 bool   noColorbuffer,
                                 GLint  minFilter = GL_LINEAR,
                                 GLint  magFilter = GL_LINEAR,
                                 GLint  wrapS = GL_CLAMP_TO_BORDER,
                                 GLint  wrapT = GL_CLAMP_TO_BORDER);

        // Renderbuffer Object Attachment
        void createRenderBufferAttachment(GLenum attachment, 
                                          GLenum bufferFormat);

        bool checkStatus();
        GLuint maxAttachments();

        // Access Attachment
        GLuint texAttachment(GLuint idx) const;
        GLuint rboAttachment(GLuint idx) const;

        // Handy helper functions for quick framebuffer creation
        void createColorTexture(GLenum attachment = GL_COLOR_ATTACHMENT0);
        void createDepthTexture();
        void createStencilDepthTexture();

        void createStencilRenderbuffer();
        void createStencilDepthRenderbuffer();

        void switchToTexAttachment(GLuint texID, GLenum attachment);

        void checkDrawBuffers();

        // Information
        void info();
        string lookupAttachment(GLenum att);

    private:
        GLuint m_width;
        GLuint m_height;
        GLint  m_prevfboID;

        GLuint m_fboID;

        GLint  m_maxNTexAtt;
        GLuint m_nrTexAtt;
        GLuint m_nrBufferAtt;

        map<GLenum, GLuint> m_texAtts;
        map<GLenum, GLuint> m_rboAtts;

};

#endif /* end of include guard: FRAMEBUFFER_OBJECT_H_ */

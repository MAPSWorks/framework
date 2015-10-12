/*=====================================================================================
                      framebuffer_object_multisample.h

        Description:  Multisample Framebuffer Object Class

                      Created by Chen Chen on 09/28/2015
=====================================================================================*/

#ifndef FRAMEBUFFER_OBJECT_MULTISAMPLE_H_
#define FRAMEBUFFER_OBJECT_MULTISAMPLE_H_

#include "headers.h"

class FrameBufferObjectMultisample {
    public:
        FrameBufferObjectMultisample(GLuint width, 
                                     GLuint height, 
                                     GLuint samples);
        ~FrameBufferObjectMultisample();

        void bind();
        void release();

        void blit();
        void blitDepth();
        void blitColor();

        void setDepthFromFBO(FrameBufferObjectMultisample *fbo);

        GLuint depthTex();
        GLuint colorTex();

        GLuint fboRender() const;
        GLuint fboTexture() const;

    private:
        GLuint m_colorRenderBuffer;
        GLuint m_depthRenderBuffer;
        GLuint m_fbo;
        GLuint m_fboResolve;

        GLuint m_texColor;
        GLuint m_texDepth;

        GLuint m_width;
        GLuint m_height;

        GLint m_filterParam;
};


#endif /* end of include guard: FRAMEBUFFER_OBJECT_MULTISAMPLE_H_ */

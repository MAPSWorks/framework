#include "framebuffer_object_multisample.h"

FrameBufferObjectMultisample::FrameBufferObjectMultisample(GLuint width,
                                                           GLuint height,
                                                           GLuint samples)
    : m_width(width), m_height(height), m_filterParam(GL_NEAREST) {
    GLsizei maxSamples = 0;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    cout << "FRAMEBUFFERMULTISAMPLE::Samples: " << samples << endl;
    cout << "FRAMEBUFFERMULTISAMPLE::MaxSamples: " << maxSamples << endl;

    // Color Renderbuffer
    params::inst().glFuncs->glGenRenderbuffers(1, &m_colorRenderBuffer);
    params::inst().glFuncs->glBindRenderbuffer(GL_RENDERBUFFER,
                                               m_colorRenderBuffer);
    params::inst().glFuncs->glRenderbufferStorageMultisample(
        GL_RENDERBUFFER, samples, GL_RGBA, width, height);

    // Depth Renderbuffer
    params::inst().glFuncs->glGenRenderbuffers(1, &m_depthRenderBuffer);
    params::inst().glFuncs->glBindRenderbuffer(GL_RENDERBUFFER,
                                               m_depthRenderBuffer);
    params::inst().glFuncs->glRenderbufferStorageMultisample(
        GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT32F, width, height);

    // Framebuffer with TWO Renderbuffer Attachment
    params::inst().glFuncs->glGenFramebuffers(1, &m_fbo);
    params::inst().glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    params::inst().glFuncs->glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
        m_colorRenderBuffer);

    params::inst().glFuncs->glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
        m_depthRenderBuffer);

    params::inst().glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Texture
    params::inst().glFuncs->glGenTextures(1, &m_texColor);
    params::inst().glFuncs->glBindTexture(GL_TEXTURE_2D, m_texColor);
    params::inst().glFuncs->glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filterParam);
    params::inst().glFuncs->glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filterParam);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
    params::inst().glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width,
                                         height, 0, GL_RGBA, GL_FLOAT, 0);

    // Depth Texture
    params::inst().glFuncs->glGenTextures(1, &m_texDepth);
    params::inst().glFuncs->glBindTexture(GL_TEXTURE_2D, m_texDepth);
    params::inst().glFuncs->glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filterParam);
    params::inst().glFuncs->glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filterParam);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
    params::inst().glFuncs->glTexImage2D(GL_TEXTURE_2D, 0,
                                         GL_DEPTH_COMPONENT32F, width, height,
                                         0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // Framebuffer with 2 Texture Attachment
    params::inst().glFuncs->glGenFramebuffers(1, &m_fboResolve);
    params::inst().glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, m_fboResolve);
    params::inst().glFuncs->glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texColor, 0);
    params::inst().glFuncs->glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texDepth, 0);
    params::inst().glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, 0);

    cout << "FRAMEBUFFERMULTISAMPLE::Size:" << width << "x" << height << endl;
}

FrameBufferObjectMultisample::~FrameBufferObjectMultisample() {
    params::inst().glFuncs->glDeleteTextures(1, &m_texDepth);
    params::inst().glFuncs->glDeleteTextures(1, &m_texColor);

    params::inst().glFuncs->glDeleteRenderbuffers(1, &m_colorRenderBuffer);
    params::inst().glFuncs->glDeleteRenderbuffers(1, &m_depthRenderBuffer);

    params::inst().glFuncs->glDeleteFramebuffers(1, &m_fbo);
    params::inst().glFuncs->glDeleteFramebuffers(1, &m_fboResolve);

    m_texDepth = 0;
    m_texColor = 0;

    m_colorRenderBuffer = 0;
    m_depthRenderBuffer = 0;

    m_fbo = 0;
    m_fboResolve = 0;

    cout << "FRAMEBUFFERMULTISAMPLE::deleted" << endl;
}

void FrameBufferObjectMultisample::bind() {
    params::inst().glFuncs->glEnable(GL_MULTISAMPLE);
    params::inst().glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void FrameBufferObjectMultisample::release() {
    params::inst().glFuncs->glBindFramebuffer(
        GL_FRAMEBUFFER,
        QOpenGLContext::currentContext()->defaultFramebufferObject());
    params::inst().glFuncs->glDisable(GL_MULTISAMPLE);
}

void FrameBufferObjectMultisample::blit() {
    params::inst().glFuncs->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    params::inst().glFuncs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                                              m_fboResolve);
    params::inst().glFuncs->glBlitFramebuffer(
        0, 0, m_width, m_height, 0, 0, m_width, m_height,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, m_filterParam);
    params::inst().glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObjectMultisample::blitDepth() {
    params::inst().glFuncs->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    params::inst().glFuncs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                                              m_fboResolve);
    params::inst().glFuncs->glBlitFramebuffer(
        0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT,
        m_filterParam);
    params::inst().glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObjectMultisample::blitColor() {
    params::inst().glFuncs->glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    params::inst().glFuncs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                                              m_fboResolve);
    params::inst().glFuncs->glBlitFramebuffer(
        0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT,
        m_filterParam);
    params::inst().glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FrameBufferObjectMultisample::depthTex() { return m_texDepth; }

GLuint FrameBufferObjectMultisample::colorTex() { return m_texColor; }

void FrameBufferObjectMultisample::setDepthFromFBO(
    FrameBufferObjectMultisample *fbo) {
    params::inst().glFuncs->glBindFramebuffer(GL_READ_FRAMEBUFFER,
                                              fbo->fboRender());
    params::inst().glFuncs->glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                                              this->fboRender());

    params::inst().glFuncs->glBlitFramebuffer(
        0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT,
        m_filterParam);

    params::inst().glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

GLuint FrameBufferObjectMultisample::fboRender() const { return m_fbo; }

GLuint FrameBufferObjectMultisample::fboTexture() const { return m_fboResolve; }

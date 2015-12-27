#include "framebuffer_object.h"

FrameBufferObject::FrameBufferObject(GLuint width, GLuint height)
    : m_width(max<GLuint>(1, width)),
      m_height(max<GLuint>(1, height)),
      m_fboID(0),
      m_nrTexAtt(0),
      m_nrBufferAtt(0) {
    params::inst().glFuncs->glGenFramebuffers(1, &m_fboID);
    m_maxNTexAtt = maxAttachments();
}

FrameBufferObject::~FrameBufferObject() {
    // Delete texture attachments
    for (auto& tex : m_texAtts) {
        if (tex.second != 0) {
            params::inst().glFuncs->glDeleteTextures(1, &tex.second);
        }
    }

    // Delete renderbuffer object attachment
    for (auto& rbo : m_rboAtts) {
        if (rbo.second != 0) {
            params::inst().glFuncs->glDeleteRenderbuffers(1, &rbo.second);
        }
    }

    params::inst().glFuncs->glDeleteFramebuffers(1, &m_fboID);

#ifdef FRAMEBUFFER_DEBUG
    cout << "\tFRAMEBUFFER::deleted" << endl;
#endif
}

void FrameBufferObject::bind() {
    // Get previous framebuffer id
    params::inst().glFuncs->glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,
                                          &m_prevfboID);
    params::inst().glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
}

void FrameBufferObject::release() {
    params::inst().glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, m_prevfboID);
}

void FrameBufferObject::bindDefault() {
    params::inst().glFuncs->glBindFramebuffer(
        GL_FRAMEBUFFER,
        QOpenGLContext::currentContext()->defaultFramebufferObject());
}

GLuint FrameBufferObject::id() const { return m_fboID; }

// Texture Attachment
// - attachment:
//      - Color attachment: GL_COLOR_ATTACHMENT0 + i
//
//      - Depth attachment: GL_DEPTH_ATTACHMENT
//          !!! iFormat has to be: GL_DEPTH_COMPONENT
//          !!! format has to be: GL_DEPTH_COMPONENT
//
//      - Stencil attachment: GL_STENCIL_ATTACHMENT
//          !!! iFormat has to be: GL_STENCIL_ATTACHMENT
//          !!! format has to be: GL_STENCIL_INDEX
//
//      - Depth and stencil attachment: GL_DEPTH_STENCIL_ATTACHMENT
//          !!! iFormat has to be: GL_DEPTH24_STENCIL8
//          !!! format has to be: GL_DEPTH_STENCIL
//          !!! type has to be: GL_UNSIGNED_INT_24_8
//
// - iFormat:
//      - GL_RGB
//      - GL_RGBA
//      - GL_DEPTH_COMPONENT
//      - GL_STENCIL_ATTACHMENT
// - format:
//      - GL_RGB
//      - GL_RGBA
//      - GL_DEPTH_COMPONENT
//      - GL_STENCIL_INDEX
// - type:
//      - GL_UNSIGNED_BYTE
//      - GL_UNSIGNED_INT_24_8
//      - ...
//
// - minFilter
//
// - magFilter
//
// - wrapS
//
// - wrapT
void FrameBufferObject::createTexAttachment(GLenum attachment, GLuint iFormat,
                                            GLuint format, GLuint type,
                                            bool noColorbuffer, GLint minFilter,
                                            GLint magFilter, GLint wrapS,
                                            GLint wrapT) {
    if (m_texAtts.size() >= m_maxNTexAtt) {
        cout << "Warning: Cannot create texture attachment for Framebuffer: "
                "reached maximum limits."
             << endl;
        return;
    }

    bind();

    GLuint texture;
    params::inst().glFuncs->glGenTextures(1, &texture);
    params::inst().glFuncs->glBindTexture(GL_TEXTURE_2D, texture);
    params::inst().glFuncs->glTexImage2D(GL_TEXTURE_2D, 0, iFormat, m_width,
                                         m_height, 0, format, type, NULL);

    params::inst().glFuncs->glTexParameteri(GL_TEXTURE_2D,
                                            GL_TEXTURE_MIN_FILTER, minFilter);
    params::inst().glFuncs->glTexParameteri(GL_TEXTURE_2D,
                                            GL_TEXTURE_MAG_FILTER, magFilter);
    params::inst().glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                            wrapS);
    params::inst().glFuncs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                            wrapT);

    if (noColorbuffer) {
        params::inst().glFuncs->glDrawBuffer(GL_NONE);
        params::inst().glFuncs->glReadBuffer(GL_NONE);
    }

    // Attach texture
    params::inst().glFuncs->glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
                                                   GL_TEXTURE_2D, texture, 0);
    m_texAtts[attachment] = texture;

    params::inst().glFuncs->glBindTexture(GL_TEXTURE_2D, 0);

    release();
}

// Renderbuffer Object Attachment
// - attachment:
//      - Color attachment: GL_COLOR_ATTACHMENT0 + i
//      - Depth attachment: GL_DEPTH_ATTACHMENT
//      - Stencil attachment: GL_STENCIL_ATTACHMENT
//
// - bufferFormat:
//      - e.g., GL_DEPTH24_STENCIL8, GL_RGBA8, GL_DEPTH_COMPONENT, etc.
void FrameBufferObject::createRenderBufferAttachment(GLenum attachment,
                                                     GLenum bufferFormat) {
    bind();

    GLuint rbo;
    params::inst().glFuncs->glGenRenderbuffers(1, &rbo);
    params::inst().glFuncs->glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    params::inst().glFuncs->glRenderbufferStorage(GL_RENDERBUFFER, bufferFormat,
                                                  m_width, m_height);

    // Attach rbo
    params::inst().glFuncs->glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo);
    m_rboAtts[attachment] = rbo;
    params::inst().glFuncs->glBindRenderbuffer(GL_RENDERBUFFER, 0);

    release();
}

bool FrameBufferObject::checkStatus() {
    bind();

    GLenum status =
        params::inst().glFuncs->glCheckFramebufferStatus(GL_FRAMEBUFFER);
    GLboolean result = false;

    switch (status) {
        case GL_FRAMEBUFFER_COMPLETE:
            std::cout << "\tFRAMEBUFFER::Complete" << std::endl;
            std::cout << "\tFRAMEBUFFER::Size: " << m_width << "x" << m_height
                      << std::endl;
            result = true;
            break;
        case GL_FRAMEBUFFER_UNDEFINED:
            std::cout << "[ERROR]FRAMEBUFFER::undefined" << std::endl;
            result = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            std::cout
                << "[ERROR]FRAMEBUFFER::incomplete: Attachment is NOT complete"
                << std::endl;
            result = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            std::cout
                << "[ERROR]FRAMEBUFFER::incomplete: No image is attached to FBO"
                << std::endl;
            result = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            std::cout << "[ERROR]FRAMEBUFFER::incomplete: Draw buffer"
                      << std::endl;
            result = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            std::cout << "[ERROR]FRAMEBUFFER::incomplete: Read buffer"
                      << std::endl;
            result = false;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            std::cout << "[ERROR]FRAMEBUFFER::unsupported" << std::endl;
            result = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            std::cout << "[ERROR]FRAMEBUFFER::incomplete: multisample"
                      << std::endl;
            result = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            std::cout << "[ERROR]FRAMEBUFFER::incomplete: layer targets"
                      << std::endl;
            result = false;
            break;
        default:
            std::cout << "[ERROR]FRAMEBUFFER::Unknown error" << std::endl;
            result = false;
            break;
    }

    release();

    return result;
}

GLuint FrameBufferObject::maxAttachments() {
    GLint maxAtt = 0;

    params::inst().glFuncs->glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAtt);

    return maxAtt;
}

// Handy Helper Functions
void FrameBufferObject::createColorTexture(GLenum attachment) {
    createTexAttachment(attachment, GL_RGBA32F, GL_RGBA, GL_FLOAT, false);
}

void FrameBufferObject::createDepthTexture() {
    createTexAttachment(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT,
                        GL_DEPTH_COMPONENT, GL_FLOAT, true);
    params::inst().glFuncs->glBindTexture(GL_TEXTURE_2D,
                                          m_texAtts[GL_DEPTH_ATTACHMENT]);

    // Clamp to border
    GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    params::inst().glFuncs->glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBufferObject::createStencilDepthTexture() {
    createTexAttachment(GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8,
                        GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, true);
}

void FrameBufferObject::createStencilRenderbuffer() {
    createRenderBufferAttachment(GL_STENCIL_ATTACHMENT, GL_STENCIL_INDEX16);
}

void FrameBufferObject::createStencilDepthRenderbuffer() {
    createRenderBufferAttachment(GL_DEPTH_STENCIL_ATTACHMENT,
                                 GL_DEPTH24_STENCIL8);
}

void FrameBufferObject::switchToTexAttachment(GLuint texID, GLenum attachment) {
    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texID, 0);
    release();
}

GLuint FrameBufferObject::texAttachment(GLenum attachment) const {
    map<GLenum, GLuint>::const_iterator iter = m_texAtts.find(attachment);

    GLuint id = 0;
    if (iter != m_texAtts.end()) id = iter->second;

    return id;
}

GLuint FrameBufferObject::rboAttachment(GLenum attachment) const {
    map<GLenum, GLuint>::const_iterator iter = m_rboAtts.find(attachment);

    GLuint id = 0;
    if (iter != m_rboAtts.end()) id = iter->second;

    return id;
}

void FrameBufferObject::info() {
    GLint maxAtt = 0;
    params::inst().glFuncs->glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAtt);

    GLint maxRenderbufferSize;
    params::inst().glFuncs->glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,
                                          &maxRenderbufferSize);

    GLint attObjectType_0 = 0;
    params::inst().glFuncs->glGetFramebufferAttachmentParameteriv(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &attObjectType_0);

    GLint attObjectName_0 = 0;
    params::inst().glFuncs->glGetFramebufferAttachmentParameteriv(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &attObjectName_0);

    GLint attRedSize_0 = 0;
    params::inst().glFuncs->glGetFramebufferAttachmentParameteriv(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE, &attRedSize_0);

    GLint attGreenSize_0 = 0;
    params::inst().glFuncs->glGetFramebufferAttachmentParameteriv(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE, &attGreenSize_0);

    GLint attBlueSize_0 = 0;
    params::inst().glFuncs->glGetFramebufferAttachmentParameteriv(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE, &attBlueSize_0);

    GLint rbWidth = 0;
    params::inst().glFuncs->glGetRenderbufferParameteriv(
        GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH,
        &rbWidth);  //?? RB not bound

    GLint rbHeight = 0;
    params::inst().glFuncs->glGetRenderbufferParameteriv(GL_RENDERBUFFER,
                                                         GL_RENDERBUFFER_HEIGHT,
                                                         &rbHeight);  //??

    std::cout << "\tFRAMEBUFFER::MaxAttachments: " << maxAtt << std::endl;
    std::cout << "\tFRAMEBUFFER::MaxRenderbufferSize: " << maxRenderbufferSize
              << std::endl;
    std::cout << "\tFRAMEBUFFER::AttachmentObjectType[0]: " << attObjectType_0
              << std::endl;
    std::cout << "\tFRAMEBUFFER::AttachmentObjectName[0]: " << attObjectName_0
              << std::endl;
    std::cout << "\tFRAMEBUFFER::AttachmentBlueSize[0]: " << attRedSize_0
              << std::endl;
    std::cout << "\tFRAMEBUFFER::AttachmentGreenSize[0]: " << attGreenSize_0
              << std::endl;
    std::cout << "\tFRAMEBUFFER::AttachmentBlueSize[0]: " << attBlueSize_0
              << std::endl;
    std::cout << "\tFRAMEBUFFER::RenderbufferWidth: " << rbWidth << std::endl;
    std::cout << "\tFRAMEBUFFER::RenderbufferHeight: " << rbHeight << std::endl;
}

std::string FrameBufferObject::lookupAttachment(GLenum att) {
    std::string text;

    switch (att) {
        case GL_COLOR_ATTACHMENT0:
            text = "GL_COLOR_ATTACHMENT0";
            break;
        case GL_COLOR_ATTACHMENT1:
            text = "GL_COLOR_ATTACHMENT1";
            break;
        case GL_COLOR_ATTACHMENT2:
            text = "GL_COLOR_ATTACHMENT2";
            break;
        case GL_COLOR_ATTACHMENT3:
            text = "GL_COLOR_ATTACHMENT3";
            break;
        case GL_COLOR_ATTACHMENT4:
            text = "GL_COLOR_ATTACHMENT4";
            break;
        case GL_COLOR_ATTACHMENT5:
            text = "GL_COLOR_ATTACHMENT5";
            break;
        case GL_COLOR_ATTACHMENT6:
            text = "GL_COLOR_ATTACHMENT6";
            break;
        case GL_COLOR_ATTACHMENT7:
            text = "GL_COLOR_ATTACHMENT7";
            break;
        case GL_COLOR_ATTACHMENT8:
            text = "GL_COLOR_ATTACHMENT8";
            break;
        case GL_DEPTH_ATTACHMENT:
            text = "GL_DEPTH_ATTACHMENT";
            break;
        case GL_STENCIL_ATTACHMENT:
            text = "GL_STENCIL_ATTACHMENT";
            break;
        case GL_DEPTH_STENCIL_ATTACHMENT:
            text = "GL_DEPTH_STENCIL_ATTACHMENT";
            break;
    }

    return text;
}

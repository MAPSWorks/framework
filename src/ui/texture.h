#ifndef TEXTURE_H
#define TEXTURE_H

#include "headers.h"

class Texture  {
public:
    // Default texture from QImage. Default settings:
    //  - m_target: GL_TEXTURE_2D
    //  - m_minFilter: GL_LINEAR_MIPMAP_LINEAR
    //  - m_magFilter: GL_LINEAR
    Texture(const char *imageName);
    Texture(const char *imageName, GLint magFilter, GLint minFilter,
            GLint wrapMode = GL_REPEAT);

    // 2D Texture array from a sequence of images
    Texture(const vector<string>& imageNames);

    ~Texture();

    void bind();
    void release();

    void setWrapMode(GLint wrap);
    void setFilter(GLint minFilter, GLint magFilter);

    GLuint id() const;
    GLuint width() const;
    GLuint height() const;

private:
    void create();

    GLvoid *m_data = nullptr;
    vector<QImage> m_frameImages;

    GLsizei m_width = 0;
    GLsizei m_height = 0;
    GLsizei m_depth = 0;

    GLuint m_id = 0;
    GLenum m_target;         // Default is GL_TEXTURE_2D
    GLint m_internalFormat;  // internal format of the texture, default is
                             // GL_RGBA8. Used in glTexStorage2D
    GLenum m_format;  // Specifies the format of the texel data. Must match
    // internal format. Default is GL_RGBA. Used in glTexSubImage2D
    GLenum m_type;  // Data type of the pixel data. Default is GL_UNSIGNED_BYTE.
                    // Used in glTexSubImage2D

    // Magnification and Minimization Filters
    GLint m_minFilter;
    GLint m_magFilter;

    // Wrap mode
    GLint m_wrap;  // Default is GL_CLAMP_TO_BORDER

    GLboolean m_createMipMaps = true;
    GLint m_mipLevel = 0;
};

#endif

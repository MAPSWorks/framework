#ifndef TEXTURE_H
#define TEXTURE_H

#include "headers.h"

class Texture
{

    public:
        Texture();
        Texture(GLsizei w, GLsizei h, GLint iFormat, GLint format, GLint type);
        Texture(GLsizei w, GLsizei h, GLint iFormat, GLint format, GLint type, GLvoid *data);

        Texture(const QImage &img);
        Texture(const QImage &img, 
                GLint magFilter, 
                GLint minFilter, 
                GLfloat anisotrophy, 
                GLboolean createMipmaps);

        Texture(const char* imageName);
        Texture(const char* imageName, 
                GLint wrapMode,
                GLint magFilter, 
                GLint minFilter, 
                GLfloat anisotrophy, 
                GLboolean createMipmaps);

        Texture(QString path);
        ~Texture();

        void bind();
        void release();

        void setWrapMode(GLint wrap);
        void setEnvMode(GLint envMode);
        void setFilter(GLint minFilter, GLint magFilter);
        void setMaxIsotropy(GLfloat anisotropy);

        void deleteTex();

        GLuint id() const;
        GLuint width() const;
        GLuint height() const;

    private:
        void create();
        GLuint              m_id;
        GLsizei             m_width;
        GLsizei             m_height;
        GLenum              m_target;
        GLint               m_mipLevel;
        GLint               m_internalFormat;
        GLenum              m_format;
        GLint               m_border;
        GLenum              m_type;
        GLvoid              *m_data;
        GLint               m_minFilter;
        GLint               m_magFilter;
        GLint               m_wrap;
        GLint               m_envMode;
        GLboolean           m_createMipMaps;
        GLfloat             m_maxAnisotropy;
};

#endif

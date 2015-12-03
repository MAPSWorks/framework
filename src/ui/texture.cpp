#include "texture.h"

//Default
Texture::Texture()
: m_id(0),
  m_width(0),
  m_height(0),
  m_target(GL_TEXTURE_2D),
  m_mipLevel(0),
  m_internalFormat(GL_RGBA),
  m_format(GL_RGBA),
  m_border(0),
  m_type(GL_UNSIGNED_BYTE),
  m_data(NULL),
  m_minFilter(GL_NEAREST),
  m_magFilter(GL_NEAREST),
  m_wrap(GL_CLAMP),
  m_envMode(GL_REPLACE),
  m_createMipMaps(GL_FALSE),
  m_maxAnisotropy(1.0f)
{

}

//Create empty texture
Texture::Texture(GLsizei w, GLsizei h, GLint iFormat, GLint format, GLint type)
: m_id(0),
  m_width(w),
  m_height(h),
  m_target(GL_TEXTURE_2D),
  m_mipLevel(0),
  m_internalFormat(iFormat),
  m_format(format),
  m_border(0),
  m_type(type),
  m_data(NULL),
  m_minFilter(GL_NEAREST),
  m_magFilter(GL_NEAREST),
  m_wrap(GL_CLAMP),
  m_envMode(GL_REPLACE),
  m_createMipMaps(GL_FALSE),
  m_maxAnisotropy(1.0f)
{
    create();
}

//Pass with Data Pointer
Texture::Texture(GLsizei w, GLsizei h, GLint iFormat, GLint format, GLint type, GLvoid *data)
: m_id(0),
  m_width(w),
  m_height(h),
  m_target(GL_TEXTURE_2D),
  m_mipLevel(0),
  m_internalFormat(iFormat),
  m_format(format),
  m_border(0),
  m_type(type),
  m_data(data),
  m_minFilter(GL_NEAREST),
  m_magFilter(GL_NEAREST),
  m_wrap(GL_CLAMP),
  m_envMode(GL_REPLACE),
  m_createMipMaps(GL_TRUE),
  m_maxAnisotropy(0.0f)
{
    create();
}

//Load from QImage
Texture::Texture(const QImage &img)
: m_id(0),
  m_width(0),
  m_height(0),
  m_target(GL_TEXTURE_2D),
  m_mipLevel(0),
  m_internalFormat(GL_RGBA),
  m_format(GL_BGRA),
  m_border(0),
  m_type(GL_UNSIGNED_BYTE),
  m_data(NULL),
  m_minFilter(GL_LINEAR_MIPMAP_LINEAR),
  m_magFilter(GL_LINEAR),
  m_wrap(GL_CLAMP),
  m_envMode(GL_REPLACE),
  m_createMipMaps(GL_TRUE),
  m_maxAnisotropy(16.0f)
{
    QImage swapped_img = img.convertToFormat(QImage::Format_RGBA8888);
    m_data = (GLvoid*)swapped_img.bits();
    m_width = (GLuint)swapped_img.width();
    m_height = (GLuint)swapped_img.height();

    create();
}

Texture::Texture(const QImage &img, GLint magFilter, GLint minFilter, GLfloat anisotrophy, GLboolean createMipmaps)
: m_id(0),
  m_width(0),
  m_height(0),
  m_target(GL_TEXTURE_2D),
  m_mipLevel(0),
  m_internalFormat(GL_RGBA),
  m_format(GL_BGRA),
  m_border(0),
  m_type(GL_UNSIGNED_BYTE),
  m_data(NULL),
  m_minFilter(minFilter),
  m_magFilter(magFilter),
  m_wrap(GL_CLAMP),
  m_envMode(GL_REPLACE),
  m_createMipMaps(createMipmaps),
  m_maxAnisotropy(anisotrophy)
{
    QImage swapped_img = img.convertToFormat(QImage::Format_RGBA8888);
    m_data = (GLvoid*)swapped_img.bits();
    m_width = (GLuint)swapped_img.width();
    m_height = (GLuint)swapped_img.height();

    create();
}

Texture::Texture(const char* imageName)
: m_id(0),
  m_width(0),
  m_height(0),
  m_target(GL_TEXTURE_2D),
  m_mipLevel(0),
  m_internalFormat(GL_RGBA),
  m_format(GL_RGBA),
  m_border(0),
  m_type(GL_UNSIGNED_BYTE),
  m_data(NULL),
  m_minFilter(GL_LINEAR_MIPMAP_LINEAR),
  m_magFilter(GL_LINEAR),
  m_wrap(GL_CLAMP),
  m_envMode(GL_REPLACE),
  m_createMipMaps(GL_TRUE),
  m_maxAnisotropy(16.0f) 
{
    QImage img = QImage(imageName).convertToFormat(QImage::Format_RGBA8888);
    if (img.isNull()) { 
        cout << "ERROR::TEXTURE::Texture() not found " << imageName << endl;
        return;
    } 

    m_data = (GLvoid*)img.bits();
    m_width = (GLuint)img.width();
    m_height = (GLuint)img.height();  

    if (m_createMipMaps) { 
        m_minFilter = GL_LINEAR_MIPMAP_LINEAR;
    } 
    else{
        m_minFilter = GL_LINEAR;
    }

    create();
}
        
Texture::Texture(const char* imageName, 
                 GLint wrapMode,
                 GLint magFilter, 
                 GLint minFilter, 
                 GLfloat anisotrophy, 
                 GLboolean createMipmaps)
: m_id(0),
  m_width(0),
  m_height(0),
  m_target(GL_TEXTURE_2D),
  m_mipLevel(0),
  m_internalFormat(GL_RGBA),
  m_format(GL_RGBA),
  m_border(0),
  m_type(GL_UNSIGNED_BYTE),
  m_data(NULL),
  m_minFilter(minFilter),
  m_magFilter(magFilter),
  m_wrap(wrapMode),
  m_envMode(GL_REPLACE),
  m_createMipMaps(createMipmaps),
  m_maxAnisotropy(anisotrophy)
{

}

Texture::Texture(QString path)
: m_id(0),
  m_width(0),
  m_height(0),
  m_target(GL_TEXTURE_2D),
  m_mipLevel(0),
  m_internalFormat(GL_RGBA),
  m_format(GL_RGBA),
  m_border(0),
  m_type(GL_UNSIGNED_BYTE),
  m_data(NULL),
  m_minFilter(GL_LINEAR_MIPMAP_LINEAR),
  m_magFilter(GL_LINEAR),
  m_wrap(GL_CLAMP),
  m_envMode(GL_REPLACE),
  m_createMipMaps(GL_TRUE),
  m_maxAnisotropy(16.0f)
{
    if (!QFileInfo::exists(path)) { 
        std::cout << "ERROR::TEXTURE::Texture() not found " << path.toStdString() << std::endl;
        return;
    } 

    QImage img = QImage(path).convertToFormat(QImage::Format_RGBA8888);

    m_data = (GLvoid*)img.bits();
    m_width = (GLuint)img.width();
    m_height = (GLuint)img.height();  

    if (m_createMipMaps) { 
        m_minFilter = GL_LINEAR_MIPMAP_LINEAR;
    } 
    else{
        m_minFilter = GL_LINEAR;
    }

    create();
}

Texture::~Texture()
{
    deleteTex();
}

void Texture::create()
{

    glGenTextures(1, &m_id);	
    glBindTexture(m_target, m_id);    

    if(m_createMipMaps)
    {           
        glTexImage2D(m_target, m_mipLevel, m_internalFormat, m_width, m_height, m_border, m_format, m_type, m_data);     
        glGenerateMipmap(m_target);
    }
    else
    {
        glTexImage2D(m_target, 0, m_internalFormat, m_width, m_height, m_border, m_format, m_type, m_data);     
    }

    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, m_minFilter);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, m_magFilter); 

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_envMode);

    glTexParameterf(m_target, GL_TEXTURE_WRAP_S, m_wrap);
    glTexParameterf(m_target, GL_TEXTURE_WRAP_T, m_wrap);
    glTexParameterf(m_target, GL_TEXTURE_WRAP_R, m_wrap);

    glTexParameterf(m_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy);

}

void Texture::bind()
{
    glBindTexture(m_target, m_id);
}

void Texture::release()
{
    glBindTexture(m_target, 0);
}

void Texture::deleteTex()
{
	if (m_id != 0)
	{
		glDeleteTextures(1, &m_id);	
	}
}

GLuint Texture::id() const
{
    return m_id;
}

void Texture::setWrapMode(GLint wrap)
{
    m_wrap = wrap;

    bind();

    glTexParameterf(m_target, GL_TEXTURE_WRAP_S, m_wrap);
    glTexParameterf(m_target, GL_TEXTURE_WRAP_T, m_wrap);
    glTexParameterf(m_target, GL_TEXTURE_WRAP_R, m_wrap);

    release();
}

void Texture::setEnvMode(GLint envMode)
{
    m_envMode = envMode;

    bind();

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_envMode);

    release();
}

void Texture::setFilter(GLint minFilter, GLint magFilter)
{
    m_minFilter = minFilter;
    m_magFilter = magFilter; 

    bind();

    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, m_minFilter);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, m_magFilter); 

    release();
}

void Texture::setMaxIsotropy(GLfloat anisotropy)
{
    m_maxAnisotropy = anisotropy;

    bind();

    glTexParameterf(m_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy);  

    release();
}

GLuint Texture::width() const
{
    return m_width;
}

GLuint Texture::height() const
{
    return m_height;
}

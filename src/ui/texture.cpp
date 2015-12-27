#include "texture.h"

Texture::Texture(const char* imageName)
    : m_target(GL_TEXTURE_2D),
      m_internalFormat(GL_RGBA8),
      m_format(GL_RGBA),
      m_type(GL_UNSIGNED_BYTE),
      m_minFilter(GL_LINEAR_MIPMAP_LINEAR),
      m_magFilter(GL_LINEAR),
      m_wrap(GL_REPEAT) {
    QImage img = QImage(imageName).convertToFormat(QImage::Format_RGBA8888);

    if (img.isNull()) {
        cout << "ERROR::TEXTURE::Texture() not found " << imageName << endl;
        return;
    }

    m_data = (GLvoid*)img.bits();
    m_width = (GLuint)img.width();
    m_height = (GLuint)img.height();
    create();
}

Texture::Texture(const char* imageName, GLint magFilter, GLint minFilter,
                 GLint wrapMode)
    : m_target(GL_TEXTURE_2D),
      m_internalFormat(GL_RGBA8),
      m_format(GL_BGRA),
      m_type(GL_UNSIGNED_BYTE),
      m_minFilter(minFilter),
      m_magFilter(magFilter),
      m_wrap(wrapMode) {
    QImage img = QImage(imageName).convertToFormat(QImage::Format_RGBA8888);

    if (img.isNull()) {
        cout << "ERROR::TEXTURE::Texture() not found " << imageName << endl;
        return;
    }

    m_data = (GLvoid*)img.bits();
    m_width = (GLuint)img.width();
    m_height = (GLuint)img.height();
    create();
}

Texture::Texture(const vector<string>& imageNames)
    : m_target(GL_TEXTURE_2D_ARRAY),
      m_internalFormat(GL_RGBA8),
      m_format(GL_RGBA),
      m_type(GL_UNSIGNED_BYTE),
      m_minFilter(GL_LINEAR_MIPMAP_LINEAR),
      m_magFilter(GL_LINEAR),
      m_wrap(GL_REPEAT) {

    bool isFirstImage = true;
    for (size_t i = 0; i < imageNames.size(); ++i) {
        QImage img = QImage(imageNames[i].c_str())
                         .convertToFormat(QImage::Format_RGBA8888);
        if (img.isNull()) {
            cout << "ERROR: Cannot load image: " << imageNames[i] << endl;
            return;
        }
        if (isFirstImage) {
            m_width = (GLuint)img.width();
            m_height = (GLuint)img.height();
            isFirstImage = false;
        } else {
            // Check frame images. They should have same size.
            if (m_width != (GLuint)img.width() ||
                m_height != (GLuint)img.height()) {
                cout << "ERROR from Texture: images are not in the same size!"
                     << endl;
                return;
            }
        }
        m_frameImages.push_back(img);
    }
    m_depth = m_frameImages.size();
    cout << "3D texture loade. There are " << m_frameImages.size() << " images."
         << endl;

    create();
}

Texture::~Texture() {
    if (m_id != 0) {
        params::inst().glFuncs->glDeleteTextures(1, &m_id);
    }
}

void Texture::create() {
    params::inst().glFuncs->glGenTextures(1, &m_id);
    params::inst().glFuncs->glBindTexture(m_target, m_id);

    switch (m_target) {
        case GL_TEXTURE_2D:
            params::inst().glFuncs->glTexImage2D(
                GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0,
                m_format, m_type, m_data);
            params::inst().glFuncs->glGenerateMipmap(
                GL_TEXTURE_2D);  // Generate mipmaps now
            params::inst().glFuncs->glTexParameteri(
                GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
            params::inst().glFuncs->glTexParameteri(
                GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);
            params::inst().glFuncs->glTexParameterf(GL_TEXTURE_2D,
                                                    GL_TEXTURE_WRAP_S, m_wrap);
            params::inst().glFuncs->glTexParameterf(GL_TEXTURE_2D,
                                                    GL_TEXTURE_WRAP_T, m_wrap);
            break;
        case GL_TEXTURE_2D_ARRAY:
            // Create storage for 3D texture
            params::inst().glFuncs->glTexImage3D(
                GL_TEXTURE_2D_ARRAY, 0, m_internalFormat, m_width, m_height,
                m_depth, 0, m_format, m_type, NULL);
            // Setup data
            for (size_t i = 0; i < m_frameImages.size(); ++i) {
                m_data = (GLvoid*)m_frameImages[i].bits();
                params::inst().glFuncs->glTexSubImage3D(
                    GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, m_width, m_height, 1,
                    m_format, m_type, m_data);
            }
            params::inst().glFuncs->glGenerateMipmap(
                GL_TEXTURE_2D_ARRAY);  // Generate mipmaps now
            params::inst().glFuncs->glTexParameteri(
                GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, m_minFilter);
            params::inst().glFuncs->glTexParameteri(
                GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, m_magFilter);
            params::inst().glFuncs->glTexParameterf(GL_TEXTURE_2D_ARRAY,
                                                    GL_TEXTURE_WRAP_S, m_wrap);
            params::inst().glFuncs->glTexParameterf(GL_TEXTURE_2D_ARRAY,
                                                    GL_TEXTURE_WRAP_T, m_wrap);
            params::inst().glFuncs->glTexParameterf(GL_TEXTURE_2D_ARRAY,
                                                    GL_TEXTURE_WRAP_R, m_wrap);
            m_frameImages.clear();
            break;
        default:
            break;
    }
}

void Texture::bind() { params::inst().glFuncs->glBindTexture(m_target, m_id); }

void Texture::release() { params::inst().glFuncs->glBindTexture(m_target, 0); }

GLuint Texture::id() const { return m_id; }

void Texture::setWrapMode(GLint wrap) {
    m_wrap = wrap;

    bind();

    params::inst().glFuncs->glTexParameterf(m_target, GL_TEXTURE_WRAP_S,
                                            m_wrap);
    params::inst().glFuncs->glTexParameterf(m_target, GL_TEXTURE_WRAP_T,
                                            m_wrap);
    params::inst().glFuncs->glTexParameterf(m_target, GL_TEXTURE_WRAP_R,
                                            m_wrap);

    release();
}

void Texture::setFilter(GLint minFilter, GLint magFilter) {
    m_minFilter = minFilter;
    m_magFilter = magFilter;

    bind();

    params::inst().glFuncs->glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER,
                                            m_minFilter);
    params::inst().glFuncs->glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER,
                                            m_magFilter);

    release();
}

GLuint Texture::width() const { return m_width; }

GLuint Texture::height() const { return m_height; }

#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

#include "main_window.h"

Shader::Shader(Config conf)
    : m_vFileName(NULL),
      m_cFileName(NULL),
      m_eFileName(NULL),
      m_gFileName(NULL),
      m_fFileName(NULL),
      m_vOldDateTime(),
      m_cOldDateTime(),
      m_eOldDateTime(),
      m_gOldDateTime(),
      m_fOldDateTime(),
      m_id(0),
      m_vertProg(0),
      m_contProg(0),
      m_evalProg(0),
      m_geomProg(0),
      m_fragProg(0),
      m_refreshTime(1000),
      m_firstUpdate(true) {
    if (!m_id) m_id = glCreateProgram();

    m_timer = new QTimer();

    if (conf == AUTO_LOAD) {
        connect(m_timer, SIGNAL(timeout()), this, SLOT(autoUpdate()));
        m_timer->start(m_refreshTime);
    }
}

Shader::Shader(const char *vFileName, const char *fFileName, Config conf)
    : m_vFileName(NULL),
      m_cFileName(NULL),
      m_eFileName(NULL),
      m_gFileName(NULL),
      m_fFileName(NULL),
      m_vOldDateTime(),
      m_cOldDateTime(),
      m_eOldDateTime(),
      m_gOldDateTime(),
      m_fOldDateTime(),
      m_id(0),
      m_vertProg(0),
      m_contProg(0),
      m_evalProg(0),
      m_geomProg(0),
      m_fragProg(0),
      m_refreshTime(1000),
      m_firstUpdate(true) {
    if (!m_id) m_id = glCreateProgram();

    attachVertexShader(vFileName);
    attachFragmentShader(fFileName);

    link();

    m_timer = new QTimer();

    if (conf == AUTO_LOAD) {
        connect(m_timer, SIGNAL(timeout()), this, SLOT(autoUpdate()));
        m_timer->start(m_refreshTime);
    }
}

Shader::Shader(const char *vFileName, const char *gFileName,
               const char *fFileName, Config conf)
    : m_vFileName(NULL),
      m_cFileName(NULL),
      m_eFileName(NULL),
      m_gFileName(NULL),
      m_fFileName(NULL),
      m_vOldDateTime(),
      m_cOldDateTime(),
      m_eOldDateTime(),
      m_gOldDateTime(),
      m_fOldDateTime(),
      m_id(0),
      m_vertProg(0),
      m_contProg(0),
      m_evalProg(0),
      m_geomProg(0),
      m_fragProg(0),
      m_refreshTime(1000),
      m_firstUpdate(true) {
    if (!m_id) m_id = glCreateProgram();

    attachVertexShader(vFileName);
    attachGeometryShader(gFileName);
    attachFragmentShader(fFileName);

    link();

    m_timer = new QTimer();

    if (conf == AUTO_LOAD) {
        connect(m_timer, SIGNAL(timeout()), this, SLOT(autoUpdate()));
        m_timer->start(m_refreshTime);
    }
}

Shader::Shader(const char *vFileName, const char *tcFileName,
               const char *teFileName, const char *gFileName,
               const char *fFileName, Config conf)
    : m_vFileName(NULL),
      m_cFileName(NULL),
      m_eFileName(NULL),
      m_gFileName(NULL),
      m_fFileName(NULL),
      m_vOldDateTime(),
      m_cOldDateTime(),
      m_eOldDateTime(),
      m_gOldDateTime(),
      m_fOldDateTime(),
      m_id(0),
      m_vertProg(0),
      m_contProg(0),
      m_evalProg(0),
      m_geomProg(0),
      m_fragProg(0),
      m_refreshTime(1000),
      m_firstUpdate(true) {
    if (!m_id) m_id = glCreateProgram();

    attachVertexShader(vFileName);
    attachControlShader(tcFileName);
    attachEvaluationShader(teFileName);
    attachGeometryShader(gFileName);
    attachFragmentShader(fFileName);

    link();

    m_timer = new QTimer();

    if (conf == AUTO_LOAD) {
        connect(m_timer, SIGNAL(timeout()), this, SLOT(autoUpdate()));
        m_timer->start(m_refreshTime);
    }
}

Shader::~Shader() {
    m_timer->stop();
    delete m_timer;

    cleanUp();
}

void Shader::bind() const { params::inst().glFuncs->glUseProgram(m_id); }

void Shader::release() const { params::inst().glFuncs->glUseProgram(0); }

void Shader::link() const {
    params::inst().glFuncs->glLinkProgram(m_id);
    char *logstr = new char[2048];
    GLint success;
    params::inst().glFuncs->glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        params::inst().glFuncs->glGetProgramInfoLog(m_id, 2048, NULL, logstr);
        std::cout << "ERROR::SHADER::link() Link-time error: " << logstr
                  << std::endl;
        delete[] logstr;
    }
}

void Shader::attachVertexShader(const char *fileName) {
    if (fileName) {
        m_vFileName = fileName;
        const char *source = readFile(fileName);

        if (source) {
            m_vertProg = compile(source, GL_VERTEX_SHADER, fileName);
            params::inst().glFuncs->glAttachShader(m_id, m_vertProg);

            // std::cout << "\tSHADER::attachVertexShader(): attached:     " <<
            // fileName << std::endl;
        } else {
            std::cout << "ERROR::SHADER::attachVertexShader(): not found: "
                      << fileName << std::endl;
        }
    }
}

void Shader::attachControlShader(const char *fileName) {
    if (fileName) {
        m_cFileName = fileName;
        const char *source = readFile(fileName);

        if (source) {
            m_contProg = compile(source, GL_TESS_CONTROL_SHADER, fileName);
            params::inst().glFuncs->glAttachShader(m_id, m_contProg);

            // std::cout << "\tSHADER::attachControlShader(): attached:    " <<
            // fileName << std::endl;
        } else {
            std::cout << "ERROR::SHADER::attachControlShader(): not found: "
                      << fileName << std::endl;
        }
    }
}

void Shader::attachEvaluationShader(const char *fileName) {
    if (fileName) {
        m_eFileName = fileName;
        const char *source = readFile(fileName);

        if (source) {
            m_evalProg = compile(source, GL_TESS_EVALUATION_SHADER, fileName);
            params::inst().glFuncs->glAttachShader(m_id, m_evalProg);

            // std::cout << "\tSHADER::attachEvaluationShader(): attached:" <<
            // fileName << std::endl;
        } else {
            std::cout << "ERROR::SHADER::attachEvaluationShader(): not found:"
                      << fileName << std::endl;
        }
    }
}

void Shader::attachGeometryShader(const char *fileName) {
    if (fileName) {
        m_gFileName = fileName;
        const char *source = readFile(fileName);

        if (source) {
            m_geomProg = compile(source, GL_GEOMETRY_SHADER, fileName);
            params::inst().glFuncs->glAttachShader(m_id, m_geomProg);

            // std::cout << "\tSHADER::attachGeometryShader(): attached:  " <<
            // fileName << std::endl;
        } else {
            std::cout << "ERROR::SHADER::attachGeometryShader(): not found: "
                      << fileName << std::endl;
        }
    }
}

void Shader::attachFragmentShader(const char *fileName) {
    if (fileName) {
        m_fFileName = fileName;
        const char *source = readFile(fileName);

        if (source) {
            m_fragProg = compile(source, GL_FRAGMENT_SHADER, fileName);
            params::inst().glFuncs->glAttachShader(m_id, m_fragProg);

            // std::cout << "\tSHADER::attachFragmentShader(): attached:  " <<
            // fileName << std::endl;
        } else {
            std::cout << "ERROR::SHADER::attachFragmentShader(): not found: "
                      << fileName << std::endl;
        }
    }
}

void Shader::attachDefault() {
    const char *vertSrc =
        "#version 400 core                                                 "
        "            \n"
        "                                                                  "
        "            \n"
        "#define VERT_POSITION 0                                           "
        "            \n"
        "#define VERT_NORMAL   1                                           "
        "            \n"
        "#define VERT_COLOR	   2                                    "
        "                   \n"
        "#define VERT_TEXTURE  3                                           "
        "            \n"
        "                                                                  "
        "            \n"
        "uniform mat4x4 matModel;                                          "
        "            \n"
        "uniform mat4x4 matView;                                           "
        "            \n"
        "uniform mat4x4 matProjection;                                     "
        "            \n"
        "                                                                  "
        "            \n"
        "layout(location = VERT_POSITION) in vec4 Position;                "
        "            \n"
        "layout(location = VERT_NORMAL)   in vec4 Normal;                  "
        "            \n"
        "layout(location = VERT_COLOR)    in vec4 Color;                   "
        "            \n"
        "layout(location = VERT_TEXTURE)  in vec4 Texture;                 "
        "            \n"
        "                                                                  "
        "            \n"
        "out vec4 VertColor;                                               "
        "            \n"
        "                                                                  "
        "            \n"
        "void main()                                                       "
        "            \n"
        "{	                                                               "
        " "
        "           \n"
        "    VertColor    = Color;                           	        "
        "               \n"
        "    gl_Position = matProjection * matView * matModel * "
        "vec4(Position.xyz, 1); \n"
        "}                                                                 "
        "            \n";

    const char *fragSrc =
        "#version 400 core           \n"
        "                            \n"
        "in vec4 VertColor;          \n"
        "                            \n"
        "void main()                 \n"
        "{                           \n"
        "   gl_FragColor = VertColor;\n"
        "}                           \n";

    attachShaderFromSource(vertSrc, GL_VERTEX_SHADER);
    attachShaderFromSource(fragSrc, GL_FRAGMENT_SHADER);
}

void Shader::attachShaderFromSource(const char *source, unsigned int type) {
    if (source) {
        if (type == GL_VERTEX_SHADER) {
            m_vertProg = compile(source, type, source);
            params::inst().glFuncs->glAttachShader(m_id, m_vertProg);
        }

        if (type == GL_TESS_CONTROL_SHADER) {
            m_contProg = compile(source, type, source);
            params::inst().glFuncs->glAttachShader(m_id, m_contProg);
        }

        if (type == GL_TESS_EVALUATION_SHADER) {
            m_evalProg = compile(source, type, source);
            params::inst().glFuncs->glAttachShader(m_id, m_evalProg);
        }

        if (type == GL_GEOMETRY_SHADER) {
            m_geomProg = compile(source, type, source);
            params::inst().glFuncs->glAttachShader(m_id, m_geomProg);
        }

        if (type == GL_FRAGMENT_SHADER) {
            m_fragProg = compile(source, type, source);
            params::inst().glFuncs->glAttachShader(m_id, m_fragProg);
        }
    }
}

void Shader::checkFile(const char *fileName, QDateTime &oldDateTime,
                       unsigned int type) {
    QString fName(fileName);
    QFileInfo fileInfo(fName);
    QDateTime dt = fileInfo.lastModified();

    if (dt != oldDateTime) {
        if (!m_firstUpdate) {
            if (type == GL_VERTEX_SHADER) {
                params::inst().glFuncs->glDetachShader(m_id, m_vertProg);
                params::inst().glFuncs->glDeleteShader(m_vertProg);

                attachVertexShader(fileName);
                std::cout << "\tSHADER::Vertex updated" << std::endl;
            }

            if (type == GL_TESS_CONTROL_SHADER) {
                params::inst().glFuncs->glDetachShader(m_id, m_contProg);
                params::inst().glFuncs->glDeleteShader(m_contProg);

                attachControlShader(fileName);
                std::cout << "\tSHADER::Control updated" << std::endl;
            }

            if (type == GL_TESS_EVALUATION_SHADER) {
                params::inst().glFuncs->glDetachShader(m_id, m_evalProg);
                params::inst().glFuncs->glDeleteShader(m_evalProg);

                attachEvaluationShader(fileName);
                std::cout << "\tSHADER::Evaluation updated" << std::endl;
            }

            if (type == GL_GEOMETRY_SHADER) {
                params::inst().glFuncs->glDetachShader(m_id, m_geomProg);
                params::inst().glFuncs->glDeleteShader(m_geomProg);

                attachGeometryShader(fileName);
                std::cout << "\tSHADER::Geometry updated" << std::endl;
            }

            if (type == GL_FRAGMENT_SHADER) {
                params::inst().glFuncs->glDetachShader(m_id, m_fragProg);
                params::inst().glFuncs->glDeleteShader(m_fragProg);

                attachFragmentShader(fileName);
                std::cout << "\tSHADER::Fragment updated" << std::endl;
            }

            link();
        }

        oldDateTime = dt;
    }
}

void Shader::autoUpdate() {
    checkFile(m_vFileName, m_vOldDateTime, GL_VERTEX_SHADER);
    checkFile(m_cFileName, m_cOldDateTime, GL_TESS_CONTROL_SHADER);
    checkFile(m_eFileName, m_eOldDateTime, GL_TESS_EVALUATION_SHADER);
    checkFile(m_gFileName, m_gOldDateTime, GL_GEOMETRY_SHADER);
    checkFile(m_fFileName, m_fOldDateTime, GL_FRAGMENT_SHADER);

    MainWindow::getInstance().updateScene();

    m_firstUpdate = false;
}

void Shader::setUpdateStatus(Config conf) {
    if (conf == AUTO_LOAD) {
        connect(m_timer, SIGNAL(timeout()), this, SLOT(autoUpdate()));
        m_timer->start(m_refreshTime);
    } else {
        disconnect(m_timer, SIGNAL(timeout()), this, SLOT(autoUpdate()));
    }
}

GLuint Shader::id() const { return m_id; }

void Shader::seti(const char *label, int arg) {
    params::inst().glFuncs->glUniform1i(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), arg);
}

void Shader::setf(const char *label, float arg) {
    params::inst().glFuncs->glUniform1f(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), arg);
}

void Shader::set2i(const char *label, int arg1, int arg2) {
    params::inst().glFuncs->glUniform2i(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), arg1, arg2);
}

void Shader::set2f(const char *label, float arg1, float arg2) {
    params::inst().glFuncs->glUniform2f(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), arg1, arg2);
}

void Shader::set2f(const char *label, const glm::vec2 &v) {
    params::inst().glFuncs->glUniform2f(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), v.x, v.y);
}

void Shader::set3i(const char *label, int arg1, int arg2, int arg3) {
    params::inst().glFuncs->glUniform3i(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), arg1, arg2,
        arg3);
}

void Shader::set3f(const char *label, float arg1, float arg2, float arg3) {
    params::inst().glFuncs->glUniform3fv(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), 1,
        glm::value_ptr(glm::vec3(arg1, arg2, arg3)));
}

void Shader::set3f(const char *label, const glm::vec3 &v) {
    params::inst().glFuncs->glUniform3f(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), v.x, v.y,
        v.z);
}

void Shader::set4i(const char *label, int arg1, int arg2, int arg3, int arg4) {
    params::inst().glFuncs->glUniform4i(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), arg1, arg2,
        arg3, arg4);
}

void Shader::set4f(const char *label, float arg1, float arg2, float arg3,
                   float arg4) {
    params::inst().glFuncs->glUniform4f(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), arg1, arg2,
        arg3, arg4);
}

void Shader::set4f(const char *label, const glm::vec4 &v) {
    params::inst().glFuncs->glUniform4f(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), v.x, v.y,
        v.z, v.w);
}

void Shader::set3iv(const char *label, const int *args) {
    params::inst().glFuncs->glUniform3iv(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), 1, args);
}

void Shader::set3fv(const char *label, const float *args) {
    params::inst().glFuncs->glUniform3fv(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), 1, args);
}

void Shader::set4fv(const char *label, const float *args) {
    params::inst().glFuncs->glUniform4fv(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), 1, args);
}

void Shader::setMatrix(const char *label, const float *m, bool transpose) {
    params::inst().glFuncs->glUniformMatrix4fv(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), 1, transpose,
        m);
}

void Shader::setMatrix(const char *label, const double *m, bool transpose) {
    float fm[] = {(float)m[0],  (float)m[1],  (float)m[2],  (float)m[3],
                  (float)m[4],  (float)m[5],  (float)m[6],  (float)m[7],
                  (float)m[8],  (float)m[9],  (float)m[10], (float)m[11],
                  (float)m[12], (float)m[13], (float)m[14], (float)m[15]};
    params::inst().glFuncs->glUniformMatrix4fv(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), 1, transpose,
        fm);
}

void Shader::setMatrix(const char *label, const glm::mat4 &mat,
                       bool transpose) {
    params::inst().glFuncs->glUniformMatrix4fv(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), 1, transpose,
        glm::value_ptr(mat));
}

void Shader::setMatrix(const char *label, const glm::mat3 &mat,
                       bool transpose) {
    params::inst().glFuncs->glUniformMatrix3fv(
        params::inst().glFuncs->glGetUniformLocation(m_id, label), 1, transpose,
        glm::value_ptr(mat));
}

/*=====================================================================================
        Private Functions
=====================================================================================*/
const char *Shader::readFile(const char *fileName) {
    m_shaderCode.clear();
    std::ifstream shaderFile;
    if (fileName) {
        shaderFile.open(fileName);
        std::stringstream shaderStream;

        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        m_shaderCode = shaderStream.str();
    }

    if (m_shaderCode.empty()) {
        return nullptr;
    } else {
        return m_shaderCode.c_str();
    }
}

unsigned int Shader::compile(const char *source, unsigned int type,
                             const char *filename) {
    GLuint id = params::inst().glFuncs->glCreateShader(type);
    params::inst().glFuncs->glShaderSource(id, 1, &source, NULL);
    params::inst().glFuncs->glCompileShader(id);

    int c = 0;

    params::inst().glFuncs->glGetShaderiv(id, GL_COMPILE_STATUS, &c);

    if (!c) {
        char *logstr = new char[2048];
        params::inst().glFuncs->glGetShaderInfoLog(id, 2048, NULL, logstr);
        printf("SHADER::Error compiling shader file %s:\n%s\n", filename,
               logstr);
        delete[] logstr;
        // exit(-1);
        return 0;
    }

    return id;
}

void Shader::cleanUp() {
    if (m_id) {
        if (m_vertProg) {
            params::inst().glFuncs->glDetachShader(m_id, m_vertProg);
            params::inst().glFuncs->glDeleteShader(m_vertProg);
            m_vertProg = 0;
        }

        if (m_contProg) {
            params::inst().glFuncs->glDetachShader(m_id, m_contProg);
            params::inst().glFuncs->glDeleteShader(m_contProg);
            m_contProg = 0;
        }

        if (m_evalProg) {
            params::inst().glFuncs->glDetachShader(m_id, m_evalProg);
            params::inst().glFuncs->glDeleteShader(m_evalProg);
            m_evalProg = 0;
        }

        if (m_geomProg) {
            params::inst().glFuncs->glDetachShader(m_id, m_geomProg);
            params::inst().glFuncs->glDeleteShader(m_geomProg);
            m_geomProg = 0;
        }

        if (m_fragProg) {
            params::inst().glFuncs->glDetachShader(m_id, m_fragProg);
            params::inst().glFuncs->glDeleteShader(m_fragProg);
            m_fragProg = 0;
        }

        params::inst().glFuncs->glDeleteProgram(m_id);
        m_id = 0;
    }
}

void Shader::selectSubroutine(const char *label, GLenum shadertype) {
    GLuint index =
        params::inst().glFuncs->glGetSubroutineIndex(m_id, shadertype, label);
    params::inst().glFuncs->glUniformSubroutinesuiv(shadertype, 1, &index);
}

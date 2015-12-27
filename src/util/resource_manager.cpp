#include "resource_manager.h"

unique_ptr<Shader> &ResourceManager::loadShader(const char *vFileName,
                                                const char *fFileName,
                                                string name) {
    m_shaders[name].reset(new Shader(vFileName, fFileName));
    return m_shaders[name];
}

unique_ptr<Shader> &ResourceManager::loadShader(const char *vFileName,
                                                const char *gFileName,
                                                const char *fFileName,
                                                string name) {
    m_shaders[name].reset(new Shader(vFileName, gFileName, fFileName));
    return m_shaders[name];
}

unique_ptr<Shader> &ResourceManager::loadShader(
    const char *vFileName, const char *tcFileName, const char *teFileName,
    const char *gFileName, const char *fFileName, string name) {
    m_shaders[name].reset(
        new Shader(vFileName, tcFileName, teFileName, gFileName, fFileName));
    return m_shaders[name];
}

unique_ptr<Shader> &ResourceManager::getShader(string name) {
    if (m_shaders.find(name) == m_shaders.end()) {
        cout << "ERROR: cannot find shader with name: " << name << endl;
        exit(1);
    }
    return m_shaders[name];
}

unique_ptr<Texture> &ResourceManager::loadTexture(const char *imageName,
                                                  string name) {
    m_textures[name].reset(new Texture(imageName));
    return m_textures[name];
}

unique_ptr<Texture> &ResourceManager::loadTexture(const char *imageName,
                                                  GLint magFilter,
                                                  GLint minFilter,
                                                  GLint wrapMode, string name) {
    m_textures[name].reset(
        new Texture(imageName, magFilter, minFilter, wrapMode));
    m_textures[name]->setWrapMode(wrapMode);
    return m_textures[name];
}

unique_ptr<Texture> &ResourceManager::loadTexture(const vector<string>& imageNames,
                                                  string name) {
    m_textures[name].reset(new Texture(imageNames));
    //m_textures[name]->setWrapMode(GL_CLAMP_TO_BORDER);
    cout << "3D texture loaded to " << name << endl;
    return m_textures[name];
}

unique_ptr<Texture> &ResourceManager::getTexture(string name) {
    if (m_textures.find(name) == m_textures.end()) {
        cout << "ERROR: cannot find texture with name: " << name << endl;
        exit(1);
    }
    return m_textures[name];
}

void ResourceManager::printStats() {
    cout << "Resource Manager Stats:" << endl;
    cout << "\t" << m_shaders.size() << " shaders." << endl;
    for (const auto &kv : m_shaders) {
        cout << "\t\t" << kv.first << endl;
    }
    cout << "\t" << m_textures.size() << " textures" << endl;
    for (const auto &kv : m_textures) {
        cout << "\t\t" << kv.first << endl;
    }
}

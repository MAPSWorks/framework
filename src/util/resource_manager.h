/*=====================================================================================
                                resource_manager.h

    Description:  This class manages OpenGL resources, such as shaders and textures.
        
    Created by Chen Chen on 11/28/2015
=====================================================================================*/

#ifndef RESOURCE_MANAGER_H_VN6ZUEDP
#define RESOURCE_MANAGER_H_VN6ZUEDP

#include "headers.h"
#include "shader.h"
#include "texture.h"

class ResourceManager
{
public:
    static ResourceManager& inst(){
        static ResourceManager singleton;
        return singleton;
    }

    // Load shader
    unique_ptr<Shader>& loadShader(const char *vFileName, 
                                   const char *fFileName,
                                   string name);
    unique_ptr<Shader>& loadShader(const char *vFileName, 
                                   const char *gFileName, 
                                   const char *fFileName,
                                   string name);
    unique_ptr<Shader>& loadShader(const char *vFileName, 
                                   const char *tcFileName, 
                                   const char *teFileName, 
                                   const char *gFileName, 
                                   const char *fFileName,
                                   string name);

    // Retrieve shader
    unique_ptr<Shader>& getShader(string name);

    // Load texture
    unique_ptr<Texture>& loadTexture(const char *imageName,
                                     GLint wrapMode,
                                     string name); 
    unique_ptr<Texture>& loadTexture(const char *imageName, 
                                     GLint wrapMode,
                                     GLint magFilter,
                                     GLint minFilter,
                                     GLfloat anisotrophy,
                                     GLboolean createMipmaps,
                                     string name); 

    // Retrieve texture
    unique_ptr<Texture>& getTexture(string name);

    void printStats();

private:
    ResourceManager() {} // private constructor, do some init here.
    ~ResourceManager() {} // private destructor

    ResourceManager( const ResourceManager& ) = delete ; // non-copyable
    ResourceManager( ResourceManager&& ) = delete ; // and non-movable

    // non-assignable
    ResourceManager& operator= ( const ResourceManager& ) = delete ;
    ResourceManager& operator= ( ResourceManager&& ) = delete ;

    map<string, unique_ptr<Shader>>    m_shaders;
    map<string, unique_ptr<Texture>>   m_textures;

    //// Loads and generates a shader from file
    //static Shader    loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
    //// Loads a single texture from file
    //static Texture2D loadTextureFromFile(const GLchar *file, GLboolean alpha);
};

#endif /* end of include guard: RESOURCE_MANAGER_H_VN6ZUEDP */

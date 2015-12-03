/*=====================================================================================
                                model.h

    Description:  This class is responsible for loading models.
        
    Created by Chen Chen on 11/28/2015
=====================================================================================*/

#ifndef MODEL_H_UVPXH4N8
#define MODEL_H_UVPXH4N8

#include "headers.h"
#include "mesh.h"
#include <set>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model 
{
    public:
        /*  Functions   */
        Model(const char* path)
        {
            this->loadModel(path);
        }
        void render(unique_ptr<Shader>& shader);	

    private:
        /*  Model Data  */
        vector<unique_ptr<Mesh>> m_meshes;
        string                   m_directory;
        set<string>              m_loadedTextures;

        /*  Functions   */
        void loadModel(const char* path);
        void processNode(aiNode* node, const aiScene* scene);
        unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
        vector<Mesh::Texture> loadMaterialTextures(aiMaterial* mat, 
                                                   aiTextureType type, 
                                                   string typeName);
};

#endif /* end of include guard: MODEL_H_UVPXH4N8 */

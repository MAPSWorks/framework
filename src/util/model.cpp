#include "model.h"
#include "texture.h"
#include "resource_manager.h"

void Model::render(unique_ptr<Shader>& shader){
    for(size_t i = 0; i < m_meshes.size(); ++i) { 
        m_meshes[i]->render(shader); 
    }
}

void Model::loadModel(const char* path){
    string str_path(path);
    // Read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(str_path, aiProcess_Triangulate | aiProcess_FlipUVs);
    // Check for errors
    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // Retrieve the directory path of the filepath
    this->m_directory = str_path.substr(0, str_path.find_last_of('/'));

    // Process ASSIMP's root node recursively
    this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene){
    // Process each mesh located at the current node
    for(GLuint i = 0; i < node->mNumMeshes; i++)
    {
        // The node object only contains indices to index the actual objects in the scene. 
        // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; 
        m_meshes.emplace_back(std::move(processMesh(mesh, scene)));	
    }
    // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(GLuint i = 0; i < node->mNumChildren; i++)
    {
        this->processNode(node->mChildren[i], scene);
    }
}

unique_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene){
    // Data to fill
    vector<Mesh::Vertex> vertices;
    vector<GLuint>       indices;
    vector<Mesh::Texture> textures;

    // Walk through each of the mesh's vertices
    for(size_t i = 0; i < mesh->mNumVertices; ++i) { 
        Mesh::Vertex vertex;
        glm::vec3 vec;
        // Positions
        vec.x = mesh->mVertices[i].x;
        vec.y = mesh->mVertices[i].y;
        vec.z = mesh->mVertices[i].z;
        vertex.Position = vec;
        // Normals
        vec.x = mesh->mNormals[i].x;
        vec.y = mesh->mNormals[i].y;
        vec.z = mesh->mNormals[i].z;
        vertex.Normal = vec;
        // Texture coordinates
        if(mesh->mTextureCoords[0]) { 
            glm::vec2 tvec; 
            tvec.x = mesh->mTextureCoords[0][i].x;
            tvec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = tvec;
        } 
        else{
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    } 
    // Now walk through each of the mesh's faces
    for(size_t i = 0; i < mesh->mNumFaces; ++i) { 
        aiFace face = mesh->mFaces[i];
        // Retrieve all indices of the face and store them
        for(size_t j = 0; j < face.mNumIndices; ++j) { 
            indices.push_back(face.mIndices[j]);
        } 
    } 
    // Process materials
    if(mesh->mMaterialIndex > 0) { 
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; 
        // We assume a convention for sampler names in the shaders. Each diffuse texture should be named as 'DiffTexN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. Sample applies to other texture as the following list summarizes:
        // Diffuse: DiffTexN 
        // Specular: SpecTexN 
        // Normal: NormTexN 
        
        // 1. Diffuse maps
        vector<Mesh::Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "DiffTex");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // 2. Specular maps
        vector<Mesh::Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "SpecTex");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    } 

    unique_ptr<Mesh> m( new Mesh(vertices, indices, textures));
    return m;
}

vector<Mesh::Texture> Model::loadMaterialTextures(aiMaterial* mat, 
                                                  aiTextureType type, 
                                                  string typeName){
    vector<Mesh::Texture> textures;
    for(size_t i = 0; i < mat->GetTextureCount(type); ++i) { 
        aiString str;
        mat->GetTexture(type, i, &str); 

        bool skip = false;
        if(m_loadedTextures.find(string(str.C_Str())) != m_loadedTextures.end()) { 
            skip = true;             
        } 
        if(!skip) { 
            string filename;
            filename = m_directory + '/' + string(str.C_Str());
            unique_ptr<Texture>& new_texture = ResourceManager::inst().loadTexture(filename.c_str(), GL_REPEAT, str.C_Str());
            Mesh::Texture texture;
            texture.id = new_texture->id();
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);      
            m_loadedTextures.emplace(string(str.C_Str()));
        } 
    } 
    return textures;
}

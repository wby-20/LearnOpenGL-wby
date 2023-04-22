#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>

#include <texture.h>
#include <mesh.h>

using namespace std;

class Model
{
public:
    Model(const char *path) {LoadModel(path);}
    void Draw(Shader* shader)
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    vector<Mesh> meshes; // 模型包含的多个mesh
    string directory; // 模型的文件夹!!!

    void LoadModel(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadTextures(aiMaterial *material, aiTextureType type, textureType typeName);
};

void Model::LoadModel(string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // 处理mesh
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]]; // 结点存的是mesh的索引
        meshes.push_back(processMesh(mesh, scene));
    }
    // 遍历子节点
    for(unsigned int i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if(mesh->HasNormals())
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        
        if(mesh->mTextureCoords[0])
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        vertices.push_back(vertex);
    }
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    vector<Texture> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE, typeDiffuce);
    vector<Texture> specularMaps = loadTextures(material, aiTextureType_SPECULAR, typeSpecular);

    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    

    return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadTextures(aiMaterial *material, aiTextureType type, textureType typeName)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString name;
        material->GetTexture(type, i, &name);
        bool flag = true;

        string path = this->directory + "/" + name.C_Str();

        for(unsigned int k = 0; k < Texture::textureHasLoaded.size(); k++)
        {
            if(strcmp(Texture::textureHasLoaded[k].path.c_str(), path.c_str()) == 0)
            {
                textures.push_back(Texture::textureHasLoaded[k]);
                flag = false;
                break;
            }
        }
        if(flag)
        {
            Texture texture = Texture(path.c_str(), typeName, path);
            textures.push_back(texture);
            Texture::textureHasLoaded.push_back(texture);
        }
    }
    return textures;
}

#endif
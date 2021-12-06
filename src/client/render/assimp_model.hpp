#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "client/render/mesh.hpp"
#include "client/render/shader.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

namespace UnifiedRender {
    class Model
    {
    public:
        // model data 
        vector<TextureStruct> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        vector<UnifiedRender::Mesh>    meshes;
        std::string directory;
        bool gammaCorrection;

        // constructor, expects a filepath to a 3D model.
        Model(std::string const& path, bool gamma = false);

        // draws the model, and thus all its meshes
        void draw(UnifiedRender::OpenGl::Program& shader) const;

    private:
        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(std::string const& path);

        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(aiNode* node, const aiScene* scene);

        UnifiedRender::Mesh processMesh(aiMesh* mesh, const aiScene* scene);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        vector<TextureStruct> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

        unsigned int TextureFromFile(const char* path, const std::string& directory);
    };
    class ModelManager {
    private:
        std::set<std::pair<Model*, std::string>> models;

    public:
        const Model* load(const std::string& path);
    };
}

extern UnifiedRender::ModelManager* g_model_manager;
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
#include <cstdint>

namespace UnifiedRender {
    class Model {
        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void load_model(std::string const& path);

        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void process_node(aiNode* node, const aiScene* scene);

        UnifiedRender::Mesh process_mesh(aiMesh* mesh, const aiScene* scene);

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        std::vector<TextureStruct> load_material_textures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

        unsigned int texture_from_file(const char* path, const std::string& directory);
    public:
        // model data 
        std::vector<TextureStruct> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<UnifiedRender::Mesh> meshes;
        std::string directory;
        bool gammaCorrection;

        // constructor, expects a filepath to a 3D model.
        Model(std::string const& path, bool gamma = false);

        // draws the model, and thus all its meshes
        void draw(UnifiedRender::OpenGl::Program& shader) const;
    };

    class ModelManager {
        std::set<std::pair<Model*, std::string>> models;
    public:
        const Model* load(const std::string& path);
    };
}

extern UnifiedRender::ModelManager* g_model_manager;
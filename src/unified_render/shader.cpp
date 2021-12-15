#include "unified_render/shader.hpp"
using namespace UnifiedRender::OpenGl;

Shader::Shader(const std::string& path, GLuint type) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // Read the entire file onto the stringstream
        file.open(Path::get(path).c_str());
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();

        buffer = stream.str();
        compile(type);
    } catch(std::ifstream::failure& e) {
        print_error("Cannot load shader %s", path.c_str());
    }
}

Shader::~Shader() {
    glDeleteShader(id);
}

void Shader::compile(GLuint type) {
    const char* c_code = buffer.c_str();
    id = glCreateShader(type);
    glShaderSource(id, 1, &c_code, NULL);
    glCompileShader(id);

    // Check for errors of the shader
    GLint r = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &r);
    if(!r) {
        std::string error_info;
        glGetShaderInfoLog(id, GL_INFO_LOG_LENGTH, NULL, &error_info[0]);
        print_error("Status: %s", error_info.c_str());
        throw ShaderException(error_info);
    }
    // print_info("Status: Sucess");
}

GLuint Shader::get_id(void) const {
    return id;
}

Program::Program(const VertexShader* vertex, const FragmentShader* fragment, const GeometryShader* geometry) {
    id = glCreateProgram();
    glBindAttribLocation(id, 0, "m_pos");
    glBindAttribLocation(id, 1, "m_texcoord");
    glBindAttribLocation(id, 2, "m_color");

    glAttachShader(id, vertex->get_id());
    glAttachShader(id, fragment->get_id());
    if(geometry != nullptr) {
        glAttachShader(id, geometry->get_id());
    }
    glLinkProgram(id);

    // Check for errors of the shader
    GLint r = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &r);
    if(!r) {
        std::string error_info;
        glGetProgramInfoLog(id, GL_INFO_LOG_LENGTH, NULL, &error_info[0]);
        print_error("Program error %s", error_info.c_str());
        throw ShaderException(error_info);
    }
}

Program* Program::create(const std::string& vs_path, const std::string& fs_path, const std::string& gs_path) {
    auto vs = UnifiedRender::OpenGl::VertexShader(Path::get("shaders/" + vs_path + ".vs"));
    auto fs = UnifiedRender::OpenGl::FragmentShader(Path::get("shaders/" + fs_path + ".fs"));

    if(!gs_path.empty()) {
        auto gs = UnifiedRender::OpenGl::GeometryShader(Path::get("shaders/" + gs_path + ".gs"));
        return new Program(&vs, &fs, &gs);
    }
    return new Program(&vs, &fs);
}

void Program::use(void) const {
    glUseProgram(id);
}

void Program::set_uniform(const std::string& name, glm::mat4 uniform) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(uniform));
}

void Program::set_uniform(const std::string& name, float value1, float value2) const {
    glUniform2f(glGetUniformLocation(id, name.c_str()), value1, value2);
}

void Program::set_uniform(const std::string& name, float value1, float value2, float value3) const {
    glUniform3f(glGetUniformLocation(id, name.c_str()), value1, value2, value3);
}

void Program::set_uniform(const std::string& name, float value1, float value2, float value3, float value4) const {
    glUniform4f(glGetUniformLocation(id, name.c_str()), value1, value2, value3, value4);
}

void Program::set_uniform(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Program::set_uniform(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Program::set_texture(int value, const std::string& name, const UnifiedRender::Texture* texture) const {
    set_uniform(name, value);
    glActiveTexture(GL_TEXTURE0 + value);
    glBindTexture(GL_TEXTURE_2D, texture->gl_tex_num);
}

void Program::set_texture(int value, const std::string& name, const UnifiedRender::TextureArray* texture) const {
    set_uniform(name, value);
    glActiveTexture(GL_TEXTURE0 + value);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture->gl_tex_num);
}

GLuint Program::get_id(void) const {
    return id;
}
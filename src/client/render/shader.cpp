#include "client/render/shader.hpp"
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
    print_info("Status: %s", get_status().c_str());
}

std::string Shader::get_status(void) {
    GLint r = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &r);
    if(!r) {
        GLchar* tmpbuf = new GLchar[255];
        glGetShaderInfoLog(id, 255, NULL, tmpbuf);

        std::string end_buf = tmpbuf;
        delete[] tmpbuf;
        return end_buf;
    }
    return "Sucess";
}

GLuint Shader::get_id(void) const {
    return id;
}

Program::Program(const VertexShader* vertex, const FragmentShader* fragment) {
    id = glCreateProgram();
    glBindAttribLocation(id, 0, "m_pos");
    glBindAttribLocation(id, 1, "m_texcoord");
    glBindAttribLocation(id, 2, "m_colour");

    glAttachShader(id, vertex->get_id());
    glAttachShader(id, fragment->get_id());
    glLinkProgram(id);

    // Check for errors of the shader
    GLint r = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &r);
    if(!r) {
        GLchar* tmpbuf = new GLchar[255];
        glGetShaderInfoLog(id, 255, NULL, tmpbuf);
        print_info("Program error %s", tmpbuf);
        delete[] tmpbuf;
    }
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

void Program::set_uniform(const std::string &name, float value1, float value2, float value3, float value4) const {
    glUniform4f(glGetUniformLocation(id, name.c_str()), value1, value2, value3, value4);
}

void Program::set_uniform(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Program::set_uniform(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

GLuint Program::get_id(void) const {
    return id;
}
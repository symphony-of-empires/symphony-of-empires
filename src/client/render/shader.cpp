#include "shader.hpp"
using namespace UnifiedRender::OpenGl;

Shader::Shader(const std::string& path) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // Read the entire file onto the stringstream
        file.open(Path::get(path).c_str());
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();

        buffer = stream.str();
    } catch(std::ifstream::failure& e) {
        print_error("Cannot load shader %s", path.c_str());
    }
}

Shader::~Shader() {
    glDeleteShader(id);
}

void Shader::compile(void) {
    
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

inline GLuint Shader::get_id(void) const {
    return id;
}

void VertexShader::compile(void) {
    const char* c_code = buffer.c_str();
    id = glCreateShader(type);
    glShaderSource(id, 1, &c_code, NULL);
    glCompileShader(id);

    print_info("Status: %s", get_status().c_str());
}

void FragmentShader::compile(void) {
    const char* c_code = buffer.c_str();
    id = glCreateShader(type);
    glShaderSource(id, 1, &c_code, NULL);
    glCompileShader(id);

    print_info("Status: %s", get_status().c_str());
}

Program::Program(const VertexShader* vertex, const FragmentShader* fragment) {
    id = glCreateProgram();
    glAttachShader(id, vertex->get_id());
    glAttachShader(id, fragment->get_id());
    glLinkProgram(id);

    // Check for errors of the shader
    GLint r = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &r);
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
    GLuint loc = glGetUniformLocation(id, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(uniform));
}

inline GLuint Program::get_id(void) const {
    return id;
}

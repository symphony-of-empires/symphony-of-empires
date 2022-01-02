#include <fstream>
#include <iostream>
#include <sstream>

#include "unified_render/shader.hpp"

using namespace UnifiedRender::OpenGL;

UnifiedRender::OpenGL::GLSL_Context::GLSL_Context(const std::string& _buffer)
    : buffer(_buffer)
{

}

UnifiedRender::OpenGL::GLSL_Context::~GLSL_Context(void) {

}

std::string UnifiedRender::OpenGL::GLSL_Context::get_identifier(std::string::iterator& it) {
    std::string::iterator start_it = it;

    // Alphanumerics, _ and dots are allowed as identifiers
    while(it != buffer.end() && (isalnum(*it) || *it == '_' || *it == '.')) {
        it++;
    }
    
    std::string str = buffer.substr(std::distance(buffer.begin(), start_it), std::distance(start_it, it));
    return str;
}

std::string UnifiedRender::OpenGL::GLSL_Context::get_literal(std::string::iterator& it) {
    std::string::iterator start_it = it;

    // Literal
    while(it != buffer.end() && (isdigit(*it) || *it == '.')) {
        it++;
    }

    // Skip "float" specifier
    if(it != buffer.end() && *it == 'f') {
        it++;
    }

    std::string str = buffer.substr(std::distance(buffer.begin(), start_it), std::distance(start_it, it));
    return str;
}

void UnifiedRender::OpenGL::GLSL_Context::lexer(void) {
    // Output the final stuff
    std::string::iterator it = buffer.begin();
    for( ; it != buffer.end(); ) {
        while(it != buffer.end() && (*it == ' ' || *it == '\t' || *it == '\r' || *it == '\n')) {
            it++;
        }
        if(it == buffer.end()) {
            break;
        }

        if((*(it + 0) == '/' && *(it + 1) == '/')) {
            while(it != buffer.end() && (*it != '\n')) {
                it++;
            }
        } else if(*it == '#') {
            std::string::iterator start_it = it;
            while(it != buffer.end() && (*it != '\n')) {
                it++;
            }
            GLSL_Token tok = GLSL_Token(GLSL_TokenType::MACRO);
            print_info("thing");
            tok.data = buffer.substr(std::distance(buffer.begin(), start_it), std::distance(start_it, it));
            print_info("think");
            tokens.push_back(tok);
        } else if(*it == ',') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::COMMA));
            it++;
        } else if(*it == ';') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::SEMICOLON));
            it++;
        } else if(*it == '(') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::LPAREN));
            it++;
        } else if(*it == ')') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::RPAREN));
            it++;
        } else if(*it == '[') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::LBRACKET));
            it++;
        } else if(*it == ']') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::RBRACKET));
            it++;
        } else if(*it == '{') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::LBRACE));
            it++;
        } else if(*it == '}') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::RBRACE));
            it++;
        } else if(*it == '+') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::ADD));
            it++;
        } else if(*it == '-') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::SUB));
            it++;
        } else if(*it == '*') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::MUL));
            it++;
        } else if(*it == '/') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::DIV));
            it++;
        } else if(*it == '%') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::REM));
            it++;
        } else if(*it == '<') {
            it++;
            if(it != buffer.end() && *it == '=') {
                tokens.push_back(GLSL_Token(GLSL_TokenType::CMP_LTEQ));
                it++;
            } else {
                tokens.push_back(GLSL_Token(GLSL_TokenType::CMP_LT));
            }
        } else if(*it == '>') {
            it++;
            if(it != buffer.end() && *it == '=') {
                tokens.push_back(GLSL_Token(GLSL_TokenType::CMP_GTEQ));
                it++;
            } else {
                tokens.push_back(GLSL_Token(GLSL_TokenType::CMP_GT));
            }
        } else if(*it == '|') {
            it++;
            if(it != buffer.end() && *it == '|') {
                tokens.push_back(GLSL_Token(GLSL_TokenType::CMP_OR));
                it++;
            } else {
                tokens.push_back(GLSL_Token(GLSL_TokenType::OR));
            }
        } else if(*it == '&') {
            it++;
            if(it != buffer.end() && *it == '&') {
                tokens.push_back(GLSL_Token(GLSL_TokenType::CMP_AND));
                it++;
            } else {
                tokens.push_back(GLSL_Token(GLSL_TokenType::AND));
            }
        } else if(*it == '=') {
            it++;
            if(it != buffer.end() && *it == '=') {
                tokens.push_back(GLSL_Token(GLSL_TokenType::CMP_EQ));
                it++;
            } else {
                tokens.push_back(GLSL_Token(GLSL_TokenType::ASSIGN));
            }
        } else if(*it == '?') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::TERNARY));
            it++;
        } else if(*it == ':') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::COLON));
            it++;
        } else if(*it == '.') {
            tokens.push_back(GLSL_Token(GLSL_TokenType::DOT));
            it++;
        } else {
            if(isdigit(*it) || *it == '.') {
                GLSL_Token tok = GLSL_Token(GLSL_TokenType::LITERAL);
                tok.data = get_literal(it);
                tokens.push_back(tok);
            } else if(isalnum(*it) || *it == '_') {
                GLSL_Token tok = GLSL_Token(GLSL_TokenType::IDENTIFIER);
                tok.data = get_identifier(it);
                tokens.push_back(tok);
            } else {
                it++;
            }
        }
    }
}

void UnifiedRender::OpenGL::GLSL_Context::parser(void) {
    GLSL_Function fn;

    fn = GLSL_Function();
    fn.name = "vec2";
    fn.ret_type = "vec2";
    fn.args.push_back(std::make_pair("float", "x"));
    fn.args.push_back(std::make_pair("float", "y"));
    funcs.push_back(fn);

    fn = GLSL_Function();
    fn.name = "vec3";
    fn.ret_type = "vec3";
    fn.args.push_back(std::make_pair("float", "x"));
    fn.args.push_back(std::make_pair("float", "y"));
    fn.args.push_back(std::make_pair("float", "z"));
    funcs.push_back(fn);

    fn = GLSL_Function();
    fn.name = "vec4";
    fn.ret_type = "vec4";
    fn.args.push_back(std::make_pair("float", "x"));
    fn.args.push_back(std::make_pair("float", "y"));
    fn.args.push_back(std::make_pair("float", "z"));
    fn.args.push_back(std::make_pair("float", "w"));
    funcs.push_back(fn);

    // Register all the overloads for this function
    std::vector<std::string> mix_strings ={ "vec2", "vec3", "vec4", "sampler2D" };
    for(std::vector<std::string>::const_iterator it1 = mix_strings.begin(); it1 != mix_strings.end(); it1++) {
        for(std::vector<std::string>::const_iterator it2 = mix_strings.begin(); it2 != mix_strings.end(); it2++) {
            fn = GLSL_Function();
            fn.name = "mix";
            fn.ret_type = "vec4";
            fn.args.push_back(std::make_pair(*it1, "x"));
            fn.args.push_back(std::make_pair(*it2, "y"));
            fn.args.push_back(std::make_pair("float", "z"));
            funcs.push_back(fn);
        }
    }

    fn = GLSL_Function();
    fn.name = "clamp";
    fn.ret_type = "float";
    fn.args.push_back(std::make_pair("float", "num"));
    fn.args.push_back(std::make_pair("float", "min"));
    fn.args.push_back(std::make_pair("float", "max"));
    funcs.push_back(fn);

    std::vector<GLSL_Token>::iterator it;
    for(it = tokens.begin(); it != tokens.end(); it++) {
        if(it->type == GLSL_TokenType::ASSIGN) {

        }
    }

    for(const auto& var : vars) {
        print_error("VAR->%i (%s) typeof(%s)", static_cast<int>(var.type), var.name.c_str(), var.type_name.c_str());
    }
}

std::string UnifiedRender::OpenGL::GLSL_Context::to_text(void) {
    std::string end_buffer;
    end_buffer += "#version 330 compatibility\r\n";

    for(const auto& define : defines) {
        end_buffer += "#define " + define.name + " " + define.value + "\r\n";
    }

    for(const auto& tok : tokens) {
        switch(tok.type) {
        case GLSL_TokenType::MACRO:
            end_buffer += "#" + tok.data + "\r\n";
            break;
        case GLSL_TokenType::SEMICOLON:
            end_buffer += ";\r\n";
            break;
        case GLSL_TokenType::COMMA:
            end_buffer += ",";
            break;
        case GLSL_TokenType::LPAREN:
            end_buffer += "(";
            break;
        case GLSL_TokenType::RPAREN:
            end_buffer += ")";
            break;
        case GLSL_TokenType::LBRACKET:
            end_buffer += "[";
            break;
        case GLSL_TokenType::RBRACKET:
            end_buffer += "]";
            break;
        case GLSL_TokenType::LBRACE:
            end_buffer += "{\n";
            break;
        case GLSL_TokenType::RBRACE:
            end_buffer += "}\n";
            break;
        case GLSL_TokenType::ADD:
            end_buffer += "+";
            break;
        case GLSL_TokenType::SUB:
            end_buffer += "-";
            break;
        case GLSL_TokenType::MUL:
            end_buffer += "*";
            break;
        case GLSL_TokenType::DIV:
            end_buffer += "/";
            break;
        case GLSL_TokenType::CMP_AND:
            end_buffer += "&&";
            break;
        case GLSL_TokenType::AND:
            end_buffer += "&";
            break;
        case GLSL_TokenType::CMP_OR:
            end_buffer += "||";
            break;
        case GLSL_TokenType::OR:
            end_buffer += "|";
            break;
        case GLSL_TokenType::CMP_LT:
            end_buffer += "<";
            break;
        case GLSL_TokenType::CMP_LTEQ:
            end_buffer += "<=";
            break;
        case GLSL_TokenType::CMP_GT:
            end_buffer += ">";
            break;
        case GLSL_TokenType::CMP_GTEQ:
            end_buffer += ">=";
            break;
        case GLSL_TokenType::TERNARY:
            end_buffer += "?";
            break;
        case GLSL_TokenType::COLON:
            end_buffer += ":";
            break;
        case GLSL_TokenType::DOT:
            end_buffer += ".";
            break;
        case GLSL_TokenType::LITERAL:
            end_buffer += tok.data;
            break;
        case GLSL_TokenType::IDENTIFIER:
            if(tok.data == "layout") {
                end_buffer += tok.data + " ";
            }
            else if(tok.data == "provided") {
                end_buffer += " uniform ";
            }
            else {
                end_buffer += " " + tok.data + " ";
            }
            break;
        case GLSL_TokenType::ASSIGN:
            end_buffer += "=";
            break;
        case GLSL_TokenType::CMP_EQ:
            end_buffer += "==";
            break;
        default:
            break;
        }
    }
    print_info("%s", end_buffer.c_str());
    return end_buffer;
}

// Construct a shader by opening the provided path and creating a temporal ifstream, reading
// from that stream in text mode and then compiling the shader
Shader::Shader(const std::string& path, GLuint type, bool use_transpiler, std::string options) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // Read the entire file onto the stringstream
        file.open(Path::get(path).c_str());
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();

        buffer = stream.str();
        
        if(use_transpiler) {
            GLSL_Context ctx(buffer);
            ctx.lexer();
            try {
                ctx.parser();
            } catch(GLSL_Exception& e) {
                print_error("%s -> %s", e.it->data.c_str(), e.what());
            }
            buffer = ctx.to_text();
        }

        const char* c_code = buffer.c_str();
        id = glCreateShader(type);
        glShaderSource(id, 1, &c_code, NULL);

        compile(type);
    } catch(std::ifstream::failure& e) {
        print_error("Cannot load shader %s", path.c_str());
    }
}

#include <sstream>
void Shader::compile(GLuint type) {
    glCompileShader(id);

    // Check for errors of the shader
    GLint r = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &r);
    if(!r) {
        std::string error_info;
        glGetShaderInfoLog(id, GL_INFO_LOG_LENGTH, NULL, &error_info[0]);
        print_error("Raw status: %s", error_info.c_str());

        // Nvidia's style of errors
        std::istringstream sline(error_info);
        int slot, row, col;
        char ch;

        // nvidia styles errors the following way:
        // slot:row(column)
        // They also like to fuck me over, so they use
        // slot(row) <-- fuck this

        sline >> slot >> ch;
        if(ch == ':') {
            sline >> row >> ch;
            if(ch == '(') {
                sline >> col;
            }
        } else if(ch == '(') {
            sline >> row;
        }

        // Now we will pray that this actually worked

        // Next we will read over the shader file and find the line
        int row_rem = row;
        std::string::iterator it;
        for(it = buffer.begin(); it != buffer.end() && row_rem; it++) {
            if(*it == '\n') {
                row_rem--;
                if(!row_rem) {
                    break;
                }
            }
        }

        std::string line_buf = "(No line info)";
        if(it != buffer.end()) {
            line_buf = buffer.substr(std::distance(buffer.begin(), it), buffer.find_first_of('\n', std::distance(buffer.begin(), it)));
        }
        throw ShaderException(line_buf + "\n" + error_info);
    }
    // print_info("Status: Sucess");
}

GLuint Shader::get_id(void) const {
    return id;
}

// Deconstructs the shader object and we have to delete the shader from the OpenGL
// driver by calling glDeleteShader
Shader::~Shader() {
    if(id) {
        glDeleteShader(id);
    }
}

//
// Vertex shader
//
VertexShader::VertexShader(const std::string& path)
    : Shader(path, GL_VERTEX_SHADER)
{

}

VertexShader::~VertexShader(void) {

}

//
// Fragment shader
//
FragmentShader::FragmentShader(const std::string& path, bool use_transpiler, std::string options)
    : Shader(path, GL_FRAGMENT_SHADER, use_transpiler, options)
{

}

FragmentShader::~FragmentShader(void) {

}

//
// Geometry shader
//
GeometryShader::GeometryShader(const std::string& path)
    : Shader(path, GL_GEOMETRY_SHADER)
{

}

GeometryShader::~GeometryShader(void) {

}

//
// Tessellation control shader
//
TessControlShader::TessControlShader(const std::string& path)
    : Shader(path, GL_TESS_CONTROL_SHADER)
{

}

TessControlShader::~TessControlShader(void) {

}

//
// Tessellation evaluation shader
//
TessEvalShader::TessEvalShader(const std::string& path)
    : Shader(path, GL_TESS_EVALUATION_SHADER)
{

}

TessEvalShader::~TessEvalShader(void) {

}

//
// Program
//
Program::Program(const VertexShader* vertex, const FragmentShader* fragment, const GeometryShader* geometry, const TessControlShader* tctrl, const TessEvalShader* tee) {
    id = glCreateProgram();
    glBindAttribLocation(id, 0, "m_pos");
    glBindAttribLocation(id, 1, "m_texcoord");
    /*glBindAttribLocation(id, 2, "m_color");*/

    attach_shader(vertex);
    attach_shader(fragment);
    if(geometry != nullptr) {
        attach_shader(geometry);
    }

    if(tctrl != nullptr) {
        attach_shader(tctrl);
    }

    if(tee != nullptr) {
        attach_shader(tee);
    }
    link();
}

std::unique_ptr<Program> Program::create(const std::string& vs_path, const std::string& fs_path, const std::string& gs_path) {
    UnifiedRender::OpenGL::VertexShader vs = UnifiedRender::OpenGL::VertexShader(Path::get(vs_path));
    UnifiedRender::OpenGL::FragmentShader fs = UnifiedRender::OpenGL::FragmentShader(Path::get(fs_path));

    if(!gs_path.empty()) {
        UnifiedRender::OpenGL::GeometryShader gs = UnifiedRender::OpenGL::GeometryShader(Path::get(gs_path));
        return std::make_unique<Program>(&vs, &fs, &gs);
    }
    return std::make_unique<Program>(&vs, &fs);
}

std::unique_ptr<Program> Program::create_regular(const std::string& vs_path, const std::string& fs_path, const std::string& gs_path) {
    return create_regular(std::vector<Option>{}, vs_path, fs_path, gs_path);
}
std::unique_ptr<Program> Program::create_regular(std::vector<Option> options, const std::string& vs_path, const std::string& fs_path, const std::string& gs_path) {
    UnifiedRender::OpenGL::VertexShader vs = UnifiedRender::OpenGL::VertexShader(Path::get(vs_path));
    std::string defined_options = "#version 330 compatibility\n";
    for(auto& option : options) {
        if(option.used)
            defined_options += option.get_option();
    }
    UnifiedRender::OpenGL::FragmentShader fs = UnifiedRender::OpenGL::FragmentShader(Path::get(fs_path), false, defined_options);

    if(!gs_path.empty()) {
        UnifiedRender::OpenGL::GeometryShader gs = UnifiedRender::OpenGL::GeometryShader(Path::get(gs_path));
        return std::make_unique<Program>(&vs, &fs, &gs);
    }
    return std::make_unique<Program>(&vs, &fs);
}

// Attaches a shader to the program - this will make it so when the program is compiled the shader
// will then be linked onto it
void Program::attach_shader(const Shader* shader) {
    glAttachShader(id, shader->get_id());
}

void Program::link(void) {
#ifdef UR_RENDER_DEBUG
    if(!id) {
        throw UnifiedRender::DebugException("Program has no Id");
    }
#endif
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

void Program::use(void) const {
    glUseProgram(id);
}

/*
 * Uniform overloads
 * It allows the game engine to call these functions without worrying about type specifications
 */

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

/*
 * Sets the texture (sampler2D) into the shader,
 */
void Program::set_texture(int value, const std::string& name, const UnifiedRender::Texture& texture) const {
#ifdef UR_RENDER_DEBUG
    if(!texture.gl_tex_num) {
        throw UnifiedRender::DebugException("Texture with invalid Id passed to set_texture");
    }
#endif
    glActiveTexture(GL_TEXTURE0 + value);
    set_uniform(name, value);
    glBindTexture(GL_TEXTURE_2D, texture.gl_tex_num);
    }

void Program::set_texture(int value, const std::string& name, const UnifiedRender::TextureArray& texture) const {
#ifdef UR_RENDER_DEBUG
    if(!texture.gl_tex_num) {
        throw UnifiedRender::DebugException("Texture with invalid Id passed to set_texture");
    }
#endif
    glActiveTexture(GL_TEXTURE0 + value);
    set_uniform(name, value);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture.gl_tex_num);
    }

GLuint Program::get_id(void) const {
    return id;
}
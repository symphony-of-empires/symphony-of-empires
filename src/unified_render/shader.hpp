#pragma once

#include <string>
#include <exception>

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"

namespace UnifiedRender::OpenGl {
    class ShaderException : public std::exception {
        std::string buffer;
    public:
        ShaderException(const std::string& _buffer) : buffer(_buffer) {};
        virtual const char* what(void) const noexcept {
            return buffer.c_str();
        }
    };

    enum class GLSL_TokenType {
        ASSIGN, TERNARY, LITERAL, IDENTIFIER,
        ADD, SUB, MUL, DIV, REM, AND, OR,
        SEMICOLON, COMMA, COLON, DOT,
        CMP_EQ, CMP_GT, CMP_LT, CMP_GTEQ, CMP_LTEQ, CMP_OR, CMP_AND,
        LPAREN, RPAREN,
        LBRACKET, RBRACKET,
        LBRACE, RBRACE,
    };

    struct GLSL_Token {
        GLSL_Token(GLSL_TokenType _type) : type(_type) {};
        ~GLSL_Token() {};

        enum GLSL_TokenType type;
        std::string data;
    };

    enum class GLSL_VariableType {
        LOCAL, PROVIDED, INPUT, OUTPUT,
    };

    struct GLSL_Variable {
        enum GLSL_VariableType type;
        std::string type_name;
        std::string name;
        bool is_const;
        int layout_n;
    };

    struct GLSL_Function {
        std::string name;
        std::vector<std::pair<std::string, std::string>> args;
        std::string ret_type;
    };

    struct GLSL_Context {
        std::vector<GLSL_Variable> vars;
        std::vector<GLSL_Function> funcs;
        std::vector<GLSL_Token> tokens;
    };

    class GLSL_Exception : public std::exception {
        std::string buffer;
    public:
        GLSL_Exception(std::vector<GLSL_Token>::iterator _it, const std::string& _buffer) : buffer(_buffer), it(_it) {};
        virtual const char* what(void) const noexcept {
            return buffer.c_str();
        }

        std::vector<GLSL_Token>::iterator it;
    };

    class Shader {
    private:        
        void compile(GLuint type);
        std::string buffer;
        GLuint id;
    public:
        Shader(const std::string& path, GLuint type);
        ~Shader();
        void lexer(GLSL_Context& ctx, const std::string& buffer);
        void parser(GLSL_Context& ctx);
        std::string to_text(GLSL_Context& ctx);

        GLuint get_id(void) const;
    };

    class VertexShader : public Shader {
    public:
        VertexShader(const std::string& path);
    };

    class FragmentShader : public Shader {
    public:
        FragmentShader(const std::string& path);
    };

    class GeometryShader : public Shader {
    public:
        GeometryShader(const std::string& path);
    };

    class TessControlShader : public Shader {
    public:
        TessControlShader(const std::string& path);
    };

    class TessEvalShader : public Shader {
    public:
        TessEvalShader(const std::string& path);
    };

    class Program {
        GLuint id;
    public:
        Program(const VertexShader* vertex, const FragmentShader* fragment, const GeometryShader* geometry = nullptr, const TessControlShader* tctrl = nullptr, const TessEvalShader* tee = nullptr);
        static Program* create(const std::string& vs_path, const std::string& fs_path, const std::string& gs_path = "");

        void attach_shader(const Shader* shader);
        void link(void);

        void use(void) const;
        void set_uniform(const std::string& name, glm::mat4 uniform) const;
        void set_uniform(const std::string& name, float value1, float value2) const;
        void set_uniform(const std::string& name, float value1, float value2, float value3) const;
        void set_uniform(const std::string& name, float value1, float value2, float value3, float value4) const;
        void set_uniform(const std::string& name, float uniform) const;
        void set_uniform(const std::string& name, int uniform) const;

        void set_texture(int value, const std::string& name, const UnifiedRender::Texture& texture) const;
        void set_texture(int value, const std::string& name, const UnifiedRender::TextureArray& texture) const;
        
        GLuint get_id(void) const;
    };
};
// Eng3D - General purpouse game engine
// Copyright (C) 2021, Eng3D contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      glsl_trans.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>
#include <exception>

namespace Eng3D::OpenGL {
    enum class GLSL_TokenType {
        NONE,
        ASSIGN, TERNARY, LITERAL, IDENTIFIER,
        ADD, SUB, MUL, DIV, REM, AND, OR, XOR,
        SEMICOLON, COMMA, COLON, DOT,
        CMP_EQ, CMP_NEQ, CMP_GT, CMP_LT, CMP_GTEQ, CMP_LTEQ, CMP_OR, CMP_AND,
        LPAREN, RPAREN,
        LBRACKET, RBRACKET,
        LBRACE, RBRACE,
        NEWLINE,
        // Special "hacky" stuff
        MACRO,
    };

    class GLSL_Token {
    public:
        GLSL_Token(GLSL_TokenType _type)
            : type{ _type }
        {

        }

        ~GLSL_Token() = default;

        enum GLSL_TokenType type = GLSL_TokenType::NONE;
        std::string data;
    };

    enum class GLSL_VariableType {
        LOCAL, PROVIDED, INPUT, OUTPUT,
    };

    class GLSL_Variable {
    public:
        GLSL_Variable() = default;
        ~GLSL_Variable() = default;
        
        enum GLSL_VariableType type;
        std::string type_name;
        std::string name;
        bool is_const;
        int layout_n;
    };

    class GLSL_Function {
    public:
        std::string name;
        std::vector<std::pair<std::string, std::string>> args;
        std::string ret_type;
    };

    class GLSL_Define {
    public:
        std::string name;
        std::string value;
    };

    class GLSL_Context {
    public:
        GLSL_Context(const std::string& _buffer)
            : buffer{ _buffer }
        {

        }

        ~GLSL_Context() = default;
        std::string get_identifier(std::string::iterator& it);
        std::string get_literal(std::string::iterator& it);
        void lexer();
        void parser();
        std::string to_text();

        std::vector<int> line_numbers;
        std::vector<GLSL_Variable> vars;
        std::vector<GLSL_Function> funcs;
        std::vector<GLSL_Token> tokens;
        std::vector<GLSL_Define> defines;
        std::string buffer;
    };

    class GLSL_Exception: public std::exception {
        std::string buffer;
    public:
        GLSL_Exception(std::vector<GLSL_Token>::iterator _it, const std::string& _buffer)
            : buffer{ _buffer },
            it{ _it }
        {

        }

        virtual const char* what() const noexcept {
            return buffer.c_str();
        }

        std::vector<GLSL_Token>::iterator it;
    };
};

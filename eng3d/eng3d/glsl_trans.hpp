// Eng3D - General purpouse game engine
// Copyright (C) 2021-2023, Eng3D contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

namespace Eng3D::GLSL {
    class Token {
    public:
        enum class Type {
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

        Token(Eng3D::GLSL::Token::Type _type)
            : type{ _type }
        {

        }

        ~Token() = default;

        enum Eng3D::GLSL::Token::Type type = Eng3D::GLSL::Token::Type::NONE;
        std::string data;
    };

    enum class VariableType {
        LOCAL, PROVIDED, INPUT, OUTPUT,
    };

    class Variable {
    public:
        Variable() = default;
        ~Variable() = default;
        
        enum Eng3D::GLSL::VariableType type;
        std::string type_name;
        std::string name;
        bool is_const;
        int layout_n;
    };

    class Type {
    public:
        /// @brief Since GLSL doesn't allow complex structures we can rely
        /// on a simple table of primitives
        enum class PrimitiveType {
            NONE,
            // Primitives
            INT, FLOAT, BOOL,
            // Vectors
            VEC2, VEC3, VEC4,
            // Matrices
            MAT4,
            // Textures
            SAMPLER2D, SAMPLER2DARRAY, SAMPLER3D, SAMPLER3DARRAY,
        };
        std::string name;
        Eng3D::GLSL::Type::PrimitiveType primitive_type = Eng3D::GLSL::Type::PrimitiveType::NONE;
    };

    class Function {
    public:
        std::string name;
        std::vector<std::pair<std::string, std::string>> args;
        std::string ret_type;
    };

    class Define {
    public:
        std::string name;
        std::string value;
    };

    class Context {
    public:
        Context(const std::string_view _buffer)
            : buffer{ _buffer }
        {

        }
        ~Context() = default;
        std::string get_identifier(std::string::iterator& it);
        std::string get_literal(std::string::iterator& it);
        void lexer();
        void parser();
        std::string to_text();

        std::vector<int> line_numbers;
        std::vector<Eng3D::GLSL::Type> types;
        std::vector<Eng3D::GLSL::Variable> vars;
        std::vector<Eng3D::GLSL::Function> funcs;
        std::vector<Eng3D::GLSL::Token> tokens;
        std::vector<Eng3D::GLSL::Define> defines;
        std::string buffer;
    };

    class Exception: public std::exception {
        std::string buffer;
    public:
        Exception(std::vector<Eng3D::GLSL::Token>::iterator _it, const std::string_view _buffer)
            : buffer{ _buffer },
            it{ _it }
        {

        }

        virtual const char* what() const noexcept {
            return buffer.data();
        }

        std::vector<Eng3D::GLSL::Token>::iterator it;
    };
}

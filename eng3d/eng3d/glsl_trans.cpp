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
//      glsl_trans.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/glsl_trans.hpp"
#include "eng3d/log.hpp"

//
// GLSL Context
//
std::string Eng3D::GLSL::Context::get_identifier(std::string::iterator& it) {
    const auto start_it = it;

    // Alphanumerics, _ and dots are allowed as identifiers
    while(it != buffer.end() && (isalnum(*it) || *it == '_' || *it == '.'))
        it++;

    std::string str = buffer.substr(std::distance(buffer.begin(), start_it), std::distance(start_it, it));
    return str;
}

std::string Eng3D::GLSL::Context::get_literal(std::string::iterator& it) {
    const auto start_it = it;
    // Literal
    while(it != buffer.end() && (isdigit(*it) || *it == '.'))
        it++;

    // Skip 'type' specifier
    while(it != buffer.end() && isalpha(*it))
        it++;

    std::string str = buffer.substr(std::distance(buffer.begin(), start_it), std::distance(start_it, it));
    return str;
}

void Eng3D::GLSL::Context::lexer() {
    // Output the final stuff
    for(auto it = buffer.begin(); it != buffer.end(); ) {
        while(it != buffer.end() && isspace(*it) && (*it == '\r'))
            it++;

        if(it == buffer.end()) break;

        if(*it == '\n') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::NEWLINE));
            it++;
        } else if((*(it + 0) == '/' && *(it + 1) == '/')) {
            it += 2;
            // Single-line comments
            while(it != buffer.end() && (*it != '\n'))
                it++;
        } else if((*(it + 0) == '/' && *(it + 1) == '*')) {
            it += 2;
            // Multiline comments
            while((it + 1) != buffer.end()) {
                if(*(it + 0) == '*' && *(it + 1) == '/') {
                    it += 2;
                    break;
                } else if(*it == '\n') {
                    tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::NEWLINE));
                }
                it++;
            }
        } else if(*it == '#') {
            it++;
            auto start_it = it;
            while(it != buffer.end() && (*it != '\n'))
                it++;

            Eng3D::GLSL::Token tok = Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::MACRO);
            tok.data = buffer.substr(std::distance(buffer.begin(), start_it), std::distance(start_it, it));
            tokens.push_back(tok);
        } else if(*it == ',') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::COMMA));
            it++;
        } else if(*it == ';') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::SEMICOLON));
            it++;
        } else if(*it == '(') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::LPAREN));
            it++;
        } else if(*it == ')') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::RPAREN));
            it++;
        } else if(*it == '[') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::LBRACKET));
            it++;
        } else if(*it == ']') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::RBRACKET));
            it++;
        } else if(*it == '{') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::LBRACE));
            it++;
        } else if(*it == '}') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::RBRACE));
            it++;
        } else if(*it == '+') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::ADD));
            it++;
        } else if(*it == '-') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::SUB));
            it++;
        } else if(*it == '*') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::MUL));
            it++;
        } else if(*it == '/') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::DIV));
            it++;
        } else if(*it == '%') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::REM));
            it++;
        } else if(*it == '^') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::XOR));
            it++;
        } else if(*it == '<') {
            it++;
            if(it != buffer.end() && *it == '=') {
                tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::CMP_LTEQ));
                it++;
            } else {
                tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::CMP_LT));
            }
        } else if(*it == '>') {
            it++;
            if(it != buffer.end() && *it == '=') {
                tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::CMP_GTEQ));
                it++;
            } else {
                tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::CMP_GT));
            }
        } else if(*it == '|') {
            it++;
            if(it != buffer.end() && *it == '|') {
                tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::CMP_OR));
                it++;
            } else {
                tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::OR));
            }
        } else if(*it == '&') {
            it++;
            if(it != buffer.end() && *it == '&') {
                tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::CMP_AND));
                it++;
            } else {
                tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::AND));
            }
        } else if(*it == '=') {
            it++;
            if(it != buffer.end() && *it == '=') {
                tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::CMP_EQ));
                it++;
            } else {
                tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::ASSIGN));
            }
        } else if(*it == '?') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::TERNARY));
            it++;
        } else if(*it == ':') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::COLON));
            it++;
        } else if(*it == '.') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::DOT));
            it++;
        } else if(*it == '!' && (it + 1) != buffer.end() && *(it + 1) == '=') {
            tokens.push_back(Eng3D::GLSL::Token(Eng3D::GLSL::Token::Type::CMP_NEQ));
            it += 2;
        } else {
            if(isdigit(*it) || *it == '.') {
                Eng3D::GLSL::Token tok(Eng3D::GLSL::Token::Type::LITERAL);
                tok.data = get_literal(it);
                tokens.push_back(tok);
            } else if(isalnum(*it) || *it == '_') {
                Eng3D::GLSL::Token tok(Eng3D::GLSL::Token::Type::IDENTIFIER);
                tok.data = get_identifier(it);
                tokens.push_back(tok);
            } else {
                it++;
            }
        }
    }
}

/// @brief Parses the current context's tokens and optimizes them
void Eng3D::GLSL::Context::parser() {
    // Register the primitive types
    this->types = {
        { "int", Eng3D::GLSL::Type::PrimitiveType::INT },
        { "float", Eng3D::GLSL::Type::PrimitiveType::FLOAT },
        { "bool", Eng3D::GLSL::Type::PrimitiveType::BOOL },
        { "vec2", Eng3D::GLSL::Type::PrimitiveType::VEC2 },
        { "vec3", Eng3D::GLSL::Type::PrimitiveType::VEC3 },
        { "vec4", Eng3D::GLSL::Type::PrimitiveType::VEC4 },
        { "mat4", Eng3D::GLSL::Type::PrimitiveType::MAT4 },
        { "sampler2D", Eng3D::GLSL::Type::PrimitiveType::SAMPLER2D },
        { "sampler2DArray", Eng3D::GLSL::Type::PrimitiveType::SAMPLER2DARRAY },
        { "sampler3D", Eng3D::GLSL::Type::PrimitiveType::SAMPLER3D },
        { "sampler3DArray", Eng3D::GLSL::Type::PrimitiveType::SAMPLER3DARRAY },
    };

    Eng3D::GLSL::Function fn;

    fn = Eng3D::GLSL::Function();
    fn.name = "vec2";
    fn.ret_type = "vec2";
    fn.args.emplace_back("float", "x");
    fn.args.emplace_back("float", "y");
    funcs.push_back(fn);

    fn = Eng3D::GLSL::Function();
    fn.name = "vec3";
    fn.ret_type = "vec3";
    fn.args.emplace_back("float", "x");
    fn.args.emplace_back("float", "y");
    fn.args.emplace_back("float", "z");
    funcs.push_back(fn);

    fn = Eng3D::GLSL::Function();
    fn.name = "vec4";
    fn.ret_type = "vec4";
    fn.args.emplace_back("float", "x");
    fn.args.emplace_back("float", "y");
    fn.args.emplace_back("float", "z");
    fn.args.emplace_back("float", "w");
    funcs.push_back(fn);

    // Register all the overloads for this function
    std::vector<std::string> vec_types = { "vec2", "vec3", "vec4", "sampler2D" };
    std::vector<std::string> vec_and_sample_types = { "vec2", "vec3", "vec4", "sampler2D" };

    for(const auto& it1 : vec_and_sample_types) {
        for(const auto& it2 : vec_and_sample_types) {
            fn = Eng3D::GLSL::Function();
            fn.name = "mix";
            fn.ret_type = "vec4";
            fn.args.emplace_back(it1, "x");
            fn.args.emplace_back(it2, "y");
            fn.args.emplace_back("float", "weight");
            funcs.push_back(fn);
        }
    }

    fn = Eng3D::GLSL::Function();
    fn.name = "clamp";
    for(const auto& it1 : vec_types) {
        fn.ret_type = it1;
        fn.args.emplace_back(it1, "num");
        fn.args.emplace_back(it1, "min");
        fn.args.emplace_back(it1, "max");
    }
    funcs.push_back(fn);

    fn = Eng3D::GLSL::Function();
    fn.name = "min";
    for(const auto& it1 : vec_types) {
        fn.ret_type = it1;
        fn.args.emplace_back(it1, "x");
        fn.args.emplace_back(it1, "y");
    }
    funcs.push_back(fn);

    fn = Eng3D::GLSL::Function();
    fn.name = "max";
    for(const auto& it1 : vec_types) {
        fn.ret_type = it1;
        fn.args.emplace_back(it1, "x");
        fn.args.emplace_back(it1, "y");
    }
    funcs.push_back(fn);

    for(const auto& tok : tokens) {
        if(tok.type == Eng3D::GLSL::Token::Type::ASSIGN) {

        }
    }
}

std::string Eng3D::GLSL::Context::to_text() {
    auto it = tokens.begin();
    std::string end_buffer;
    int current_line = 1;

    while(it != tokens.end() && it->type == Eng3D::GLSL::Token::Type::NEWLINE)
        it++;

    // Go after the first instance of a preprocessor macro
    if(it->type == Eng3D::GLSL::Token::Type::MACRO) {
        end_buffer += "#" + it->data + "\r\n";
        line_numbers.push_back(current_line++);
        it += 2; // Skip the NEWLINE also
        for(const auto& define : defines) {
            end_buffer += "#define " + define.name + " " + define.value + "\r\n";
            line_numbers.push_back(current_line);
        }
    }

    bool had_newline = false;
    for(; it != tokens.end(); it++) {
        if(it->type != Eng3D::GLSL::Token::Type::NEWLINE)
            had_newline = false;

        switch(it->type) {
        case Eng3D::GLSL::Token::Type::MACRO:
            end_buffer += "#" + it->data;
            break;
        case Eng3D::GLSL::Token::Type::NEWLINE:
            line_numbers.push_back(current_line++);
            if(!had_newline)
                end_buffer += "\n";
            had_newline = true;
            break;
        case Eng3D::GLSL::Token::Type::SEMICOLON:
            end_buffer += ";";
            break;
        case Eng3D::GLSL::Token::Type::COMMA:
            end_buffer += ",";
            break;
        case Eng3D::GLSL::Token::Type::LPAREN:
            end_buffer += "(";
            break;
        case Eng3D::GLSL::Token::Type::RPAREN:
            end_buffer += ")";
            break;
        case Eng3D::GLSL::Token::Type::LBRACKET:
            end_buffer += "[";
            break;
        case Eng3D::GLSL::Token::Type::RBRACKET:
            end_buffer += "]";
            break;
        case Eng3D::GLSL::Token::Type::LBRACE:
            end_buffer += "{";
            break;
        case Eng3D::GLSL::Token::Type::RBRACE:
            end_buffer += "}";
            break;
        case Eng3D::GLSL::Token::Type::ADD:
            end_buffer += "+";
            break;
        case Eng3D::GLSL::Token::Type::SUB:
            end_buffer += "-";
            break;
        case Eng3D::GLSL::Token::Type::MUL:
            end_buffer += "*";
            break;
        case Eng3D::GLSL::Token::Type::DIV:
            end_buffer += "/";
            break;
        case Eng3D::GLSL::Token::Type::REM:
            end_buffer += "%";
            break;
        case Eng3D::GLSL::Token::Type::XOR:
            end_buffer += "^";
            break;
        case Eng3D::GLSL::Token::Type::CMP_AND:
            end_buffer += "&&";
            break;
        case Eng3D::GLSL::Token::Type::AND:
            end_buffer += "&";
            break;
        case Eng3D::GLSL::Token::Type::CMP_OR:
            end_buffer += "||";
            break;
        case Eng3D::GLSL::Token::Type::OR:
            end_buffer += "|";
            break;
        case Eng3D::GLSL::Token::Type::CMP_LT:
            end_buffer += "<";
            break;
        case Eng3D::GLSL::Token::Type::CMP_LTEQ:
            end_buffer += "<=";
            break;
        case Eng3D::GLSL::Token::Type::CMP_GT:
            end_buffer += ">";
            break;
        case Eng3D::GLSL::Token::Type::CMP_GTEQ:
            end_buffer += ">=";
            break;
        case Eng3D::GLSL::Token::Type::TERNARY:
            end_buffer += "?";
            break;
        case Eng3D::GLSL::Token::Type::COLON:
            end_buffer += ":";
            break;
        case Eng3D::GLSL::Token::Type::DOT:
            end_buffer += ".";
            break;
        case Eng3D::GLSL::Token::Type::LITERAL:
            end_buffer += it->data;
            break;
        case Eng3D::GLSL::Token::Type::IDENTIFIER:
            if(it->data == "layout")
                end_buffer += it->data + " ";
            else if(it->data == "provided")
                end_buffer += "uniform ";
            else if(it->data == "in")
                end_buffer += "in ";
            else if(it->data == "out")
                end_buffer += "out ";
            else
                end_buffer += it->data + " ";
            break;
        case Eng3D::GLSL::Token::Type::ASSIGN:
            end_buffer += "=";
            break;
        case Eng3D::GLSL::Token::Type::CMP_EQ:
            end_buffer += "==";
            break;
        case Eng3D::GLSL::Token::Type::CMP_NEQ:
            end_buffer += "!=";
            break;
        default:
            break;
        }
    }

    Eng3D::Log::debug("glsl_trans", end_buffer);
    return end_buffer;
}

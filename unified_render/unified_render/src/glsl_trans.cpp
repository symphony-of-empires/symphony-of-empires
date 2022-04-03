// Unified Render - General purpouse game engine
// Copyright (C) 2021, Unified Render contributors
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
//      glsl_trans.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/glsl_trans.hpp"
#include "unified_render/log.hpp"

//
// GLSL Context
//
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
            it++;

            std::string::iterator start_it = it;
            while(it != buffer.end() && (*it != '\n')) {
                it++;
            }
            
            GLSL_Token tok = GLSL_Token(GLSL_TokenType::MACRO);
            tok.data = buffer.substr(std::distance(buffer.begin(), start_it), std::distance(start_it, it));
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
    std::vector<std::string> mix_strings = { "vec2", "vec3", "vec4", "sampler2D" };
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
}

std::string UnifiedRender::OpenGL::GLSL_Context::to_text(void) {
    std::vector<GLSL_Token>::const_iterator it = tokens.begin();
    std::string end_buffer;

    // Go after the first instance of a preprocessor macro
    if(it->type == GLSL_TokenType::MACRO) {
        end_buffer += "#" + it->data + "\r\n";
        it++;
        for(const auto& define : defines) {
            end_buffer += "#define " + define.name + " " + define.value + "\r\n";
        }
    }

    for( ; it != tokens.end(); it++) {
        switch(it->type) {
        case GLSL_TokenType::MACRO:
            end_buffer += "#" + it->data + "\r\n";
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
            end_buffer += it->data;
            break;
        case GLSL_TokenType::IDENTIFIER:
            if(it->data == "layout") {
                end_buffer += it->data + " ";
            } else if(it->data == "provided") {
                end_buffer += " uniform ";
            } else {
                end_buffer += " " + it->data + " ";
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

    UnifiedRender::Log::debug("glsl_trans", end_buffer);
    return end_buffer;
}
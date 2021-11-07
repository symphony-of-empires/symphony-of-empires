#pragma once

#include <GL/glew.h>

#include "client/render/texture.hpp"

namespace UnifiedRender {
    namespace OpenGl {
        class Framebuffer {
        public:
            Framebuffer();
            ~Framebuffer();
            void set_texture(int index, const UnifiedRender::Texture* texture);
            void use();
        private:
            GLuint id = 0;
        };
    }
}
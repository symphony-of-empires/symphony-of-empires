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
//      framebuffer.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/framebuffer.hpp"
#include "unified_render/log.hpp"

UnifiedRender::OpenGL::Framebuffer::Framebuffer() {
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

UnifiedRender::OpenGL::Framebuffer::~Framebuffer() {
	if(id) {
		glDeleteFramebuffers(1, &id);
	}
}

void UnifiedRender::OpenGL::Framebuffer::set_texture(int index, const UnifiedRender::Texture& texture){
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture.gl_tex_num, 0);
    
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        UnifiedRender::Log::error("opengl", "Frame buffer error");
	}
}

void UnifiedRender::OpenGL::Framebuffer::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}
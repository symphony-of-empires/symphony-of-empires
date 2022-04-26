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
//      framebuffer.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "eng3d/framebuffer.hpp"
#include "eng3d/log.hpp"

Eng3D::OpenGL::Framebuffer::Framebuffer() {
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

Eng3D::OpenGL::Framebuffer::~Framebuffer() {
	if(id) {
		glDeleteFramebuffers(1, &id);
	}
}

void Eng3D::OpenGL::Framebuffer::set_texture(int index, const Eng3D::Texture& texture){
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture.gl_tex_num, 0);
    
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Eng3D::Log::error("opengl", "Frame buffer error");
	}
}

void Eng3D::OpenGL::Framebuffer::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}
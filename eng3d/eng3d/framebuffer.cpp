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
//      framebuffer.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------


#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#   include <GL/gl.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl3.h>
#   include <GLES3/gl31.h>
#endif

#include "eng3d/framebuffer.hpp"
#include "eng3d/log.hpp"
#include "eng3d/string.hpp"

Eng3D::OpenGL::Framebuffer::Framebuffer() {
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

Eng3D::OpenGL::Framebuffer::~Framebuffer() {
	if(id)
		glDeleteFramebuffers(1, &id);
}

void Eng3D::OpenGL::Framebuffer::set_texture(int index, const Eng3D::Texture& texture) {
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture.id, 0);
	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_buffers);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Eng3D::Log::error("opengl", Eng3D::Locale::translate("Framebuffer error"));
}

void Eng3D::OpenGL::Framebuffer::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

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
//      renderbuffer.cpp
//
// Abstract:
//      Wrapper for render buffers.
// ----------------------------------------------------------------------------

#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#   include <GL/gl.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl3.h>
#   include <GLES3/gl31.h>
#endif

#include "eng3d/renderbuffer.hpp"
#include "eng3d/log.hpp"
#include "eng3d/string.hpp"

Eng3D::OpenGL::Renderbuffer::Renderbuffer() {
	glGenRenderbuffers(1, &id);
	glBindRenderbuffer(GL_FRAMEBUFFER, id);
}

Eng3D::OpenGL::Renderbuffer::~Renderbuffer() {
	if(id)
		glDeleteRenderbuffers(1, &id);
}

void Eng3D::OpenGL::Renderbuffer::use() {
	glBindRenderbuffer(GL_FRAMEBUFFER, id);
}

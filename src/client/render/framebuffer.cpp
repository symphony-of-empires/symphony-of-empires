#include "client/render/framebuffer.hpp"
#include "print.hpp"

using namespace UnifiedRender::OpenGl;

Framebuffer::Framebuffer() {
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

Framebuffer::~Framebuffer() {
	if(id)
		glDeleteFramebuffers(1, &id);
}

void Framebuffer::set_texture(int index, const UnifiedRender::Texture* texture){
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture->gl_tex_num, 0);
}

void Framebuffer::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}
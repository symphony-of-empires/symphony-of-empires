#include "unified_render/framebuffer.hpp"
#include "unified_render/print.hpp"

UnifiedRender::OpenGl::Framebuffer::Framebuffer() {
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

UnifiedRender::OpenGl::Framebuffer::~Framebuffer() {
	if(id)
		glDeleteFramebuffers(1, &id);
}

void UnifiedRender::OpenGl::Framebuffer::set_texture(int index, const UnifiedRender::Texture& texture){
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture.gl_tex_num, 0);
    
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        print_error("Frame buffer error");
	}
}

void UnifiedRender::OpenGl::Framebuffer::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}
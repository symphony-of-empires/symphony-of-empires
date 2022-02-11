#pragma once

#include <stdio.h>
#include <thread>
#include <mutex>
#include <string.h>
#include <GL/glew.h>

#include "unified_render/stb_image_write.h"
#include "unified_render/texture.h"

namespace UnifiedRender
{
    class TextureSave
    {
    public:
        TextureSave(std::string _filename, Texture* texture)
        {
            filename = _filename;
            width = texture->width;
            height = texture->height;
            channel_count = 3;

            stride = channel_count * width;

            data_size = stride * height;

            glGenBuffers(1, &pbo);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
            glBufferData(GL_PIXEL_PACK_BUFFER, data_size, NULL, GL_STREAM_DRAW);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture->gl_tex_num);

            glGetTexImage(GL_TEXTURE_2D,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                (GLvoid*)0);
            //ensure we don't try and read data before the transfer is complete
            sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            glFlush();
        }

        void try_save()
        {
            if(done)
                return;

            // then regularly check for completion
            GLint result;

            glGetSynciv(sync, GL_SYNC_STATUS, sizeof(result), NULL, &result);
            if(result == GL_SIGNALED){
                glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
                GLubyte* src = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
                if(src)
                {
                    int success = stbi_write_png(filename.c_str(), width, height, channel_count, src, stride);
                }

                glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
                glDeleteBuffers(1, &pbo);
                pbo = 0;
                done = true;
            }
        }

    private:
        int width;
        int height;
        int channel_count;

        int stride;
        GLsync sync;

        int data_size;
        std::string filename;

        GLuint pbo;
        bool done = false;
    };
}
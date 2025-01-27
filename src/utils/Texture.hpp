#include "utils.hpp"

class Texture
{
public:
    Texture() : m_handle(0) {}
    
    ~Texture()
    {
        if (m_handle != 0)
        {
            glDeleteTextures(1, &m_handle);
        }
    }

    //I guess it makes sense to avoid copy constructors for textures
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    //Move constructor instead makes sense
    Texture(Texture&& other) noexcept : m_handle(other.m_handle)
    {
        other.m_handle = 0;
    }

    Texture& operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            if (m_handle != 0)
            {
                glDeleteTextures(1, &m_handle);
            }
            m_handle = other.m_handle;
            other.m_handle = 0;
        }
        return *this;
    }

    void generate(GLenum textureUnit, GLenum internalFormat, GLenum format, int width, int height, int bpp, unsigned char* data)
    {
        if (m_handle != 0)
        {
            glDeleteTextures(1, &m_handle);
        }

        glGenTextures(1, &m_handle);
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, m_handle);

        glTexImage2D(
            GL_TEXTURE_2D,
            0, internalFormat,
            width, height,
            0, format,
            GL_UNSIGNED_BYTE,
            data
        );

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 40);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint getID() const { return m_handle; }

private:
    GLuint m_handle;
};

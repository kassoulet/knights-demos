#include "OpenGLWrapper.h"
#include <iostream>
#include <fstream>
#include <vector>

// Include stb_image for texture loading
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Include stb_image_write for screenshot capture
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

OpenGLRenderer* g_pRenderer = nullptr;

OpenGLRenderer::OpenGLRenderer() 
    : window(nullptr), glContext(nullptr), width(0), height(0), isFullscreen(false),
      blendEnabled(false), srcBlend(0), dstBlend(0), textureEnabled(false), currentTexture(0) {
}

OpenGLRenderer::~OpenGLRenderer() {
    Shutdown();
}

bool OpenGLRenderer::Initialize(int w, int h, const char* title, bool fullscreen) {
    width = w;
    height = h;
    isFullscreen = fullscreen;
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Configure SDL OpenGL attributes - Use Compatibility Profile for fixed-function pipeline
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Create window
    Uint32 flags = SDL_WINDOW_OPENGL;
    if (fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    
    // Create OpenGL context
    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    
    // Set up viewport
    glViewport(0, 0, width, height);
    
    blendEnabled = true;
    srcBlend = GL_SRC_ALPHA;
    dstBlend = GL_ONE_MINUS_SRC_ALPHA;
    textureEnabled = true;
    
    g_pRenderer = this;
    return true;
}

void OpenGLRenderer::Shutdown() {
    // Clean up textures
    for (auto& tex : textures) {
        glDeleteTextures(1, &tex.second);
    }
    textures.clear();
    
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
        glContext = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    SDL_Quit();
    
    g_pRenderer = nullptr;
}

void OpenGLRenderer::Clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::BeginScene() {
    // Set up rendering state
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OpenGLRenderer::EndScene() {
    // Nothing needed for immediate mode
}

void OpenGLRenderer::Present() {
    SDL_GL_SwapWindow(window);
    SDL_PumpEvents(); // Process events
}

GLuint OpenGLRenderer::LoadTexture(const std::string& filename) {
    // Check if texture is already loaded
    auto it = textures.find(filename);
    if (it != textures.end()) {
        return it->second;
    }

    // First, check if the file exists by attempting to load it
    int width, height, channels;
    unsigned char* original_data = stbi_load(filename.c_str(), &width, &height, &channels, 4);

    if (!original_data) {
        // Try to load from gfx/ directory
        std::string gfxPath = "gfx/" + filename;
        original_data = stbi_load(gfxPath.c_str(), &width, &height, &channels, 4);
    }

    unsigned char* data_to_use;
    bool created_placeholder = false;

    if (original_data) {
        // File loaded successfully, use original data
        data_to_use = original_data;
    } else {
        std::cerr << "Failed to load texture: " << filename << ", creating placeholder..." << std::endl;

        // Create a simple 2x2 placeholder texture
        width = 2;
        height = 2;
        channels = 4;
        data_to_use = new unsigned char[width * height * channels];
        created_placeholder = true;

        // Create a simple checkerboard pattern
        for (int i = 0; i < width * height; i++) {
            int x = i % width;
            int y = i / width;

            // Checkerboard pattern
            if ((x + y) % 2 == 0) {
                data_to_use[i * 4 + 0] = 255; // Red
                data_to_use[i * 4 + 1] = 255; // Green
                data_to_use[i * 4 + 2] = 255; // Blue
                data_to_use[i * 4 + 3] = 255; // Alpha
            } else {
                data_to_use[i * 4 + 0] = 128; // Red
                data_to_use[i * 4 + 1] = 128; // Green
                data_to_use[i * 4 + 2] = 128; // Blue
                data_to_use[i * 4 + 3] = 255; // Alpha
            }
        }
    }

    // Generate OpenGL texture
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_to_use);

    // Free image data appropriately
    if (created_placeholder) {
        delete[] data_to_use;
    } else {
        // This was original data loaded by stbi_load
        stbi_image_free(original_data);
    }

    // Store in map
    textures[filename] = textureId;

    return textureId;
}

bool OpenGLRenderer::TakeScreenshot(const std::string& filename) {
    std::vector<unsigned char> pixels(static_cast<size_t>(width) * height * 4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // glReadPixels returns rows bottom-to-top; PNG expects top-to-bottom.
    stbi_flip_vertically_on_write(1);
    if (!stbi_write_png(filename.c_str(), width, height, 4, pixels.data(), width * 4)) {
        std::cerr << "Failed to write screenshot: " << filename << std::endl;
        return false;
    }
    return true;
}

void OpenGLRenderer::BindTexture(GLuint textureId) {
    if (textureId == 0) {
        glDisable(GL_TEXTURE_2D);
        textureEnabled = false;
    } else {
        glEnable(GL_TEXTURE_2D);
        textureEnabled = true;
        glBindTexture(GL_TEXTURE_2D, textureId);
        currentTexture = textureId;
    }
}

void OpenGLRenderer::DeleteTexture(GLuint textureId) {
    if (textureId != 0) {
        glDeleteTextures(1, &textureId);

        // Remove from map
        for (auto it = textures.begin(); it != textures.end(); ++it) {
            if (it->second == textureId) {
                textures.erase(it);
                break;
            }
        }
    }
}

void OpenGLRenderer::SetRenderState(int state, int value) {
    switch (state) {
        case 0: // D3DRENDERSTATE_ALPHABLENDENABLE
            if (value) {
                glEnable(GL_BLEND);
                blendEnabled = true;
            } else {
                glDisable(GL_BLEND);
                blendEnabled = false;
            }
            break;
        case 1: // D3DRENDERSTATE_SRCBLEND
            switch (value) {
                case 1: srcBlend = GL_ZERO; break;
                case 2: srcBlend = GL_ONE; break;
                case 3: srcBlend = GL_SRC_COLOR; break;
                case 4: srcBlend = GL_ONE_MINUS_SRC_COLOR; break;
                case 5: srcBlend = GL_SRC_ALPHA; break;
                case 6: srcBlend = GL_ONE_MINUS_SRC_ALPHA; break;
                default: srcBlend = GL_SRC_ALPHA; break;
            }
            if (blendEnabled) {
                glBlendFunc(srcBlend, dstBlend);
            }
            break;
        case 2: // D3DRENDERSTATE_DESTBLEND
            switch (value) {
                case 1: dstBlend = GL_ZERO; break;
                case 2: dstBlend = GL_ONE; break;
                case 3: dstBlend = GL_SRC_COLOR; break;
                case 4: dstBlend = GL_ONE_MINUS_SRC_COLOR; break;
                case 5: dstBlend = GL_SRC_ALPHA; break;
                case 6: dstBlend = GL_ONE_MINUS_SRC_ALPHA; break;
                default: dstBlend = GL_ONE_MINUS_SRC_ALPHA; break;
            }
            if (blendEnabled) {
                glBlendFunc(srcBlend, dstBlend);
            }
            break;
        default:
            break;
    }
}

void OpenGLRenderer::SetTextureStageState(int stage, int type, int value) {
    // For now, we'll just handle basic texture states
    // This is a simplified implementation
}

void OpenGLRenderer::DrawPrimitive(int primitiveType, const std::vector<Vertex>& vertices) {
    if (vertices.empty()) return;

    glBegin(primitiveType);

    for (const auto& vertex : vertices) {
        // Set color
        glColor4f(vertex.r, vertex.g, vertex.b, vertex.a);

        // Set texture coordinates if texture is enabled
        if (textureEnabled) {
            glTexCoord2f(vertex.tu, vertex.tv);
        }

        // Set vertex position
        glVertex3f(vertex.x, vertex.y, vertex.z);
    }

    glEnd();
}


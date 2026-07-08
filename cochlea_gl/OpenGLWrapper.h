#ifndef OPENGL_WRAPPER_H
#define OPENGL_WRAPPER_H

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <map>

// Structure to represent a vertex (similar to D3DTLVERTEX but for OpenGL)
struct Vertex {
    float x, y, z;      // Position
    float rhw;          // Reciprocal homogeneous W
    float r, g, b, a;   // Color
    float tu, tv;       // Texture coordinates
};

class OpenGLRenderer {
public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    bool Initialize(int width, int height, const char* title, bool fullscreen = false);
    void Shutdown();

    void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
    void BeginScene();
    void EndScene();
    void Present();

    // Texture management
    GLuint LoadTexture(const std::string& filename);
    void BindTexture(GLuint textureId);
    void DeleteTexture(GLuint textureId);

    // Capture the current framebuffer to a PNG file
    bool TakeScreenshot(const std::string& filename);

    // Rendering functions (immediate mode style)
    void SetRenderState(int state, int value);
    void SetTextureStageState(int stage, int type, int value);

    // Drawing primitives
    void DrawPrimitive(int primitiveType, const std::vector<Vertex>& vertices);

    // Getters
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

private:
    SDL_Window* window;
    SDL_GLContext glContext;
    int width, height;
    bool isFullscreen;

    // Texture management
    std::map<std::string, GLuint> textures;

    // Render states
    bool blendEnabled;
    int srcBlend, dstBlend;
    bool textureEnabled;
    GLuint currentTexture;

    void SetupOrthographicProjection();
};

// Global renderer instance
extern OpenGLRenderer* g_pRenderer;

// Compatibility functions for Direct3D-style operations
void D3DToGLMatrix(float* d3dMatrix);

#endif // OPENGL_WRAPPER_H
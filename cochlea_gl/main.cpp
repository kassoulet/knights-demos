#include <iostream>
#include <fstream>
#include <chrono>
#include <sys/stat.h>
#include <cstring>
#include <SDL2/SDL.h>
#include "OpenGLWrapper.h"
#include "Cochlea.h"
#include "xm_player.h"

// Global variables
float VBLframe = 0.0f;
float VBLdelta = 0.0f;
int VBLsync = 0;
bool g_demoFinished = false;

extern xm_player_t* g_xmPlayer;

// Screenshot settings
const int SCREENSHOT_INTERVAL_MS = 10000; // 10 seconds
const std::string SCREENSHOT_DIR = "screens/";

// Default settings
int g_width = 640;
int g_height = 480;
int g_sample_rate = 44100;

void PrintUsage(const char* program) {
    std::cout << "Usage: " << program << " [options]\n"
              << "Options:\n"
              << "  -w, --width <pixels>   Window width (default: 640)\n"
              << "  -h, --height <pixels>  Window height (default: 480)\n"
              << "  -r, --rate <hz>        Audio sample rate (default: 44100)\n"
              << "                         Common values: 22050, 44100, 48000, 88200, 96000\n"
              << "  --help                 Show this help message\n"
              << std::endl;
}

bool ParseArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            PrintUsage(argv[0]);
            return false;
        } else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--width") == 0) {
            if (i + 1 < argc) {
                g_width = atoi(argv[++i]);
                if (g_width < 320) g_width = 320;
                if (g_width > 4096) g_width = 4096;
            }
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--height") == 0) {
            if (i + 1 < argc) {
                g_height = atoi(argv[++i]);
                if (g_height < 240) g_height = 240;
                if (g_height > 2160) g_height = 2160;
            }
        } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--rate") == 0) {
            if (i + 1 < argc) {
                g_sample_rate = atoi(argv[++i]);
                // Validate sample rate
                if (g_sample_rate < 22050) g_sample_rate = 22050;
                if (g_sample_rate > 192000) g_sample_rate = 192000;
            }
        } else {
            std::cerr << "Unknown option: " << argv[i] << std::endl;
            PrintUsage(argv[0]);
            return false;
        }
    }
    return true;
}

// Ensure directory exists
void EnsureDirectoryExists(const std::string& dir) {
    mkdir(dir.c_str(), 0755);
}

// Generate screenshot filename with timestamp
std::string GetScreenshotFilename() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_now = std::localtime(&time_t_now);
    
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), "%04d%02d%02d_%02d%02d%02d.png",
                  tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
                  tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
    
    return SCREENSHOT_DIR + std::string(buffer);
}

// Audio callback for SDL
void AudioCallback(void* userdata, Uint8* stream, int len) {
    xm_player_t* player = (xm_player_t*)userdata;
    if (player && player->playing) {
        float* buffer = (float*)stream;
        uint32_t num_samples = len / (sizeof(float) * 2); // 2 for stereo
        xm_player_process_audio(player, buffer, num_samples);
    } else {
        memset(stream, 0, len);
    }
}

// Function declarations from the original WinMain.cpp
HRESULT App_OneTimeSceneInit();
HRESULT App_FrameMove(float fTimeKey);
HRESULT App_Render();
HRESULT App_InitDeviceObjects();
HRESULT App_FinalCleanup();

int main(int argc, char* argv[]) {
    // Parse command-line arguments
    if (!ParseArguments(argc, argv)) {
        return 1;
    }

    std::cout << "Starting Cochlea OpenGL..." << std::endl;
    std::cout << "Resolution: " << g_width << "x" << g_height << std::endl;
    std::cout << "Audio sample rate: " << g_sample_rate << " Hz" << std::endl;

    // Initialize SDL video and audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }
    std::cout << "SDL initialized successfully." << std::endl;

    // Initialize the OpenGL renderer
    OpenGLRenderer renderer;
    if (!renderer.Initialize(g_width, g_height, "Cochlea OpenGL")) {
        std::cerr << "Failed to initialize OpenGL renderer" << std::endl;
        SDL_Quit();
        return -1;
    }
    std::cout << "OpenGL renderer initialized." << std::endl;

    // Initialize the application
    if (FAILED(App_OneTimeSceneInit())) {
        std::cerr << "Failed to initialize application" << std::endl;
        renderer.Shutdown();
        return -1;
    }
    std::cout << "Application scene initialized." << std::endl;

    if (FAILED(App_InitDeviceObjects())) {
        std::cerr << "Failed to initialize device objects" << std::endl;
        App_FinalCleanup();
        renderer.Shutdown();
        return -1;
    }
    std::cout << "Device objects initialized." << std::endl;

    // Set up SDL audio
    SDL_AudioSpec wanted_spec, spec;
    SDL_zero(wanted_spec);
    wanted_spec.freq = g_sample_rate;
    wanted_spec.format = AUDIO_F32SYS;
    wanted_spec.channels = 2;
    wanted_spec.samples = 1024;
    wanted_spec.callback = AudioCallback;
    wanted_spec.userdata = g_xmPlayer;

    std::cout << "Opening audio device..." << std::endl;
    SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &spec, 0);
    if (audio_device == 0) {
        std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
    } else {
        std::cout << "Audio device opened: " << spec.freq << "Hz, " << (int)spec.channels << " channels." << std::endl;
        SDL_PauseAudioDevice(audio_device, 0); // Start playing audio
    }

    // Main rendering loop
    std::cout << "Starting main loop..." << std::endl;
    float lastTime = 0.0f;
    bool running = true;

    // Initialize SDL timer
    Uint32 lastFrameTime = SDL_GetTicks();
    Uint32 lastScreenshotTime = SDL_GetTicks();

    // Ensure screenshot directory exists
    EnsureDirectoryExists(SCREENSHOT_DIR);
    std::cout << "Screenshots will be saved to " << SCREENSHOT_DIR << " every " 
              << (SCREENSHOT_INTERVAL_MS / 1000) << " seconds." << std::endl;

    while (running) {
        // Calculate frame time
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastFrameTime) / 1000.0f; // Convert to seconds
        lastFrameTime = currentTime;

        float fTimeKey = currentTime / 1000.0f; // Convert to seconds

        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                // Manual screenshot with 'S' key
                if (event.key.keysym.sym == SDLK_s) {
                    renderer.TakeScreenshot(GetScreenshotFilename());
                }
            }
        }

        // Auto-save screenshot every 10 seconds
        if (currentTime - lastScreenshotTime >= SCREENSHOT_INTERVAL_MS) {
            renderer.TakeScreenshot(GetScreenshotFilename());
            lastScreenshotTime = currentTime;
        }

        // Frame move (animate the scene)
        if (FAILED(App_FrameMove(fTimeKey))) {
            std::cerr << "App_FrameMove failed" << std::endl;
            break;
        }

        if (g_demoFinished) {
            running = false;
            continue;
        }

        // Render the scene
        if (FAILED(App_Render())) {
            std::cerr << "App_Render failed" << std::endl;
            break;
        }

        // Present the frame
        renderer.Present();
    }

    // Cleanup
    if (audio_device != 0) {
        SDL_CloseAudioDevice(audio_device);
    }
    App_FinalCleanup();
    renderer.Shutdown();

    return 0;
}

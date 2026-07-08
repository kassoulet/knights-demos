#ifndef XM_PLAYER_H
#define XM_PLAYER_H

#include <stdint.h>
#include <stdbool.h>
#include <atomic>
#include <mutex>
#include <vector>
#include <libmodplug/modplug.h>

// XM Module Player using libmodplug (HQ resampling settings - see xm_player.cpp)
//
// xm_player_process_audio() runs on the SDL audio callback thread while
// xm_player_get_position()/xm_player_get_sync_info() are called every frame
// from the main render thread - `mutex` guards every access to `mod` (which
// libmodplug does not allow concurrent calls into) and to `sync_info`.
typedef struct {
    ModPlugFile *mod;
    uint32_t sample_rate;
    std::atomic<bool> playing;
    int sync_info;
    std::mutex mutex;
    std::vector<int16_t> pcmScratch; // reused across calls to avoid allocating on the audio thread
} xm_player_t;

// Initialize the XM player with module data
xm_player_t* xm_player_init(const uint8_t* module_data, uint32_t module_size, uint32_t sample_rate);

// Start playing the module
void xm_player_play(xm_player_t* player);

// Stop playing the module
void xm_player_stop(xm_player_t* player);

// Process audio - fills buffer with stereo samples (32-bit float)
void xm_player_process_audio(xm_player_t* player, float* buffer, uint32_t num_samples);

// Get current playback position for synchronization
uint32_t xm_player_get_position(xm_player_t* player);

// Get sync info for visualization
int xm_player_get_sync_info(xm_player_t* player);

// Cleanup
void xm_player_destroy(xm_player_t* player);

#endif // XM_PLAYER_H

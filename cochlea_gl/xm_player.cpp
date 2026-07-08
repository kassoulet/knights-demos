#include "xm_player.h"
#include <string.h>
#include <iostream>

// Initialize the XM player with module data
xm_player_t* xm_player_init(const uint8_t* module_data, uint32_t module_size, uint32_t sample_rate) {
    // `new` (not malloc+memset) is required here: xm_player_t now contains a
    // std::mutex and std::atomic<bool>, which must be properly constructed -
    // malloc'd-then-memset'd memory never runs their constructors.
    xm_player_t* player = new xm_player_t();
    player->sample_rate = sample_rate;

    // HQ settings: 8-tap FIR resampling (libmodplug's highest quality mode),
    // oversampling and noise reduction enabled. Settings apply at the next
    // ModPlug_Load(), so they must be set before loading the module.
    ModPlug_Settings settings;
    ModPlug_GetSettings(&settings);
    settings.mFlags = MODPLUG_ENABLE_OVERSAMPLING | MODPLUG_ENABLE_NOISE_REDUCTION;
    settings.mChannels = 2;
    settings.mBits = 16;
    settings.mFrequency = sample_rate;
    settings.mResamplingMode = MODPLUG_RESAMPLE_FIR;
    settings.mStereoSeparation = 128;
    settings.mMaxMixChannels = 256;
    settings.mLoopCount = -1; // loop forever
    ModPlug_SetSettings(&settings);

    player->mod = ModPlug_Load(module_data, (int)module_size);
    if (!player->mod) {
        std::cerr << "Failed to load module with libmodplug!" << std::endl;
        delete player;
        return NULL;
    }

    std::cout << "Module loaded: " << ModPlug_GetName(player->mod) << std::endl;

    return player;
}

// Start playing the module
void xm_player_play(xm_player_t* player) {
    if (player) {
        player->playing = true;
    }
}

// Stop playing the module
void xm_player_stop(xm_player_t* player) {
    if (player) {
        player->playing = false;
    }
}

// Process audio - fills buffer with stereo samples (32-bit float)
void xm_player_process_audio(xm_player_t* player, float* buffer, uint32_t num_samples) {
    if (!player || !player->playing || !player->mod) {
        for (uint32_t i = 0; i < num_samples * 2; i++) {
            buffer[i] = 0.0f;
        }
        return;
    }

    std::lock_guard<std::mutex> lock(player->mutex);

    // Reused across calls instead of allocating a fresh vector every time -
    // this function runs on SDL's real-time audio callback thread.
    if (player->pcmScratch.size() < (size_t)num_samples * 2) {
        player->pcmScratch.resize((size_t)num_samples * 2);
    }
    int16_t* pcm = player->pcmScratch.data();

    // ModPlug_Read decodes into interleaved stereo PCM at the mBits/mChannels
    // configured in xm_player_init() (16-bit here) - convert to float.
    int bytesRead = ModPlug_Read(player->mod, pcm, (int)(num_samples * 2 * sizeof(int16_t)));
    uint32_t samplesRead = bytesRead > 0 ? (uint32_t)bytesRead / (2 * sizeof(int16_t)) : 0;

    for (uint32_t i = 0; i < samplesRead * 2; i++) {
        buffer[i] = pcm[i] / 32768.0f;
    }
    for (uint32_t i = samplesRead * 2; i < num_samples * 2; i++) {
        buffer[i] = 0.0f;
    }

    // Update sync info using current row
    player->sync_info = ModPlug_GetCurrentRow(player->mod) % 5;
}

// Get current playback position for synchronization
uint32_t xm_player_get_position(xm_player_t* player) {
    if (!player || !player->mod) return 0;

    std::lock_guard<std::mutex> lock(player->mutex);
    int order = ModPlug_GetCurrentOrder(player->mod);
    int row = ModPlug_GetCurrentRow(player->mod);

    // Return a position value that cycles through different sections
    // This mimics the original behavior: (order << 8) | row
    return (uint32_t)((order << 8) | row);
}

// Get sync info for visualization
int xm_player_get_sync_info(xm_player_t* player) {
    if (!player) return 0;
    std::lock_guard<std::mutex> lock(player->mutex);
    return player->sync_info;
}

// Cleanup
void xm_player_destroy(xm_player_t* player) {
    if (player) {
        if (player->mod) {
            ModPlug_Unload(player->mod);
        }
        delete player;
    }
}

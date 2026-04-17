/**
 * @file audio.h
 * @brief Terminal Audio System Header
 * 
 * Provides simple audio through terminal beeps, ANSI bell characters,
 * and file-based sound effects. Pure stdio/stdlib implementation.
 */

#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * SOUND DEFINITIONS
 * ============================================================ */

/** Single sound effect: frequency (Hz) and duration (ms) */
typedef struct {
    int frequency;      /* Frequency in Hertz (60-8000 Hz) */
    int durationMs;     /* Duration in milliseconds */
} AudioSoundEffect;

/** Simple melody: array of sounds */
typedef struct {
    AudioSoundEffect sounds[128];
    int soundCount;
} AudioMelody;

/* ============================================================
 * PREDEFINED SOUND EFFECTS
 * ============================================================ */

#define SOUND_CLICK       { 800, 50 }
#define SOUND_SELECT      { 600, 100 }
#define SOUND_ERROR       { 200, 300 }
#define SOUND_SUCCESS     { 1000, 150 }
#define SOUND_LEVEL_UP    { 400, 80 }, { 600, 80 }, { 800, 120 }
#define SOUND_HURT        { 150, 200 }

/* ============================================================
 * PUBLIC API
 * ============================================================ */

/** Initialize the audio system */
void Audio_Init(void);

/** Shutdown the audio system */
void Audio_Shutdown(void);

/** Play a simple tone (frequency in Hz, duration in ms) */
void Audio_PlayTone(int frequency, int durationMs);

/** Play a predefined sound effect */
void Audio_PlaySound(const AudioSoundEffect* sound);

/** Play the terminal bell character */
void Audio_Beep(void);

/** Play a melody */
void Audio_PlayMelody(const AudioMelody* melody);

/** Stop any currently playing audio immediately */
void Audio_StopMusic(void);

/* ============================================================
 * UTILITY FUNCTIONS
 * ============================================================ */

/** Sleep for specified milliseconds (cross-platform) */
void Audio_Sleep(int ms);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_H */

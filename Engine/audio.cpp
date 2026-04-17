/**
 * @file audio.cpp
 * @brief Terminal Audio System Implementation
 * 
 * Provides simple audio through terminal beeps, ANSI bell characters,
 * and platform-specific tone generation. Pure stdio/stdlib implementation.
 */

#define _CRT_SECURE_NO_WARNINGS

#include "audio.h"
#include <stdio.h>

/* ============================================================
 * PLATFORM-SPECIFIC INCLUDES
 * ============================================================ */

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
    #include <unistd.h>
    #include <sys/time.h>
    #include <signal.h>
    #include <termios.h>
#endif

/* ============================================================
 * INTERNAL STATE
 * ============================================================ */

static int g_audioInitialized = 0;

/* ============================================================
 * CROSS-PLATFORM SLEEP
 * ============================================================ */

void Audio_Sleep(int ms) {
#if defined(_WIN32) || defined(_WIN64)
    Sleep((DWORD)ms);
#else
    /* Use usleep for better compatibility */
    usleep(ms * 1000);
#endif
}

/* ============================================================
 * TONE GENERATION
 * ============================================================ */

/** Play a tone at the specified frequency and duration */
void Audio_PlayTone(int frequency, int durationMs) {
    if (!g_audioInitialized || frequency <= 0 || durationMs <= 0) return;
    
#if defined(_WIN32) || defined(_WIN64)
    /* Windows: use Beep() API */
    Beep(frequency, (DWORD)durationMs);
#else
    /* Unix/Linux/macOS: use ANSI bell or system beep */
    if (frequency >= 250 && frequency <= 4000) {
        /* Use the terminal bell character for simple beeps */
        putchar(7); /* BEL character */
        fflush(stdout);
        
        /* Sleep for approximate duration */
        Audio_Sleep(durationMs);
    } else {
        /* For out-of-range frequencies, just play a short beep */
        putchar(7);
        fflush(stdout);
        Audio_Sleep(durationMs / 2);
    }
#endif
}

void Audio_PlaySound(const AudioSoundEffect* sound) {
    if (!sound) return;
    
    Audio_PlayTone(sound->frequency, sound->durationMs);
}

void Audio_Beep(void) {
    putchar(7); /* ASCII Bell character */
    fflush(stdout);
}

void Audio_PlayMelody(const AudioMelody* melody) {
    if (!melody) return;
    
    for (int i = 0; i < melody->soundCount && i < 128; i++) {
        Audio_PlaySound(&melody->sounds[i]);
        
        /* Small gap between notes */
        Audio_Sleep(30);
    }
}

void Audio_StopMusic(void) {
    /* No ongoing music to stop in this simple implementation */
}

/* ============================================================
 * INITIALIZATION & SHUTDOWN
 * ============================================================ */

void Audio_Init(void) {
    g_audioInitialized = 1;
}

void Audio_Shutdown(void) {
    g_audioInitialized = 0;
}

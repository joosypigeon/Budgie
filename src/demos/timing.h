#ifndef CYCLONE_DEMO_TIMING_H
#define CYCLONE_DEMO_TIMING_H

#include <stdbool.h>
/**
 * Represents all the information that the demo might need about the
 * timing of the game: current time, fps, frame number, and so on.
 */
typedef struct 
{
    /** The current render frame. This simply increments. */
    unsigned frameNumber;

    /**
     * The timestamp when the last frame ended. Times are
     * given in milliseconds since some undefined time.
     */
    unsigned lastFrameTimestamp;

    /**
     * The duration of the last frame in milliseconds.
     */
    unsigned lastFrameDuration;

    /**
     * The clockstamp of the end of the last frame.
     */
    unsigned long lastFrameClockstamp;

    /**
     * The duration of the last frame in clock ticks.
     */
    unsigned long lastFrameClockTicks;

    /**
     * Keeps track of whether the rendering is paused.
     */
    bool isPaused;

    // Calculated data

    /**
     * This is a recency weighted average of the frame time, calculated
     * from frame durations.
     */
    double averageFrameDuration;

    /**
     * The reciprocal of the average frame duration giving the mean
     * fps over a recency weighted average.
     */
    float fps;
} TimingData;

/**
 * Gets the global timing data object.
 */
TimingData *getTiming();

/**
 * Updates the timing system, should be called once per frame.
 */
void updateTiming();

/**
 * Initialises the frame information system. Use the overall
 * init function to set up all modules.
 */
void initTiming();

/**
 * Deinitialises the frame information system.
 */
void deinitTiming();

/**
 * Gets the global system time, in the best resolution possible.
 * Timing is in milliseconds.
 */
unsigned getTime();

/**
 * Gets the clock ticks since process start.
 */
unsigned long getClock();
#endif // CYCLONE_DEMO_TIMING_H



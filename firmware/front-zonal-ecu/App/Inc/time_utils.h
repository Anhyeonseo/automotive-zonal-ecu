#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <stdint.h>

/*
 * Returns elapsed time for a 32-bit millisecond counter.
 * Normal counter rollover is supported. If 'then_ms' is slightly newer than
 * 'now_ms' because an interrupt updated it after the caller sampled time,
 * the elapsed time is clamped to zero instead of underflowing.
 *
 * Valid measured intervals must be shorter than 2^31 milliseconds.
 */
static inline uint32_t TimeUtils_ElapsedMs(
    uint32_t now_ms,
    uint32_t then_ms)
{
    uint32_t elapsed_ms = now_ms - then_ms;

    if (elapsed_ms > 0x7FFFFFFFU)
    {
        return 0U;
    }

    return elapsed_ms;
}

#endif /* TIME_UTILS_H */

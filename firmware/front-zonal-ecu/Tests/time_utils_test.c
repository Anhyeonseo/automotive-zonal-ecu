#include <assert.h>
#include <stdio.h>

#include "time_utils.h"

int main(void)
{
    assert(TimeUtils_ElapsedMs(1000U, 900U) == 100U);
    assert(TimeUtils_ElapsedMs(1000U, 1000U) == 0U);

    /* An interrupt recorded a timestamp after the caller sampled now_ms. */
    assert(TimeUtils_ElapsedMs(1000U, 1001U) == 0U);

    /* Normal uint32_t tick rollover remains valid. */
    assert(TimeUtils_ElapsedMs(10U, 0xFFFFFFF0U) == 26U);

    puts("time_utils tests: PASS");
    return 0;
}

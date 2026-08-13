#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "monitor.h"

static int close_enough(double a, double b) {
    return fabs(a - b) < 0.001;
}

static void test_no_time_elapsed_returns_zero(void) {
    CpuStats stats = {.user = 10, .system = 5, .idle = 100};
    assert(close_enough(calculate_cpu_usage(stats, stats), 0.0));
}

static void test_fully_idle_interval_returns_zero(void) {
    CpuStats prev = {.idle = 100};
    CpuStats curr = {.idle = 200};
    assert(close_enough(calculate_cpu_usage(prev, curr), 0.0));
}

static void test_fully_busy_interval_returns_100(void) {
    CpuStats prev = {.user = 0, .idle = 100};
    CpuStats curr = {.user = 100, .idle = 100};
    assert(close_enough(calculate_cpu_usage(prev, curr), 100.0));
}

static void test_half_busy_interval_returns_50(void) {
    CpuStats prev = {.user = 0, .idle = 0};
    CpuStats curr = {.user = 50, .idle = 50};
    assert(close_enough(calculate_cpu_usage(prev, curr), 50.0));
}

static void test_windows_style_stats_without_extra_fields(void) {
    // En Windows, nice/iowait/irq/softirq/steal siempre quedan en 0.
    CpuStats prev = {.user = 20, .system = 10, .idle = 70};
    CpuStats curr = {.user = 40, .system = 20, .idle = 140};
    // total: 100 -> 200 (+100), idle: 70 -> 140 (+70) => 30% de uso
    assert(close_enough(calculate_cpu_usage(prev, curr), 30.0));
}

int main(void) {
    test_no_time_elapsed_returns_zero();
    test_fully_idle_interval_returns_zero();
    test_fully_busy_interval_returns_100();
    test_half_busy_interval_returns_50();
    test_windows_style_stats_without_extra_fields();

    printf("Todos los tests pasaron.\n");
    return 0;
}

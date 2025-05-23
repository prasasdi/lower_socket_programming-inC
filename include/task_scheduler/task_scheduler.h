// task_scheduler.h
#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <stdio.h>
#include <stdint.h>

#define MAX_TASKS 10
#define CORO_BEGIN(c) switch((c)->state) { case 0:
#define CORO_YIELD(c) do { (c)->state = __LINE__; return 1; case __LINE__:; } while (0)
#define CORO_END(c) } (c)->state = -1; return 0;

typedef struct {
    int id;
    int state;
    uint64_t wakeup_time;
} Coroutine;

typedef int (*TaskFunc)(Coroutine*);

typedef struct {
    Coroutine coro;
    TaskFunc func;
    int active;
} TaskEntry;

static TaskEntry g_task_list[MAX_TASKS];
static int g_task_count = 0;

static inline void await(TaskFunc func) {
    if (g_task_count >= MAX_TASKS) {
        fprintf(stderr, "Task queue full\n");
        return;
    }

    TaskEntry* entry = &g_task_list[g_task_count];
    entry->coro.id = g_task_count + 1;
    entry->coro.state = 0;
    entry->coro.wakeup_time = 0;
    entry->func = func;
    entry->active = 1;

    g_task_count++;
}

#ifdef _WIN32
    #include <windows.h>
    #define sleep_ms(ms) Sleep(ms)
    static inline uint64_t now_ms() { return GetTickCount64(); }
#else
    #include <sys/time.h>
    #include <unistd.h>
    #define sleep_ms(ms) usleep((ms)*1000)
    static inline uint64_t now_ms() {
        struct timeval tv; gettimeofday(&tv, NULL);
        return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }
#endif

static inline void run_scheduler() {
    int active;
    do {
        active = 0;
        for (int i = 0; i < g_task_count; i++) {
            if (g_task_list[i].coro.state != -1) {
                int running = g_task_list[i].func(&g_task_list[i].coro);
                if (running)
                    active++;
            }
        }
        sleep_ms(100);
    } while (active > 0);
}

#endif // TASK_SCHEDULER_H

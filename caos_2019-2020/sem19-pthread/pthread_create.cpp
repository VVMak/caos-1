// %%cpp pthread_create.cpp
// %run gcc -fsanitize=thread pthread_create.cpp -lpthread -o pthread_create.exe
// %run ./pthread_create.exe

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <pthread.h>

const char* log_prefix() {
    struct timeval tp; gettimeofday(&tp, NULL);
    static __thread char prefix[100];
    size_t time_len = strftime(prefix, sizeof(prefix), "%H:%M:%S", localtime(&tp.tv_sec));
    sprintf(prefix + time_len, ".%03ld [tid=%ld]", tp.tv_usec / 1000, syscall(__NR_gettid));
    return prefix;
}

#define log_printf_impl(fmt, ...) { time_t t = time(0); dprintf(2, "%s: " fmt "%s", log_prefix(), __VA_ARGS__); }
#define log_printf(...) log_printf_impl(__VA_ARGS__, "")

// thread-aware assert
#define ta_assert(stmt) if (stmt) {} else { log_printf("'" #stmt "' failed"); exit(EXIT_FAILURE); }

// Возвращаемое значение потока (~код возврата процесса) -- любое машинное слово.
static void* thread_func(void* arg) 
{
    log_printf("  Thread func started\n");
    log_printf("  Thread func finished\n");
    return NULL;
}

int main()
{
    log_printf("Main func started\n");
    pthread_t thread;
    log_printf("Thread creating\n");
    ta_assert(pthread_create(&thread, NULL, thread_func, 0) == 0); // В какой-то момент будет создан поток и в нем вызвана функция
    // Начиная отсюда неизвестно в каком порядке выполняются инструкции основного и дочернего потока
    ta_assert(pthread_join(thread, NULL) == 0); // -- аналог waitpid. Второй аргумент -- указатель в который запишется возвращаемое значение
    log_printf("Thread joined\n");
    log_printf("Main func finished\n");
    return 0;
}


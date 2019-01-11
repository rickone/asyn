#include "asyn_override.h"
#include <cstdio>
#include <pthread.h>
#include "asyn_master.h"

ASYN_OVERRIDE(pthread_create)
int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void*(*start_routine)(void*), void* arg) {
    puts("pthread_create hooked");
    return ASYN_ORIGIN(pthread_create)(thread, attr, start_routine, arg);
}

ASYN_OVERRIDE(pthread_self)
pthread_t pthread_self() {
    puts("pthread_self hooked");
    return ASYN_ORIGIN(pthread_self)();
}

ASYN_OVERRIDE(pthread_detach)
int pthread_detach(pthread_t thread) {
    puts("pthread_detach hooked");
    return ASYN_ORIGIN(pthread_detach)(thread);
}

ASYN_OVERRIDE(pthread_join)
int pthread_join(pthread_t thread, void** retval) {
    puts("pthread_join hooked");
    return ASYN_ORIGIN(pthread_join)(thread, retval);
}

ASYN_OVERRIDE(pthread_equal)
int pthread_equal(pthread_t thread1, pthread_t thread2) {
    puts("pthread_equal hooked");
    return ASYN_ORIGIN(pthread_equal)(thread1, thread2);
}

ASYN_OVERRIDE(pthread_cancel)
int pthread_cancel(pthread_t thread) {
    puts("pthread_cancel hooked");
    return ASYN_ORIGIN(pthread_cancel)(thread);
}

ASYN_OVERRIDE(pthread_exit)
void pthread_exit(void* retval) {
    puts("pthread_exit hooked");
    ASYN_ORIGIN(pthread_exit)(retval);
}

ASYN_OVERRIDE(pthread_getschedparam)
int pthread_getschedparam(pthread_t thread, int* policy, struct sched_param* param) {
    puts("pthread_getschedparam hooked");
    return ASYN_ORIGIN(pthread_getschedparam)(thread, policy, param);
}

ASYN_OVERRIDE(pthread_setschedparam)
int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param* param) {
    puts("pthread_setschedparam hooked");
    return ASYN_ORIGIN(pthread_setschedparam)(thread, policy, param);
}
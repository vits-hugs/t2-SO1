#include "thread.h"

__BEGIN_API

int Thread::switch_context(Thread * prev, Thread * next){

    prev->context()->save();
    next->context()->load();

    return 0;
}

void Thread::thread_exit(int exit_code){
    exit_code = 50;
}



__END_API
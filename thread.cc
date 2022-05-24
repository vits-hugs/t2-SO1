#include "thread.h"

__BEGIN_API
Thread *Thread::_running = 0;
int Thread::thread_counter= 0;
int Thread::switch_context(Thread * prev, Thread * next){
    _running = next;
    CPU::switch_context(prev->context(),next->context());
    return 0;
}

void Thread::thread_exit(int exit_code){
    db<Thread>(TRC) << "tread finalizou com exit_" << exit_code;

    delete _context;
    // decrementa contador;  
}



__END_API
#include "thread.h"

__BEGIN_API
Thread *Thread::_running = 0;
int Thread::thread_counter= 0;
int Thread::switch_context(Thread * prev, Thread * next){
    _running = next;
    CPU::switch_context(prev->context(),next->context());
    db<Thread>(INF) << "Thread trocou o contexto";
    return 0;
}

void Thread::thread_exit(int exit_code){
    db<Thread>(TRC) << "tread finalizou com exit_" << exit_code;

    delete _context;
    // decrementa contador;  
}

int Thread::id(){
    return _id;
    db<Thread>(TRC) << "o id da Thread é:" << _id;
}



__END_API
#include "thread.h"

__BEGIN_API

Thread Thread::_main;
Thread Thread::_dispatcher;
CPU::Context Thread::_main_context;
Thread::Ready_Queue Thread::_ready;
Thread *Thread::_running = 0;

unsigned int Thread::thread_counter= 0;
int Thread::switch_context(Thread * prev, Thread * next){
    _running = next;

    db<Thread>(TRC) << "Thread: "<< prev->_id << "trocou com "
                    << "thread: "<< next->_id << "\n";
    if (prev != next){
        prev->_state = READY;
        CPU::switch_context(prev->context(),next->context());
    }
    return 0;
}

void Thread::thread_exit(int exit_code){
    this->_state = FINISHING;
    db<Thread>(TRC) << "tread finalizou com exit_" << exit_code << "\n";

    // decrementa contador;  
    Thread::yield();
}
int Thread::id(){
    return _id;
    db<Thread>(TRC) << "o id da Thread é:" << _id << '\n';
}

Thread::~Thread(){
    db<Thread>(INF) << "Thread "<< _id << " sendo destruida\n";
    delete _context;
    if (_id!= 0){

        _ready.remove(this);
    }
    thread_exit(0);
}
// STATIC FUNCTIONS


void Thread::dispatcher() {
    db<Thread>(TRC) << "dispatcher dispatchando\n";
    //Equanto houver mais de 2 threads, pois tem a main e a disaptcher
    /*
    while (!_ready.empty()) {
    //escolha a próxima
    
    auto ponteiro = _ready.head();
    Thread * next = ponteiro->object();
    //
   _dispatcher._state = READY;
   _ready.insert(&_dispatcher._link);
   _running = next;
   _running->_state = RUNNING;
   Thread::switch_context(&_dispatcher,_running);
    if (_ready.head()->object()->_state == FINISHING) {
        _ready.remove_head();
    }
    }
   
    _dispatcher._state = FINISHING;
    _ready.remove(&_dispatcher._link);
    //Thread::switch_context(&_dispatcher,&_main);
    */
}
// cria main e dispatcher
void Thread::init(void (*main)(void *)){
    
    new (&_main) Thread(main,(void*) "main");

    new (&_dispatcher) Thread((void(*)(void*)) &Thread::dispatcher,(void *)NULL);
    _ready.insert(&_dispatcher._link);

    new (&_main_context) CPU::Context();

    _running = &_main;
    _main._state = RUNNING;


    CPU::switch_context(& _main_context,_main.context());
    
}



void Thread::yield(){
    db<Thread>(TRC) <<  "chamando yield\n";
    
    Thread * prev = running();
    Thread * next = _ready.remove()->object();
    
    if (prev != &_main && prev != &_dispatcher && _running->_state != FINISHING ){
        db<Thread>(TRC) << "atualizando e adicionando na fila\n";
        prev->_state = READY;
        int now = (std::chrono::duration_cast<std::chrono::microseconds>
(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
        prev->_link.rank(now);
    }
    
    if( prev != &_main){
        _ready.insert(&prev->_link);
    }

    _running = next;

    next->_state = RUNNING;

    switch_context(prev,next);
    
}


__END_API
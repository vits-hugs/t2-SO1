#include "thread.h"

__BEGIN_API

Thread Thread::_main;
Thread Thread::_dispatcher;
CPU::Context Thread::_main_context;
Thread::Ready_Queue Thread::_ready;
Thread *Thread::_running = 0;

unsigned int Thread::thread_counter;
int Thread::switch_context(Thread * prev, Thread * next){
    _running = next;

    if (prev->_id==1){
         db<Thread>(TRC) << "Thread "<< "Dispatcher" << " trocou com "
                    << "thread: "<< next->_id << "\n";
    }
    else if (next->_id==1)
    {
    db<Thread>(TRC) << "Thread: "<< prev->_id << " trocou com "
                    << "thread "<< "Dispatcher" << "\n";

    }
    
    if (prev != next){
        prev->_state = READY;
        CPU::switch_context(prev->context(),next->context());
    }
    return 0;
}

void Thread::thread_exit(int exit_code){
    this->_state = FINISHING;
    db<Thread>(TRC) << "thread: "<<this->_id << " FINALIZADA com exit_" << exit_code << "\n";

    // decrementa contador; 

    thread_counter--; 
    //printf("thread %d exitando | counter = %d\n",this->_id,thread_counter);

    Thread::yield();
}
int Thread::id(){
    return _id;
    db<Thread>(TRC) << "o id da Thread é:" << _id << '\n';
}

Thread::~Thread(){
    db<Thread>(TRC) << "Thread "<< _id << " sendo destruida\n";
    delete _context;

        
    _ready.remove(this);
    
    thread_exit(0);
}
// STATIC FUNCTIONS


void Thread::dispatcher() {
     while (thread_counter > 2) {
    //escolha a próxima
    
    auto ponteiro = _ready.head();
    Thread * next = ponteiro->object();
    
   _dispatcher._state = READY;

    int now = (std::chrono::duration_cast<std::chrono::microseconds>
    (std::chrono::high_resolution_clock::now().time_since_epoch()).count());
   _ready.remove(&_dispatcher);
   _dispatcher._link.rank(now);
   _ready.insert(&_dispatcher._link);
   _running = next;
   next->_state = RUNNING;
   Thread::switch_context(&_dispatcher,next);
    if (_ready.head()->object()->_state == FINISHING) {
        _ready.remove_head();
    }

    }
   
    printf("Dispatcher sendo finalizada e indo pro main");
    db<Thread>(TRC) << "Dispatcher sendo finalizada e indo pro main";
    _dispatcher._state = FINISHING;
    _ready.remove(&_dispatcher._link);
    Thread::switch_context(&_dispatcher,&_main);

    
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
    
    if (prev != &_main && prev != &_dispatcher && prev->_state != FINISHING ){
        db<Thread>(TRC) << "atualizando e adicionando na fila\n";
        prev->_state = READY;
        int now = (std::chrono::duration_cast<std::chrono::microseconds>
(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
        prev->_link.rank(now);
    }
    
    if( prev != &_main ){
        _ready.insert(&prev->_link);
    }

    _running = next;

    next->_state = RUNNING;

    switch_context(prev,next);
    
}


__END_API
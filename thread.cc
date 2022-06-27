#include "thread.h"

__BEGIN_API

Thread Thread::_main;
Thread Thread::_dispatcher;
CPU::Context Thread::_main_context;
Thread::Ready_Queue Thread::_ready;
Thread::Ready_Queue Thread::_suspend;
Thread::Ready_Queue Thread::_wait_queue;
Thread *Thread::_running;
Thread *Thread::_joining = 0;

unsigned int Thread::thread_counter;
int Thread::switch_context(Thread * prev, Thread * next){

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
        CPU::switch_context(prev->context(),next->context());
        prev->_state = READY;
    }
    return 0;
}

void Thread::thread_exit(int exit_code){
    this->_state = FINISHING;
    db<Thread>(TRC) << "thread: "<<this->_id << "chamou exit()\n";

    if (!_suspend.empty()) _suspend.head()->object()->resume();
    
    Thread::yield();
}
int Thread::id(){
    return _id;
    db<Thread>(TRC) << "o id da Thread é:" << _id << '\n';
}

Thread::~Thread(){
    db<Thread>(TRC) << "Thread "<< _id << " sendo destruida\n";
    thread_counter--;
    _ready.remove(this);
    delete _context;

}
// STATIC FUNCTIONS


void Thread::dispatcher() {
    while (_ready.size() > 2) {
    //escolha a próxima
    
    Thread * next = _ready.remove()->object();
    
   _dispatcher._state = READY;

   int now = (std::chrono::duration_cast<std::chrono::microseconds>
   (std::chrono::high_resolution_clock::now().time_since_epoch()).count());

   _dispatcher._link.rank(now);
   _ready.insert(&_dispatcher._link);
   _running = next;
   next->_state = RUNNING;
   Thread::switch_context(&_dispatcher,next);

    }

    db<Thread>(TRC) << "Dispatcher sendo finalizada e indo pro main";
    _dispatcher._state = FINISHING;
    _ready.remove(&_dispatcher._link);
    Thread::switch_context(&_dispatcher,&_main);

    
}
// cria main e dispatcher
void Thread::init(void (*main)(void *)){
    db<Thread>(TRC) << "inicializando main e dispatcher\n";
    new (&_main) Thread(main,(void*) "main");

    new (&_dispatcher) Thread((void(*)(void*)) &Thread::dispatcher,(void *)NULL);
    _ready.insert(&_dispatcher._link);

    new (&_main_context) CPU::Context();

    _running = &_main;
    _main._state = RUNNING;


    CPU::switch_context(&_main_context,_main.context());
    
}



void Thread::yield(){
    db<Thread>(TRC) <<  "chamando yield\n";
    
    Thread * prev = running();
    Thread * next = _ready.remove()->object();
    
    int now = std::chrono::duration_cast<std::chrono::microseconds>
    (std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    if (prev->_state != FINISHING && prev->id() > 1) {

        prev->_link.rank(now);
        prev->_state = READY;
        _ready.insert(&prev->_link);
    }
    db<Semaphore>(TRC) << "rodando: " << prev->_state << "\n";
    _running = next;

    next->_state = RUNNING;

    switch_context(prev,next);
    
}

void Thread::suspend() {
    db<Thread>(TRC) << "Thread: " << this->_id << "entrando na fila de suspensos\n";
    this->_state = SUSPENDED;
    if (this != &_main){
        _ready.remove(&this->_link);
    }
    _suspend.insert(&this->_link);
    if (_running == this){
        yield();
    }
}

int Thread::join() {
    db<Thread>(TRC) << "Thread: " << this->_id << " chamou join\n";
    if (this->_state != FINISHING){
        _running->suspend();
    }
    return _running->exit_code++;
    
}

void Thread::resume() {
    db<Thread>(TRC) << "Thread: " << this->_id << "saindo da fila de suspensos\n";
    if (this->_state == SUSPENDED){
    this->_state = READY;
    _suspend.remove(&this->_link);
    _ready.insert(&this->_link);
    }
    db<Thread>(WRN) << "Não há threads na fila de suspensos\n";
}

// SEM FUNCTIONS

// talvez passar ponteiro para semaforo para ficar na fila dele
// Fila de wait é global ou é própria do semaforo
void Thread::sleep() {
    if (_running != &_main){
        _running->_state = WAITING;
        _ready.remove(&_running->_link);
    int now = std::chrono::duration_cast<std::chrono::microseconds>
    (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    _running->_link.rank(now);
    _wait_queue.insert(&_running->_link);
    }
    if (_running->_state == WAITING) yield();
    
}

void Thread::wake() {
    db<Semaphore>(TRC) << "wake\n";
    Thread * to_wake = Thread::_wait_queue.remove()->object();
    db<Thread>(TRC) << "Thread: " << to_wake->_id << "wake\n";
    if (to_wake->_state == WAITING){
    to_wake->_state = READY;
    _wait_queue.remove(&to_wake->_link);
    _ready.insert(&to_wake->_link);
    }
    db<Thread>(WRN) << "Não há threads na fila de suspensos\n";

}
__END_API
#include "thread.h"

__BEGIN_API

Thread Thread::_main;
Thread Thread::_dispatcher;
CPU::Context Thread::_main_context;
Thread::Ready_Queue Thread::_ready;
Thread::Ready_Queue Thread::_suspend;
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
        db<Thread>(TRC) << "switch\n";
        if (prev->_state != SUSPENDED) {
        prev->_state = READY;
        }
        //_running = next;
        //next->_state = RUNNING;
        CPU::switch_context(prev->context(),next->context());
        db<Thread>(TRC) << "volta\n";
   
    }
    return 0;
}

void Thread::thread_exit(int exit_code){
    this->_state = FINISHING;
    if (this->_sem_queues) {
        this->_sem_queues->remove(this);
    }
    db<Thread>(TRC) << "thread: "<<this->_id << " chamou exit()\n";

    
    if (!_suspend.empty()) _suspend.head()->object()->resume();
    
    Thread::yield();
}
int Thread::id(){
    return _id;
    db<Thread>(TRC) << "o id da Thread é:" << _id << '\n';
}

Thread::~Thread(){
    db<Thread>(TRC) << "Thread "<< _id << " sendo destruida\n";
    // talvez deixar ou n o thread_counter--;
    _ready.remove(this);
    delete _context;

}
// STATIC FUNCTIONS


void Thread::dispatcher() {
     db<Thread>(TRC) << "dispatcher\n";
    while (thread_counter > 2) {
    //escolha a próxima
    
        Thread * next = _ready.remove()->object();
        db<Thread>(TRC) << "next="<<next << "\n";
       _dispatcher._state = READY;

       int now = (std::chrono::duration_cast<std::chrono::microseconds>
       (std::chrono::high_resolution_clock::now().time_since_epoch()).count());
       _dispatcher._link.rank(now);
        
       
       _ready.insert(&_dispatcher._link);
       _running = next;
       Thread::switch_context(&_dispatcher,next);
        if (next->_state == FINISHING) {
            _ready.remove(&next->_link);
            thread_counter --;
        }
       next->_state = RUNNING;
    }

    db<Thread>(TRC) << "Dispatcher sendo finalizada e indo pro main \n";
    _dispatcher._state = FINISHING;
    _ready.remove(&_dispatcher._link);
    thread_counter --;
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
    Thread * next = _ready.remove()->object();;
    // não deveria precisar if (!_ready.empty()) {next= }
    
  
    if (prev->_state != FINISHING && prev->id() > 1) {

        int now = (std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count());
        prev->_link.rank(now);
        prev->_state = READY;
        
    }
    if (prev->_id > 0 && prev->_state != FINISHING) {
        db<Thread>(TRC) << "Thread: " << prev->id() << "insert();\n";
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
    db<Thread>(TRC) << "Thread: " << this->id() << "suspendo;\n";
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
    if (this->_state == SUSPENDED  /*|| this == &_main*/){
    this->_state = READY;
    _suspend.remove(&this->_link);

    db<Thread>(TRC) << "Thread: " << this->id() << "insert();\n";
    _ready.insert(&this->_link);
    
    }
    db<Thread>(WRN) << "Não há threads na fila de suspensos\n";
}

// SEM FUNCTIONS

// talvez passar ponteiro para semaforo para ficar na fila dele
// Fila de wait é global ou é própria do semaforo
void Thread::sleep(Ready_Queue * Fila_sem) {
    Thread * run = running();      
    run->_state = WAITING;
    run->_sem_queues = Fila_sem;
     db<Thread>(TRC) << "Thread: " << run->id() << "Fila_sem;\n";
    Fila_sem->insert(&run->_link);
    Thread * next = _ready.remove()->object();
    next->_state = RUNNING;
    _running = next;
    switch_context(run,next);
}

void Thread::wake(Ready_Queue * Fila_sem) {
    db<Semaphore>(TRC) << "wake\n";
   if(!Fila_sem->empty()) {
   Thread * to_wake = Fila_sem->remove()->object();
   to_wake->_sem_queues = nullptr;
   to_wake->_state = READY;
    db<Thread>(TRC) << "Thread: " << to_wake->id() << "insert();\n";
   _ready.insert(&to_wake->_link);
   }
   yield();
}

void Thread::wakeup_all(Ready_Queue * Fila_sem) {
    while (!Fila_sem->empty()) {
    Thread * to_wake = Fila_sem->remove()->object();
    to_wake->_sem_queues = nullptr;
    to_wake->_state = READY;
    _ready.insert(&to_wake->_link);
   }
   yield();
}
__END_API

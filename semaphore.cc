#include "semaphore.h"

__BEGIN_API 
Semaphore::Semaphore(int d) {
    db<Semaphore>(TRC) << "Semaforo criado \n";
    _v = d;
    db<Semaphore>(TRC) <<"atribute v: "<< _v << "\n"; 
}

Semaphore::~Semaphore() {
    db<Semaphore>(TRC) << "morri \n";
}

// Implementar como se fosse um mutex primeiro

void Semaphore::p() {
    db<Semaphore>(TRC) << "thread: " << Thread::running()->id() << ".p()\n"; 
    while (_v == 0) {
        db<Semaphore>(TRC) << "sleepo" << _v << "\n"; 
        sleep();
        Thread::yield();
    }
    fdec(_v);
    db<Semaphore>(TRC) << "p -> _v = " << _v << "\n"; 

}

void Semaphore::v() {
    finc(_v);
    db<Semaphore>(TRC) << "thread: " << Thread::running()->id() << " .v() \n";
    
    wakeup();
    db<Semaphore>(TRC) << "v -> _v = " << _v << "\n"; 
}

// Funções pra threads
// chama sleep da thread atual
void Semaphore::sleep() {
    db<Semaphore>(TRC) << "thread: " << Thread::running()->id() << ".sleep()\n";
    Thread::sleep(/* passar fila */);
}

// chama wake da thread atual
void Semaphore::wakeup() {

   Thread * to_wake = _wait_queue.remove()->object();
   db<Semaphore>(TRC) << "Thread: "<<to_wake->id() << "removida da fila de waiting";
   Thread::wake(/* passar fila */);
}

//acho que é no caso de destruir o semaforo
void Semaphore::wakeup_all() {
    Thread::wakeup_all(/* passar fila*/);.
    // itera pela fila e tira da wake em todos
}

//Operações atomicas
// Fazer utilizando assembly prof deu os links
// Incrementa
int Semaphore::finc(volatile int & number) {
    int i = 1;
    __asm__ __volatile__ ( "lock ; xadd %0, %1;": "=r"(i) : "m"(number), "0" (i) : "memory");
    return number;
}



// Decrementa 
int Semaphore::fdec(volatile int & number) {
    int i = -1;
    __asm__ __volatile__ ( "lock ; xadd %0, %1;": "=r"(i) : "m"(number), "0" (i) : "memory");
    return number;
}

__END_API

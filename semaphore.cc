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
    db<Semaphore>(TRC) << "1: p -> v : " << _v << "\n"; 
    if (_v == 0) {
        db<Semaphore>(TRC) << "thread: " << Thread::running()->id() << " a mimir \n";

        Thread::sleep();
    }
    fdec(_v);
    db<Semaphore>(TRC) << "2: p -> v : " << _v << "\n"; 

}

void Semaphore::v() {
    
    finc(_v);
    db<Semaphore>(TRC) << "thread: " << Thread::running()->id() << " a mimir \n";
    Thread::wake();
    db<Semaphore>(TRC) << "v -> v : " << _v << "\n"; 
}

// Funções pra threads
// chama sleep da thread atual
void Semaphore::sleep() {

}

// chama wake da thread atual
void Semaphore::wakeup() {
  
}

//acho que é no caso de destruir o semaforo
void Semaphore::wakeup_all() {
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
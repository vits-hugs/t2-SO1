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
   

    if (CPU::fdec(_v) < 0) {
        sleep();
    }
    db<Semaphore>(TRC) << "p -> _v = " << _v << "\n"; 

}

void Semaphore::v() {
  
    db<Semaphore>(TRC) << "thread: " << Thread::running()->id() << " .v() \n";
    if (CPU::finc(_v) < 1) {
        wakeup();
    }

    db<Semaphore>(TRC) << "v -> _v = " << _v << "\n"; 
}

// Funções pra threads
// chama sleep da thread atual
void Semaphore::sleep() {
    db<Semaphore>(TRC) << "thread: " << Thread::running()->id() << ".sleep()\n";
    Thread::sleep(&this->_wait_queue);
}

// chama wake da thread atual
void Semaphore::wakeup() {
   Thread::wake(&this->_wait_queue);
}

//acho que é no caso de destruir o semaforo
void Semaphore::wakeup_all() {
    Thread::wakeup_all(&this->_wait_queue);
    // itera pela fila e tira da wake em todos
}

//Operações atomicas
// Fazer utilizando assembly prof deu os links
// Incrementa
int Semaphore::finc(volatile int & number) {
    return CPU::finc(number);
}



// Decrementa 
int Semaphore::fdec(volatile int & number) {
    return CPU::fdec(number);
}

__END_API

#include "semaphore.h"

__BEGIN_API 
Semaphore::Semaphore(int) {
    db<Semaphore>(TRC) << "Semaforo criado \n";

}

Semaphore::~Semaphore() {
    db<Semaphore>(TRC) << "morri \n";
}

// Implementar como se fosse um mutex primeiro

void Semaphore::p() {
    db<Semaphore>(TRC) << "mimir \n";
    printf("Sleep \n");
}

void Semaphore::v() {
    db<Semaphore>(TRC) << "acorda \n";
    printf("Acorda \n");
}

// Funções pra threads
void Semaphore::sleep() {

}

void Semaphore::wakeup() {

}

void Semaphore::wakeup_all() {

}

//Operações atomicas
// Fazer utilizando assembly prof deu os links
// Incrementa
int Semaphore::finc(volatile int & number) {

}
// Decrementa 
int Semaphore::fdec(volatile int & number) {

}

__END_API
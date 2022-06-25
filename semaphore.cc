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
    /*
    if (_v == 0) {

        _running.sleep();
        
    }
    */
    fdec(_v);
    printf("%d",_v);

    printf("Sleep \n");
}

void Semaphore::v() {
    db<Semaphore>(TRC) << "acorda \n";
    printf("Acorda \n");
    finc(_v);
    printf("%d",_v);
    // pega primeira da fila do waiting e acorda
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
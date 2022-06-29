#ifndef semaphore_h
#define semaphore_h

#include "cpu.h"
#include "thread.h"
#include "traits.h"
#include "debug.h"
#include "list.h"

__BEGIN_API

class Semaphore
{
public:
    typedef Ordered_List<Thread> Ready_Queue;
    Semaphore(int _v = 1);
    ~Semaphore();

    void p();
    void v();
    Ready_Queue _wait_queue;
private:
    // Atomic operations
    int finc(volatile int & number);
    int fdec(volatile int & number);

    // Thread operations
    void sleep();
    void wakeup();
    void wakeup_all();

private:
    //DECLARAÇÃO DOS ATRIBUTOS DO SEMÁFORO
    //adicionar fila de threads  
    int _v{1};
};

__END_API

#endif


#ifndef thread_h
#define thread_h

#include <iostream>
#include "cpu.h"
#include "traits.h"
#include "debug.h"

__BEGIN_API
class Thread
{
protected:
    typedef CPU::Context Context;

public:
    /*
     * Cria uma Thread passando um ponteiro para a função a ser executada
     * e os parâmetros passados para a função, que podem variar.
     * Cria o contexto da Thread.
     * PS: devido ao template, este método deve ser implementado neste mesmo arquivo .h
     */ 
    template<typename ... Tn>
    Thread(void (* entry)(Tn ...), Tn ... an){ 
        _id = Thread::thread_counter++;
        db<Thread>(INF) << "Thread criada com id:"<<_id;
        _context = new Context( entry,an...);
    }

    /*
     * Retorna a Thread que está em execução.
     */ 
    static Thread * running() {
        return _running; }

    /*
     * Método para trocar o contexto entre duas thread, a anterior (prev)
     * e a próxima (next).
     * Deve encapsular a chamada para a troca de contexto realizada pela class CPU.
     * Valor de retorno é negativo se houve erro, ou zero.
     */ 
    static int switch_context(Thread * prev, Thread * next);

    /*
     * Termina a thread.
     * exit_code é o código de término devolvido pela tarefa (ignorar agora, vai ser usado mais tarde).
     * Quando a thread encerra, o controle deve retornar à main. 
     */  
    void thread_exit (int exit_code);
        /*
        da um switch com o main
        e ai retorna ext_code
        */
        
  

    /*
     * Retorna o ID da thread.
     */
    int id();

    /*
     * Qualquer outro método que você achar necessário para a solução.
     */ 
    Context* context(){
        return _context;
    }

private:
    int _id;
    Context * volatile _context;
    static Thread * _running;
    static int thread_counter;

    /*
     * Qualquer outro atributo que você achar necessário para a solução.
     */ 
};

__END_API

#endif

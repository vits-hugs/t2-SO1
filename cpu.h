#ifndef cpu_h
#define cpu_h

#include <ucontext.h>
#include <iostream>
#include "traits.h"

__BEGIN_API

class CPU
{
    public:

        class Context
        {
        private:
            static const unsigned int STACK_SIZE = Traits<CPU>::STACK_SIZE;
            static const unsigned int CONTEXT_STACK_SIZE = Traits<CPU>::CONTEXT_STACK_SIZE;
        public:
            Context() { _stack = 0; }

            template<typename ... Tn>
            Context(void (* func)(Tn ...), Tn ... an) {
                save();
                _stack = new char[CONTEXT_STACK_SIZE];
                _context.uc_link = NULL;
                _context.uc_stack.ss_flags = 0;
                _context.uc_stack.ss_sp = _stack;
                _context.uc_stack.ss_size = CONTEXT_STACK_SIZE;
                makecontext(&_context,(void(*) (void)) func,sizeof...(Tn),an...);


            }

            ~Context();

            void save();
            void load();


        private:            
            char *_stack;
        public:
            ucontext_t _context;
        };

    public:

        static void switch_context(Context *from, Context *to);
        static int finc(volatile int & number);
        static int fdec(volatile int & number);

};

__END_API

#endif


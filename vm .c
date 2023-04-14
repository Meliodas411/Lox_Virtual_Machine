#include <stdio.h>
#include "common.h"
#include "compiler.h"
#include "vm.h"
#include "debug.h"

VM vm;

// ===================================
static InterpretResult run(void);
static void resetStack();
InterpretResult interpret(const char* source);
// ===================================

void initVM() {
    resetStack();
};

static void resetStack() {
    //  将stackTop 指向 stack起始位置
  vm.stackTop = vm.stack;
}

void freeVM() {
};
// 
InterpretResult interpret(const char* source) {

    Chunk chunk;
    initChunk(&chunk);
    if(!compile(source,&chunk)){
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }
    vm.chunk = &chunk;
    vm.ip = vm.chunk -> code;
    initChunk(&chunk);
    compile(source, &chunk);
    InterpretResult result  = run();
    free(&chunk);
    return result;
  };

void push(Value value) {
  *vm.stackTop = value;
  vm.stackTop++;
};

Value pop(){
    vm.stackTop--;
    return *vm.stackTop;
}

static InterpretResult run(){
    #define READ_BYTE() (*vm.ip++)
    #define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    #define BINARY_OP(op) \
            do { \
            double b = pop(); \
            double a = pop(); \
            push(a op b); \
            } while (false)
    for (;;){

    #ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
        printf("[ ");
        printValue(*slot);
        printf(" ]");
    }
    printf("\n");
   disassembleInstruction(vm.chunk,
                          (int)(vm.ip - vm.chunk->code));
    #endif
        uint8_t instruction;

        switch (instruction = READ_BYTE()){
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                printValue(constant);
                printf("\n");
                break;
      }
            case OP_ADD:      BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE:   BINARY_OP(/); break;
            case OP_NEGATE:   push(-pop()); break;

            case OP_RETURN:{
                printValue(pop());
                printf("\n");
                return INTERPRET_OK;
            }

            
             
        }
    }
    // 下面不再用到这个宏
    #undef READ_BYTE
    #undef READ_CONSTANT
    #undef BINARY_OP
}


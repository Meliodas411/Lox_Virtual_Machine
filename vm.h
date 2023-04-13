#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "value.h"


#define STACK_MAX 256
typedef struct {

    Chunk* chunk;
    // 记录执行位置
    uint8_t* ip;
    Value stack[STACK_MAX];
    Value* stackTop;
} VM;

// 定义虚拟机运行状态
typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(const char* source);
void push(Value value);
Value pop();
#endif

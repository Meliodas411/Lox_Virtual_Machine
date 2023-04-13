#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN,
} OpCode;

typedef struct {
    int count;
    int capacity;
    uint8_t* code;
    // 数组中的每个数字都是字节码中对应字节所在的行号
    int* lines;
    ValueArray constants;
} Chunk;

// 初始化Chunk
void initChunk(Chunk *chunk);
// 写入Chunk
void writeChunk(Chunk* chunk, uint8_t byte,int line);
// 释放chunk
void freeChunk(Chunk* chunk);
// 增加常量
int addConstant(Chunk* chunk, Value value);
#endif
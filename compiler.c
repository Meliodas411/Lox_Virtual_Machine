#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

// ===============================first define function
bool compile(const char* source, Chunk* chunk);
static void errorAtCurrent(const char* message);
static void error(const char* message);
static void errorAt(Token* token,const char* message);
static void consume(TokenType type,const char* message);
static void emitByte(uint8_t byte);
static void advance();
static Chunk* currentChunk();
static void endCompiler();
static void emitReturn();
static void emitBytes(uint8_t byte1, uint8_t byte2);
static void expression();
static void number();
static void emitConstant(Value value);
static uint8_t makeConstant(Value Value);
// =============================== end define function

// parser 解析
typedef struct {
  Token current;
  Token previous;
  bool hadError;
  bool panicMode;
} Parser;

Parser parser;
Chunk* compilingChunk;

bool compile(const char* source, Chunk* chunk) {
    initScanner(source);
    compilingChunk = chunk;
    // init the hadError and panicMode
    parser.hadError = false;
    parser.panicMode = false;
    advance();
    expression();
    consume(TOKEN_EOF, "“Expect end of expression.");
    endCompiler();
    // 返回compile是否有bug
    return !parser.hadError;
};

static void expression(){

};

static void number(){
  double value = strtod(parser.previous.start,NULL);
  emitConstant(value);
};

static void emitConstant(Value value){
  emitBytes(OP_CONSTANT,makeConstant(value));
};

// 判断Value范围，是否符合规范
static uint8_t makeConstant(Value Value){
  return 0;
};



// add the consume
static void consume(TokenType type,const char* message){
  if(parser.current.type == type){
    advance();
    return;
  };

  errorAtCurrent(message);
}


static void emitByte(uint8_t byte){
  writeChunk(currentChunk(),byte,parser.previous.line);
};




static void endCompiler() {
  emitReturn();
};

static void emitReturn(){
  emitByte(OP_RETURN);
};

// 连续写入, 比如字节码里是 const_枚举 数字，连续写入两次
static void emitBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}


static Chunk* currentChunk() {
  return compilingChunk;
};




static void advance(){
  parser.previous = parser.current;
  // 跳过错误TOKEN并做报告
  for (;;) {
    parser.current = scanToken();
    if (parser.current.type != TOKEN_ERROR) break;
    errorAtCurrent(parser.current.start);
  }
  
};





static void errorAtCurrent(const char* message) {
  errorAt(&parser.current, message);
};




static void error(const char* message){
  errorAt(&parser.previous,message);
};

static void errorAt(Token* token,const char* message){
  // recovery to right code if ;
  if (parser.panicMode == true) return;
  parser.panicMode = true;
  // stderr
  fprintf(stderr, "[line %d] Error", token->line);
  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token->type == TOKEN_ERROR) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", message);
  // 报告是否有bug
  parser.hadError = true;
}
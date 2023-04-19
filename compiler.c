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
static void expression();
// ===============================

// parser 解析
typedef struct {
  Token current;
  Token previous;
  bool hadError;
  bool panicMode;
} Parser;

Parser parser;

bool compile(const char* source, Chunk* chunk) {
    initScanner(source);
    // init the hadError and panicMode
    parser.hadError = false;
    parser.panicMode = false;
    advance();
    expression();
    consume(TOKEN_EOF, "“Expect end of expression.");
    // 返回compile是否有bug
    return !parser.hadError;
};

static void expression(){

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

}

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
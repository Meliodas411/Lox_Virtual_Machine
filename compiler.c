#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

// ===============================first define function
bool compile(const char* source, Chunk* chunk);
static void errorAtCurrent(const char* message);
static void error(const char* message);
static void errorAt(Token* token,const char* message);
// ===============================

// parser 解析
typedef struct {
  Token current;
  Token previous;
  bool hadError;
} Parser;

Parser parser;

bool compile(const char* source, Chunk* chunk) {
    initScanner(source);
    advance();
    expressiom();
    consume(TOKEN_EOF, "“Expect end of expression.");
    // 返回compile是否有bug
    return !parser.hadError;
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
  // stderr
  fprintf(stderr, "[line %d] Error", token->line);
  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token->type == TOKEN_ERROR) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  printf(stderr, ": %s\n", message);
  // 报告是否有bug
  parser.hadError = true;
}
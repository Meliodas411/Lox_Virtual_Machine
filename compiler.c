#include <stdio.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

// ===============================first define function
bool compile(const char* source, Chunk* chunk);
// ===============================

// parser 解析
typedef struct {
  Token current;
  Token previous;
} Parser;

Parser parser;

bool compile(const char* source, Chunk* chunk) {
    initScanner(source);
    advance();
    expressiom();
    consume(TOKEN_EOF, "“Expect end of expression.");
  // TODO
    return true;
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
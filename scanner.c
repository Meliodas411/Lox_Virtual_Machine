#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

// ==============================
void initScanner(const char* source);
static bool isAtEnd();
static Token makeToken(TokenType type);
static Token errorToken(const char* message);
static char advance();
static bool match(char expected);
static char peek();
static void skipWhitespace();
Token scanToken();
static Token string();
static Token number();
static bool isDigit(char c);
static bool isAlpha(char c);
static Token identifier();
static TokenType identifierType();
static TokenType checkKeyword(int start, int length,
    const char* rest, TokenType type);
// ==============================
typedef struct {
  const char* start;
  const char* current;
  int line;
} Scanner;

Scanner scanner;

void initScanner(const char* source) {
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
};

static bool isAtEnd() {
  return *scanner.current == '\0';
};


static Token makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.start = scanner.start;
  token.length = (int)(scanner.current - scanner.start);
  token.line = scanner.line;
  return token;
};

static Token errorToken(const char* message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  token.line = scanner.line;
  return token;
};

static char advance() {
  scanner.current++;
  return scanner.current[-1];
};

// 看是否满足下一个token
static bool match(char expected) {
  if (isAtEnd()) return false;
  if (*scanner.current != expected) return false;
  scanner.current++;
  return true;
};

// 查看当前curreent
static char peek() {
  return *scanner.current;
};

static char peekNext() {
  if (isAtEnd()) return '\0';
  return scanner.current[1];
};

static void skipWhitespace() {
  for (;;) {
    char c = peek();
    switch (c) {
      case ' ':
      case '\r':
      case '\t':
        advance();
        break;
        //  遇到\n 换行 line++;
      case '\n':
        scanner.line++;
        advance();
        break;
        // 注释
      case '/':
        if (peekNext() == '/') {
          // A comment goes until the end of the line.
          while (peek() != '\n' && !isAtEnd()) advance();
        } else {
          return;
        }
        break;
      default:
        return;
    }
  }
};

static Token string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') scanner.line++;
    advance();
  }

  if (isAtEnd()) return errorToken("Unterminated string.");

  // The closing quote.
  advance();
  return makeToken(TOKEN_STRING);
};

static bool isDigit(char c) {
  return c >= '0' && c <= '9';
};

static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
          c == '_';
};
static Token number() {
  while (isDigit(peek())) advance();

  // Look for a fractional part.
  if (peek() == '.' && isDigit(peekNext())) {
    // Consume the ".".
    advance();

    while (isDigit(peek())) advance();
  }

  return makeToken(TOKEN_NUMBER);
};


static Token identifier() {
  while (isAlpha(peek()) || isDigit(peek())) advance();
  return makeToken(identifierType());
};


static TokenType identifierType() {

  switch (scanner.start[0]) {
    // check 处理起来比较麻烦
    case 'a': return checkKeyword(1, 2, "nd", TOKEN_AND);
    case 'c': return checkKeyword(1, 4, "lass", TOKEN_CLASS);
    case 'e': return checkKeyword(1, 3, "lse", TOKEN_ELSE);
    case 'f':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
          case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
          case 'u': return checkKeyword(2, 1, "n", TOKEN_FUN);
        }
      };
      break;
    case 'i': return checkKeyword(1, 1, "f", TOKEN_IF);
    case 'n': return checkKeyword(1, 2, "il", TOKEN_NIL);
    case 'o': return checkKeyword(1, 1, "r", TOKEN_OR);
    case 'p': return checkKeyword(1, 4, "rint", TOKEN_PRINT);
    case 'r': return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    case 's': return checkKeyword(1, 4, "uper", TOKEN_SUPER);
    case 't':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
          case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
        }
      }
      break;
    case 'v': return checkKeyword(1, 2, "ar", TOKEN_VAR);
    case 'w': return checkKeyword(1, 4, "hile", TOKEN_WHILE);
  };
  return TOKEN_IDENTIFIER;
};

// TODO
static TokenType checkKeyword(int start, int length,
    const char* rest, TokenType type){
      // memcmp 将两个缓冲区的字符进行比较
      // 比较到 返回的字符是不是等于 我们期望的字符
      // 如果是，就返回我们期望的type，入如果不是，就返回TOKEN_IDENTIFIER;
      if(scanner.current - scanner.start==start + length && 
          memcmp(scanner.start + start, rest, length)){
        return type;}

      return TOKEN_IDENTIFIER;

    };


Token scanToken(){
    skipWhitespace();
    // 方便取到 curreent - start;
    scanner.start = scanner.current;
    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();
    if (isDigit(c)) return number();
    if (isAlpha(c)) return identifier();
    switch (c) {
        case '(': return makeToken(TOKEN_LEFT_PAREN);
        case ')': return makeToken(TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TOKEN_LEFT_BRACE);
        case '}': return makeToken(TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TOKEN_SEMICOLON);
        case ',': return makeToken(TOKEN_COMMA);
        case '.': return makeToken(TOKEN_DOT);
        case '-': return makeToken(TOKEN_MINUS);
        case '+': return makeToken(TOKEN_PLUS);
        case '/': return makeToken(TOKEN_SLASH);
        case '*': return makeToken(TOKEN_STAR);
        // lookkhead;
        case '!':
          return makeToken(
              match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
          return makeToken(
              match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '"': return string();

        case '<':
          return makeToken(
              match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
          return makeToken(
              match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

    }


    return errorToken("Unexpected character.");
};
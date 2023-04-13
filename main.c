#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "chunk.h"
#include "stdio.h"
#include "debug.h"
#include "vm.h"
// =======================
static void runFile(const char* path);
static void repl();
static char* readFile(const char* path);





int main(int argc, const char* argv[]) {
    // 启动虚拟机
    initVM();
    if (argc == 1){
        repl();
    }else if (argc == 2){
        runFile(argv[1]);
    }else {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
  }

    freeVM();

    // 调用反汇编器进行测试代码是否执行

    return 0;
};


static void repl(){
    char line[1024];
    for(;;){
        printf("> ");
        //   如果不能通过 一般输出流 stdin一样读入的话，就报错。
        if (!fgets(line, sizeof(line), stdin)) {
        printf("\n");
        break;
        }
        interpret(line);    
    }
};

static void runFile(const char* path){
    char* source = readFile(path);
    InterpretResult result = interpret(source);
    free(source);
    // 判断是否 犯错，以不同的形式进行推出
    //  INTERPRET_OK,
    //INTERPRET_COMPILE_ERROR,
    //INTERPRET_RUNTIME_ERROR
    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
};

static char* readFile(const char* path){
    FILE* file = fopen(path, "rb");
    // 如果无法读入文件，就报错。
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
};
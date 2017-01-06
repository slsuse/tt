#include "tt.h"
#include <string.h>
#include <stdio.h>

typedef enum args{
  help,
  ls,
  lsr,
  add,
  rm
} args_t;

args_t args(int argc, char** argv){
  if(argc == 0)
    return help;

  if(0 == strcmp("-h", argv[1]))
    return help;

  if(0 == strcmp("--help", argv[1]))
    return help;

  if(0 == strcmp("h", argv[1]))
    return help;

  if(0 == strcmp("help", argv[1]))
    return help;

  if(0 == strcmp("ls", argv[1]))
    return ls;

  if(0 == strcmp("lsr", argv[1]))
    return lsr;

  if(0 == strcmp("add", argv[1]))
    return add;
  
  if(0 == strcmp("rm", argv[1]))
    return rm;
}

void pr_help(const char* name){
  printf("%s [command]\n", name);
  printf("command is:\n");
  printf("\t[-h|--help|h|help] - print this screen.\n");
  printf("\tls - list registered projects.\n");
  printf("\tlsr - list recursively, i.e. all data down to clocked times.\n");
  printf("\tadd - register a project.\n");
  printf("\trm - rm a project.\n");
            
}

int main(int argc, char** argv){
  
  switch(args(argc, argv)){
  case help:
    pr_help(argv[0]);
    break;
  case ls:
    break;
  case lsr:
    break;
  case add:
    break;
  case rm:
    break;
  case default:
    pr_help(argv[0]);
    break;
  }

  return 0;
}

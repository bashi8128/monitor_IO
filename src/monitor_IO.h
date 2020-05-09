/*
 * Author: Masahiro Itabashi <masahiro.itabashi@hpe.com>
 * Last modified: Fri, 08 May 2020 11:13:34 +0900
 */
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

char* chomp( char* str );
void print_help();
void terminate_io(int signum);

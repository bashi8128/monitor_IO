/*
 * Author: Masahiro Itabashi <itabasi.lm@gmail.com>
 * Last modified: Sun, 10 May 2020 08:38:56 +0900
 */
#include "monitor_IO.h"

volatile sig_atomic_t eflag = 0;

int main(int argc, char* argv[]){
  // Declare and initialize variables abount file I/O
  char *input = NULL;
  char *output = NULL;
  long long bit_size = 256;
  int interval = 1;
  int rnum = 0;

  // Declare and initialize variables to look up system time
  struct timeval now;
  struct tm *local;
  double last_sec, now_sec;

  // Declare and initialize variables to handle arguments
  int opt = 0;
  extern char *optarg;
  char *endptr = NULL;

  // Declare and initialize variables for interruption
  struct sigaction action_for_sigint;
  memset(&action_for_sigint, 0, sizeof(action_for_sigint));

  // Declare and initialize variables used for statistics
  int write_size = 0;
  int errcnt = 0;
  int iocnt = 0;
  double time_spent_in_write = 0;
  double tsum = 0;
  double tmin = 999999999.0;
  double tmax = 0;

  // Variable for error flag
  int errflag = 0;

  // Handle arguments
  while((opt = getopt(argc, argv, "i:o:s:t:h")) != -1){
    switch(opt){
      case 'i':
	input = (char*)malloc(sizeof(char) * sizeof(optarg));
	strcpy(input, optarg);
	break;
      case 'o':
	output = (char*)malloc(sizeof(char) * sizeof(optarg));
	strcpy(output, optarg);
	break;
      case 's':
	bit_size = strtoll(optarg, &endptr, 10);
	if(*endptr){
	  printf("Unable to convert %s to integer.\n", optarg);
	  printf("The specified value suppose to be too big or to contain ivalid character.");
	  printf("The bit_size was set to default value.");
          bit_size = 2147483647;
	}
	break;
      case 't':
	interval = (int)strtol(optarg, &endptr, 10);
	if(*endptr){
	  printf("Unable to convert %s to integer.\n", optarg);
	  printf("The specified value suppose to be too big or to contain ivalid character.");
	  printf("The interval was set to default value.");
	  interval = 1;
	}
	break;
      case 'h':
	print_help();
	return 0;
      case '?':
	print_help();
	errflag = 1;
    }
  }

  if(input == NULL){
    input = (char*)malloc(sizeof(char) * 1024);
    printf("Input file or device: ");
    if(!(fgets(input, 1024, stdin))){
      printf("Failed to get your input.\n");
      errflag = 1;
    }
    chomp(input);
  }

  if(output == NULL){
    output = (char*)malloc(sizeof(char) * 1024);
    printf("Output file or device: ");
    if(!(fgets(output, 1024, stdin))){
      printf("Failed to get your input.\n");
      errflag = 1;
    }
    chomp(output);
  }

  // Open input file
  int input_fd = open(input, O_RDONLY);

  if(input_fd == -1){
    printf("Failed to open input file(%s).\n", input);
    errflag = 1;
  }

  // Open output file
  int output_fd = open(output, O_WRONLY | O_SYNC | O_CREAT, S_IRWXU);

  if(output_fd == -1){
    printf("Failed to open output file(%s).\n", output);
    errflag = 1;
  }

  // Allocate memory for buffer
  void *buf = NULL;
  buf = (char *)malloc(bit_size * sizeof(char));

  // Try block writing if errors did not occur.
  if(!errflag){
    // Read specified size of specified device or file
    if((rnum = read(input_fd, buf, bit_size)) == -1){
      printf("Failed to read %s.\n", input);
    }
    printf("Write %d Byte to %s.\n", rnum, output);

    // Set up signal handler
    action_for_sigint.sa_handler = terminate_io;
    action_for_sigint.sa_flags = SA_RESETHAND;
    sigaction(SIGINT, &action_for_sigint, NULL);

    // Print header
    printf("date,time\n");

    // Iterate until SIGINT is send
    while(!eflag){
      gettimeofday(&now, NULL);
      now_sec = (double)now.tv_sec + (double)now.tv_usec * 0.000001;
      last_sec = now_sec;
      lseek(output_fd, 0, SEEK_SET);
      if((write_size = write(output_fd, buf, rnum)) == -1){
        errcnt++;
      }
      sync();
      gettimeofday(&now, NULL);
      local = localtime(&now.tv_sec);
      now_sec = (double)now.tv_sec + (double)now.tv_usec * 0.000001;
      time_spent_in_write = now_sec - last_sec;
      printf("%d/%02d/%02d %02d:%02d:%02d,%lf\n",
             local->tm_year + 1900,
             local->tm_mon + 1,
             local->tm_mday,
             local->tm_hour,
             local->tm_min,
             local->tm_sec,
             time_spent_in_write);
      
      // Update statistics.
      iocnt++;
      tsum += time_spent_in_write;
      if(time_spent_in_write < tmin){
        tmin = time_spent_in_write;
      }
      if(time_spent_in_write > tmax){
        tmax = time_spent_in_write;
      }
      sleep(interval);
    }

    printf("\nI/O statistics\n");
    printf("%d output task was executed, %d suceeded, %d failed.\n",
           iocnt, iocnt - errcnt, errcnt);
    printf("min/avg/max = %lf/%lf/%lf\n",
            tmin, tsum / (iocnt - errcnt), tmax);
  }

  close(input_fd);
  close(output_fd);

  free(input);
  free(output);
  free(buf);

  return 0;
}

char* chomp(char* str){
  int len = strlen(str);
  if(len > 0 && str[len-1] == '\n'){
    str[len-1] = '\0';
  }
  return str;
}

void print_help(){
  printf("Usage: monitorIO [-i inputfile] [-o outputfile]\n");
  printf("                 [-s blocksize] [-t interval] [-h]\n");
}

void terminate_io(int signum){
  eflag = 1;
}

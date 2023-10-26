// Authors: Maize Booker & Khloe Wright

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_index 128

char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;
pid_t pid;
pid_t child;

int main() {
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
  
    // Stores the tokenized command line input.
    char *arguments[MAX_COMMAND_LINE_index];

		long size;
		char *buf;
		char *ptr;
		int index;

		size = pathconf(".", _PC_PATH_MAX);
    	
    while (true) {
      
        do{ 
            // Print the shell prompt.
						// 0. Modify the prompt to print the current working directory
						printf("%s%s", getcwd(buf, (size_t)size), prompt);
						fflush(stdout);

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
        
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }
 
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        // TODO:
        // 1. Tokenize the command line input (split it on whitespace)
				arguments[0] = strtok(command_line, delimiters); // argument 0 holds command
				index = 0;
				while (arguments[index] != NULL) {
					index++;
					arguments[index] = strtok(NULL, delimiters);
				}
			  bool background_process=false;
      	if (strcmp(arguments[index-1],"&")==0){
					background_process=true;
					arguments[index-1] = NULL;
      	}
        // 2. Implement Built-In Commands
				if (strcmp(arguments[0], "cd") == 0) {
					chdir(arguments[1]);
				}
				else if (strcmp(arguments[0], "pwd") == 0) {
					printf("%s\n", getcwd(buf, (size_t)size));
				}
        else if (strcmp(arguments[0], "echo") == 0) {
          index = 1;
          while (arguments[index] != NULL) {
            if (*arguments[index] == '$') {
              printf("%s ",getenv(arguments[index]+1));
            } else {
              printf("%s ", arguments[index]);
            }
            index++;
          }
          printf("\n");
        } 
				else if (strcmp(arguments[0], "exit") == 0) {
					exit(0);
				}
				else if (strcmp(arguments[0], "env") == 0) {
					printf("%s\n", getenv(arguments[1]));
				} 
				else if (strcmp(arguments[0], "setenv") == 0) {
					setenv(arguments[1], arguments[2], 1);
					printf("\n");
				}
        // 3. Create a child process which will execute the command line input
				else {
					pid = fork();
					if (pid==0){
						signal(SIGINT, SIG_DFL);
						execvp(arguments[0], arguments);
						printf("Error occurred.\n");
						exit(0);
					} else {
						// 4. The parent process should wait for the child to complete unless its a background process
						waitpid(pid, NULL, 0);
						kill(child, SIGINT);
						waitpid(child, NULL, 0);
					}
				}
  
      
      
        // Hints (put these into Google):
        // man fork
        // man execvp
        // man wait
        // man strtok
        // man environ
        // man signals
        
        // Extra Credit
        // man dup2
        // man open
        // man pipes
    }
    // This should never be reached.
    return -1;
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

int build_cmd_buff(char* cmd_buff, cmd_buff_t* cmd) {

    int cmd_count = 0;
    bool is_echo = false;
    bool in_quotes = false;
    char* cmd_ptr = cmd_buff;
    
    // Allocate memory for _cmd_buffer
    cmd->_cmd_buffer = malloc(SH_CMD_MAX * sizeof(char));
    if (!cmd->_cmd_buffer) {
        return ERR_MEMORY;
    }
    strcpy(cmd->_cmd_buffer, cmd_buff);

    // Allocate memory for each argument
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd->argv[i] = NULL;
    }

    while (*cmd_ptr) {
        // Skip leading spaces
        while (*cmd_ptr == ' ') cmd_ptr++;
        if (*cmd_ptr == '\0') break;

        // Handle quoted arguments
        if (*cmd_ptr == '"') {
            in_quotes = true;
            cmd_ptr++; // Skip the opening quote
        }

        // If echo is detected, capture everything after it as a single argument
        if (cmd_count == 0 && strncmp(cmd_ptr, "echo", 4) == 0 && (cmd_ptr[4] == ' ' || cmd_ptr[4] == '\0')) {
            is_echo = true;
        }

        // Handle echo: everything after "echo" is one argument
        if (is_echo && cmd_count > 0) {
            char* start = cmd_ptr;
            char* end = start + strlen(start) - 1;

            // Remove the trailing quote if present
            if (*end == '"') {
                *end = '\0';
            }

            cmd->argv[cmd_count] = strdup(start);
            cmd_count++;
            break;
        }

        // Parse each argument
        char* start = cmd_ptr;
        while (*cmd_ptr && (in_quotes || (*cmd_ptr != ' ' && *cmd_ptr != '\n'))) {
            if (*cmd_ptr == '"' && in_quotes) { 
                in_quotes = false; 
                *cmd_ptr = '\0'; // Terminate quoted string
            }
            cmd_ptr++;
        }

        // Allocate and store argument
        if (*cmd_ptr) *cmd_ptr++ = '\0'; // Null-terminate argument
        cmd->argv[cmd_count] = strdup(start);
        if (!cmd->argv[cmd_count]) return ERR_MEMORY;
        cmd_count++;
    }

    cmd->argv[cmd_count] = NULL;
    cmd->argc = cmd_count;
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    if (!cmd_buff) {
        return ERR_MEMORY;  // Handle NULL pointer case
    }

    // Free dynamically allocated command buffer
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }

    // Free each argument in argv
    for (int i = 0; i < cmd_buff->argc; i++) {
        if (cmd_buff->argv[i]) {
            free(cmd_buff->argv[i]);
            cmd_buff->argv[i] = NULL;
        }
    }

    // Reset argument count
    cmd_buff->argc = 0;

    return OK;
}


int exec_cmd(cmd_buff_t* cmd) {
    char cmd_exec[CMD_MAX];
    strcpy(cmd_exec, cmd->argv[0]);
    
    // execute
    pid_t pid = fork();

    if (pid == 0) { // child process -- check for specific commands
        // command cd
        if (strcmp(cmd_exec, "cd") == 0) {
            if (cmd->argc < 2) {
                exit(OK); // no directory provided, exit normally
            } else if (chdir(cmd->argv[1]) != 0) {
                printf(CMD_ERR_EXECUTE);
                exit(ERR_EXEC_CMD);
            }
        // command pwd
        } else if (strcmp(cmd_exec, "pwd") == 0) {
            execvp("pwd", cmd->argv);
            printf(CMD_ERR_EXECUTE);
            exit(ERR_EXEC_CMD);
        // command echo
        } else if (strcmp(cmd_exec, "echo") == 0) {
            if (cmd->argc > 1) {
                for (int i = 1; i < cmd->argc; i++) {
                    printf("%s", cmd->argv[i]);
                    if (i < cmd->argc - 1) {
                        printf(" ");  // Print a single space between arguments
                    }
                }
                printf("\n");
                
            } else {
                exit(ERR_CMD_ARGS_BAD); // Exit if no arguments
            }
        // command ls
        } else if (strcmp(cmd_exec, "ls") == 0) {
            execvp("ls", cmd->argv);
            printf(CMD_ERR_EXECUTE);
            exit(ERR_EXEC_CMD);
        // command uname
        } else if (strcmp(cmd_exec, "uname") == 0) {
            execvp("uname", cmd->argv);
            printf(CMD_ERR_EXECUTE);
            exit(ERR_EXEC_CMD);
        }
    } else if (pid < 0) { // fork failed
        printf(CMD_ERR_EXECUTE);
        return ERR_MEMORY;
    } else { // parent process
        wait(NULL); // wait for child process to finish
    }

    return OK;
}

int exec_local_cmd_loop() {
    char *cmd_buff;
    int rc = 0;
    cmd_buff_t* cmd;

    cmd_buff = malloc(SH_CMD_MAX * sizeof(char));
    if (!cmd_buff) {
        return ERR_MEMORY;
    }
    
    cmd = malloc(sizeof(cmd_buff_t)); 
    if (!cmd) {
        free(cmd_buff);
        return ERR_MEMORY;
    }

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        // check if no command was entered
        if (strlen(cmd_buff) == 0) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        // if exit was entered stop reading input
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            clear_cmd_buff(cmd);
            break;
        }
        
        // build cmd and check rc
        rc = build_cmd_buff(cmd_buff, cmd);
        if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_ARGV_MAX);
        } else if (rc == ERR_MEMORY) {
            printf(CMD_ERR_EXECUTE);
        } else {
            rc = exec_cmd(cmd);
        }
    }
    
    // Free allocated memory
    free(cmd_buff);
    free(cmd->_cmd_buffer);
    for (int i = 0; i < cmd->argc; i++) {
        free(cmd->argv[i]);
    }
    free(cmd);
    
    return OK;
}
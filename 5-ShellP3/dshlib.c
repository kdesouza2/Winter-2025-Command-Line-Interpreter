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
 
 int build_cmd_list(char *cmd_line, command_list_t *clist) {
    if (!cmd_line || !clist) {
        return ERR_CMD_OR_ARGS_TOO_BIG;
    }

    memset(clist, 0, sizeof(command_list_t));
    char *cmd_token, *arg_token;
    char *saveptr1, *saveptr2;
    int cmd_count = 0;

    cmd_token = strtok_r(cmd_line, "|", &saveptr1);
    while (cmd_token) {
        if (cmd_count >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        while (*cmd_token == ' ') cmd_token++;
        char *end = cmd_token + strlen(cmd_token) - 1;
        while (end > cmd_token && *end == ' ') *end-- = '\0';

        arg_token = strtok_r(cmd_token, " ", &saveptr2);
        if (!arg_token) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        strncpy(clist->commands[cmd_count].exe, arg_token, EXE_MAX - 1);
        clist->commands[cmd_count].exe[EXE_MAX - 1] = '\0';

        clist->commands[cmd_count].args[0] = '\0';
        int first_arg = 1;
        while ((arg_token = strtok_r(NULL, " ", &saveptr2)) != NULL) {
            if (strlen(clist->commands[cmd_count].args) + strlen(arg_token) + 2 >= ARG_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            if (!first_arg) {
                strncat(clist->commands[cmd_count].args, " ", ARG_MAX - strlen(clist->commands[cmd_count].args) - 1);
            }
            strncat(clist->commands[cmd_count].args, arg_token, ARG_MAX - strlen(clist->commands[cmd_count].args) - 1);
            first_arg = 0;
        }

        cmd_count++;
        cmd_token = strtok_r(NULL, "|", &saveptr1);
    }

    clist->num = cmd_count;
    return OK;
}


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
        return ERR_MEMORY;
    }

    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;  // Set to NULL after free
    }

    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        if (cmd_buff->argv[i]) {
            free(cmd_buff->argv[i]);
            cmd_buff->argv[i] = NULL;  // Prevent double free
        }
    }

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
        } else {
            execvp(cmd_exec, cmd->argv);
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


int execute_pipeline(command_list_t *clist) {
    if (clist->num == 0) {
        return WARN_NO_CMDS;
    }
    
    int pipes[CMD_MAX - 1][2];
    pid_t pids[CMD_MAX];
    cmd_buff_t cmd;
    
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_MEMORY;
        }
    }
    
    for (int i = 0; i < clist->num; i++) {
        if (build_cmd_buff(clist->commands[i].exe, &cmd) != OK) {
            return ERR_MEMORY;
        }

        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return ERR_MEMORY;
        }
        
        if (pids[i] == 0) {
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            execvp(cmd.argv[0], cmd.argv);
            perror("execvp");
            exit(ERR_EXEC_CMD);
        }
        
        clear_cmd_buff(&cmd);
    }
    
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], NULL, 0);
    }
    
    return OK;
}

int exec_local_cmd_loop() {
    char *cmd_buff = malloc(SH_CMD_MAX * sizeof(char));
    if (!cmd_buff) {
        return ERR_MEMORY;
    }
    
    command_list_t clist;
    
    while (1) {
        printf("%s", SH_PROMPT);
        fflush(stdout);
        
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        
        if (strlen(cmd_buff) == 0) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }
        
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            break;
        }
        
        int rc = build_cmd_list(cmd_buff, &clist);
        if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        } else if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            continue;
        } else {
            execute_pipeline(&clist);
        }
    }
    
    free(cmd_buff);
    return OK;
}

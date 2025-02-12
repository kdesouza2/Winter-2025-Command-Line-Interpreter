#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist) {
    if (!cmd_line || !clist) {
        return ERR_CMD_OR_ARGS_TOO_BIG;
    }

    memset(clist, 0, sizeof(command_list_t));
    char *cmd_token, *arg_token;
    char *saveptr1, *saveptr2;
    int cmd_count = 0;

    // Split command line by '|'
    cmd_token = strtok_r(cmd_line, "|", &saveptr1);
    while (cmd_token) {
        if (cmd_count >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        // Trim leading spaces
        while (*cmd_token == ' ') {
            cmd_token++;
        }

        // Trim trailing spaces
        char *end = cmd_token + strlen(cmd_token) - 1;
        while (end > cmd_token && *end == ' ') {
            *end = '\0';
            end--;
        }

        // Tokenize command into executable and arguments
        arg_token = strtok_r(cmd_token, " ", &saveptr2);
        if (!arg_token) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        // Copy executable name
        strncpy(clist->commands[cmd_count].exe, arg_token, EXE_MAX - 1);
        clist->commands[cmd_count].exe[EXE_MAX - 1] = '\0';

        // Store arguments as a space-separated string
        clist->commands[cmd_count].args[0] = '\0';  // Ensure it's empty
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
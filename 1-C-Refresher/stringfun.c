#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int setup_buff(char* buff, char* user_str, int len) {
    // validate parameters
    if (buff == NULL || user_str == NULL || len <= 0) {
        printf("Error: Invalid Arguments.\n");
        exit(3);
    }

    // temp pointers
    char* input = user_str;
    char* output = buff;

    // length of output string
    int output_len = 0;

    bool last_char_was_space = false;

    while (*input != '\0') {
        if (*input == ' ' || *input == '\t') {
            if (!last_char_was_space) {
                if (output_len < len) {
                    *output++ = ' ';
                    output_len++;
                } else {
                    exit(1);
                }
            }

            last_char_was_space = true;

        } else {
            if (output_len < len) {
                *output++ = *input;
                output_len++;
            } else {
                exit(1);
            }
            last_char_was_space = false;
        }

        input++;
    }

    // fill the remaining space in the buffer with '.'
    while (output_len < len) {
        *output++ = '.';
        output_len++;
    }

    return (output_len - (len - BUFFER_SZ));
}

int is_space(char c) {
    return (c == ' ' || c == '\t' || c == '\n');
}

int count_words(char *buff, int len, int str_len){
    if (buff == NULL || str_len <= 0 || len <= 0) {
        exit(1);
    }

    int count = 0;
    bool inside_word = false;

    for (int i = 0; i < str_len; i++) {
        if (!is_space(buff[i])) {
            if (!inside_word) {
                count++;
                inside_word = true;
            }
        } else {
            inside_word = false;
        }
    }

    return count;
}

void reverse_string(char* str) {
    if (!str){
        printf("Error: Invalid Arguments.\n");
        exit(1);
    } 

    char* start = str;
    char* end = str;
    char* write_ptr = str;

    // Move the `end` pointer to the last character of the string
    while (*end != '\0') {
        if (*end != '.') {
            *write_ptr++ = *end; // Copy non-dot characters to the string
        }
        end++;
    }
    *write_ptr = '\0'; // Null-terminate the string after removing dots

    // Adjust `end` to the new last character after excluding dots
    end = write_ptr - 1;

    // Reverse the string in place
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;

        start++;
        end--;
    }
}

void print_words_and_lengths(char* str, int user_str_len) {
    if (!str){
        printf("Error: Invalid Arguments.\n");
        exit(1); // Handle null input
    } 

    char* word_start = NULL;
    int word_length = 0;
    int word_count = count_words(str, BUFFER_SZ, user_str_len);

    printf("Word Print\n----------\n");

    while (*str != '\0') {
        // Ignore spaces and periods
        if (!is_space(*str) && *str != '.') {
            if (word_start == NULL) {
                word_start = str; // Mark the beginning of a word
            }
            word_length++;
        } else if (word_start != NULL) {
            // End of a word, print it, ignoring periods in the word length
            printf("%d. %.*s (%d)\n", word_count, word_length, word_start, word_length);
            word_start = NULL;
            word_length = 0;
        }
        str++;
    }

    // Handle the last word if the string doesn't end with a space
    if (word_start != NULL) {
        printf("%d. %.*s (%d)\n", word_count, word_length, word_start, word_length);
    }
}

 
//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if argv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /*
    The *argv[1] != '-' then that means that argv[1] is not a flag and therefore the program doesn't have the proper inputs.
    Because of that we would want the program to exit(1), because it is a command line problem
    */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /*
    If argc is less than 3 then that means that less than 3 arguments were passed in the command line
    Since we have too little arguments we want the program to exit(1) because it is a command line problem
    */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = malloc(sizeof(char)*BUFFER_SZ);
    if (buff == NULL) {
        printf("Error: Memory Allocation Failed.\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            reverse_string(buff);
            printf("Reversed String: %s\n", buff);
            break;
        
        case 'w':
            print_words_and_lengths(buff, user_str_len);
            break;

        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    // print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE

/*
I think providing the pointer and the length is good practice, because it makes our jobs a lot easier
Even though we know buff can have a length of 50, it's very probable that some of that space may not be used up
Keeping track of the size of buff and not just its capacity allows us to focus on the useful information and not spend time trying 
to figure out how much of buff we need to use
*/
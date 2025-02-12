1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is a good choice because it reads a full line of input safely, preventing buffer overflow by limiting the number of characters read.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() allows for dynamic memory allocation, enabling the shell to handle variable-length input efficiently. A fixed-size array would waste memory when handling short commands and might be too small for long commands, causing truncation or buffer overflows.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming spaces ensures accurate command parsing and execution. If leading spaces remain, the command might not be recognized correctly.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  
    1. Output Redirection (>): Redirects STDOUT to a file (e.g., ls > output.txt). Challenge: Ensuring the file is properly created or overwritten. 
    2. Input Redirection (<): Reads from a file instead of STDIN (e.g., sort < file.txt). Challenge: Handling errors if the file doesn’t exist.
    3. Appending Output (>>): Appends STDOUT to a file (e.g., echo "Hello" >> log.txt). Challenge: Ensuring correct file handling without overwriting existing data.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection changes where input or output is read from or written to, typically involving files.  Piping connects one command's output to another's input, allowing real-time data processing between commands.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Keeping STDERR separate prevents error messages from mixing with regular output, making debugging easier.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our shell should capture command exit statuses and print meaningful error messages.
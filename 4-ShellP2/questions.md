1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  The fork system call creates a child process that is an exact copy of the parent. This allows the parent to continue running while the child executes a new program using execvp. If we called execvp directly in the parent process, it would replace the entire shell with the new program, preventing the shell from continuing to accept commands. fork ensures that the shell remains active while executing external commands in a separate process.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, it returns -1, indicating that a new process could not be created due to system resource constraints (e.g., too many processes running, memory limits). In my implementation, if fork() returns -1, an error message is printed (CMD_ERR_EXECUTE), and the function returns ERR_MEMORY. This prevents the shell from trying to execute commands in an invalid state.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: In this application for example, execvp("ls", cmd->argv); searches for the ls executable in the directories listed in the PATH environment variable. The execvp() function checks each directory in PATH (e.g., /bin, /usr/bin) to find an executable named ls. If found, execvp() replaces the current process with ls, passing cmd->argv as arguments. If ls is not found or execution fails, execvp() returns an error, and the child process prints CMD_ERR_EXECUTE before terminating.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The wait() function makes the parent process pause until the child process terminates. This prevents zombie processes (terminated child processes that still occupy system resources). Without wait(), the child process could become a zombie, and the system's process table would eventually fill up, leading to performance issues.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS(status) extracts the exit status of a child process from the status returned by wait(). This allows the parent process to check if the child exited successfully (0) or encountered an error (nonzero values). It is useful for debugging and handling errors in scripts and programs.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation ensures that arguments enclosed in quotes (e.g., "hello world") are treated as a single argument while still stripping the quotes. This is necessary because commands like echo "hello world" should treat "hello world" as one argument instead of two separate words. Without handling quoted arguments, spaces inside quotes would be incorrectly treated as argument delimiters.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I modified build_cmd_buff() to properly handle quoted arguments and ensure that echo captures everything after it as a single argument. One unexpected challenge was ensuring that trailing quotes were correctly removed while preserving spaces inside quotes. Initially, the parser left trailing quotes, leading to incorrect command execution. Debugging this required careful string manipulation and boundary checks.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals are a form of asynchronous communication used to notify processes of events, such as termination requests, interrupts, or exceptions. Unlike other IPC mechanisms (e.g., pipes, message queues), signals do not require explicit data exchange between processes. Instead, they act as simple notifications that a process must handle.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL: Immediately terminates a process. It cannot be caught, ignored, or handled. Used when a process must be forcefully stopped.
SIGTERM: Requests a process to terminate gracefully, allowing it to clean up resources before exiting. Preferred over SIGKILL when shutting down programs.
SIGINT: Interrupts a process, usually sent when the user presses Ctrl+C. The process can handle this signal to perform cleanup before exiting.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP pauses a process, preventing it from executing further until it receives a SIGCONT signal. Unlike SIGINT, SIGSTOP cannot be caught, ignored, or handled by the process. This ensures that an administrator or system utility can suspend a process unconditionally, regardless of the process’s behavior.
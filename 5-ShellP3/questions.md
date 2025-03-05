1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My shell ensures that all child processes complete before accepting new input by calling waitpid() on each child process created during command execution. When executing a piped command sequence, the shell forks multiple times to create separate processes for each command. After forking, the parent process (the shell) loops through the list of child process IDs and calls waitpid() to block execution until all children terminate.

If waitpid() is not called on all child processes, they would become zombie processes, consuming system resources unnecessarily. Additionally, failing to wait for child processes could lead to race conditions where the shell might start accepting new input before the piped commands finish executing, potentially causing unpredictable behavior.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

After using dup2() to redirect standard input or output to a pipe, it is necessary to close the unused pipe ends to prevent resource leaks and ensure proper process termination. Pipes are implemented as file descriptors, and if a process keeps unused pipe ends open, the corresponding read or write operation may hang indefinitely.

For example, if a process is reading from a pipe and the write end is not closed, the reader may never receive an EOF signal, causing it to hang. Similarly, if a process writes to a pipe without closing the read end, it may prevent the system from reclaiming resources, leading to resource exhaustion. Closing unused pipe ends ensures that processes properly detect when they should stop reading or writing, preventing deadlocks and leaks.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd command is implemented as a built-in because it modifies the shell’s working directory, which is a property of the parent shell process. If cd were implemented as an external command, it would execute in a separate child process. Since each child process has its own copy of the shell’s environment, changing the working directory in the child process would have no effect on the parent shell, meaning that as soon as the child process exits, the shell would remain in the same directory as before.

To change the working directory of the shell itself, cd must be handled directly within the shell process using chdir(), ensuring that the change persists across commands. If cd were an external command, workarounds such as modifying shell scripts or using subshells would be required, but these approaches would be inefficient and counterintuitive.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To support an arbitrary number of piped commands, I would use dynamic memory allocation rather than a fixed-size array for storing pipe file descriptors and child process IDs. Instead of defining CMD_MAX, I would use malloc() and realloc() to allocate memory as needed when parsing commands.
# **Command Line Interpreter**  

## **Overview**  
This project, developed for **CS 283 (Winter 2025)**, is a **compiler-like system** that simulates a command-line interface. It supports **piping commands** and executing built-in shell commands such as `cd`, `ls`, `echo`, and more.  

## **Features**  
- **Command Execution:** Simulates Unix-like commands (`cd`, `ls`, `echo`, etc.).  
- **Piping Support:** Allows commands to be piped together (`ls | grep txt`).  
- **Argument Parsing:** Properly handles command arguments and flags.  
- **Error Handling:** Detects invalid commands and incorrect syntax.  
- **Automated Testing:** Uses **BATS (Bash Automated Testing System)** to verify command execution and output.  

## **Technologies Used**  
- **C** (Core application logic)
- **Visual Studio Code** (IDE)
- **Lexical & Syntax Parsing** for command interpretation  
- **Unix System Calls** for process execution  
- **BATS (Bash Automated Testing System)** for testing 

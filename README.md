# Simple Command-Line Shell

## Overview

This program implements a basic command-line shell that can execute Unix/Linux commands with support for input and output redirection. It reads commands from the user, 
parses them into tokens, and executes them in a child process. The shell supports redirection using < and >, allowing you to manage input and output file streams.

## Features

- Execute Unix/Linux commands.
- Support for input redirection (<).
- Support for output redirection (>).
- Basic error handling for common issues.
- Dynamic memory management for command arguments and redirection filenames.

## Compilation and Execution

1. Compile the program

    gcc -o simple_shell simple_shell.c

2. Run the program

    ./simpleShell

3. Enter commands into the shell! For example:

    ls -l
    cat < input.txt
    echo "Hello, World!" > output.txt

4. Exit the program

    Press "CTRL + D"

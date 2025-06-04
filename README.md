# JCShell - Custom Unix Shell Implementation

A custom Unix shell implementation in C that supports command execution, piping, signal handling, and process monitoring with detailed statistics reporting.

## Information
- **Development Platform**: VSCode, Workbench2, and Docker
- **Course**: COMP3230 Operating Systems
- **Assignment**: Programming Assignment 1

## Overview

JCShell is a simplified Unix shell that demonstrates fundamental operating system concepts including:
- Process creation and management
- Inter-process communication via pipes
- Signal handling and process synchronization
- Process statistics collection and reporting
- Command parsing and execution

## Features

### Core Functionality
- ✅ **Command Execution**: Execute standard Unix commands
- ✅ **Command Piping**: Support for multi-command pipelines using `|`
- ✅ **Process Statistics**: Detailed process monitoring and reporting
- ✅ **Signal Handling**: Proper interrupt and signal management
- ✅ **Error Handling**: Comprehensive input validation and error reporting

### Advanced Features
- **Concurrent Process Execution**: Multiple commands in pipeline run simultaneously
- **Process Synchronization**: SIGUSR1-based process coordination
- **Real-time Statistics**: CPU time, context switches, and exit status tracking
- **Robust Input Validation**: Handles malformed pipe sequences and invalid commands

## System Requirements

### Dependencies
- **Operating System**: Linux/Unix-based system
- **Compiler**: GCC with standard libraries
- **System Features**: 
  - `/proc` filesystem for process statistics
  - POSIX signals support
  - Standard Unix utilities

### Compilation
```bash
gcc -o JCshell [JCshell_305790769.c](http://_vscodecontentref_/0) -Wall -Wextra

## Usage
- Starting the Shell
./JCShell
- Command Format
## JCSHELL [PID] ## command1 | command2 | command3
- Examples 
# Simple command execution
## JCSHELL [1234] ## ls -l

# Pipeline execution
## JCSHELL [1234] ## ps aux | grep bash | wc -l

# Multiple command pipeline
## JCSHELL [1234] ## cat file.txt | sort | uniq | head -10

- Exit Command 
## JCSHELL [1234] ## exit







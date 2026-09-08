# Simple Shell - A Custom Unix Shell Implementation

A custom Unix shell implementation written in **C**, developed to explore Unix process management, command parsing, execution, environment variables, redirection, command chaining, aliases, and signal handling.

## 📖 Description

**Simple Shell** is a lightweight command-line interpreter implemented in C.

The project provides an interactive Unix-like shell capable of executing external commands and handling several built-in commands and shell features.

The shell supports:

* Built-in commands
* External command execution
* Command chaining
* Output redirection
* Background execution
* Environment variable expansion
* Command aliases
* Signal handling
* Interactive execution
* Batch/script execution

## ✨ Features

### Built-in Commands

* `exit` — Exit the shell
* `cd` — Change the current directory
* `env` — Display environment variables
* `setenv` — Set an environment variable
* `unsetenv` — Remove an environment variable
* `path` — Manage/display the command search path
* `alias` — Create and manage command aliases

### Shell Features

* Interactive shell mode
* Batch/script execution
* Command chaining:

  * `&&`
  * `||`
  * `;`
* Output redirection using `>`
* Background execution using `&`
* Environment variable expansion using `$`
* Command aliases
* Unix signal handling
* External command execution

## 🛠️ Technologies

* **C**
* **Unix/Linux**
* **GNU Make**
* **POSIX system interfaces**
* **Shell scripting concepts**

## 🚀 Build

Clone the repository:

```bash
git clone https://github.com/kibrom-zewdu/simple-shell.git
```

Enter the project:

```bash
cd simple-shell
```

Build the shell:

```bash
cd src
make
```

## ▶️ Run

### Interactive Mode

From the `src` directory:

```bash
./shell
```

The shell will start an interactive command prompt.

Example:

```text
$ ./shell
shell$ pwd
shell$ ls
shell$ cd ..
shell$ exit
```

### Batch Mode

Run the shell with a script file:

```bash
./shell script.sh
```

Example:

```bash
./shell commands.sh
```

## 💻 Example Commands

### Execute commands

```bash
ls
pwd
whoami
```

### Change directory

```bash
cd /tmp
cd ..
```

### Environment variables

```bash
env
setenv NAME Kibrom
unsetenv NAME
```

### Path

```bash
path
```

### Aliases

```bash
alias ll="ls -la"
```

### Command chaining

Using `&&`:

```bash
mkdir test && cd test
```

Using `||`:

```bash
false || echo "Command failed"
```

Using `;`:

```bash
pwd ; ls ; whoami
```

### Output redirection

```bash
ls > output.txt
```

### Background execution

```bash
sleep 10 &
```

### Variable expansion

```bash
setenv USERNAME Kibrom
echo $USERNAME
```

### Exit

```bash
exit
```

## 📁 Project Structure

```text
simple-shell/
│
├── man/
│   ├── alias.1
│   ├── cd.1
│   ├── env.1
│   ├── exit.1
│   ├── path.1
│   ├── setenv.1
│   └── unsetenv.1
│
└── src/
    ├── Makefile
    ├── builtins.c
    ├── executor.c
    ├── main.c
    ├── parser.c
    ├── shell.c
    ├── shell.h
    ├── signal.c
    └── utils.c
```

## 🧩 Source Code Overview

| File         | Purpose                                      |
| ------------ | -------------------------------------------- |
| `main.c`     | Program entry point and shell initialization |
| `shell.c`    | Main shell loop and shell control            |
| `parser.c`   | Command parsing and token processing         |
| `executor.c` | Process creation and command execution       |
| `builtins.c` | Built-in command implementation              |
| `utils.c`    | Utility and helper functions                 |
| `signal.c`   | Signal handling                              |
| `shell.h`    | Shared declarations and definitions          |
| `Makefile`   | Build and compilation automation             |

## 📚 Manual Pages

Manual pages are provided for the supported built-in commands:

```text
man/
├── alias.1
├── cd.1
├── env.1
├── exit.1
├── path.1
├── setenv.1
└── unsetenv.1
```

Example:

```bash
man ./man/cd.1
```

## 🎓 Course Information

This project was developed as part of a **C / Unix Systems Programming course**.

The project focuses on practical implementation of:

* C programming
* Unix/Linux concepts
* Processes and process management
* System calls
* Command-line parsing
* Environment variables
* File descriptors
* Signals
* Shell architecture
* Build automation with Make

## 👨‍💻 Author

**Kibrom Zewdu**

Computer Science Student

Interested in:

* Full-Stack Development
* Artificial Intelligence
* Cybersecurity
* Systems Programming

## 📌 Project Status

**Status: Completed / Academic Project**

The project provides a functional custom shell implementation for learning and demonstrating Unix shell concepts in C.

## 📄 License

This project is licensed under the **MIT License**. See the `LICENSE` file for details.

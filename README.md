[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/fNtavDLR)
# Inter-process Communication

## Introduction

In this assignment, you will practice working with I/O and pipes

To get started, first clone from GitHub. You may need to "close" the existing folder first.

### Build Instructions

To build this project run the `make` command in the terminal. You must run this *every time you change a file*.

### Commit Instructions

After completing Part A, first add the file called parta.c` to staging:

    git add parta.c

Then do the actual commit:

    git commit -m 'Completed Part A'

When you are ready to submit the assignment, run the following command:

    git push

## Part A Instructions

Write a program that reads a file in chunks of *8 bytes* and count how many letters of each category
that exists. There are 5 categories:

1. Uppercase alphabet letters (isupper)
2. Lowercase alphabet letters (islower)
3. Numbers (isdigit)
4. Space (isspace)
5. Others

You are to use the `open` system call to open the file, and read the file using the `read` system
call. The buffer to use is provided, and you should not change this.

The program takes a command-line argument, the name of the file. For example:

    ./parta tests/file1.txt
    Upper,1
    Lower,34
    Number,0
    Space,9
    Others,0

Once you are reasonably sure your program works correctly, you can run the unit tests like so:

    $ bats tests/parta.bats

## Part B

Write a program relies on the program from Part A. It execs the program from Part A. But *BEFORE* the exec
system call, redirect the process' STDOUT to a file (using `dup2`). The process should
open a file with write mode, create options, and 0770 permissions.

    int fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0770);
    if(open_fails) { print error message }

If this is successful, redirect STDOUT to the file.

    dup2(fd, STDOUT_FILENO);

 After the file descriptor redirection, print the following line and flush the STDOUT:

    printf("Category,Count\n");
    fflush(stdout);

Then, use the exec system call. The path to the program from Part A will be `.//parta`, so the
execv line will look like this:

    int eret = execv("./parta", parta_args);

What will happen is that all output from the child process will be redirected to the file called
`output_filename`. This filename will be provided from the command-line arguments.

    $ ./partb
    USAGE: partb FILEIN FILEOUT

    $ ./partb tests/file1.txt
    USAGE: partb FILEIN FILEOUT

    $ ./partb tests/file1.txt output.txt

The file `output.txt` should contain:

    Category,Count
    Upper,1
    Lower,34
    Number,0
    Space,9
    Others,0

> [!NOTE]
> If the "Category,Count" line doesn't exist, add the `fflush(stdout)` line above.

Once you are reasonably sure your program works correctly, you can run the unit tests like so:

    $ bats tests/partb.bats

## Part C

In this part, we'll use a pipe to connect the parent process and the child process. The parent
process will be the `parta` program. The child process will be the `sort` program.

Here are the steps to take:

1. Check for command-line arguments
2. Create the pipe()
3. Create the child process
4. If parent
    A. close the read end (`close(pipe_read_fd);`)
    B. Redirect STDOUT to the pipe's write end (`dup2(pipe_write_fd, STDOUT_FILENO);`)
    C. Exec parta with the name of the input file
5. If child
    A. Close the write end (`close(pipe_write_fd);`)
    B. Redirect STDIN to the pipe's read end (`dup2(pipe_read_fd, STDIN_FILENO);`)
    C. Exec `sort` with the arguments: "-t,", "-k2", and "-n"
        a. "-t," means split at commas (notice the `,`!)
        b. "-k2" means use the second column
        c. "-n" means sort using numerical values (so 3 < 11)

> [!NOTE]
> Notice the sort program does NOT get have any file names! The data to sort comes
> through the pipe.

This will cause the output of part A to be sorted according to value:

    ./partc tests/file1.txt
    Number,0
    Other,0
    Upper,1
    Space,9
    Lower,34

Once you are reasonably sure your program works correctly, you can run the unit tests like so:

    $ bats tests/partc.bats


# Linux-Shell-Command-Simulation
Simulated Linux Shell commands for input redirection, output redirection, background jobs, and pipe

The four functional characters (<, >, |, &) are for input redirection,
output redirection, pipe and background jobs respectively. By default, they can be interpreted
correctly by a UNIX terminal, but not by a C/C++ program. In a UNIX terminal if you type
in ”echo hello > file1”, you will find a file named ”file1” created which has string hello stored
inside, and thats because the terminal interpreted ”>” correctly and redirected the output of
string hello from screen to file1.

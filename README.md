# nlstat

This is a simple tool to check the newline conventions used in a given text file. It will also check if there is a newline at the end of the file.

Currently, `nlstat` detects three major newline conventions:

* DOS/Windows (`"\r\n"`, CR LF)
* Unix (`"\n"`, LF)
* Classic Mac OS (`"\r"`, CR)

To use it, simply pass the name of one or files as an argument on the command line:

	nlstat file...

## Machine-Readable Output

You can pass the optional `-m` option as the first argument to change the output to a machine-readable format:

	nlstat -m file...

The format is similar to that of the `wc` program on Unix-like systems. Each file results in a single line of output. Each line consists of multiple fields separated by spaces.

In order, the fields are:

1. Number of DOS/Windows newlines
2. Number of Unix newlines
3. Number of Classic Mac newlines
4. Newline at end of file (1 if there is a newline, 0 if not)
5. The filename

For example:

	0 21 0 1 code-with-unix-newlines.c
	35 0 0 0 text-with-windows-newlines.txt
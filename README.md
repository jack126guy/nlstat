# nlstat

This is a simple tool to check the newline conventions used in a given text file. It will also check if there is a newline at the end of the file.

Currently, `nlstat` detects three major newline conventions:

* DOS/Windows (`"\r\n"`, CR LF)
* Unix (`"\n"`, LF)
* Classic Mac OS (`"\r"`, CR)

To use it, simply pass the name of a file as an argument on the command line:

	nlstat [file]

## Machine-Readable Output

The lines beginning with a colon (":") are intended to convey machine-readable information. In order, they are:

* Number of DOS/Windows newlines
* Number of Unix newlines
* Number of Classic Mac newlines
* Newline at end of file (1 if there is a newline, 0 if not)
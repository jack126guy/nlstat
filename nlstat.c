/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Jack126Guy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>

static unsigned long doscount = 0;
static unsigned long unixcount = 0;
static unsigned long maccount = 0;
static int lastnewline = 0;

static void print_stats(void);

int main(int argc, char** argv) {
	int c;
	int hascr = 0;
	FILE* infile;

	if(argc < 2) {
		fprintf(stderr, "Usage: %s [file]\n", argv[0]);
		return EXIT_FAILURE;
	} else if(argc > 2) {
		fputs("Warning: nlstat does not (currently) support multiple arguments.\n\n", stderr);
	}
	infile = fopen(argv[1], "rb");
	if(!infile) {
		perror("Could not open the file");
		return EXIT_FAILURE;
	}
	while((c = getc(infile)) != EOF) {
		switch(c) {
			case '\r':
				maccount++;
				hascr = 1;
				lastnewline = 1;
				break;
			case '\n':
				if(hascr) {
					/* Originally assumed Mac but incorrect */
					maccount--;
					doscount++;
				} else {
					unixcount++;
				}
				hascr = 0;
				lastnewline = 1;
				break;
			default:
				hascr = 0;
				lastnewline = 0;
		}
	}
	if(ferror(infile)) {
		fputs("Error reading file; printing stats so far...\n", stderr);
		print_stats();
		fclose(infile);
		return EXIT_FAILURE;
	}
	print_stats();
	fclose(infile);
	return EXIT_SUCCESS;
}

void print_stats(void) {
	puts("DOS/Windows newlines (\\r\\n):");
	printf(":%lu\n", doscount);
	puts("Unix newlines (\\n):");
	printf(":%lu\n", unixcount);
	puts("Classic Mac newlines(\\r):");
	printf(":%lu\n", maccount);
	if(lastnewline) {
		puts("Found a newline at the end of the file");
		puts(":1");
	} else {
		puts("No newline at the end of the file");
		puts(":0");
	}
}
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

struct newline_data {
	unsigned long dos_count;
	unsigned long unix_count;
	unsigned long mac_count;
	int newline_at_end;
};

static void count_newlines(FILE* file, struct newline_data* data);
static void print_stats(const struct newline_data* data);

int main(int argc, char** argv) {
	struct newline_data data = { 0 };
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
	count_newlines(infile, &data);
	fclose(infile);
	print_stats(&data);
	return EXIT_SUCCESS;
}

void count_newlines(FILE* file, struct newline_data* data) {
	int c;
	int hascr = 0;

	data->dos_count = 0;
	data->unix_count = 0;
	data->mac_count = 0;
	data->newline_at_end = 0;
	while((c = getc(file)) != EOF) {
		switch(c) {
			case '\r':
				data->mac_count++;
				hascr = 1;
				data->newline_at_end = 1;
				break;
			case '\n':
				if(hascr) {
					/* Originally assumed Mac but incorrect */
					data->mac_count--;
					data->dos_count++;
				} else {
					data->unix_count++;
				}
				hascr = 0;
				data->newline_at_end = 1;
				break;
			default:
				hascr = 0;
				data->newline_at_end = 0;
		}
	}
	if(ferror(file)) {
		fputs("Error reading file; printing stats so far...\n", stderr);
	}
}

void print_stats(const struct newline_data* data) {
	puts("DOS/Windows newlines (\\r\\n):");
	printf(":%lu\n", data->dos_count);
	puts("Unix newlines (\\n):");
	printf(":%lu\n", data->unix_count);
	puts("Classic Mac newlines (\\r):");
	printf(":%lu\n", data->mac_count);
	if(data->newline_at_end) {
		puts("Found a newline at the end of the file");
		puts(":1");
	} else {
		puts("No newline at the end of the file");
		puts(":0");
	}
}
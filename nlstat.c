/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2018 Half-Gray (Jack126Guy)
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
#include <errno.h>
#include <string.h>

struct newline_data {
	unsigned long dos_count;
	unsigned long unix_count;
	unsigned long mac_count;
	int newline_at_end;
};

typedef void (*print_stats_func)(const struct newline_data*, const char*);

static int process_file(const char* filename, struct newline_data* data);
static int count_newlines(FILE* file, struct newline_data* data);
static void print_stats(const struct newline_data* data, const char* filename);
static void print_stats_machine(const struct newline_data* data, const char* filename);

int main(int argc, char** argv) {
	int i;
	struct newline_data data;
	print_stats_func print = print_stats;
	int first_file_arg = 1;
	int has_failures = 0;

	if(argc < 2) {
		fprintf(stderr, "Usage: %s [-m] file...\n", argv[0]);
		return EXIT_FAILURE;
	}

	if(strcmp(argv[1], "-m") == 0) {
		print = print_stats_machine;
		first_file_arg = 2;
	}

	for(i = first_file_arg; i < argc; i++) {
		int result = process_file(argv[i], &data);
		if(result == 0) {
			print(&data, argv[i]);
		} else {
			fprintf(stderr,
				"Error reading %s: %s\n",
				argv[i],
				strerror(result));
			has_failures = 1;
		}
	}

	return has_failures ? EXIT_FAILURE : EXIT_SUCCESS;
}

int process_file(const char* filename, struct newline_data* data) {
	FILE* file;
	int result;

	file = fopen(filename, "rb");
	if(!file) {
		return errno;
	}
	result = count_newlines(file, data);
	if(result != 0) {
		fclose(file);
		return result;
	}
	fclose(file);
	return 0;
}

int count_newlines(FILE* file, struct newline_data* data) {
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
		return errno;
	}
	return 0;
}

void print_stats(const struct newline_data* data, const char* filename) {
	printf("Stats for %s\n", filename);
	printf("DOS/Windows newlines (\\r\\n): %lu\n", data->dos_count);
	printf("Unix newlines (\\n): %lu\n", data->unix_count);
	printf("Classic Mac newlines (\\r): %lu\n", data->mac_count);
	if(data->newline_at_end) {
		puts("Found a newline at the end of the file");
	} else {
		puts("No newline at the end of the file");
	}
	puts("");
}

void print_stats_machine(const struct newline_data* data, const char* filename) {
	printf(
		"%lu %lu %lu %d %s\n",
		data->dos_count,
		data->unix_count,
		data->mac_count,
		data->newline_at_end,
		filename);
}
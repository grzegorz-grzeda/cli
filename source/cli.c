/**
 * MIT License
 * Copyright (c) 2023 Grzegorz Grzęda
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "cli.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define COMMANDS_INPUT_BUFFER_SIZE (200)
#define COMMANDS_PARAMETER_MAX_COUNT (10)

static cli_print_t cli_print_function;
static cli_entry_t *entries;

static char input_buffer[COMMANDS_INPUT_BUFFER_SIZE] = {0};
static size_t input_buffer_end = 0;
static char *argv[COMMANDS_PARAMETER_MAX_COUNT] = {0};
static int argc = 0;

static void help_execute(cli_print_t cli_print, int, char **)
{
    size_t max_cmd_length = 0;
    for (cli_entry_t *entry = entries; entry; entry = entry->_next) {
        size_t size = strlen(entry->name);
        if (size > max_cmd_length) {
            max_cmd_length = size;
        }
    }
    cli_print("Available commands:\n");
    for (cli_entry_t *entry = entries; entry; entry = entry->_next) {
        cli_print(" %*s - %s\n", max_cmd_length, entry->name, entry->help);
    }
}

static cli_entry_t help_command = {
    .name = "help",
    .help = "Print available commands",
    .execute = help_execute,
};

static char *get_first_non_white_character(char *input)
{
    while (*input) {
        if (isgraph(*input) != 0) {
            break;
        }
        input++;
    }
    return input;
}

static char *get_first_white_character(char *input)
{
    while (*input) {
        if (isgraph(*input) == 0) {
            break;
        }
        input++;
    }
    return input;
}

static void parse_arguments(void)
{
    if (input_buffer_end < 1) {
        return;
    }
    argc = 0;
    for (char *ptr = input_buffer; *ptr || (ptr < (input_buffer + input_buffer_end));) {
        ptr = get_first_non_white_character(ptr);
        if (*ptr == '\0') {
            break;
        }
        argv[argc++] = ptr;
        ptr = get_first_white_character(ptr);
        if (*ptr == '\0') {
            break;
        }
        *(ptr++) = '\0';
    }
    input_buffer_end = 0;
}

static void execute_command(void)
{
    if (argc < 1) {
        return;
    }
    for (cli_entry_t *entry = entries; entry; entry = entry->_next) {
        if (strcmp(argv[0], entry->name) == 0) {
            entry->execute(cli_print_function, argc, argv);
            return;
        }
    }
    help_execute(cli_print_function, 0, 0);
}

void initialize_cli(cli_print_t cli_print)
{
    entries = NULL;
    input_buffer_end = 0;
    cli_print_function = cli_print;
    register_cli(&help_command);
}

void register_cli(cli_entry_t *entry)
{
    if (!entry) {
        return;
    }
    entry->_next = entries;
    entries = entry;
}

void process_cli(char c)
{
    if ((c == '\n') || (c == '\r')) {
        input_buffer[input_buffer_end] = '\0';
        parse_arguments();
        execute_command();
    } else {
        input_buffer[input_buffer_end] = c;
        input_buffer_end++;
    }
}
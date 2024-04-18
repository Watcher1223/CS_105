#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Converts characters to lowercase for case-insensitive comparison
int my_tolower(int c) {
    return (c >= 'A' && c <= 'Z') ? c + 'a' - 'A' : c;
}

// Converts a string to lowercase
void to_lower_case(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = my_tolower(str[i]);
    }
}

// Searches the file for the pattern, modifying output based on flags
// Searches the file for the pattern, modifying output based on flags
int search_in_file(FILE *fp, const char *filename, const char *pattern, int ignore_case, int list_names, int show_line_numbers, int quiet_mode, int include_filename) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int found = 0, line_number = 1;
    char *search_pattern = strdup(pattern);
    if (ignore_case) {
        to_lower_case(search_pattern);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        char *line_to_search = strdup(line);
        if (ignore_case) {
            to_lower_case(line_to_search);
        }
        if (strstr(line_to_search, search_pattern) != NULL) {
            found = 1;
            if (!quiet_mode) {
                if (list_names) {
                    printf("%s\n", filename);
                    free(line_to_search);
                    break;
                } else if (show_line_numbers) {
                    if (include_filename) {
                        printf("%s:%d:%s", filename, line_number, line);
                    } else {
                        printf("%d:%s", line_number, line);
                    }
                } else {
                    if (include_filename) {
                        printf("%s:%s", filename, line); // Ensure colon is added correctly
                    } else {
                        printf("%s", line);
                    }
                }
            }
        }
        free(line_to_search);
        line_number++;
    }

    free(search_pattern);
    free(line);
    fclose(fp);
    return found;
}
// Main function to process command line arguments and handle files
int main(int argc, char *argv[]) {
    int i_flag = 0, l_flag = 0, n_flag = 0, q_flag = 0;
    int opt, overall_found = 0, file_error = 0;
    char *pattern = NULL;

    while ((opt = getopt(argc, argv, "ilnq")) != -1) {
        switch (opt) {
            case 'i': i_flag = 1; break;
            case 'l': l_flag = 1; break;
            case 'n': n_flag = 1; break;
            case 'q': q_flag = 1; break;
            default:

                fprintf(stderr, "Usage: fgrep [-i] [-l] [-n] [-q] pattern [files]\n");
                return 2; // Exit with usage error
        }
    }

    if (optind == argc) {
        fprintf(stderr, "Expected pattern after options\n");
        return 2; // Exit with usage error
    }

    pattern = argv[optind++];
    int multiple_files = (argc - optind > 1); // Check if multiple files are involved

    if (optind == argc) {
        // If no files are specified, use stdin
        overall_found = search_in_file(stdin, "(standard input)", pattern, i_flag, l_flag, n_flag, q_flag, multiple_files);
    } else {
        for (int i = optind; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (!fp) {
                fprintf(stderr, "Couldn't open '%s': No such file or directory\n", argv[i]);
                file_error = 1; // Mark that an error occurred
                continue;
            }
            if (!search_in_file(fp, argv[i], pattern, i_flag, l_flag, n_flag, q_flag, multiple_files)) {
                file_error = 1; // Update error status if no matches found
            }
        }
    }

    // In quiet mode, suppress normal output but consider file errors
    if (q_flag) {
        return file_error ? 1 : 0; // Return 1 if any file errors, otherwise 0
    }

    return overall_found ? 0 : 1; // Return 0 if matches found, 1 if no matches
}

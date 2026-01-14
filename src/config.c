/**
 * @file config.c
 * @brief Implementation of configuration file parsing
 */

#include "config.h"
#include <ctype.h>
#include <dirent.h>
#include <glob.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

int read_config_line(FILE *fp, char *buffer, size_t buffer_size) {
    // Read a line from the file
    if (fgets(buffer, buffer_size, fp) == NULL) {
        return -1;
    }
    // Find the # character and truncate the line there (comments)
    char *comments = strchr(buffer, '#');
    *comments      = '\0';
    // Trim leading and trailing whitespace
    char *start = buffer;
    while (isspace(*start)) {
        start++;
    }

    char *end = buffer + buffer_size - 1;
    while (end > start && isspace(*end)) {
        *end = '\0';
        end--;
    }

    // Return the length of the cleaned line, or -1 on EOF
    if (end - start < 0) {
        return -1;
    }
    return end - start;
    //
    // Hints:
    // - Use fgets() to read a line//fgets(char *str, int size, FILE *stream);
    // - Use strchr() to find the # character//char *strchr(const char *str, int
    // c);
    // - Use isspace() to check for whitespace characters
    // - Don't forget to handle the case where the line is empty after removing
    // comments

    //(void)fp; // Remove these lines when implementing
    (void)buffer;
    (void)buffer_size;
}

int identify_section(const char *line) {
    // Check if the line is "required:" or "optional:"
    // Return 1 for "required:", 2 for "optional:", 0 otherwise
    char *start = (char *) line;
    while (isspace(*start)) {
        start++;
    }

    char *end = (char *) line + strlen(line) - 1;
    while (end > start && isspace(*end)) {
        *end = '\0';
        end--;
    }
    

    if (strcmp(line, "required") == 0) {
        return 1;
    } else if (strcmp(line, "optional") == 0) {
        return 2;
    } else {
        return 0;
    }
    //
    // Hints:
    // - Use strcmp() to compare strings
    // - Make sure to handle any trailing whitespace

    //(void)line; // Remove this line when implementing
}

bool is_indented_line(const char *line) {
    // Check if the line starts with 2 spaces or a tab
    // Return true if indented, false otherwise
    //
    if ((line[0] == ' ' && line[1] == ' ') || line[0] == '\t') {
        return true;
    }
    // Hints:
    // - Check line[0] and line[1] for spaces
    // - Check line[0] for '\t'

    //(void)line; // Remove this line when implementing
    return false;
}

int extract_filename(const char *line, char *filename, size_t filename_size) {
    // Remove the indentation (skip first 2 spaces or 1 tab)
    char *start = (char *) line;
    while (isspace(*start)) {
        start++;
    }

    //  Copy the remaining text to filename buffer
    if (filename_size <= 0) {
        return -1;
    }
    strncpy(filename, start, filename_size - 1);
    filename[filename_size - 1] = '\0';

    // Remove trailing whitespace from filename
    char *end = filename + strlen(filename) - 1;
    while (end > filename && isspace(*end)) {
        *end = '\0';
        end--;
    }

    if (strlen(filename) > 0) {
        return SUCCESS;
    }

    //
    // Hints:
    // - Use strncpy() or strcpy() to copy strings//char *strncpy(char *dest,
    // const char *src, size_t n);
    // - Be careful not to overflow the buffer
    // - Check filename_size before copying

    (void)line;
    (void)filename;
    (void)filename_size;
    return ERROR_INVALID_ARGS;
}

bool is_file(const char *path) {
    // Use stat() to get file information
    struct stat st;
    int         result = stat(path, &st);
    if (result != 0) {
        return false;
    }
    // Check if it's a regular file using S_ISREG() macro
    // Return true if it's a file, false otherwise
    int check_regular = S_ISREG(st.st_mode);
    if (check_regular != 0) {
        return true;
    }
    //
    // Hints:
    // - Include <sys/stat.h>
    // - struct stat st;
    // - stat(path, &st);
    // - S_ISREG(st.st_mode) checks if it's a regular file

    //(void)path; // Remove this line when implementing
    return false;
}

bool has_trailing_slash(const char *path) {
    // Check if the last character of path is '/'
    // Return true if it has trailing slash, false otherwise
    if (path == NULL) {
        return false;
    }
    char *last_character = (char *) path + strlen(path) - 1;
    if (*last_character == '/') {
        return true;
    }
    //
    // Hints:
    // - Use strlen() to get the length
    // - Check path[length-1] == '/'
    // - Handle the case where path is empty

    //(void)path; // Remove this line when implementing
    return false;
}

bool validate_required_path(const char *path) {
    // Check if the path exists (could be file or directory)
    // Use access() or stat() to check existence
    // If it doesn't exist, print an error message and return false
    // If it exists, return true
    if (access(path, F_OK) == 0) {
        return true;
    } else {
        print_error("the path doesn't exist");
        return false;
    }
    //
    // Hints:
    // - access(path, F_OK) returns 0 if path exists
    // - Use fprintf(stderr, ...) to print error messages

    //(void)path; // Remove this line when implementing
}

int list_directory_files(const char *dir_path, char **file_list, int max_files,
                         int *file_count) {
    // Open the directory using opendir()
    DIR           *dir = opendir(dir_path);
    struct dirent *entry;
    // Read each entry using readdir()
    // For each entry:
    //       - Skip "." and ".."
    //       - Build the full path (dir_path + "/" + entry name)
    //       - If it's a file, add it to file_list using add_file_to_list()
    //       - If it's a directory, recursively call list_directory_files()
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0
            || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[MAX_PATH_LENGTH];
        // strcpy (full_path,dir_path);
        // strcat (full_path,"/");
        // strcat (full_path,entry ->d_name);
        // unsafe, may cause buffer overflow !
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path,
                 entry->d_name);

        if (is_file(full_path)) {
            if (add_file_to_list(file_list, file_count, max_files, full_path)
                != SUCCESS) {
                return ERROR_IO;
            }
        } else {
            if (list_directory_files(full_path, file_list, max_files,
                                     file_count)
                != SUCCESS) {
                return ERROR_IO;
            }
        }
    }
    // Close the directory using closedir()
    closedir(dir);
    return SUCCESS;
    //
    // Hints:
    // - Include <dirent.h>
    // - DIR *dir = opendir(dir_path);
    // - struct dirent *entry;
    // - while ((entry = readdir(dir)) != NULL) { ... }
    // - Use stat() to check if entry is a file or directory
    // - Be careful with buffer sizes when building paths

    (void)dir_path;
    (void)file_list;
    (void)max_files;
    (void)file_count;
}

int add_file_to_list(char **file_list, int *file_count, int max_files,
                     const char *file_path) {
    // Check if file_count < max_files (don't overflow the array)
    if (*file_count > max_files) {
        return -1;
    }
    // Allocate memory for the file path string using malloc()
    char *copy = malloc(strlen(file_path) + 1);
    if (copy == NULL) {
        return ERROR_MEMORY_ALLOCATION;
    }

    // Copy the file_path to the allocated memory using strcpy()
    strcpy(copy, file_path);

    // Store the pointer in file_list[*file_count]
    file_list[*file_count] = copy;

    // Increment *file_count
    (*file_count) ++;
    //
    return SUCCESS;
    // Hints:
    // - Use strlen() to get the length of file_path
    // - malloc(strlen(file_path) + 1) to allocate memory (+1 for null
    // terminator)
    // - Check if malloc returns NULL (out of memory)
    // - strcpy(allocated_memory, file_path) to copy the string

    (void)file_list;
    (void)file_count;
    (void)max_files;
    (void)file_path;
}

bool is_glob_pattern(const char *path) {
    // Check if the path contains * or ? characters
    // Return true if it's a glob pattern, false otherwise
    //
    if (strchr(path, '*') != NULL || strchr(path, '?') != NULL) {
        return true;
    }
    // Hints:
    // - Use strchr() to search for '*' or '?' in the string
    // - if (strchr(path, '*') != NULL || strchr(path, '?') != NULL)

    //(void)path; // Remove this line when implementing
    return false;
}

int expand_glob_pattern(const char *pattern, char **file_list, int max_files,
                        int *file_count) {
    // Use glob() function to expand the pattern
    glob_t results;

    int glob_ret = glob(pattern, 0, NULL, &results);
    if (glob_ret == 0){
        globfree(&results);
        return SUCCESS;
    }
    // For each matching file:
    //       - Add it to file_list using add_file_to_list()
    for (size_t i = 0; i < results.gl_pathc; i++){
        char *match = results.gl_pathv[i];
        if (add_file_to_list(file_list, file_count, max_files, match) != SUCCESS) {
            globfree(&results);
            return ERROR_IO;
        }
    }
    //  Free the glob results using globfree()
    globfree(&results);
    //
    // Hints:
    // - Include <glob.h>
    // - glob_t results;
    // - glob(pattern, 0, NULL, &results);
    // - results.gl_pathc is the number of matches
    // - results.gl_pathv is an array of matched paths
    // - for (size_t i = 0; i < results.gl_pathc; i++) { ... }
    // - Don't forget to call globfree(&results) when done
    // - For optional files, no matches is OK (return SUCCESS)

    (void)pattern;
    (void)file_list;
    (void)max_files;
    (void)file_count;
    return SUCCESS;
}

int parse_config_file(const char *config_path, char **file_list, int max_files,
                      int *file_count) {
    // Open the config file using fopen()
    FILE *fp = fopen(config_path, "r");
    // Initialize current_section to track if we're in required/optional
    char buffer[MAX_PATH_LENGTH];
    int  current_section = 0;
    // Read the file line by line using read_config_line()
    // int read_config_line(FILE *fp, char *buffer, size_t buffer_size)
    // For each line:
    //       - Check if it's a section header using identify_section()
    //       - If it's a section header, update current_section
    //       - If it's an indented line:
    //         * Extract the filename using extract_filename()
    //         * If in required section:
    //           - Validate the path exists using validate_required_path()
    //           - If it fails validation, return error
    //           - If it's a directory, use list_directory_files()
    //           - If it's a file, use add_file_to_list()
    //         * If in optional section:
    //           - Check if it's a glob pattern using is_glob_pattern()
    //           - If glob, use expand_glob_pattern()
    //           - If not glob and exists, add to list
    //           - If doesn't exist, skip (no error)

    while (read_config_line(fp, buffer, sizeof(buffer)) != -1) {
        if (identify_section(buffer) != 0) {
            current_section = identify_section(buffer);
            continue;
        }
        if (is_indented_line(buffer)) {
            char filename[MAX_PATH_LENGTH];
            if (extract_filename(buffer, filename, sizeof(filename))
                != SUCCESS) {
                print_error("Warning: Invalid filename format");
                continue;
            }
            if (current_section == 1) {//If in required section:
                if (!validate_required_path(filename)) {
                    print_error("Error: fails validation");
                    return ERROR_INVALID_ARGS;
                }
                if (is_file(filename)) {
                    if (add_file_to_list(file_list, file_count, max_files, filename)!= SUCCESS) {
                        print_error("fail to add file to list");
                        fclose(fp);
                        return ERROR_IO;
                    } 
                }else {
                    if (list_directory_files(filename, file_list, max_files, file_count) != SUCCESS) {
                        print_error("fail to list directory files");
                        fclose(fp);
                        return ERROR_IO;
                    }
                }
            }
            if (current_section == 2){//If in optional section:
                if(is_glob_pattern(filename)){
                    if (expand_glob_pattern(filename, file_list,max_files,file_count) != SUCCESS){
                        print_error("fail to expand global pattern");
                        continue;
                    }

                }else if (validate_required_path(filename)){
                    if (add_file_to_list(file_list, file_count, max_files, filename)!= SUCCESS) {
                        print_error("fail to add file to list");
                        fclose(fp);
                        return ERROR_IO;
                    } 
                }else{
                    continue;
                }
            }
        }
    }

    // Close the file using fclose()
    fclose(fp);
    //  Return SUCCESS if everything worked
    return SUCCESS;
    //
    // This is the main function that coordinates all the other functions!
    //
    // Hints:
    // - FILE *fp = fopen(config_path, "r");
    // - Use a char buffer[MAX_PATH_LENGTH] for reading lines
    // - Keep track of which section you're in (0=none, 1=required, 2=optional)
    // - Handle errors at each step and clean up properly
    // - Print helpful error messages when things go wrong

    (void)config_path;
    (void)file_list;
    (void)max_files;
    (void)file_count;
    return ERROR_FILE_NOT_FOUND;
}

void free_file_list(char **file_list, int file_count) {
    // Loop through the file_list array
    // Free each string using free()
    // Set each pointer to NULL after freeing (good practice)
    for (int i = 0; i < file_count; i++) {
        free(file_list[i]);
        file_list[i] = NULL;
    }

    //
    // Hints:
    // - for (int i = 0; i < file_count; i++) { ... }
    // - free(file_list[i]);
    // - file_list[i] = NULL;

    (void)file_list;
    (void)file_count;
}

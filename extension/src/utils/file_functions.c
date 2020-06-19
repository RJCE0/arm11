#include "extension.h"

#define MAX_QUESTION_SIZE 5

quizNode *create_quiz_node(char **quizQuestion) {
    quizNode *result = (quizNode *) malloc(sizeof(quizNode));
    result->next = NULL;
    result->quizQuestion = quizQuestion;
    return result;
}

void free_node(quizNode *curr) {
    for (int i = 0; i < MAX_QUESTION_SIZE; i++) {
        free(curr->quizQuestion[i]);
    }
    free(curr->quizQuestion);
    free(curr);
}


bool check_answers(char **quizQuestion) {
    // returns false if question or any of first two answers are blank
    for (int i = 0; i < MAX_QUESTION_SIZE - 2; i++) {
        if (strcmp(quizQuestion[i], "") == 0) {
            return false;
        }
    }
    return true;
}

char **convert_questions(char **quizQuestion) {
    char *temp = malloc(100 * sizeof(char));
    strcpy(temp, quizQuestion[0]);
    strcpy(quizQuestion[0], "Q:");
    strcat(quizQuestion[0], temp);
    for (int i = 1; i < 5; i++) {
        if (strcmp(quizQuestion[i], "") == 0) {
            break;
        }
        strcpy(temp, quizQuestion[i]);
        strcpy(quizQuestion[i], "A:");
        strcat(quizQuestion[i], temp);
    }
    free(temp);
    return quizQuestion;
}

quizNode *insert_quiz_node(quizNode *start, char **quizQuestion, bool *valid) {
    if (!check_answers(quizQuestion)) {
        *valid = false;
        return NULL;
    }
    quizNode *newNode = create_quiz_node(convert_questions(quizQuestion));
    if (!start) {
        start = newNode;
        return start;
    }
    while (start->next) {
        start = start->next;
    }
    start->next = newNode;
    return newNode;
}

char *convert_file_name(char *fileName) {
    char *directoryName = "src/quizzes/";
    char *fileExtension = ".txt";
    char *finalDirectory = malloc(100 * sizeof(char));
    strcpy(finalDirectory, directoryName);
    strcat(finalDirectory, fileName);
    free(fileName);
    strcat(finalDirectory, fileExtension);
    return finalDirectory;
}

void write_file(quizNode *start, const char *fileName) {
    quizNode *curr = start;
    quizNode *prev;
    char *newFileName = convert_file_name(fileName);
    FILE *f;
    f = fopen(newFileName, "w");
    free(newFileName);
    while (curr) {
        for (int i = 0; i < 5; i++) {
            if (strcmp(curr->quizQuestion[i], "") == 0) {
                break;
            }
            fputs(curr->quizQuestion[i], f);
            fputs("\n", f);
        }
        prev = curr;
        curr = curr->next;
        free_node(prev);
    }
    fclose(f);
}

char **get_all_files(int *size) {
    char **fileNames = (char **) malloc(sizeof(char *));
    DIR *d;
    int fileCount = 0;
    struct dirent *dir;
    d = opendir("src/quizzes");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                fileNames = (char **) realloc(fileNames,
                                              (1 + fileCount) * sizeof(char *));
                fileNames[fileCount] = (char *) malloc(100 * sizeof(char));
                strcpy(fileNames[fileCount], dir->d_name);
                fileCount += 1;
            }
        }
        closedir(d);
    }
    *size = fileCount;
    return fileNames;
}

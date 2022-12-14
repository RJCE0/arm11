#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifndef GENERATE_TAGS
#define GENERATE_TAGS

typedef struct {
    char *sentence;
    int totalWords;
    char **wordList;
    int totalTags;
} questionBreakdown;

void free_question_breakdown(questionBreakdown *breakdown) {
    for (int i = 0; i < breakdown->totalWords; i++) {
        if (breakdown->wordList[i]) {
            free(breakdown->wordList[i]);
        }
    }
    free(breakdown->wordList);
}

void split_on_spaces(questionBreakdown *breakdown) {
    int count = 0;
    char *word = strtok(breakdown->sentence, " ");
    while (word) {
        breakdown->wordList = (char **) realloc(breakdown->wordList,
                                                (count + 1) * sizeof(char *));
        breakdown->wordList[count] = malloc(20 * sizeof(char));
        strcpy(breakdown->wordList[count], word);
        count++;
        word = strtok(NULL, " ");
    }
    breakdown->totalWords = count;
}

#define IS_NUMERICAL(x) (x > 47 && x < 58)
#define IS_ALPHA(x) (x > 96 && x < 123)
#define IS_DASH(x) (x == 45)

void remove_non_alphanumeric(questionBreakdown *breakdown) {
    int characterCount = 0;
    for (int i = 0; i < breakdown->totalWords; ++i) {
        if (!breakdown->wordList) {
            continue;
        }
        for (int j = 0; breakdown->wordList[i][j]; ++j) {
            if (IS_ALPHA(breakdown->wordList[i][j])
                || IS_NUMERICAL(breakdown->wordList[i][j])
                || IS_DASH(breakdown->wordList[i][j])) {
                breakdown->wordList[i][characterCount] = breakdown->wordList[i][j];
                characterCount++;
            }
        }
        breakdown->wordList[i][characterCount] = '\0';
        characterCount = 0;
    }
}

void remove_stop_words(questionBreakdown *breakdown) {
    FILE *stopWordFile;
    stopWordFile = fopen("english.txt", "r");
    if (!stopWordFile) {
        exit(EXIT_FAILURE);
    }
    char str[20];
    for (int i = 0; i < breakdown->totalWords; ++i) {
        if (!breakdown->wordList[i]) {
            --i;
            continue;
        }
        char *str2;
        while (fgets(str, 20, stopWordFile)) {
            str2 = strtok(str, "\n");
            if (strcmp(str2, breakdown->wordList[i]) == 0) {
                *breakdown->wordList[i] = '\0';
            }
        }
        fseek(stopWordFile, 0, SEEK_SET);
    }
    fclose(stopWordFile);
}

void lowercase_all(questionBreakdown *breakdown) {
    for (int i = 0; i < breakdown->totalWords; ++i) {
        if (*breakdown->wordList[i]) {
            for (int j = 0; breakdown->wordList[i][j] != '\0'; ++j) {
                breakdown->wordList[i][j] = tolower(breakdown->wordList[i][j]);
            }
        }
    }
}

char *generate_tags_string(questionBreakdown *breakdown) {
    int count = 1;
    char *strStart = "Automatically generated tags: ";
    char *tags = (char *) malloc(511 * sizeof(char));
    tags = strcpy(tags, strStart);
    for (int i = 0; i < breakdown->totalWords; ++i) {
        if (*breakdown->wordList[i]) {
            tags = strcat(tags, breakdown->wordList[i]);
            tags = strcat(tags, ", ");
            count++;
        }
    }
    free_question_breakdown(breakdown);
    return tags;
}

char *get_tags(char *sentence) {
    questionBreakdown *breakdown = (questionBreakdown *) malloc(
            sizeof(*breakdown));
    breakdown->sentence = sentence;
    breakdown->wordList = malloc(sizeof(char *));
    split_on_spaces(breakdown);
    lowercase_all(breakdown);
    remove_non_alphanumeric(breakdown);
    remove_stop_words(breakdown);
    return generate_tags_string(breakdown);
}

#endif

#ifndef MULTIPLE_CHOICE
#define MULTIPLE_CHOICE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

typedef struct {
    char *que;
    char **answers;
    int answerNum;
} quest;

typedef struct linked node;

struct linked {
    node *next;
    node *prev;
    quest *question;
};

bool check_question(const char *str) {
    char *ptr = "Q:";
    while (*ptr) {
        if (*ptr++ != *str++) {
            return false;
        }
    }
    return true;
}

node *create_node(char *str) {
    quest *value = (quest *) malloc(sizeof(quest));
    if (!value) {
        perror("Error while creating node...");
    }
    value->que = (char *) malloc(511 * sizeof(char));
    strcpy(value->que, str);
    value->answerNum = 0;
    value->answers = (char **) malloc(1 * sizeof(char *));
    node *newNode = (node *) malloc(sizeof(node));
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->question = value;
    return newNode;

}

void linked_question(node *curr, node *next) {
    next->prev = curr;
    if (!curr) {
        return;
    }
    curr->next = next;
}

void add_answers(quest *curr, char *ans) {
    curr->answers = (char **) realloc(curr->answers,
                                      (curr->answerNum + 1) * sizeof(char *));
    curr->answers[curr->answerNum] = (char *) malloc(511 * sizeof(char));
    if (!curr->answers[curr->answerNum]) {
        perror("unable to allocate");
    }
    strcpy(curr->answers[curr->answerNum], ans);
    curr->answerNum += 1;
}

void remove_current(node *curr) {
    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
}

void free_quest(quest *q) {
    for (int i = 0; i < q->answerNum; i++) {
        free(q->answers[i]);
    }
    free(q->answers);
    free(q->que);
    free(q);
}

node *get_left(node *curr) {
    remove_current(curr);
    node *result = curr->prev;
    free_quest(curr->question);
    free(curr);
    return result;
}

node *get_right(node *curr) {
    remove_current(curr);
    node *result = curr->next;
    free_quest(curr->question);
    free(curr);
    return result;
}

void free_nodes(node *curr) {
    node *currNode = curr;
    node *last;
    while (currNode->next) {
        last = currNode;
        currNode = last->next;
        currNode->prev = last->prev;
        free_quest(last->question);
        free(last);
    }
    while (currNode->prev) {
        last = currNode;
        currNode = last->prev;
        currNode->next = last->next;
        free_quest(last->question);
        free(last);
    }
    free_quest(currNode->question);
    free(currNode);
}

node *read_file(char *fileName, int *questionNum) {
    FILE *file;
    file = fopen(fileName, "r");
    if (file == NULL) {
        printf("file not found, exiting...\n");
    }
    node *curr = NULL;
    int line = 0;
    char str[511];
    while (fgets(str, 511, file)) {
        if (check_question(str)) {
            node *value = create_node(str + 2);
            linked_question(curr, value);
            curr = value;
            *questionNum += 1;
        } else {
            add_answers(curr->question, str + 2);
        }
        line++;
    }
    fclose(file);
    return curr;
}

void print_answers(quest *curr) {
    for (int i = 0; i < curr->answerNum; i++) {
        printf("Answer%d:%s", i + 1, curr->answers[i]);
    }
}

node *initialise_questions(int *maxQuestions, char *fileName) {
    int questionNum = 0;
    char *directory = (char *) malloc(50 * sizeof(char));
    strcpy(directory, "src/quizzes/");
    strcat(directory, fileName);
    node *curr = read_file(directory, &questionNum);
    free(directory);
    for (int i = 0; i < questionNum / 2; i++) {
        curr = curr->prev;
    }
    *maxQuestions = questionNum / 2 + questionNum % 2;
    return curr;
}

#endif

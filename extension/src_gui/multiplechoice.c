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


bool check_question(const char *str){
  char *ptr = "Q:";
  while (*ptr) {
    if (*ptr++ != *str++) {
      return false;
    }
  }
  return true;
}

node *create_node(char *str){
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

void linked_question(node *curr, node *next){
  next->prev = curr;
  if (!curr) {
    return;
  }
  curr->next = next;
}

void add_answers(quest *curr, char *ans){
  curr->answers = (char **) realloc(curr->answers, (curr->answerNum + 1) * sizeof(char *));
  curr->answers[curr->answerNum] = (char *) malloc(511 * sizeof(char));
  if (!curr->answers[curr->answerNum]) {
      perror("unable to allocate");
  }
  strcpy(curr->answers[curr->answerNum], ans);
  curr->answerNum += 1;
}

void remove_current(node *curr){
  curr->prev->next = curr->next;
  curr->next->prev = curr->prev;
}

void free_quest(quest *q){
    for (int i = 0; i < q->answerNum; i++) {
        free(q->answers[i]);
    }
    free(q->answers);
    free(q->que);
    free(q);
}

node *get_left(node *curr){
  remove_current(curr);
  node *result = curr->prev;
  free_quest(curr->question);
  free(curr);
  return result;
}

node *get_right(node *curr){
  remove_current(curr);
  node *result = curr->next;
  free_quest(curr->question);
  free(curr);
  return result;
}

void free_nodes(node *curr){
    node *vamos = curr;
    node *last;
    while (vamos->next) {
        last = vamos;
        vamos = last->next;
        vamos->prev = last->prev;
        free_quest(last->question);
        free(last);
    }
    while (vamos->prev){
        last = vamos;
        vamos = last->prev;
        vamos->next = last->next;
        free_quest(last->question);
        free(last);
    }
    free_quest(vamos->question);
    free(vamos);
}

node *read_file(char *fileName, int *questionNum){
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
        add_answers(curr->question, str + 2);      }
      line++;
  }
  fclose(file);
  return curr;
}

void print_answers(quest *curr){
  for (int i = 0; i < curr->answerNum; i++) {
    printf("Answer%d:%s", i + 1, curr->answers[i]);
  }
}

node *initialise_questions(int *maxQuestions, char *fileName){
  int questionNum = 0;
  char *directory = (char *) malloc(50*sizeof(char));
  strcpy(directory, "src_gui/quizzes/");
  strcat(directory, fileName);
  printf("file:%s\n", directory);
  node *curr = read_file(directory, &questionNum);
  free(directory);
  for (int i = 0; i < questionNum/2; i++) {
    curr = curr->prev;
  }
  *maxQuestions = questionNum/2 + questionNum % 2;
  return curr;
}

char **get_all_files(int *size){
    char **fileNames = (char **) malloc(sizeof(char *));
    DIR *d;
    int fileCount = 0;
	struct dirent *dir;
	d = opendir("src_gui/quizzes");
  	if (d) {
    	while ((dir = readdir(d)) != NULL) {
			if (dir->d_type == DT_REG){
                fileNames = (char **) realloc(fileNames, (1 + fileCount) *sizeof(char *));
                fileNames[fileCount] = (char *) malloc(100 * sizeof(char));
                strcpy(fileNames[fileCount], dir->d_name);
				printf("%s\n", dir->d_name);
                fileCount += 1;
			}
    	}
  	closedir(d);
  	}
    *size = fileCount;
  	return fileNames;
}

/*
int main (int argc, char **argv) {
    get_all_files();
}
*/
/*
int main(int argc, char **argv) {
  int questionNum = 0;
  node *curr = read_file(argv[1], &questionNum);
  for (int i = 0; i < questionNum/2; i++) {
    curr = curr->prev;
  }
  printf("Question b4-%s", curr->question->que);
  printf("Next question after-%s", curr->next->question->que);
  curr = get_right(curr);
  printf("Question after-%s", curr->question->que);
  print_answers(curr->question);
  if (!curr->next) {
    printf("next is null\n");
  }
  printf("Prev question-%s", curr->prev->question->que);
  print_answers(curr->prev->question);
  return 0;
}

*/

#include "extension.h"

quizNode *create_quiz_node(char **quizQuestion) {
	quizNode *result = (quizNode *) malloc(sizeof(quizNode));
	result->next = NULL;
	result->quizQuestion = quizQuestion;
	return result;
}

void free_node(quizNode *curr){
	for (int i = 0; i < 5; i++) {
		free(curr->quizQuestion[i]);
	}
	free(curr->quizQuestion);
	free(curr);
}

bool check_answers(char **quizQuestion){
	for (int i = 0; i < 3; i++) {
		if (strcmp(quizQuestion[i], "") == 0) {
			return false;
		}
	}
	return true;
}

char **convert_questions(char **quizQuestion){
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

void insert_quiz_node(quizNode *start, char **quizQuestion, bool *valid){
	if (!check_answers(quizQuestion)) {
		*valid = false;
		return;
	}
	quizNode *newNode = create_quiz_node(convert_questions(quizQuestion));
	while (start->next) {
		start = start->next;
	}
	start->next = newNode;
}

char *convert_file_name(const char *fileName){
	printf("og filename:%s\n", fileName);
    char *directoryName = "src_gui/quizzes/";
    char *fileExtension = ".txt";
    char *finalDirectory = malloc(100 * sizeof(char));
	strcpy(finalDirectory, directoryName);
    strcat(finalDirectory, fileName);
    strcat(finalDirectory, fileExtension);
	return finalDirectory;
}

void write_file(quizNode *start, const char *fileName){
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

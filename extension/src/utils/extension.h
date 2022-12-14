#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdbool.h>
#include "multiple_choice.c"
#include "generate_tags.c"
#include <stdio.h>
#include <time.h>

#ifndef EXTENSION
#define EXTENSION

#define MAX_CHECKBOX_ANSWERS 14

typedef struct singleLinked quizNode;

struct singleLinked {
    quizNode *next;
    char **quizQuestion;
};

typedef struct {
    char *fileName;
    quizNode *start;
} fileState;

typedef struct {
    char *queFile;
    char *ansFile;
    int *answer;
    char *question;
    int maxAns;
} image;

typedef struct {
    GtkWidget *stack;
    node *curNode;
    int quizScore;
    int maxQuestions;
    int currentQuestion;
    int *guesses;
    GtkWidget *finalScoreLabel;
    image *images;
    fileState *addQuest;
} data;

int check_image_score(int *guesses, int *imAnswers);

const char *get_label(GtkWidget *button);

void on_checkbox_toggle(GtkWidget *togglebutton, data *myData);

// called when window is closed
void on_window_main_destroy(void);

void go_to_home_page(GtkWidget *whatever, data *myData);

void go_to_question_page(GtkWidget *whatever, data *myData);

void go_to_about_us_page(GtkWidget *whatver, data *myData);

void go_to_add_quiz_page(GtkWidget *whatver, data *myData);

void go_to_successful_added_quiz_page(GtkWidget *whatver, data *myData);

char *int_to_string(int i);

void go_to_final_screen(GtkWidget *widget, data *myData);

void go_to_picQ1Ans(GtkWidget *widget, data *myData);

void go_to_picQ2Ans(GtkWidget *widget, data *myData);

void go_to_picQ1(GtkWidget *whatever, data *myData);

void go_to_picQ2(GtkWidget *whatever, data *myData);

void go_to_correct_answer(GtkWidget *whatever, data *myData);

void go_to_wrong_answer(GtkWidget *widget, data *myData);

GCallback check_answer(int i);

int *randomise_questions(int size);

void set_question(data *myData);

void allocate_new_question(data *myData);

void begin_add_quiz(GtkWidget *button, data *myData);

void add_question(GtkButton *button, data *myData);

void finish_quiz_build(GtkWidget *button, data *myData);

void begin_quiz(GtkWidget *widget, data *myData);

void quiz_selector(GtkWidget *whatever, data *myData);

void open_blm_site(GtkWidget *whatever, data *myData);

void advance_right_question(GtkWidget *button, data *myData, quest *question);

void advance_left_question(GtkWidget *button, data *myData, quest *question);

void on_final_screen_quit_clicked(GtkWidget *button, data *myData);

void free_all(data *d);

void add_question_amended(GtkWidget *widget, data *myData);

void add_question_first(GtkWidget *widget, data *myData);

void finish_quiz(GtkWidget *widget, data *myData);

#endif

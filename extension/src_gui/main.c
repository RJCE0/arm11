#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdbool.h>
#include "multiplechoice.c"
#include "generatetags.c"
#include <stdio.h>
#include <time.h>

#define MAX_CHECKBOX_ANSWERS 18

typedef struct {
    GtkWidget *tickBox1;
    GtkWidget *tickBox2;
    GtkWidget *tickBox3;
    GtkWidget *tickBox4;
    GtkWidget *tickBox5;
    GtkWidget *tickBox6;
    GtkWidget *tickBox7;
    GtkWidget *tickBox8;
    GtkWidget *tickBox9;
} tickBoxes;

typedef struct {
    GtkWidget *stack;
    GtkWidget *questionLabel;
    GtkWidget *scoreLabelFromWrong;
    GtkWidget *scoreLabelFromRight;
    GtkWidget *tagsLabel;
    GtkWidget *answerA;
    GtkWidget *answerB;
    GtkWidget *answerC;
    GtkWidget *answerD;
    GtkWidget *picQ1;
    GtkWidget *picQ1Ans;
    GtkWidget *picQ2;
    GtkWidget *picQ2Ans;
    tickBoxes *q1Boxes;
    tickBoxes *q2Boxes;
    node *curNode;
    int quizScore;
    int maxQuestions;
    int currentQuestion;
    int *guesses;
    int *imAns1;
    int *imAns2;
    GtkWidget *finalScoreLabel;
    GtkWidget *imScore1;
    GtkWidget *imScore2;
    GtkWidget *newQuestion;
    GtkWidget *newAnswerA;
    GtkWidget *newAnswerB;
    GtkWidget *newAnswerC;
    GtkWidget *newAnswerD;
    node *curNode;
    int quizScore;
    int numAddedQuestions;
    questionToAdd *addedQuestions;
} data;

typedef struct {
    char *question;
    char *answerA;
    char *answerB;
    char *answerC;
    char *answerD;
} questionToAdd;

int check_im_score(int *guesses, int *imAnswers) {
    int score = 0;
    for (int i = 0; i < 9; i++) {
        printf("guess:%d answer:%d\n", guesses[i], imAnswers[i]);
        if (guesses[i]) {
            if (guesses[i] == imAnswers[i]) {
                score += 1;
            } else {
                score -= 1;
            }
        }
    }
    if (score < 0) {
        score = 0;
    }
    return score;
}

void on_checkbox_toggle(GtkToggleButton *togglebutton, data *myData){
    int num = atoi(gtk_widget_get_name(GTK_WIDGET(togglebutton)));
    if (gtk_toggle_button_get_active(togglebutton)) {
        myData->guesses[num - 1] = 1;
        g_print("Option %d is Checked\n", num);
    }
    else {
        myData->guesses[num - 1] = 0;
        g_print("Option %d is Unchecked\n", num);
    }
}
// called when window is closed
void on_window_main_destroy(void) {
    gtk_main_quit();
}

void go_to_home(GtkWidget *whatever, data *myData) {
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "home_page");
}

void go_to_question_page(GtkWidget *whatever, data *myData) {
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "question_page");
}

void go_to_about_us(GtkWidget *whatver, data *myData) {
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "about_us_page");
}

char *int_to_string(int i) {
    char *score = calloc(5, sizeof(char));
    sprintf(score, "%d", i);
    return score;
}

void go_to_final_screen(GtkWidget *widget, data *myData) {
    char *str = malloc(100 * sizeof(char));
    if (!str) {
        fprintf(stderr,
        "A memory allocation error has occured while printing out final string. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    strcpy(str, "Well done on finishing the quiz! Your final score was: ");
    strcat(str, int_to_string(myData->quizScore));
    strcat(str, " / ");
    strcat(str, int_to_string(myData->maxQuestions + MAX_CHECKBOX_ANSWERS));
    gtk_label_set_text((GtkLabel *) myData->finalScoreLabel, str);
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "final_screen");
}

void go_to_picQ1(GtkWidget *whatever, data *myData) {
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "PicQ1");
}

void go_to_picQ1Ans(GtkWidget *whatever, data *myData) {
    int score1 = check_im_score(myData->guesses, myData->imAns1);
    myData->quizScore += score1;
    gtk_label_set_text((GtkLabel *) myData->imScore1, int_to_string(score1));
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "PicQ1Ans");
}

void go_to_picQ2(GtkWidget *whatever, data *myData) {
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "PicQ2");
}

void go_to_picQ2Ans(GtkWidget *whatever, data *myData) {
    int score2 = check_im_score(myData->guesses, myData->imAns2);
    myData->quizScore += score2;
    gtk_label_set_text((GtkLabel *) myData->imScore2, int_to_string(score2));
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "PicQ2Ans");
}

void go_to_correct_answer(GtkWidget *whatever, data *myData) {
    char *str = malloc(100 * sizeof(char));
    if (!str) {
        fprintf(stderr,
                "An error has occured while printing the correct answer. Exiting...");
        exit(EXIT_FAILURE);
    }
    strcpy(str, "Congratulations on getting the correct answer: ");
    strcat(str, myData->curNode->question->answers[0]);
    gtk_label_set_text((GtkLabel *) myData->scoreLabelFromRight, str);
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "correct_answer_page");
}

void go_to_wrong_answer(GtkWidget *widget, data *myData) {
    char *str = malloc(100 * sizeof(char));
    if (!str) {
        fprintf(stderr,
         "An error has occured while printing the answer you got wrong. Exiting...");
        exit(EXIT_FAILURE);
    }
    strcpy(str, "Correct answer is: ");
    strcat(str, myData->curNode->question->answers[0]);
    gtk_label_set_text((GtkLabel *) myData->scoreLabelFromWrong, str);
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "wrong_answer_page");
}

GCallback check_answer(int i) {
    if (i == 0) {
        return (GCallback) & go_to_correct_answer;
    }
    return (GCallback) & go_to_wrong_answer;
}

int *randomise_questions(int size) {
    int randArr[size];
    static int *array;
    array = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        randArr[i] = i;
    }
    srand(time(0));
    for (int i = 0; i < size; i++) {
        int r = i + rand() % (size - i);
        array[i] = randArr[r];
        randArr[r] = randArr[i];
    }
    return array;
}

void set_question(data *myData) {
    quest *question = myData->curNode->question;
    gtk_label_set_text((GtkLabel *) myData->questionLabel, question->que);
    int *random = randomise_questions(question->answerNum);
    if (question->answerNum <= 3) {
        gtk_widget_set_sensitive(myData->answerD, false);
        if (question->answerNum == 2) {
            gtk_widget_set_sensitive(myData->answerC, false);
        } else {
            gtk_widget_set_sensitive(myData->answerC, true);
        }
    } else {
        gtk_widget_set_sensitive(myData->answerC, true);
        gtk_widget_set_sensitive(myData->answerD, true);
    }

    gtk_button_set_label((GtkButton *) myData->answerA,
                         question->answers[random[0]]);
    gtk_button_set_label((GtkButton *) myData->answerB,
                         question->answers[random[1]]);

    if (question->answerNum >= 3) {
        gtk_button_set_label((GtkButton *) myData->answerC,
                             question->answers[random[2]]);
        g_signal_connect(myData->answerC, "clicked", check_answer(random[2]),
                         myData);
        if (question->answerNum == 4) {
            gtk_button_set_label((GtkButton *) myData->answerD,
                                 question->answers[random[3]]);
            g_signal_connect(myData->answerD, "clicked",
                             check_answer(random[3]), myData);
        } else {
            gtk_button_set_label((GtkButton *) myData->answerD, "");
        }
    } else {
        gtk_button_set_label((GtkButton *) myData->answerC, "");
        gtk_button_set_label((GtkButton *) myData->answerD, "");
    }
    g_signal_connect(myData->answerA, "clicked", check_answer(random[0]),
                     myData);
    g_signal_connect(myData->answerB, "clicked", check_answer(random[1]),
                     myData);

    char *str = get_tags(question->que);
    gtk_label_set_text((GtkLabel *) myData->tagsLabel, str);
}

void add_question(GtkButton *button, data *myData) {
    myData->numAddedQuestions++;
    myData->questionsAdded = (questionToAdd *) realloc (myData->questionsAdded,
                    (myData->numQuestionsAdded + 1) * sizeof(questionToAdd *));
    myData->questionsAdded = malloc (sizeof(questionToAdd));
    myData->addedQuestions[myData->numAddedQuestions]->question =
                    gtk_entry_get_text((GtkEntry *) myData->newQuestion);
    myData->addedQuestions[myData->numAddedQuestions]->answerA =
                    gtk_entry_get_text((GtkEntry *) myData->newAnswerA);
    myData->addedQuestions[myData->numAddedQuestions]->answerB =
                    gtk_entry_get_text((GtkEntry *) myData->newAnswerB);
    // possible room for error here?
    if (gtk_entry_get_text((GtkEntry *) myData->newAnswerC)) {
        myData->addedQuestions[myData->numAddedQuestions]->answerC =
                        gtk_entry_get_text((GtkEntry *) myData->newAnswerC);
    }
    if (gtk_entry_get_text((GtkEntry *) myData->newAnswerD)) {
        myData->addedQuestions[myData->numAddedQuestions]->answerD =
                        gtk_entry_get_text((GtkEntry *) myData->newAnswerD);
    }
}

void finish_quiz_build(GtkButton *button, data *myData) {
    FILE *newQuizFile;
    newQuizFile = fopen("someNewQuiz", "w");
    for (int i = 0; i <= numAddedQuestions; ++i) {
        fputs(myData->addedQuestions[i]->question, newQuizFile);
        free(myData->addedQuestions[i]->question);
        fputs("\n", newQuizFile);
        fputs(myData->addedQuestions[i]->answerA, newQuizFile);
        free(myData->addedQuestions[i]->answerA);
        fputs("\n", newQuizFile);
        fputs(myData->addedQuestions[i]->answerB, newQuizFile);
        free(myData->addedQuestions[i]->answerB);
        fputs("\n", newQuizFile);
        if (myData->addedQuestions[i]->answerC != NULL) {
            fputs(myData->addedQuestions[i]->answerC, newQuizFile);
            fputs("\n", newQuizFile);
            free(myData->addedQuestions[i]->answerC);
        }
        if (myData->addedQuestions[i]->answerD != NULL) {
            fputs(myData->addedQuestions[i]->answerD, newQuizFile);
            free(myData->addedQuestions[i]->answerD);
            fputs("\n", newQuizFile);
        }
        free(myData->addedQuestions[i]);
    }
    myData->questionsAdded = 0;
}

const char *get_label(GtkWidget *button) {
    return gtk_button_get_label((GtkButton *) button);
}

void begin_quiz(GtkWidget *widget, data *myData) {
    int maxQuestions;
    char *fileName = get_label(widget);
    myData->curNode = initialise_questions(&maxQuestions, fileName);
    myData->maxQuestions = maxQuestions;
    myData->currentQuestion = 0;
    myData->quizScore = 0;
    set_question(myData);
    go_to_question_page(widget, myData);
}

void quiz_selector(GtkWidget *whatever, data *myData){
    if (gtk_stack_get_child_by_name((GtkStack *) myData->stack, "quiz_selector")) {
        gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "quiz_selector");
        return;
    }
    int size;
    char **labels = get_all_files(&size);
    for (int i = 0; i < size; i++) {
        printf("%s\n", labels[i]);
    }
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new ("Pick a Quiz");
    gtk_widget_show(header);
    gtk_box_pack_start((GtkBox *) box, header, FALSE, TRUE, 20);
    for (int i = 0; i < size; i++) {
        GtkWidget *button = gtk_button_new_with_label (labels[i]);
        gtk_box_pack_start((GtkBox *) box, button, TRUE, TRUE, 0);
        gtk_widget_show(button);
        g_signal_connect(button, "clicked", (GCallback) &begin_quiz, myData);
    }
    gtk_stack_add_named((GtkStack *) myData->stack, box, "quiz_selector");
    gtk_widget_show(box);
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "quiz_selector");
}

void reset_checkboxes(GtkToggleButton *togglebutton, data *myData, GtkBuilder *builder){
    for(int i = 1; i <= 9; i++ ){
        GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object(builder, int_to_string(i)));
        gtk_toggle_button_set_active (gtk_widget_get_name((GtkWidget *) int_to_string(i)), false);
    }
}

void open_blm_site(GtkWidget *whatever, data *myData) {
    quiz_selector(whatever, myData);
    //gtk_show_uri_on_window(NULL, "https://blacklivesmatter.com/",
    //                       GDK_CURRENT_TIME, NULL);
}

void advance_right_question(GtkButton *button, data *myData, quest *question) {
    //Moves to harder next question.
    myData->quizScore += 1;
    myData->currentQuestion += 1;
    if (myData->currentQuestion == myData->maxQuestions) {
        go_to_picQ1((GtkWidget *) button, myData);
        return;
    }
    myData->curNode = get_right(myData->curNode);
    set_question(myData);
    go_to_question_page((GtkWidget *) button, myData);
}

void advance_left_question(GtkButton *button, data *myData, quest *question) {
    //Move to easier next question.
    myData->currentQuestion += 1;
    if (myData->currentQuestion == myData->maxQuestions) {
        go_to_picQ1((GtkWidget *) button, myData);
        return;
    }
    myData->curNode = get_left(myData->curNode);
    set_question(myData);
    go_to_question_page((GtkWidget *) button, myData);
}

void q1_initialise_tickboxes(tickBoxes *t, GtkBuilder *builder) {
    if (t) {
        t->tickBox1 = GTK_WIDGET(gtk_builder_get_object(builder, "check1"));
        t->tickBox2 = GTK_WIDGET(gtk_builder_get_object(builder, "check2"));
        t->tickBox3 = GTK_WIDGET(gtk_builder_get_object(builder, "check3"));
        t->tickBox4 = GTK_WIDGET(gtk_builder_get_object(builder, "check4"));
        t->tickBox5 = GTK_WIDGET(gtk_builder_get_object(builder, "check5"));
        t->tickBox6 = GTK_WIDGET(gtk_builder_get_object(builder, "check6"));
        t->tickBox7 = GTK_WIDGET(gtk_builder_get_object(builder, "check7"));
        t->tickBox8 = GTK_WIDGET(gtk_builder_get_object(builder, "check8"));
        t->tickBox9 = GTK_WIDGET(gtk_builder_get_object(builder, "check9"));
    }
}

void q2_initialise_tickboxes(tickBoxes *t, GtkBuilder *builder) {
    if (t) {
        t->tickBox1 = GTK_WIDGET(gtk_builder_get_object(builder, "2ndCheck1"));
        t->tickBox2 = GTK_WIDGET(gtk_builder_get_object(builder, "2ndCheck2"));
        t->tickBox3 = GTK_WIDGET(gtk_builder_get_object(builder, "2ndCheck3"));
        t->tickBox4 = GTK_WIDGET(gtk_builder_get_object(builder, "2ndCheck4"));
        t->tickBox5 = GTK_WIDGET(gtk_builder_get_object(builder, "2ndCheck5"));
        t->tickBox6 = GTK_WIDGET(gtk_builder_get_object(builder, "2ndCheck6"));
        t->tickBox7 = GTK_WIDGET(gtk_builder_get_object(builder, "2ndCheck7"));
        t->tickBox8 = GTK_WIDGET(gtk_builder_get_object(builder, "2ndCheck8"));
        t->tickBox9 = GTK_WIDGET(gtk_builder_get_object(builder, "2ndCheck9"));
    }
}

void on_final_screen_quit_clicked(GtkWidget *button, data *myData) {
    open_blm_site(button, myData);
    on_window_main_destroy();
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;
    gtk_init(&argc, &argv);
    builder = gtk_builder_new_from_file("glade/window_main.glade");
    data *myData = malloc(sizeof(myData));
    tickBoxes *boxes = malloc(sizeof(*boxes));
    tickBoxes *q2boxes = malloc(sizeof(*q2boxes));
    q1_initialise_tickboxes(boxes, builder);
    q2_initialise_tickboxes(q2boxes, builder);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, myData);
    myData->stack = GTK_WIDGET(gtk_builder_get_object(builder, "stack1"));
    myData->questionLabel = GTK_WIDGET(
            gtk_builder_get_object(builder, "question_label"));
    myData->scoreLabelFromWrong = GTK_WIDGET(
            gtk_builder_get_object(builder, "current_score_label_wrong"));
    myData->scoreLabelFromRight = GTK_WIDGET(
            gtk_builder_get_object(builder, "current_score_label_correct"));
    myData->answerA = GTK_WIDGET(
            gtk_builder_get_object(builder, "answer_a_button"));
    myData->answerB = GTK_WIDGET(
            gtk_builder_get_object(builder, "answer_b_button"));
    myData->answerC = GTK_WIDGET(
            gtk_builder_get_object(builder, "answer_c_button"));
    myData->answerD = GTK_WIDGET(
            gtk_builder_get_object(builder, "answer_d_button"));
    myData->tagsLabel = GTK_WIDGET(
            gtk_builder_get_object(builder, "tags_label"));
    myData->quizScore = 0;
    myData->picQ1 = GTK_WIDGET(gtk_builder_get_object(builder, "PicQ1"));
    myData->picQ1Ans = GTK_WIDGET(gtk_builder_get_object(builder, "PicQ1Ans"));
    myData->picQ2 = GTK_WIDGET(gtk_builder_get_object(builder, "PicQ2"));
    myData->picQ2Ans = GTK_WIDGET(gtk_builder_get_object(builder, "PicQ2Ans"));
    myData->q1Boxes = boxes;
    myData->q2Boxes = q2boxes;
    myData->imScore1 = GTK_WIDGET(gtk_builder_get_object(builder, "score1"));
    myData->imScore2 = GTK_WIDGET(gtk_builder_get_object(builder, "score2"));
    int temp1[] = {0, 1, 1, 0, 0, 1, 1, 0, 1};
    myData->imAns1 = temp1;
    int temp2[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    myData->imAns2 = temp2;
    myData->guesses = calloc(9, sizeof(int));
    myData->finalScoreLabel = GTK_WIDGET(
            gtk_builder_get_object(builder, "final_score"));
    g_object_unref(builder);
    gtk_widget_show(window);
    gtk_main();
    printf("finished\n");

    return 0;
}

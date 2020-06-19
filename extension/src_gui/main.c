#include "extension.h"
#include "pages.c"
#include "write_to_file.c"

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

const char *get_label(GtkWidget *button) {
    return gtk_button_get_label((GtkButton *) button);
}

void on_checkbox_toggle(GtkWidget *togglebutton, data *myData){
    int num = atoi(get_label(togglebutton));
    if (gtk_toggle_button_get_active((GtkToggleButton *) togglebutton)) {
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

void go_to_add_quiz_page(GtkWidget *whatver, data *myData) {
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "add_quiz_page");
}

void go_to_successful_added_quiz_page(GtkWidget *whatver, data *myData) {
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "new_quiz_successful_page");
}

char *int_to_string(int i) {
    char *score = calloc(5, sizeof(char));
    sprintf(score, "%d", i);
    return score;
}

void go_to_final_screen(GtkWidget *widget, data *myData) {
    GtkWidget *prev = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);
    char *str = malloc(511 * sizeof(char));
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
    free(str);
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "final_screen");
}

void go_to_picQ1Ans(GtkWidget *widget, data *myData) {
    GtkWidget *prev = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);
    int score1 = check_im_score(myData->guesses, myData->images[0].answer);
    myData->quizScore += score1;
    create_im_answer(0, score1, myData);
    /*gtk_label_set_text((GtkLabel *) myData->imScore1, int_to_string(score1));
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "PicQ1Ans");*/
}

void go_to_picQ2Ans(GtkWidget *widget, data *myData) {
    GtkWidget *prev = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);
    int score2 = check_im_score(myData->guesses, myData->images[1].answer);
    myData->quizScore += score2;
    create_im_answer(1, score2, myData);
}

void go_to_picQ1(GtkWidget *whatever, data *myData) {
    create_image(0, myData);
}


void go_to_picQ2(GtkWidget *widget, data *myData) {
    GtkWidget *prev = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);
    create_image(1, myData);
}

void go_to_correct_answer(GtkWidget *widget, data *myData) {
    GtkWidget *prev = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);
    create_qu_answer(myData, TRUE);
}

void go_to_wrong_answer(GtkWidget *widget, data *myData) {
    GtkWidget *prev = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);
    create_qu_answer(myData, FALSE);
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


void allocate_new_question(data *myData) {
    myData->addedQuestions = (questionToAdd **) realloc (myData->addedQuestions,
                    (myData->numAddedQuestions + 1) * sizeof(questionToAdd *));
    myData->addedQuestions[myData->numAddedQuestions] = malloc (sizeof(questionToAdd));
    myData->addedQuestions[myData->numAddedQuestions]->newQuestionStr = malloc (500 * sizeof(char));
    myData->addedQuestions[myData->numAddedQuestions]->newAnswerAStr = malloc (500 * sizeof(char));
    myData->addedQuestions[myData->numAddedQuestions]->newAnswerBStr = malloc (500 * sizeof(char));
    myData->addedQuestions[myData->numAddedQuestions]->newAnswerCStr = malloc (500 * sizeof(char));
    myData->addedQuestions[myData->numAddedQuestions]->newAnswerDStr = malloc (500 * sizeof(char));
}

/*
void begin_add_quiz(GtkWidget *button, data *myData) {
    gtk_widget_set_sensitive(myData->finishAddingQuizButton, false);
    myData->numAddedQuestions = 0;
    myData->addedQuestions = malloc (sizeof(questionToAdd *));
    go_to_add_quiz_page(button, myData);
    allocate_new_question(myData);
}
*/

void begin_add_quiz(GtkWidget *button, data *myData){
    create_file_name(myData);
}



void free_new_question(questionToAdd *question) {
    free(question->newQuestionStr);
    free(question->newAnswerAStr);
    free(question->newAnswerBStr);
    free(question->newAnswerCStr);
    free(question->newAnswerDStr);
    free(question);
}

void add_question_first(GtkWidget *widget, data *myData){
    GtkWidget *box = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    GList *list = gtk_container_get_children(GTK_CONTAINER(box));
    while (list) {
        if (strcmp(gtk_widget_get_name(GTK_WIDGET(list->data)), "textBox") == 0) {
            printf("fileName assigned:%s\n", gtk_entry_get_text(GTK_ENTRY(list->data)));
            myData->addQuest->fileName = gtk_entry_get_text(GTK_ENTRY(list->data));
        }
        list = list->next;
    }
    gtk_widget_destroy(box);
    create_add_question(myData);
}

void add_question_amended(GtkWidget *widget, data *myData){
    GtkWidget *grid = gtk_widget_get_ancestor(widget, GTK_TYPE_GRID);
    GList *list = gtk_container_get_children(GTK_CONTAINER(grid));
    char **result = malloc(5 * sizeof(char *));
    for (int i = 0; i < 5; i++) {
        result[i] = malloc(100 * sizeof(char));
    }
    int count = 4;
    while (list) {
        if (strcmp(gtk_widget_get_name(GTK_WIDGET(list->data)), "textBox") == 0) {
            strcpy(result[count--], gtk_entry_get_text(GTK_ENTRY(list->data)));
        }
        list = list->next;
    }
    bool valid = true;
    if (!myData->addQuest->start) {
        myData->addQuest->start = create_quiz_node(convert_questions(result));
    } else {
        insert_quiz_node(myData->addQuest->start, result, &valid);
    }
    for (int i = 0; i < 5; i++) {
        printf("%d:%s\n", i, result[i]);
    }
    if (valid) {
        gtk_widget_destroy(gtk_widget_get_ancestor(grid, GTK_TYPE_BOX));
        create_add_question(myData);
        return;
    } else {
        //create_error_screen();
        printf("error\n");
    }
}

void finish_quiz(GtkWidget *widget, data *myData){
    GtkWidget *box = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(box);
    write_file(myData->addQuest->start, myData->addQuest->fileName);
    go_to_home(widget, myData);
}

void add_question(GtkButton *button, data *myData) {

    allocate_new_question(myData);

    strcpy(myData->addedQuestions[myData->numAddedQuestions]->newQuestionStr,
                    gtk_entry_get_text((GtkEntry *) myData->newQuestion));
    strcpy(myData->addedQuestions[myData->numAddedQuestions]->newAnswerAStr,
                    gtk_entry_get_text((GtkEntry *) myData->newAnswerA));
    strcpy(myData->addedQuestions[myData->numAddedQuestions]->newAnswerBStr,
                    gtk_entry_get_text((GtkEntry *) myData->newAnswerB));
    if (strcmp (myData->addedQuestions[myData->numAddedQuestions]->newQuestionStr, "") == 0 ||
        strcmp (myData->addedQuestions[myData->numAddedQuestions]->newAnswerAStr, "") == 0 ||
        strcmp (myData->addedQuestions[myData->numAddedQuestions]->newAnswerBStr, "") == 0) {
            gtk_label_set_text((GtkLabel *) myData->errorAddingQuestionLabel,
            "Invalid quiz question entry! Please try again.");
            return;
    }
    if (gtk_entry_get_text((GtkEntry *) myData->newAnswerC)) {
        strcpy(myData->addedQuestions[myData->numAddedQuestions]->newAnswerCStr,
                        gtk_entry_get_text((GtkEntry *) myData->newAnswerC));
        if (gtk_entry_get_text((GtkEntry *) myData->newAnswerD)) {
            strcpy(myData->addedQuestions[myData->numAddedQuestions]->newAnswerDStr,
                        gtk_entry_get_text((GtkEntry *) myData->newAnswerD));
        }
    }
    gtk_entry_set_text((GtkEntry *) myData->newQuestion, "");
    gtk_entry_set_text((GtkEntry *) myData->newAnswerA, "");
    gtk_entry_set_text((GtkEntry *) myData->newAnswerB, "");
    gtk_entry_set_text((GtkEntry *) myData->newAnswerC, "");
    gtk_entry_set_text((GtkEntry *) myData->newAnswerD, "");
    gtk_label_set_text((GtkLabel *) myData->errorAddingQuestionLabel, "Added!");
    myData->numAddedQuestions++;
    gtk_widget_set_sensitive(myData->finishAddingQuizButton, true);
}

void finish_quiz_build(GtkWidget *button, data *myData) {
    FILE *newQuizFile;
    char *nameOfNewQuiz = gtk_entry_get_text((GtkEntry *) myData->nameOfQuizEntry);
    printf("%s\n", nameOfNewQuiz);
    char *directoryName = "src_gui/quizzes/";
    char *fileExtension = ".txt";
    //char *finalDirectory = malloc (strlen(nameOfNewQuiz) + strlen(directoryName) + strlen(fileExtension));
    char *finalDirectory = malloc(100 * sizeof(char));
    strcpy(finalDirectory, directoryName);
    strcat(finalDirectory, nameOfNewQuiz);
    strcat(finalDirectory, fileExtension);
    newQuizFile = fopen(finalDirectory, "w");
    for (int i = 0; i < myData->numAddedQuestions; ++i) {
        fputs("Q:", newQuizFile);
        fputs(myData->addedQuestions[i]->newQuestionStr, newQuizFile);
        fputs("\n", newQuizFile);
        fputs("A:", newQuizFile);
        fputs(myData->addedQuestions[i]->newAnswerAStr, newQuizFile);
        fputs("\n", newQuizFile);
        fputs("A:", newQuizFile);
        fputs(myData->addedQuestions[i]->newAnswerBStr, newQuizFile);
        fputs("\n", newQuizFile);
        if (myData->addedQuestions[i]->newAnswerCStr != NULL) {
            fputs("A:", newQuizFile);
            fputs(myData->addedQuestions[i]->newAnswerCStr, newQuizFile);
            fputs("\n", newQuizFile);
        }
        if (myData->addedQuestions[i]->newAnswerDStr != NULL) {
            fputs("A:", newQuizFile);
            fputs(myData->addedQuestions[i]->newAnswerDStr, newQuizFile);
            fputs("\n", newQuizFile);
        }
        free(myData->addedQuestions[i]);
    }
    go_to_successful_added_quiz_page(button, myData);
    free(myData->addedQuestions);
    free(finalDirectory);
    go_to_home(button, myData);
}

void begin_quiz(GtkWidget *widget, data *myData) {
    GtkWidget *prev = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);

    int maxQuestions;
    char *fileName = get_label(widget);
    myData->curNode = initialise_questions(&maxQuestions, fileName);
    myData->maxQuestions = maxQuestions;
    myData->currentQuestion = 0;
    myData->quizScore = 0;
    create_question(myData);
}

void quiz_selector(GtkWidget *whatever, data *myData){
    int size;
    char **labels = get_all_files(&size);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new ("Pick a Quiz");
    gtk_widget_show(header);
    gtk_box_pack_start((GtkBox *) box, header, FALSE, TRUE, 20);
    for (int i = 0; i < size; i++) {
        GtkWidget *button = gtk_button_new_with_label (labels[i]);
        gtk_box_pack_start((GtkBox *) box, button, TRUE, TRUE, 0);
        gtk_widget_show(button);
        g_signal_connect(button, "clicked", (GCallback) &begin_quiz, myData);
        free(labels[i]);
    }
    free(labels);
    gtk_stack_add_named((GtkStack *) myData->stack, box, "quiz_selector");
    gtk_widget_show(box);
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "quiz_selector");
}

void open_blm_site(GtkWidget *whatever, data *myData) {
    gtk_show_uri_on_window(NULL, "https://blacklivesmatter.com/",
    GDK_CURRENT_TIME, NULL);
}

void advance_right_question(GtkWidget *button, data *myData, quest *question) {
    GtkWidget *prev = gtk_widget_get_ancestor(button, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);
    //Moves to harder next question.
    myData->quizScore += 1;
    myData->currentQuestion += 1;
    if (myData->currentQuestion == myData->maxQuestions) {
        free_nodes(myData->curNode);
        go_to_picQ1((GtkWidget *) button, myData);
        return;
    }
    myData->curNode = get_right(myData->curNode);
    create_question(myData);
}

void advance_left_question(GtkWidget *button, data *myData, quest *question) {
    GtkWidget *prev = gtk_widget_get_ancestor(button, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);
    //Move to easier next question.
    myData->currentQuestion += 1;
    if (myData->currentQuestion == myData->maxQuestions) {
        free_nodes(myData->curNode);
        go_to_picQ1((GtkWidget *) button, myData);
        return;
    }
    myData->curNode = get_left(myData->curNode);
    create_question(myData);
}

void on_final_screen_quit_clicked(GtkWidget *button, data *myData) {
    on_window_main_destroy();
}

void free_all(data *d){
    free(d->stack);
    free(d->guesses);
    free(d->finalScoreLabel);
    free(d->finishAddingQuizButton);
    free(d->newQuestion);
    free(d->newAnswerA);
    free(d->newAnswerB);
    free(d->newAnswerC);
    free(d->newAnswerD);
    free(d->errorAddingQuestionLabel);
    free(d->nameOfQuizEntry);
}

int main(int argc, char *argv[]) {

    GtkBuilder *builder;
    GtkWidget *window;
    gtk_init(&argc, &argv);
    builder = gtk_builder_new_from_file("glade/window_main.glade");

    data *myData = malloc(sizeof(data));

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, myData);
    myData->stack = GTK_WIDGET(gtk_builder_get_object(builder, "stack1"));
    myData->quizScore = 0;
    myData->finishAddingQuizButton = GTK_WIDGET(gtk_builder_get_object(builder, "finish_adding_quiz_button"));
    myData->newQuestion = GTK_WIDGET(gtk_builder_get_object(builder, "new_question_entry"));
    myData->newAnswerA = GTK_WIDGET(gtk_builder_get_object(builder, "new_answer_a_entry"));
    myData->newAnswerB = GTK_WIDGET(gtk_builder_get_object(builder, "new_answer_b_entry"));
    myData->newAnswerC = GTK_WIDGET(gtk_builder_get_object(builder, "new_answer_c_entry"));
    myData->newAnswerD = GTK_WIDGET(gtk_builder_get_object(builder, "new_answer_d_entry"));
    myData->errorAddingQuestionLabel = GTK_WIDGET(gtk_builder_get_object(builder, "error_add_question_label"));
    myData->nameOfQuizEntry = GTK_WIDGET(gtk_builder_get_object(builder, "name_of_new_quiz_entry"));
    int temp1[] = {0, 1, 1, 0, 0, 1, 1, 0, 1};
    int temp2[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    myData->guesses = calloc(9, sizeof(int));
    myData->finalScoreLabel = GTK_WIDGET(gtk_builder_get_object(builder, "final_score"));
    myData->images = malloc(2 * sizeof(image));
    myData->images[0].queFile = "src/stb_image/Q1.jpg";
    myData->images[0].answer = temp1;
    myData->images[0].question = "Which of these items have a black inventor?";
    myData->images[0].ansFile = "src/stb_image/Q1Ans.jpg";
    myData->images[0].maxAns = 5;
    myData->images[1].queFile = "src/stb_image/Q2.jpg";
    myData->images[1].answer = temp2;
    myData->images[1].question = "Which of these items have a black inventor?";
    myData->images[1].ansFile = "src/stb_image/Q2Ans.jpg";
    myData->images[1].maxAns = 9;
    myData->addQuest = malloc(sizeof(fileState));
    myData->addQuest->start = NULL;
    g_object_unref(builder);
    gtk_widget_show(window);
    gtk_main();
    printf("finished\n");

    return 0;
}

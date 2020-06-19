#include "utils/extension.h"
#include "utils/pages.c"
#include "utils/file_functions.c"

int check_image_score(int *guesses, int *imageAnswers) {
    int score = 0;
    for (int i = 0; i < 9; i++) {
        printf("guess:%d answer:%d\n", guesses[i], imageAnswers[i]);
        if (guesses[i]) {
            if (guesses[i] == imageAnswers[i]) {
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


void on_checkbox_toggle(GtkWidget *toggleButton, data *myData) {
    int num = atoi(gtk_button_get_label((GtkButton *) toggleButton));
    if (gtk_toggle_button_get_active((GtkToggleButton *) toggleButton)) {
        myData->guesses[num - 1] = 1;
        g_print("Option %d is Checked\n", num);
    } else {
        myData->guesses[num - 1] = 0;
        g_print("Option %d is Unchecked\n", num);
    }
}

// called when window is closed
void on_window_main_destroy(void) {
    gtk_main_quit();
}

void go_to_home_page(GtkWidget *widget, data *myData) {
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "home_page");
}

void go_to_about_us_page(GtkWidget *widget, data *myData) {
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "about_us_page");
}

void go_to_successful_added_quiz_page(GtkWidget *widget, data *myData) {
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "new_quiz_successful_page");
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
    char *quizScore = int_to_string(myData->quizScore);
    strcat(str, quizScore);
    free(quizScore);
    strcat(str, " / ");
    char *maxQuest = int_to_string(myData->maxQuestions + MAX_CHECKBOX_ANSWERS);
    strcat(str, maxQuest);
    free(maxQuest);
    gtk_label_set_text((GtkLabel *) myData->finalScoreLabel, str);
    free(str);
    gtk_stack_set_visible_child_name((GtkStack *)myData->stack, "final_screen");
}

char *int_to_string(int i) {
    char *score = calloc(5, sizeof(char));
    sprintf(score, "%d", i);
    return score;
}

void go_to_picQ1Ans(GtkWidget *widget, data *myData) {
    GtkWidget *prev = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);
    int score1 = check_image_score(myData->guesses, myData->images[0].answer);
    myData->quizScore += score1;
    create_im_answer(0, score1, myData);
}

void go_to_picQ2Ans(GtkWidget *widget, data *myData) {
    GtkWidget *prev = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);
    int score2 = check_image_score(myData->guesses, myData->images[1].answer);
    myData->quizScore += score2;
    create_im_answer(1, score2, myData);
}

void go_to_picQ1(GtkWidget *whatever, data *myData) {
    for (int i = 0; i < 9; i++) {
        myData->guesses[i] = 0;
    }
    create_image(0, myData);
}

void go_to_picQ2(GtkWidget *widget, data *myData) {
    GtkWidget *prev = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);
    for (int i = 0; i < 9; i++) {
        myData->guesses[i] = 0;
    }
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

void begin_add_quiz(GtkWidget *button, data *myData) {
    create_file_name(myData);
}

void add_question_first(GtkWidget *widget, data *myData) {
    GtkWidget *box = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    GList *list = gtk_container_get_children(GTK_CONTAINER(box));
    while (list) {
        if (strcmp(gtk_widget_get_name(GTK_WIDGET(list->data)), "textBox") ==
            0) {
            // checks supplied name is not empty
            if (strcmp (gtk_entry_get_text(GTK_ENTRY(list->data)), "") == 0) {
                return;
            }
            // checks filename doesn't match existing quiz
            int numOfQuizzes = 0;
            char **allQuizNames = get_all_files(&numOfQuizzes);
            for (int i = 0; i < numOfQuizzes; ++i) {
                allQuizNames[i][strlen(allQuizNames[i]) - 4] = '\0';
                if (strcmp (gtk_entry_get_text(GTK_ENTRY(list->data)), allQuizNames[i]) == 0) {
                    for (int j = 0; j < numOfQuizzes; ++j) {
                        free(allQuizNames[j]);
                    }
                    free(allQuizNames);
                    return;
                }
            }
            strcpy(myData->addQuest->fileName,
                   gtk_entry_get_text(GTK_ENTRY(list->data)));
        }
        list = list->next;
    }
    gtk_widget_destroy(box);
    create_add_question(myData);
}

void add_question_amended(GtkWidget *widget, data *myData) {
    GtkWidget *grid = gtk_widget_get_ancestor(widget, GTK_TYPE_GRID);
    GList *list = gtk_container_get_children(GTK_CONTAINER(grid));
    char **result = malloc(5 * sizeof(char *));
    for (int i = 0; i < 5; i++) {
        result[i] = malloc(100 * sizeof(char));
    }
    int count = 4;
    while (list) {
        if (strcmp(gtk_widget_get_name(GTK_WIDGET(list->data)), "textBox") ==
            0) {
            strcpy(result[count--], gtk_entry_get_text(GTK_ENTRY(list->data)));
        }
        list = list->next;
    }
    bool valid = true;
    if (!myData->addQuest->start) {
        myData->addQuest->start = insert_quiz_node(myData->addQuest->start,
                                                   result, &valid);
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
        gtk_widget_destroy(gtk_widget_get_ancestor(grid, GTK_TYPE_BOX));
        wrong_add_qu(myData);
    }
}

void finish_quiz(GtkWidget *widget, data *myData) {
    GtkWidget *box = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(box);
    write_file(myData->addQuest->start, myData->addQuest->fileName);
    go_to_home_page(widget, myData);
}

void begin_quiz(GtkWidget *widget, data *myData) {
    GtkWidget *prev = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);
    gtk_widget_destroy(prev);

    int maxQuestions;
    char *fileName = gtk_button_get_label((GtkButton *) widget);
    myData->curNode = initialise_questions(&maxQuestions, fileName);
    myData->maxQuestions = maxQuestions;
    myData->currentQuestion = 0;
    myData->quizScore = 0;
    create_question(myData);
}

void quiz_selector(GtkWidget *whatever, data *myData) {
    int size;
    char **labels = get_all_files(&size);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new("Pick a Quiz");
    gtk_widget_show(header);
    gtk_box_pack_start((GtkBox *) box, header, FALSE, TRUE, 20);
    for (int i = 0; i < size; i++) {
        GtkWidget *button = gtk_button_new_with_label(labels[i]);
        gtk_box_pack_start((GtkBox *) box, button, TRUE, TRUE, 0);
        gtk_widget_show(button);
        g_signal_connect(button, "clicked", (GCallback) & begin_quiz, myData);
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

void free_all(data *d) {
    free(d->guesses);
    free(d->images);
    free(d->addQuest);
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
    int temp1[] = {0, 1, 1, 0, 0, 1, 1, 0, 1};
    int temp2[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    myData->guesses = calloc(9, sizeof(int));
    myData->finalScoreLabel = GTK_WIDGET(
            gtk_builder_get_object(builder, "final_score"));
    myData->images = malloc(2 * sizeof(image));
    myData->images[0].queFile = "src/images/Q1.jpg";
    myData->images[0].answer = temp1;
    myData->images[0].question = "Which of these items have a black inventor?";
    myData->images[0].ansFile = "src/images/Q1Ans.jpg";
    myData->images[0].maxAns = 5;
    myData->images[1].queFile = "src/images/Q2.jpg";
    myData->images[1].answer = temp2;
    myData->images[1].question = "Which of these items have a black inventor?";
    myData->images[1].ansFile = "src/images/Q2Ans.jpg";
    myData->images[1].maxAns = 9;
    myData->addQuest = malloc(sizeof(fileState));
    myData->addQuest->start = NULL;
    myData->addQuest->fileName = malloc(100 * sizeof(char));

    g_object_unref(builder);
    gtk_widget_show(window);

    gtk_main();

    free_all(myData);
    return 0;
}

#include "extension.h"

void create_im_answer(int imageNum, int score, data *myData) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new(myData->images[imageNum].question);
    gtk_widget_show(header);
    gtk_box_pack_start(GTK_BOX(box), header, FALSE, TRUE, 10);
    GtkWidget *image = gtk_image_new_from_file(myData->images[imageNum].ansFile);
    gtk_widget_show(image);
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, TRUE, 20);
    char *scoreMessage = malloc(20 * sizeof(char));
    char *qScore = int_to_string(score);
    strcpy(scoreMessage, "You got ");
    strcat(scoreMessage, qScore);
    free(qScore);
    strcat(scoreMessage, " out of ");
    char *maxScore = int_to_string(myData->images[imageNum].maxAns);
    strcat(scoreMessage, maxScore);
    free(maxScore);
    GtkWidget *message = gtk_label_new(scoreMessage);
    free(scoreMessage);
    gtk_widget_show(message);
    gtk_box_pack_start(GTK_BOX(box), message, FALSE, TRUE, 10);
    GtkWidget *cont = gtk_button_new_with_label("Continue");
    GCallback nextFunc = (GCallback) & go_to_picQ2;
    if (imageNum == 1) {
        nextFunc = (GCallback) & go_to_final_screen;
    }
    g_signal_connect(cont, "clicked", nextFunc, myData);
    gtk_box_pack_start((GtkBox *) box, cont, FALSE, TRUE, 10);
    gtk_widget_show(cont);
    gtk_widget_show(box);
    gtk_stack_add_named((GtkStack *) myData->stack, box, "image_ans");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "image_ans");
}

void create_image(int imageNum, data *myData) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new(myData->images[imageNum].question);
    gtk_widget_show(header);
    gtk_box_pack_start(GTK_BOX(box), header, FALSE, TRUE, 10);
    GtkWidget *image = gtk_image_new_from_file(myData->images[imageNum].queFile);
    gtk_widget_show(image);
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, TRUE, 20);
    for (int i = 0; i < 3; i++) {
        GtkWidget *buttonBox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
        for (int j = 0; j < 3; j++) {
            char *checkbox = int_to_string(3 * i + j + 1);
            GtkWidget *check = gtk_check_button_new_with_label(checkbox);
            free(checkbox);
            g_signal_connect(check, "clicked", (GCallback) & on_checkbox_toggle,
                             myData);
            gtk_widget_show(check);
            gtk_container_add((GtkContainer *) buttonBox, check);
        }
        gtk_box_pack_start(GTK_BOX(box), buttonBox, FALSE, TRUE, 0);
        gtk_widget_show(buttonBox);
    }
    GtkWidget *submit = gtk_button_new_with_label("Submit");
    GCallback nextFunc = (GCallback) & go_to_picQ1Ans;
    if (imageNum == 1) {
        nextFunc = (GCallback) & go_to_picQ2Ans;
    }
    g_signal_connect(submit, "clicked", nextFunc, myData);
    gtk_box_pack_start(GTK_BOX(box), submit, FALSE, TRUE, 10);
    gtk_widget_show(submit);
    gtk_widget_show(box);
    gtk_stack_add_named((GtkStack *) myData->stack, box, "image_que");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "image_que");
}

void create_question(data *myData) {
    quest *question = myData->curNode->question;
    int *random = randomise_questions(question->answerNum);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new(question->que);
    gtk_label_set_line_wrap((GtkLabel *) header, TRUE);
    gtk_widget_show(header);
    gtk_box_pack_start(GTK_BOX(box), header, FALSE, TRUE, 50);
    // The automated tags functionality had bugs at the end :(
    // GtkWidget *tags = gtk_label_new (get_tags(question->que));
    // gtk_label_set_line_wrap ((GtkLabel *) tags, TRUE);
    // gtk_widget_show(tags);
    // gtk_box_pack_start(GTK_BOX(box), tags, FALSE, TRUE, 50);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous((GtkGrid *) grid, TRUE);
    gtk_grid_set_column_homogeneous((GtkGrid *) grid, TRUE);
    for (int i = 0; i < question->answerNum; i++) {
        int x = 0;
        if (i >= 2) {
            x = 1;
        }
        GtkWidget *button = gtk_button_new_with_label(
                question->answers[random[i]]);
        g_signal_connect(button, "clicked", check_answer(random[i]), myData);
        gtk_widget_show(button);
        gtk_grid_attach((GtkGrid *) grid, button, x, i % 2, 1, 1);
    }
    free(random);
    gtk_widget_show(grid);
    gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);
    gtk_widget_show(box);
    gtk_stack_add_named((GtkStack *) myData->stack, box, "questions_test");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "questions_test");
}

void create_qu_answer(data *myData, bool correct) {
    char *title;
    char *mess = malloc(511 * sizeof(char));
    GCallback next;
    if (correct) {
        title = "Correct Answer!";
        strcpy(mess, "Congratulations on getting the right answer: ");
        next = (GCallback) & advance_right_question;
    } else {
        title = "Wrong Answer!";
        strcpy(mess, "The correct answer is: ");
        next = (GCallback) & advance_left_question;
    }
    strcat(mess, myData->curNode->question->answers[0]);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new(title);
    gtk_widget_show(header);
    gtk_box_pack_start(GTK_BOX(box), header, TRUE, TRUE, 0);
    GtkWidget *message = gtk_label_new(mess);
    free(mess);
    gtk_label_set_line_wrap((GtkLabel *) message, TRUE);
    gtk_widget_show(message);
    gtk_box_pack_start(GTK_BOX(box), message, TRUE, TRUE, 0);
    GtkWidget *button = gtk_button_new_with_label("Continue");
    g_signal_connect(button, "clicked", next, myData);
    gtk_widget_show(button);
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
    gtk_widget_show(box);
    gtk_stack_add_named((GtkStack *) myData->stack, box, "answers_test");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "answers_test");
}

void wrong_add_qu(data *myData) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new("Incorrect question format added");
    gtk_widget_show(header);
    gtk_box_pack_start(GTK_BOX(box), header, TRUE, TRUE, 0);
    GtkWidget *button = gtk_button_new_with_label("Return to Menu");
    g_signal_connect(button, "clicked", (GCallback) & go_to_home_page, myData);
    gtk_widget_show(button);
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
    gtk_widget_show(box);
    gtk_stack_add_named((GtkStack *) myData->stack, box, "wrong_add");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "wrong_add");
}

void create_add_question(data *myData) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new("Add new questions below");
    gtk_widget_show(header);
    gtk_box_pack_start((GtkBox *) box, header, FALSE, TRUE, 50);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous((GtkGrid *) grid, TRUE);
    gtk_grid_set_column_homogeneous((GtkGrid *) grid, TRUE);
    for (int i = 0; i < 5; i++) {
        GtkWidget *textBox = gtk_entry_new();
        gtk_widget_set_name(textBox, "textBox");
        gtk_widget_show(textBox);
        gtk_grid_attach(GTK_GRID(grid), textBox, 1, i, 1, 1);
    }
    GtkWidget *q = gtk_label_new("Question:");
    gtk_widget_show(q);
    gtk_grid_attach(GTK_GRID(grid), q, 0, 0, 1, 1);
    GtkWidget *op1 = gtk_label_new("Correct Ans A");
    gtk_widget_show(op1);
    gtk_grid_attach(GTK_GRID(grid), op1, 0, 1, 1, 1);
    GtkWidget *op2 = gtk_label_new("Ans B");
    gtk_widget_show(op2);
    gtk_grid_attach(GTK_GRID(grid), op2, 0, 2, 1, 1);
    GtkWidget *op3 = gtk_label_new("Ans C");
    gtk_widget_show(op3);
    gtk_grid_attach(GTK_GRID(grid), op3, 0, 3, 1, 1);
    GtkWidget *op4 = gtk_label_new("Ans D");
    gtk_widget_show(op4);
    gtk_grid_attach(GTK_GRID(grid), op4, 0, 4, 1, 1);
    GtkWidget *addQ = gtk_button_new_with_label("Add Question");
    g_signal_connect(addQ, "clicked", (GCallback) & add_question_amended,
                     myData);
    gtk_widget_show(addQ);
    gtk_grid_attach(GTK_GRID(grid), addQ, 0, 5, 1, 1);
    GtkWidget *finishQ = gtk_button_new_with_label("Finish Quiz");
    g_signal_connect(finishQ, "clicked", (GCallback) & finish_quiz, myData);
    gtk_widget_show(finishQ);
    gtk_grid_attach(GTK_GRID(grid), finishQ, 1, 5, 1, 1);
    gtk_widget_show(grid);
    gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);
    gtk_widget_show(box);
    gtk_stack_add_named((GtkStack *) myData->stack, box, "add_que");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "add_que");
}

void create_file_name(data *myData) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new("Enter Quiz Name:");
    gtk_widget_show(header);
    gtk_box_pack_start(GTK_BOX(box), header, TRUE, TRUE, 0);
    GtkWidget *textBox = gtk_entry_new();
    gtk_widget_set_name(textBox, "textBox");
    gtk_widget_show(textBox);
    gtk_box_pack_start(GTK_BOX(box), textBox, TRUE, TRUE, 0);
    GtkWidget *cont = gtk_button_new_with_label("Continue");
    g_signal_connect(cont, "clicked", (GCallback) & add_question_first, myData);
    gtk_widget_show(cont);
    gtk_box_pack_start(GTK_BOX(box), cont, TRUE, TRUE, 0);
    gtk_widget_show(box);
    gtk_stack_add_named((GtkStack *) myData->stack, box, "add_que_file");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack,
                                     "add_que_file");
}

void create_home(data *myData) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *begin = gtk_button_new_with_label("Begin Quiz");
    g_signal_connect(begin, "clicked", (GCallback) & quiz_selector, myData);
    gtk_widget_show(begin);
    gtk_box_pack_start(GTK_BOX(box), begin, TRUE, TRUE, 0);
    GtkWidget *add = gtk_button_new_with_label("Add New Quiz");
    g_signal_connect(add, "clicked", (GCallback) & begin_add_quiz, myData);
    gtk_widget_show(add);
    gtk_box_pack_start(GTK_BOX(box), add, TRUE, TRUE, 0);
    GtkWidget *learn = gtk_button_new_with_label(
            "Learn More (will open browser link)");
    g_signal_connect(learn, "clicked", (GCallback) & open_blm_site, myData);
    gtk_widget_show(learn);
    gtk_box_pack_start(GTK_BOX(box), learn, TRUE, TRUE, 0);
    GtkWidget *about = gtk_button_new_with_label("About Us");
    g_signal_connect(about, "clicked", (GCallback) & go_to_about_us_page, myData);
    gtk_widget_show(about);
    gtk_box_pack_start(GTK_BOX(box), about, TRUE, TRUE, 0);
    GtkWidget *quit = gtk_button_new_with_label("Quit");
    g_signal_connect(quit, "clicked", (GCallback) & on_window_main_destroy,
                     myData);
    gtk_widget_show(quit);
    gtk_box_pack_start(GTK_BOX(box), quit, TRUE, TRUE, 0);
    gtk_widget_show(box);
    gtk_stack_add_named((GtkStack *) myData->stack, box, "home");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "home");
}

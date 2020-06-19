#include "extension.h"

void create_im_answer(int imageNo, int score, data *myData){
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new (myData->images[imageNo].question);
    gtk_widget_show(header);
    gtk_box_pack_start(GTK_BOX(box), header, FALSE, TRUE, 10);
    GtkWidget *image = gtk_image_new_from_file (myData->images[imageNo].ansFile);
    gtk_widget_show(image);
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, TRUE, 20);
	char *scoreMessage = malloc(20 * sizeof(char));
    char *qScore = int_to_string(score);
	strcpy(scoreMessage, "You got ");
	strcat(scoreMessage, qScore);
    free(qScore);
	strcat(scoreMessage, " out of ");
    char *maxScore = int_to_string(myData->images[imageNo].maxAns);
	strcat(scoreMessage, maxScore);
    free(maxScore);
	GtkWidget *message = gtk_label_new (scoreMessage);
    free(scoreMessage);
    gtk_widget_show(message);
    gtk_box_pack_start(GTK_BOX(box), message, FALSE, TRUE, 10);
    GtkWidget *cont = gtk_button_new_with_label("Continue");
    GCallback nextFunc = (GCallback) &go_to_picQ2;
    if (imageNo == 1) {
        nextFunc = (GCallback) &go_to_final_screen;
    }
    g_signal_connect(cont, "clicked", nextFunc, myData);
    gtk_box_pack_start((GtkBox *) box, cont, FALSE, TRUE, 10);
    gtk_widget_show(cont);
    gtk_widget_show(box);
    gtk_stack_add_named((GtkStack *) myData->stack, box, "image_ans");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "image_ans");
}

void create_image(int imageNo, data *myData){
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new (myData->images[imageNo].question);
    gtk_widget_show(header);
    gtk_box_pack_start(GTK_BOX(box), header, FALSE, TRUE, 10);
    GtkWidget *image = gtk_image_new_from_file (myData->images[imageNo].queFile);
    gtk_widget_show(image);
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, TRUE, 20);
    for (int i = 0; i < 3; i++) {
        GtkWidget *buttonBox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
        for (int j = 0; j < 3; j++) {
            char *checkbox = int_to_string(3*i + j + 1);
            GtkWidget *check = gtk_check_button_new_with_label(checkbox);
            free(checkbox);
            g_signal_connect(check, "clicked", (GCallback) &on_checkbox_toggle, myData);
            gtk_widget_show(check);
            gtk_container_add ((GtkContainer *) buttonBox, check);
        }
        gtk_box_pack_start(GTK_BOX(box), buttonBox, FALSE, TRUE, 0);
        gtk_widget_show(buttonBox);
    }
    GtkWidget *submit = gtk_button_new_with_label("Submit");
    GCallback nextFunc = (GCallback) &go_to_picQ1Ans;
    if (imageNo == 1) {
        nextFunc = (GCallback) &go_to_picQ2Ans;
    }
    g_signal_connect(submit, "clicked", nextFunc, myData);
    gtk_box_pack_start(GTK_BOX(box), submit, FALSE, TRUE, 10);
    gtk_widget_show(submit);
    gtk_widget_show(box);
    gtk_stack_add_named((GtkStack *) myData->stack, box, "image_que");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "image_que");
}

void create_question(data *myData){
	quest *question = myData->curNode->question;
	int *random = randomise_questions(question->answerNum);
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	GtkWidget *header = gtk_label_new (question->que);
    gtk_label_set_line_wrap ((GtkLabel *) header, TRUE);
    gtk_widget_show(header);
	gtk_box_pack_start(GTK_BOX(box), header, FALSE, TRUE, 50);
	/*GtkWidget *tags = gtk_label_new (get_tags(question->que));
    gtk_label_set_line_wrap ((GtkLabel *) tags, TRUE);
    gtk_widget_show(tags);
	gtk_box_pack_start(GTK_BOX(box), tags, FALSE, TRUE, 50);*/
	GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous ((GtkGrid *) grid, TRUE);
    gtk_grid_set_column_homogeneous ((GtkGrid *) grid, TRUE);
	for (int i = 0; i < question->answerNum; i++) {
        int x = 0;
        if (i >= 2) {
            x = 1;
        }
		GtkWidget *button = gtk_button_new_with_label(question->answers[random[i]]);
		g_signal_connect(button, "clicked", check_answer(random[i]), myData);
		gtk_widget_show(button);
		gtk_grid_attach((GtkGrid *) grid, button, x, i % 2, 1, 1);
	}
    free(random);
	gtk_widget_show(grid);
	gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);
	gtk_widget_show(box);
	gtk_stack_add_named((GtkStack *) myData->stack, box, "questions_test");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "questions_test");
}

void create_qu_answer(data *myData, bool correct){
    char *title;
    char *mess = malloc(511 *sizeof(char));
    GCallback next;
    if (correct) {
        title = "Correct Answer!";
        strcpy(mess,"Congratulations on getting the right answer: ");
        next = (GCallback) &advance_right_question;
    } else{
        title = "Wrong Answer!";
        strcpy(mess, "The correct answer is: ");
        next = (GCallback) &advance_left_question;
    }
    strcat(mess, myData->curNode->question->answers[0]);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new (title);
    gtk_widget_show(header);
    gtk_box_pack_start(GTK_BOX(box), header, TRUE, TRUE, 0);
    GtkWidget *message = gtk_label_new (mess);
    free(mess);
    gtk_label_set_line_wrap ((GtkLabel *) message, TRUE);
    gtk_widget_show(message);
    gtk_box_pack_start(GTK_BOX(box), message, TRUE, TRUE, 0);
    GtkWidget *button = gtk_button_new_with_label("Continue");
    g_signal_connect(button, "clicked", next, myData);
    gtk_widget_show(button);
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
    gtk_widget_show(box);
	gtk_stack_add_named((GtkStack *) myData->stack, box, "answers_test");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "answers_test");
}

void create_add_question(data *myData){
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new ("Add new questions below");
    gtk_widget_show(header);
    gtk_box_pack_start((GtkBox *) box, header, FALSE, TRUE, 50);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous ((GtkGrid *) grid, TRUE);
    gtk_grid_set_column_homogeneous ((GtkGrid *) grid, TRUE);
    for (int i = 0; i < 5; i++) {
        GtkWidget *textBox = gtk_entry_new();
        gtk_widget_set_name (textBox, "textBox");
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
    g_signal_connect(addQ, "clicked", (GCallback) &add_question_amended, myData);
    gtk_widget_show(addQ);
    gtk_grid_attach(GTK_GRID(grid), addQ, 0, 5, 1, 1);
    GtkWidget *finishQ = gtk_button_new_with_label("Finish Quiz");
    g_signal_connect(finishQ, "clicked", (GCallback) &finish_quiz, myData);
    gtk_widget_show(finishQ);
    gtk_grid_attach(GTK_GRID(grid), finishQ, 1, 5, 1, 1);
    gtk_widget_show(grid);
    gtk_box_pack_start(GTK_BOX(box), grid, TRUE, TRUE, 0);
    gtk_widget_show(box);
	gtk_stack_add_named((GtkStack *) myData->stack, box, "add_que");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "add_que");
}

void create_file_name(data *myData){
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new ("Enter Quiz Name:");
    gtk_widget_show(header);
    gtk_box_pack_start(GTK_BOX(box), header, TRUE, TRUE, 0);
    GtkWidget *textBox = gtk_entry_new();
    gtk_widget_set_name (textBox, "textBox");
    gtk_widget_show(textBox);
    gtk_box_pack_start(GTK_BOX(box), textBox, TRUE, TRUE, 0);
    GtkWidget *cont = gtk_button_new_with_label("Continue");
    g_signal_connect(cont, "clicked", (GCallback) &add_question_first, myData);
    gtk_widget_show(cont);
    gtk_box_pack_start(GTK_BOX(box), cont, TRUE, TRUE, 0);
    gtk_widget_show(box);
	gtk_stack_add_named((GtkStack *) myData->stack, box, "add_que_file");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "add_que_file");
}

void create_home(data *myData){
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *begin = gtk_check_button_new_with_label("Begin Quiz");
    GtkWidget *add = gtk_check_button_new_with_label("Add New Quiz");
    GtkWidget *learn = gtk_check_button_new_with_label("Learn More (will open browser link)");
    GtkWidget *about = gtk_check_button_new_with_label("About Us");
    GtkWidget *quit = gtk_check_button_new_with_label("Quit");
}

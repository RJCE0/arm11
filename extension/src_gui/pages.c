#include "extension.h"

void create_im_answer(int imageNo, int score, data *myData){
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *header = gtk_label_new (myData->images[imageNo].question);
    gtk_widget_show(header);
    gtk_box_pack_start((GtkBox *) box, header, FALSE, TRUE, 10);
    GtkWidget *image = gtk_image_new_from_file (myData->images[imageNo].ansFile);
    gtk_widget_show(image);
    gtk_box_pack_start((GtkBox *) box, image, FALSE, TRUE, 20);
	char *scoreMessage = malloc(20 * sizeof(char));
	strcpy(scoreMessage, "You got ");
	strcat(scoreMessage, int_to_string(score));
	strcat(scoreMessage, " out of ");
	strcat(scoreMessage, int_to_string(myData->images[imageNo].maxAns));
	GtkWidget *message = gtk_label_new (scoreMessage);
    gtk_widget_show(message);
    gtk_box_pack_start((GtkBox *) box, message, FALSE, TRUE, 10);
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
    gtk_box_pack_start((GtkBox *) box, header, FALSE, TRUE, 10);
    GtkWidget *image = gtk_image_new_from_file (myData->images[imageNo].queFile);
    gtk_widget_show(image);
    gtk_box_pack_start((GtkBox *) box, image, FALSE, TRUE, 20);
    for (int i = 0; i < 3; i++) {
        GtkWidget *buttonBox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
        for (int j = 0; j < 3; j++) {
            GtkWidget *check = gtk_check_button_new_with_label(int_to_string(3*i + j + 1));
            g_signal_connect(check, "clicked", (GCallback) &on_checkbox_toggle, myData);
            gtk_widget_show(check);
            gtk_container_add ((GtkContainer *) buttonBox, check);
        }
        gtk_box_pack_start((GtkBox *) box, buttonBox, FALSE, TRUE, 0);
        gtk_widget_show(buttonBox);
    }
    GtkWidget *submit = gtk_button_new_with_label("Submit");
    GCallback nextFunc = (GCallback) &go_to_picQ1Ans;
    if (imageNo == 1) {
        nextFunc = (GCallback) &go_to_picQ2Ans;
    }
    g_signal_connect(submit, "clicked", nextFunc, myData);
    gtk_box_pack_start((GtkBox *) box, submit, FALSE, TRUE, 10);
    gtk_widget_show(submit);
    gtk_widget_show(box);
    gtk_stack_add_named((GtkStack *) myData->stack, box, "image_que");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "image_que");
}

void create_queston(data *myData){
	quest *question = myData->curNode->question;
	int *random = randomise_questions(question->answerNum);
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	GtkWidget *header = gtk_label_new (question->que);
    gtk_widget_show(header);
	gtk_box_pack_start((GtkBox *) box, header, TRUE, TRUE, 50);
	GtkWidget *tags = gtk_label_new (get_tags(question->que));
    gtk_widget_show(tags);
	gtk_box_pack_start((GtkBox *) box, tags, TRUE, TRUE, 50);
	GtkWidget *grid = gtk_grid_new();
	gtk_grid_insert_row ((GtkGrid *)grid, 0);
	gtk_grid_insert_row ((GtkGrid *)grid, 0);
	gtk_grid_insert_column ((GtkGrid *)grid, 0);
	gtk_grid_insert_column ((GtkGrid *)grid, 0);
	for (int i = 0; i < question->answerNum; i++) {
		GtkWidget *button = gtk_button_new_with_label(question->answers[random[0]]);
		g_signal_connect(button, "clicked", check_answer(random[i]), myData);
		gtk_widget_show(button);
		//gtk_container_add ((GtkContainer *) grid, button);
		gtk_grid_attach((GtkGrid *) grid, button, 0, i, 1, 1);
	}
	gtk_widget_show(grid);
	gtk_widget_show(box);
	gtk_stack_add_named((GtkStack *) myData->stack, box, "questions_test");
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "questions_test");
}

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdbool.h>
#include "multiplechoice.c"

typedef struct {
    GtkWidget *stack;
    GtkWidget *questionLabel;
    GtkWidget *answerA;
    GtkWidget *answerB;
    GtkWidget *answerC;
    GtkWidget *answerD;
} data;

// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}

void go_to_home(GtkWidget *whatever, data *myData) {
    gtk_stack_set_visible_child_name ((GtkStack *) myData->stack, "home_page");
}

void go_to_question_page(GtkWidget *whatever, data *myData) {
    gtk_stack_set_visible_child_name ((GtkStack *) myData->stack, "question_page");
}

void go_to_about_us(GtkWidget *whatver, data *myData) {
    gtk_stack_set_visible_child_name ((GtkStack *) myData->stack, "about_us_page");
}

void go_to_correct_answer(GtkWidget *whatever, data *myData) {
    gtk_stack_set_visible_child_name ((GtkStack *) myData->stack, "correct_answer_page");
}

void open_blm_site() {
    system("firefox www.blacklivesmatter.com");
}

/*
void set_question(char *question, char *answerA, char *answerB, char *answerC, char *answerD, data *myData) {
    gtk_label_set_text((GtkLabel *) myData->questionLabel, question);

    if (answerD == NULL) {
        gtk_widget_set_sensitive(myData->answerD, false);
    }
    if (answerC == NULL) {
        gtk_widget_set_sensitive(myData->answerC, false);
    }
    if (answerB == NULL) {
        exit(EXIT_FAILURE);
    }
    gtk_button_set_label((GtkButton *) myData->answerA, answerA);
    gtk_button_set_label((GtkButton *) myData->answerB, answerB);
    gtk_button_set_label((GtkButton *) myData->answerC, answerC);
    gtk_button_set_label((GtkButton *) myData->answerD, answerD);

}
*/

void set_question(quest *question, data *myData) {
    gtk_label_set_text((GtkLabel *) myData->questionLabel, question->que);

    if (question->answerNum <= 3) {
        gtk_widget_set_sensitive(myData->answerD, false);
				if (question->answerNum == 2) {
					gtk_widget_set_sensitive(myData->answerC, false);
				}
    }
    gtk_button_set_label((GtkButton *) myData->answerA, question->answers[0]);
    gtk_button_set_label((GtkButton *) myData->answerB, question->answers[1]);
    gtk_button_set_label((GtkButton *) myData->answerC, question->answers[2]);
    gtk_button_set_label((GtkButton *) myData->answerD, question->answers[3]);

}

int main(int argc, char *argv[])
{
    data *myData = malloc (sizeof(myData));

    GtkBuilder      *builder;
    GtkWidget       *window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("glade/window_main.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, myData);

    myData->stack = GTK_WIDGET(gtk_builder_get_object(builder, "stack1"));
    myData->questionLabel = GTK_WIDGET(gtk_builder_get_object(builder, "question_label"));
    myData->answerA = GTK_WIDGET(gtk_builder_get_object(builder, "answer_a_button"));
    myData->answerB = GTK_WIDGET(gtk_builder_get_object(builder, "answer_b_button"));
    myData->answerC = GTK_WIDGET(gtk_builder_get_object(builder, "answer_c_button"));
    myData->answerD = GTK_WIDGET(gtk_builder_get_object(builder, "answer_d_button"));

		quest *curr = initalise_questions();

    set_question(curr, myData);

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}

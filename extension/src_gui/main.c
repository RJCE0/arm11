#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdbool.h>
#include "multiplechoice.c"

typedef struct {
    GtkWidget *stack;
    GtkWidget *questionLabel;
    GtkWidget *scoreLabel;
    GtkWidget *answerA;
    GtkWidget *answerB;
    GtkWidget *answerC;
    GtkWidget *answerD;
    tickBoxes *t;
} data;

typedef struct{
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



// called when window is closed
void on_window_main_destroy(void)
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
    static int score = 0;
    char str_count[30] = {0};
    score++;
    gtk_stack_set_visible_child_name ((GtkStack *) myData->stack, "correct_answer_page");
    //sprintf(str_count, "Your score is now %d!", score);
}

void go_to_picQ1(GtkWidget *whatever, data *myData) {
    gtk_stack_set_visible_child_name ((GtkStack *) myData->stack, "PicQ1");
    gtk_stack_set_visible_child_name (
}

void go_to_picQ1Ans(GtkWidget *whatever, data *myData) {
    gtk_stack_set_visible_child_name ((GtkStack *) myData->stack, "PicQ1Ans");
}

void go_to_picQ2(GtkWidget *whatever, data *myData) {
    gtk_stack_set_visible_child_name ((GtkStack *) myData->stack, "PicQ2");
}

void go_to_picQ2Ans(GtkWidget *whatever, data *myData) {
    gtk_stack_set_visible_child_name ((GtkStack *) myData->stack, "PicQ2Ans");
}

void open_blm_site(void) {
    system("www.blacklivesmatter.com");
}

void go_to_wrong_answer(GtkWidget *widget, data *myData) {
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "wrong_answer_page");
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

void show_pic_question(image *img, data *myData){


}


void set_question(quest *question, data *myData) {
    gtk_label_set_text((GtkLabel *) myData->questionLabel, question->que);

    if (question->answerNum <= 3) {
        gtk_widget_set_sensitive(myData->answerD, false);
				if (question->answerNum == 2) {
					gtk_widget_set_sensitive(myData->answerC, false);
				}
    }
    void (*correct_ptr) (GtkWidget *, data) = &go_to_correct_answer;
    gtk_button_set_label((GtkButton *) myData->answerA, question->answers[0]);
    gtk_button_set_label((GtkButton *) myData->answerB, question->answers[1]);
    gtk_button_set_label((GtkButton *) myData->answerC, question->answers[2]);
    gtk_button_set_label((GtkButton *) myData->answerD, question->answers[3]);
    g_signal_connect(myData->answerA, "clicked", correct_ptr, myData);
    g_signal_connect(myData->answerB, "clicked", &go_to_wrong_answer,myData);
    g_signal_connect(myData->answerC, "clicked", &go_to_wrong_answer,myData);
    g_signal_connect(myData->answerD, "clicked", &go_to_wrong_answer,myData);

}

void check_answer(GtkButton *button, GtkButton *answer_button, data *myData) {
    const char *user_answer = gtk_button_get_label(button);
    const char *answer = gtk_button_get_label(answer_button);
    if (strcmp(user_answer, answer)) {
        go_to_correct_answer(button, myData);
    } else {
        go_to_wrong_answer(button, myData);
    }
} 

const char *get_label(GtkButton *button) {
    return gtk_button_get_label(button);
}

void advance(GtkButton *button, data *myData, quest *question) {
    //Moves to next question.
    set_question(question, myData);
    go_to_question_page(button, myData);
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
    myData->scoreLabel = GTK_WIDGET(gtk_builder_get_object(builder,"count"));
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

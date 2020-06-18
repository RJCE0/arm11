#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdbool.h>
#include "multiplechoice.c"
#include "generatetags.c"
#include <stdio.h>
#include <time.h>

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
    char *guesses[9];
} data;


void decrement_score(GtkWidget *tickbox, data *myData) {
    myData->quizScore = ((myData->quizScore - 1) <= 0) ? 0 : (myData->quizScore)--;
}

void increment_score(GtkWidget *tickbox, data *myData) {
    (myData->quizScore)++;
}


// called when window is closed
void on_window_main_destroy(void) {
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

char *int_to_string(int i) {
    printf("!----%d----!", i);
    char *score = calloc (5, sizeof(char));
    sprintf(score, "%d", i);
    printf("%s", score);
    return score;
}

void go_to_picQ1(GtkWidget *whatever, data *myData) {
    gtk_stack_set_visible_child_name ((GtkStack *) myData->stack, "PicQ1");
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

void go_to_correct_answer(GtkWidget *whatever, data *myData) {
    char *str = malloc(100*sizeof(char));
    strcpy(str, "Congratulations on getting the correct answer: ");
    strcat(str, myData->curNode->u.question->answers[0]);
    gtk_label_set_text((GtkLabel *) myData->scoreLabelFromRight, str);
    gtk_stack_set_visible_child_name ((GtkStack *) myData->stack, "correct_answer_page");
}

void go_to_wrong_answer(GtkWidget *widget, data *myData) {
    char *str = malloc(100*sizeof(char));
    strcpy(str, "Correct answer is: ");
    strcat(str, myData->curNode->u.question->answers[0]);
    gtk_label_set_text((GtkLabel *) myData->scoreLabelFromWrong, str);
    gtk_stack_set_visible_child_name((GtkStack *) myData->stack, "wrong_answer_page");
}

void check_box1q1(GtkWidget *widget, data *myData, int i) {
    char checker;
    if (gtk_toggle_button_get_active(widget)) {
        checker = 't';
	}
    else{
        checker = 'f';
    }
    myData->guesses[i] = checker;
}



void open_blm_site(void) {
    gtk_show_uri_on_window(NULL, "https://blacklivesmatter.com/", GDK_CURRENT_TIME, NULL);
}

GCallback check_answer(int i){
    if (i == 0) {
        return (GCallback) &go_to_correct_answer;
    }
    return (GCallback) &go_to_wrong_answer;
}

int *randomise_questions(int size){
    int randArr[size];
    static int* array;
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
    quest *question = myData->curNode->u.question;
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

    gtk_button_set_label((GtkButton *) myData->answerA, question->answers[random[0]]);
    gtk_button_set_label((GtkButton *) myData->answerB, question->answers[random[1]]);

    if (question->answerNum >= 3) {
        gtk_button_set_label((GtkButton *) myData->answerC, question->answers[random[2]]);
        g_signal_connect(myData->answerC, "clicked", check_answer(random[2]),myData);
        if (question->answerNum == 4) {
            gtk_button_set_label((GtkButton *) myData->answerD, question->answers[random[3]]);
            g_signal_connect(myData->answerD, "clicked", check_answer(random[3]),myData);
        } else {
            gtk_button_set_label((GtkButton *) myData->answerD, "");
        }
    } else {
        gtk_button_set_label((GtkButton *) myData->answerC, "");
        gtk_button_set_label((GtkButton *) myData->answerD, "");
    }
    g_signal_connect(myData->answerA, "clicked", check_answer(random[0]), myData);
    g_signal_connect(myData->answerB, "clicked", check_answer(random[1]), myData);

    char *str = get_tags(question->que);
    gtk_label_set_text((GtkLabel *) myData->tagsLabel, str);
}


void begin_quiz(GtkWidget *widget, data *myData) {
    int maxQuestions;
    myData->curNode = initialise_questions(&maxQuestions);
    myData->maxQuestions = maxQuestions;
    myData->currentQuestion = 0;
    set_question(myData);
    go_to_question_page(widget, myData);
}


const char *get_label(GtkButton *button) {
    return gtk_button_get_label(button);
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
        t->tickBox1 = GTK_WIDGET(gtk_builder_get_object(builder,"check1"));
        t->tickBox2 = GTK_WIDGET(gtk_builder_get_object(builder,"check2"));
        t->tickBox3 = GTK_WIDGET(gtk_builder_get_object(builder,"check3"));
        t->tickBox4 = GTK_WIDGET(gtk_builder_get_object(builder,"check4"));
        t->tickBox5 = GTK_WIDGET(gtk_builder_get_object(builder,"check5"));
        t->tickBox6 = GTK_WIDGET(gtk_builder_get_object(builder,"check6"));
        t->tickBox7 = GTK_WIDGET(gtk_builder_get_object(builder,"check7"));
        t->tickBox8 = GTK_WIDGET(gtk_builder_get_object(builder,"check8"));
        t->tickBox9 = GTK_WIDGET(gtk_builder_get_object(builder,"check9"));
    }
}

void q2_initialise_tickboxes(tickBoxes *t, GtkBuilder *builder) {
    if (t) {
        t->tickBox1 = GTK_WIDGET(gtk_builder_get_object(builder,"2ndCheck1"));
        t->tickBox2 = GTK_WIDGET(gtk_builder_get_object(builder,"2ndCheck2"));
        t->tickBox3 = GTK_WIDGET(gtk_builder_get_object(builder,"2ndCheck3"));
        t->tickBox4 = GTK_WIDGET(gtk_builder_get_object(builder,"2ndCheck4"));
        t->tickBox5 = GTK_WIDGET(gtk_builder_get_object(builder,"2ndCheck5"));
        t->tickBox6 = GTK_WIDGET(gtk_builder_get_object(builder,"2ndCheck6"));
        t->tickBox7 = GTK_WIDGET(gtk_builder_get_object(builder,"2ndCheck7"));
        t->tickBox8 = GTK_WIDGET(gtk_builder_get_object(builder,"2ndCheck8"));
        t->tickBox9 = GTK_WIDGET(gtk_builder_get_object(builder,"2ndCheck9"));
    }
}

int main(int argc, char *argv[]) {


    GtkBuilder      *builder;
    GtkWidget       *window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("glade/window_main.glade");
    data *myData = malloc (sizeof(myData));
    tickBoxes *boxes = malloc(sizeof(*boxes));
    tickBoxes *q2boxes = malloc(sizeof(*q2boxes));
    q1_initialise_tickboxes(boxes, builder);
    q2_initialise_tickboxes(q2boxes, builder);


    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, myData);

    myData->stack = GTK_WIDGET(gtk_builder_get_object(builder, "stack1"));
    myData->questionLabel = GTK_WIDGET(gtk_builder_get_object(builder, "question_label"));
    myData->scoreLabelFromWrong = GTK_WIDGET(gtk_builder_get_object(builder,"current_score_label_wrong"));
    myData->scoreLabelFromRight = GTK_WIDGET(gtk_builder_get_object(builder,"current_score_label_correct"));
    myData->answerA = GTK_WIDGET(gtk_builder_get_object(builder, "answer_a_button"));
    myData->answerB = GTK_WIDGET(gtk_builder_get_object(builder, "answer_b_button"));
    myData->answerC = GTK_WIDGET(gtk_builder_get_object(builder, "answer_c_button"));
    myData->answerD = GTK_WIDGET(gtk_builder_get_object(builder, "answer_d_button"));
    myData->tagsLabel = GTK_WIDGET(gtk_builder_get_object(builder, "tags_label"));
    myData->quizScore = 0;
    myData->picQ1 = GTK_WIDGET(gtk_builder_get_object(builder,"PicQ1"));
    myData->picQ1Ans = GTK_WIDGET(gtk_builder_get_object(builder,"PicQ1Ans"));
    myData->picQ2 = GTK_WIDGET(gtk_builder_get_object(builder,"PicQ2"));
    myData->picQ2Ans = GTK_WIDGET(gtk_builder_get_object(builder,"PicQ2Ans"));
    myData->q1Boxes = boxes;
    myData->q2Boxes = q2boxes;


    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();
    printf("finished\n");

    return 0;
}

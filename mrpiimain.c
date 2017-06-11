#include <gtk/gtk.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <stdlib.h>
#include "MRPII.h"
typedef unsigned char uint8_t;

#define _BV(bit) (1 << (bit))

#define TRIG 22
#define ECHO 21
#define MOTORLATCH  0
#define MOTORCLK    1
#define MOTORENABLE 2
#define MOTORDATA   3
#define MOTOR_1_PWM 4
#define MOTOR_2_PWM 5
#define MOTOR_3_PWM 6
#define MOTOR_4_PWM 7

#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR4_A 0
#define MOTOR4_B 6
#define MOTOR3_A 5
#define MOTOR3_B 7

#define FORWARD  1
#define BACKWARD 2
#define BRAKE    3
#define RELEASE  4




GtkWidget *g_lbl_hello;
GtkWidget *g_lbl_count;
GtkWidget *g_lbl_sonar;
GtkToggleButton *g_btn_sonar;
GtkToggleButton *g_btn_auto;
GtkEntry *g_entry_turn;
GtkEntry *g_entry_block;

int main(int argc, char *argv[])
{
    if (wiringPiSetup () == -1)
   {
      fprintf (stdout, "oops: %s\n", strerror (errno)) ;
      return 1 ;
   }
   pinMode(MOTOR_1_PWM, OUTPUT);
   pinMode(MOTOR_2_PWM, OUTPUT);
   pinMode(MOTOR_3_PWM, OUTPUT);
   pinMode(MOTOR_4_PWM, OUTPUT);

   digitalWrite(MOTOR_1_PWM, 0);
   digitalWrite(MOTOR_2_PWM, 0);
   digitalWrite(MOTOR_3_PWM, 0);
   digitalWrite(MOTOR_4_PWM, 0);


   enable();

   DCMotorInit(1);
   DCMotorInit(2);
   DCMotorInit(3);
   DCMotorInit(4);

    GtkBuilder      *builder; 
    GtkWidget       *window;
    gtk_init(&argc, &argv);
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "window_main.glade", NULL);
 
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);
    
    // get pointers
    g_lbl_hello = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_hello"));
    g_lbl_count = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_count"));
    g_lbl_sonar = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_sonar"));
    g_btn_sonar = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "btn_sonar"));
    g_btn_auto = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "btn_auto"));
    g_entry_block = GTK_ENTRY(gtk_builder_get_object(builder,"entry_block"));
    g_entry_turn = GTK_ENTRY(gtk_builder_get_object(builder,"entry_turn"));
    g_object_unref(builder);
    gtk_widget_show(window);                
    gtk_main();
 
    return 0;
}
 //manual drive
void btn_right_button_press_event_cb(){
	rright();
}
void btn_right_button_release_event_cb(){
	stop();
}
void btn_back_button_press_event_cb(){
	bbackwd();
}
void btn_back_button_release_event_cb(){
	stop();
}
void btn_left_button_press_event_cb(){
	lleft();
}
void btn_left_button_release_event_cb(){
	stop();
}
void btn_forward_button_press_event_cb(){
	fforwd();
}

void btn_forward_button_release_event_cb(){
	stop();
}
void on_btn_stopauto_clicked(){
	stop();
}
void updatesonarlabel(int distance){
	gchar *sonardistance;
	sonardistance = g_strdup_printf("%d", distance);
	gtk_label_set_text (GTK_LABEL(g_lbl_sonar), sonardistance);
	g_free(sonardistance);
}
//autopilot algorithm
void autopilot(){

int clockn [12];
const char *getentry;
getentry = gtk_entry_get_text(GTK_ENTRY(g_entry_block));
int block = atoi(getentry);
int max_distance = 0;
int direction = 6;
int i;
const char *getturn;
getturn = gtk_entry_get_text(GTK_ENTRY(g_entry_turn));
double fraction = atof(getturn);
fraction = fraction/12;
fraction = fraction*1000;
int turn_degree = 0;
	while(gtk_toggle_button_get_active(g_btn_auto)){
fforwd();
	//drive forward untill obsticle closer than block(cm)
while (getCM()>=block){}
stop();
delay(500);
lleft();
	//turn left untill sonar gets 12 readings
for (i = 1;i<=12;i++) {
        delay(fraction);
        clockn[i]=getCM();
        if (clockn[i] > max_distance){
                if (i!=5 && i!=6 && i!=7){
                max_distance = clockn[i];
                direction = i;
                }
        }
}
stop();
delay(500);
	if (direction <= 4){
	turn_degree = fraction * direction;
	left(turn_degree);
	}
	else {
	turn_degree = fraction * (12 - direction);
	right(turn_degree);
	}
delay(500);
max_distance = 0;
direction = 6;
//main iteration, checking for toggle button condition
while (gtk_events_pending()) {
	gtk_main_iteration();
}
	}

}
	//enable sonar
void on_btn_sonar_toggled(){	
int distance;
while(gtk_toggle_button_get_active(g_btn_sonar)){
distance=getCM();
updatesonarlabel(distance);
delay(500);
	//main iteration, checking for toggle button condition
	while (gtk_events_pending()) {
	gtk_main_iteration();
	}
}
}
	//autopilot
void on_btn_auto_toggled(){
autopilot();
}


 
// called when window is closed
void on_window_main_destroy(){
    gtk_main_quit();}

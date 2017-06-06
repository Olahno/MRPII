#include <gtk/gtk.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <stdlib.h>
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

static unsigned char latch_state;

void latch_tx(void)
{
   unsigned char i;
   
      digitalWrite(MOTORLATCH, LOW);

   digitalWrite(MOTORDATA, LOW);

   for (i=0; i<8; i++)
   {
      digitalWrite(MOTORCLK, LOW);

      if (latch_state & _BV(7-i))
      {
         digitalWrite(MOTORDATA, HIGH);
      }
      else
      {
         digitalWrite(MOTORDATA, LOW);
      }
      digitalWrite(MOTORCLK, HIGH);
   }
   digitalWrite(MOTORLATCH, HIGH);
}


void enable(void)
{
   wiringPiSetup();
   pinMode(TRIG, OUTPUT);
   pinMode(ECHO, INPUT);
   pinMode(MOTORLATCH,  OUTPUT);
   pinMode(MOTORENABLE, OUTPUT);
   pinMode(MOTORDATA,   OUTPUT);
   pinMode(MOTORCLK,    OUTPUT);

   //TRIG pin must start LOW
   digitalWrite(TRIG, LOW);
   delay(20);
   latch_state = 0;

   latch_tx();

   digitalWrite(MOTORENABLE, LOW);
}

int getCM() {
       //delay(200);
	 //Send trig pulse
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(60);
        digitalWrite(TRIG, LOW);

        //Wait for echo start
        while(digitalRead(ECHO) == LOW);

        //Wait for echo end
        long startTime = micros();
        while(digitalRead(ECHO) == HIGH);
        long travelTime = micros() - startTime;

        //Get distance in cm
        int distance = travelTime / 58;
       // printf("Distance: %dcm\n", distance);
        return distance;
}

void DCMotorInit(uint8_t num)
{
   switch (num)
   {
      case 1:
         latch_state &= ~_BV(MOTOR1_A) & ~_BV(MOTOR1_B);
         latch_tx();
         break;
      case 2:
         latch_state &= ~_BV(MOTOR2_A) & ~_BV(MOTOR2_B);
         latch_tx();
         break;
      case 3:
         latch_state &= ~_BV(MOTOR3_A) & ~_BV(MOTOR3_B);
         latch_tx();
         break;
      case 4:
         latch_state &= ~_BV(MOTOR4_A) & ~_BV(MOTOR4_B);
         latch_tx();
         break;
   }
}

void DCMotorRun(uint8_t motornum, uint8_t cmd)
{
   uint8_t a, b;

   switch (motornum)
   {
      case 1:
         a = MOTOR1_A; b = MOTOR1_B;
         break;
      case 2:
         a = MOTOR2_A; b = MOTOR2_B;
         break;
      case 3:
         a = MOTOR3_A; b = MOTOR3_B;
         break;
      case 4:
         a = MOTOR4_A; b = MOTOR4_B;
         break;
	  default:
         return;
   }

   switch (cmd)
   {
      case FORWARD:
         latch_state |= _BV(a);
         latch_state &= ~_BV(b);
         latch_tx();
         break;
      case BACKWARD:
         latch_state &= ~_BV(a);
         latch_state |= _BV(b);
         latch_tx();
         break;
      case RELEASE:
         latch_state &= ~_BV(a);
         latch_state &= ~_BV(b);
         latch_tx();
       break;
   }
}

void left(unsigned int t)
{
   DCMotorRun(2, BACKWARD);
   digitalWrite(MOTOR_2_PWM, 1);
   DCMotorRun(3, FORWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(1, BACKWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(4, FORWARD);
   digitalWrite(MOTOR_4_PWM, 1);
 delay(t);
   DCMotorRun(1, RELEASE);
   DCMotorRun(2, RELEASE);
   DCMotorRun(3, RELEASE);
   DCMotorRun(4, RELEASE);
}

void lleft()
{
   DCMotorRun(2, BACKWARD);
   digitalWrite(MOTOR_2_PWM, 1);
   DCMotorRun(3, FORWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(1, BACKWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(4, FORWARD);
   digitalWrite(MOTOR_4_PWM, 1);
}

void right(unsigned int t)
{
   DCMotorRun(1, FORWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(2, FORWARD);
   digitalWrite(MOTOR_2_PWM, 1);
   DCMotorRun(3, BACKWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(4, BACKWARD);
   digitalWrite(MOTOR_4_PWM, 1);
 delay(t);
   DCMotorRun(1, RELEASE);
   DCMotorRun(2, RELEASE);
   DCMotorRun(3, RELEASE);
   DCMotorRun(4, RELEASE);
}
void rright()
{
   DCMotorRun(1, FORWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(2, FORWARD);
   digitalWrite(MOTOR_2_PWM, 1);
   DCMotorRun(3, BACKWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(4, BACKWARD);
   digitalWrite(MOTOR_4_PWM, 1);
}
void backwd(int t)
{
   DCMotorRun(1, FORWARD);
 digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(2, BACKWARD);
   digitalWrite(MOTOR_2_PWM, 1);

   DCMotorRun(3, FORWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(4, BACKWARD);
   digitalWrite(MOTOR_4_PWM, 1);
   sleep(t);

   DCMotorRun(1, RELEASE);
   DCMotorRun(2, RELEASE);
   DCMotorRun(3, RELEASE);
   DCMotorRun(4, RELEASE);
 sleep(1);

}
void bbackwd()
{
   DCMotorRun(1, FORWARD);
 digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(2, BACKWARD);
   digitalWrite(MOTOR_2_PWM, 1);

   DCMotorRun(3, FORWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(4, BACKWARD);
   digitalWrite(MOTOR_4_PWM, 1);
} 
void forwd(int t)
{
   DCMotorRun(1, BACKWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(2, FORWARD);
   digitalWrite(MOTOR_2_PWM, 1);

   DCMotorRun(3, BACKWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(4, FORWARD);
   digitalWrite(MOTOR_4_PWM, 1);
   sleep(t);

   DCMotorRun(1, RELEASE);
   DCMotorRun(2, RELEASE);
   DCMotorRun(3, RELEASE);
   DCMotorRun(4, RELEASE);
 sleep(1);

}
void fforwd()
{
 DCMotorRun(1, BACKWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   DCMotorRun(2, FORWARD);
 digitalWrite(MOTOR_2_PWM, 1);

   DCMotorRun(3, BACKWARD);
   digitalWrite(MOTOR_3_PWM, 1);
   DCMotorRun(4, FORWARD);
   digitalWrite(MOTOR_4_PWM, 1);
}
void stop()
{
  DCMotorRun(1, RELEASE);
   DCMotorRun(2, RELEASE);
   DCMotorRun(3, RELEASE);
   DCMotorRun(4, RELEASE);
}
void autopilot(int times,int block)
{
int clockn [12];
int i,j = 1;
int max_distance = 0;
int direction = 6;
unsigned int fraction = 209;
int turn_degree = 0;
	for (j = times;j>0;j--){
fforwd();
while (getCM()>=block){}
stop();
delay(500);
lleft();
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
printf("Max distance: %d\n", max_distance);
printf("Direction: %d\n", direction);
max_distance = 0;
direction = 6;
	}

}
GtkWidget *g_lbl_hello;
GtkWidget *g_lbl_count;
GtkWidget *g_lbl_sonar;

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
    
    // get pointers to the two labels

    g_lbl_hello = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_hello"));
    g_lbl_count = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_count"));
    g_lbl_sonar = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_sonar"));
    g_object_unref(builder);
 
    gtk_widget_show(window);                
    gtk_main();
 
    return 0;
}
 
void btn_right_button_press_event_cb()
{
	rright();
}
void btn_right_button_release_event_cb()
{
	stop();
}
void btn_back_button_press_event_cb()
{
	bbackwd();
}
void btn_back_button_release_event_cb()
{
	stop();
}
void btn_left_button_press_event_cb()
{
	lleft();
}
void btn_left_button_release_event_cb()
{
	stop();
}
void btn_forward_button_press_event_cb()
{
	fforwd();
}

void btn_forward_button_release_event_cb()
{
	stop();
}
void updatesonarlabel(int distance)
{
	gchar *sonardistance;
	sonardistance = g_strdup_printf("%d", distance);
	gtk_label_set_text (GTK_LABEL(g_lbl_sonar), sonardistance);
	g_free(sonardistance);
}
void on_btn_sonar_toggled(GtkToggleButton *btn_sonar)
{	int distance;
	distance=getCM();
	//gtk_label_set_text(GTK_LABEL(g_lbl_sonar), "fuckyou");
	while(gtk_toggle_button_get_active (btn_sonar))
{ 	distance=getCM();
	updatesonarlabel(distance);	
}
}
/*void btn_cam_clicked_cb()
{
	system("./cam.sh");
}*/
 
// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}

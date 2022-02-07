/*
 * Author: Kyle Ah Von
 * Date:             4/9/2021 8:58:01 PM
 *
 * Purpose:  Play a game of modified simon's game
 *
 */
#define _CRT_SECURE_NO_WARNINGS

 /* include required header files */
#include <stdlib.h>
#include <stdio.h>
#include <DAQlib.h>
#include <Windows.h>
#include <time.h>
#include <math.h>

 /* define input/output channels */
#define pushbutton0 0
#define pushbutton1 1
#define pushbutton2 2
#define pushbutton3 3
#define LED0 0
#define LED1 1
#define LED2 2
#define LED3 3

 /* define other symbolic constants, e.g. state of the switches and LEDs */
#define ON 1
#define OFF 0
#define TRUE 1
#define FALSE 0
#define sequencelength 5
#define NUM_LEDS 4
#define sizeofmoddata 15
/*To change the number of sequences needed to win, change the 'sequencelength' 
then calculate the corresponding sizeofmoddata using 'obtainmoddatastartindex' function*/

 /* work function */
void runSimon(void);

/* other function prototypes here*/
int obtainmoddatastartindex(int dataindex);
int obtainmoddatalength(int dataindex);
int randInt(int lower, int upper);
void generateSequence(int length, int data[]);
void modifiedSimon(int moddata[], int data[]);
int readbutton();
void flashLED(int LED);
void playerwins();
void playerloses();

int main(void)
{ /* using our usual main function */
	int setupNum;

	srand((unsigned)time(NULL));

	printf("Enter configuration type (6 for the Simon's Says game simulator): ");
	scanf("%d", &setupNum);

	if (setupDAQ(setupNum) == TRUE) {

		runSimon();
	}
	else {

		printf("ERROR: Cannot initialise system\n");
	}

	system("PAUSE");

	return 0;
}

void runSimon(void) {
	/* your code here */

	/*the variable sizeofmoddata is the calculated using the function obtainmoddatastartindex*/

	int data[sequencelength] = { 0 };
	int moddata[sizeofmoddata] = { 0 };
	int playcounter = 1;
	/*the variables below help to modify the counter to
	work with the moddata array, 'modcounter' is a constant used to modify playcounter
	into moddedplaycounter to match the moddeddata*/

	int moddedplaycounter = 1;
	int modcounter = 2;

	/*generates first sequence and stores it into an array*/
	generateSequence(sequencelength, data);
	modifiedSimon(moddata, data);

	/*Small delay allow the DAQ to load*/
	Sleep(1000);

	while (continueSuperLoop() == TRUE) {

		/*flashes the pattern of n LEDS, for the nth round*/
		for (int sequence = 0; sequence < playcounter; sequence++) {
			flashLED(data[sequence]);
		}

		/*checks if the player has wrongly clicked a button*/
		for (int index = 0; index < moddedplaycounter; index++) {

			if (readbutton() != moddata[index]) {

				/*flashes red light three times*/
				playerloses();

				/*starts a new game with a new pattern*/
				generateSequence(sequencelength, data);
				modifiedSimon(moddata, data);
				moddedplaycounter = 0;
				playcounter = 0;
				modcounter = 1;
			}
		}
		/*checks if player wins*/
		if (playcounter == sequencelength) {

			/*flashes green light three times*/
			playerwins();

			/*starts a new game with a new pattern*/
			generateSequence(sequencelength, data);
			modifiedSimon(moddata, data);
			moddedplaycounter = 0;
			playcounter = 0;
			modcounter = 1;
		}
		/*added delay to help player recall the pattern*/
		Sleep(1000);
		playcounter++;
		moddedplaycounter = moddedplaycounter + modcounter;
		modcounter++;
	}
}

/* generates random integer on the interval*/
int randInt(int lower, int upper) {

	return rand() % upper;
}

/*generates random sequence of a given length and stores it into an array*/
void generateSequence(int length, int data[]) {

	for (int i = 0; i < length; i++) {

		data[i] = randInt(0, NUM_LEDS);

	}
}

/*modifies the array to meet program statement condition (ntimes the nth LED)*/
void modifiedSimon(int moddata[], int data[]) {

	int start;
	int end;

	for (int i = 0; i < sequencelength; i++) {

		start = obtainmoddatastartindex(i);

		end = obtainmoddatalength(i);

		for (int j = start; j <= end; j++) {

			moddata[j] = data[i];
		}
	}
}


/*The function checks whether a pushbutton has been pressed and released.
Note the work function 'pauses' right after the pattern has been displayed because
it is waiting for an input to continue    */
int readbutton() {

	while (digitalRead(LED0) == OFF || digitalRead(LED1) == OFF || digitalRead(LED2) == OFF || digitalRead(LED3) == OFF) {

		if (continueSuperLoop() == FALSE) {
			return -1;
		}


		if (digitalRead(pushbutton0) == TRUE) {
			while (digitalRead(pushbutton0) == TRUE) {}
			return 0;
		}
		else if (digitalRead(pushbutton1) == TRUE) {
			while (digitalRead(pushbutton1) == TRUE) {}
			return 1;

		}
		else if (digitalRead(pushbutton2) == TRUE) {
			while (digitalRead(pushbutton2) == TRUE) {}
			return 2;

		}
		else if (digitalRead(pushbutton3) == TRUE) {
			while (digitalRead(pushbutton3) == TRUE) {}
			return 3;
		}
	}
}

/*flashes LED at a given position (350 ms chosen because at smaller time interval
it is difficult to distinguish between consecutive flashes*/
void flashLED(int LED) {

	digitalWrite(LED, ON);
	Sleep(350);
	digitalWrite(LED, OFF);
	Sleep(350);
}

/*flashes green led 3 times to show that the player won*/
void playerwins() {

	for (int i = 0; i < 3; i++) {
		flashLED(LED0);
	}

}

/*flashes red led 3 times to show that the player lost*/
void playerloses() {

	for (int i = 0; i < 3; i++) {
		flashLED(LED1);
	}

}

/*calculates the index of the last element for a given input index*/
int obtainmoddatalength(int dataindex)
{
	int modlength;

	modlength = 0.5 * pow(dataindex, 2) + 1.5 * (dataindex);

	return modlength;
}

/*calculates the start index of the first element for a given input index*/
int obtainmoddatastartindex(int dataindex)
{
	int modlength;
	double dummy;

	dummy = (double)dataindex;

	modlength = (0.5 * pow(dummy, 2.0)) + (0.5 * (dummy));

	return modlength;
}
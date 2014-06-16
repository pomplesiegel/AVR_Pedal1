/*
* BasicMethods.h
*
* Created: 6/3/2014 2:14:05 PM
*  Author: MS71010
*/


#ifndef BASICMETHODS_H_
#define BASICMETHODS_H_

//inputs
#define SWT PB0 // Define ext stomp switch input pin

//outputs
#define RELAY_POS PB1 //Positive terminal of relay
#define RELAY_NEG PB2 //Negative terminal of relay
#define LED PB3 // LED pin
#define FET_MUTE PB4 // FET_MUTE control output

#define F_CPU 1200000UL //Hz - Define software reference clock for delay duration

//Switch parameters
//#define DEBOUNCE_TIME 2        /* time to wait while "de-bouncing" button */
//#define LOCK_INPUT_TIME 200     /* time to wait after a button press */
//
//Switch parameters
const int DEBOUNCE_TIME = 2; //ms - Time in-between button press checks, used to debounce the input switch
const int LOCK_INPUT_TIME = 200; //ms - How long after detecting actual button press to not accept any new button presses

////Output parameters
//#define RELAY_FLIP_HIGH_VOLTAGE_TIME 1
//
////Output parameters
const int RELAY_FLIP_HIGH_VOLTAGE_TIME = 4; //ms - How long to hold voltage when switching relay position

////Momentary feature parameters
//#define MOMENTARY_COUNTER_THRESHOLD 5
////Momentary feature parameters
const int MOMENTARY_COUNTER_THRESHOLD_FROM_OFF = 6; //Number of MOMENTARY_WAIT_PERIODs before entering momentary mode
const int MOMENTARY_WAIT_PERIOD_FROM_OFF = 100; //ms - time of each waiting period for each momentary check. Total wait time is MOMENTARY_WAIT_PERIOD * MOMENTARY_COUNTER_THRESHOLD seconds

const int MOMENTARY_COUNTER_THRESHOLD_FROM_ON = 4; //Number of MOMENTARY_WAIT_PERIODs before entering momentary mode
const int MOMENTARY_WAIT_PERIOD_FROM_ON = 100; //ms - time of each waiting period for each momentary check. Total wait time is MOMENTARY_WAIT_PERIOD * MOMENTARY_COUNTER_THRESHOLD seconds



//ms - Determines time period between when momentary mode comes up for air, checks to see if user is still holding down button. Also determines LED flicker rate during momentary mode.
const int MOMENTARY_MODE_ACTIVE_POLL_PERIOD = 49;

const int HOLD_OFF_MODE_ACTIVE_POLL_PERIOD = 10; //ms - How often to poll switch when user is holding pedal in 'off' position, with their foot held on the switch




//Global variables
bool effectState; //Keeps state for the effect
unsigned int momentaryCounter = 0; //Counts number of times to come up for air from _delay, before we've entered momentary mode.


#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>


//Turn on the pin of this #define name or number
//By our convention, 'on' means this pin will have a value of Vcc Volts
//HIGH VOLTAGE
void TurnOn(int pinNumber) //"TurnOn seems to pull the IO high, which gives a red result"
{
	PORTB |= (1 << pinNumber); // Set 1 on pinNumber pin (turn on this port)
}

//Turn off the pin of this #define name or number
//By our convention, 'off' means this pin will have a value of 0 Volts
//LOW VOLTAGE
void TurnOff(int pinNumber)
{
	PORTB &= ~(1 << pinNumber); // Set 0 on pinNumber pin (turn off this port)
}




bool ButtonIsPressed()
{
	if(bit_is_clear(PINB, SWT)) //If we detect 0V on the switch
	{
		_delay_ms(DEBOUNCE_TIME); // Wait DEBOUNCE_TIME ms
		
		if(bit_is_clear(PINB, SWT)) //If it's still 0V after the delay
		{
			return 1; //It's a real button press
		}
	}
	
	return 0; //Otherwise, ignore it
}


//Switches the relay position. 'desiredRelayPosition' of 0 turns the effect off. 1 turns effect on
void FlipRelay(bool desiredRelayPosition)
{
	if(desiredRelayPosition == 0) //if it's 0
	{
		//turn effect off, by applying pos voltage to the + side
		TurnOn(RELAY_POS);
		_delay_ms(RELAY_FLIP_HIGH_VOLTAGE_TIME);
		TurnOff(RELAY_POS);
	}
	
	else //if it's 1
	{
		//turn effect on, , by applying pos voltage to the - side
		TurnOn(RELAY_NEG);
		_delay_ms(RELAY_FLIP_HIGH_VOLTAGE_TIME);
		TurnOff(RELAY_NEG);
	}
}

void TurnOnEffect()
{
	TurnOn(LED); //Drive LED high
	
	//TurnOn(FET_MUTE); //Mute
	FlipRelay(1); //Switch relay position
	TurnOff(FET_MUTE); //Unmute
	
	effectState = 1;
}

void TurnOffEffect()
{
	TurnOff(LED); //Drive LED low
	
	//TurnOn(FET_MUTE); //Mute
	FlipRelay(0); //Switch relay position
	TurnOn(FET_MUTE); //Mute effect side - ready for next switch
	
	effectState = 0;
}




void toggle_led()
{
	PORTB ^= _BV(LED); //XOR this bit, so we toggle it on and off with each function call
}



void InitGPIO()
{
	//inputs - SET input direction on all of these pins
	//Default data direction is an input
	PORTB |= (1 << SWT); //Enable pull-up resistor on SWT
	
	//outputs - SET output direction on all of these pins
	DDRB = (1 << RELAY_POS) | (1 << RELAY_NEG) | (1 << LED) | (1 << FET_MUTE); // Set output direction for these ports
	
	//Initial states for outputs
	TurnOffEffect();
}



//Momentary features

bool MomentaryTriggerDetectedFromOff()
{
	momentaryCounter = 0; //reset
	
	//Momentary prototype
	while (ButtonIsPressed() && momentaryCounter <= MOMENTARY_COUNTER_THRESHOLD_FROM_OFF)
	{
		momentaryCounter++;
		_delay_ms(MOMENTARY_WAIT_PERIOD_FROM_OFF);
	}
	
	if(momentaryCounter > MOMENTARY_COUNTER_THRESHOLD_FROM_OFF) //If the button's been held down for long enough
	{
		return true;
	}
	
	return false;
}

//Momentary trigger detector from 'On' position - differerent time constants
bool MomentaryTriggerDetectedFromOn()
{
	momentaryCounter = 0; //reset
	
	//Momentary prototype
	while (ButtonIsPressed() && momentaryCounter <= MOMENTARY_COUNTER_THRESHOLD_FROM_ON)
	{
		momentaryCounter++;
		_delay_ms(MOMENTARY_WAIT_PERIOD_FROM_ON);
	}
	
	if(momentaryCounter > MOMENTARY_COUNTER_THRESHOLD_FROM_ON) //If the button's been held down for long enough
	{
		return true;
	}
	
	return false;
}

void EnterMomentaryMode()
{
	//Enter momentary mode
	while (ButtonIsPressed()) //While the button's still pressed in momentary mode
	{
		toggle_led(); //Toggle LED to indicate momentary mode
		_delay_ms(MOMENTARY_MODE_ACTIVE_POLL_PERIOD); //Wait another few ms, then re-poll to see if user's foot is still on button
	}
	
	TurnOffEffect(); //When button's no longer down, turn the effect off
	//momentaryCounter = 0; //Reset momentary counter
}

void WaitUntilButtonReleased()
{
	while(ButtonIsPressed())
	{
		_delay_ms(HOLD_OFF_MODE_ACTIVE_POLL_PERIOD); //Wait another few ms, then re-poll to see if user's foot is still on button
	}
	
	//momentaryCounter = 0;
}







#endif /* BASICMETHODS_H_ */
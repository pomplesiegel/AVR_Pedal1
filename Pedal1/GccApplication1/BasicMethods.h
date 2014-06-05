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

//Switch parameters
#define DEBOUNCE_TIME 2        /* time to wait while "de-bouncing" button */
#define LOCK_INPUT_TIME 200     /* time to wait after a button press */

//Output parameters
#define RELAY_FLIP_HIGH_VOLTAGE_TIME 1

//Momentary feature parameters
#define MOMENTARY_COUNTER_THRESHOLD 5


#define F_CPU 1200000UL // Define software reference clock for delay duration


//Global variables
bool relayPosition; //Keeps state for relay position, as this is a momentary change
bool effectState; //Keeps state for the effect

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
		//turn effect off
		TurnOn(RELAY_NEG);
		_delay_ms(RELAY_FLIP_HIGH_VOLTAGE_TIME);
		TurnOff(RELAY_NEG);	
	}	
	
	else //if it's 1
	{
		//turn effect on
		TurnOn(RELAY_POS);
		_delay_ms(RELAY_FLIP_HIGH_VOLTAGE_TIME);
		TurnOff(RELAY_POS);	
	}	
	
	relayPosition = desiredRelayPosition; //We've achieved the desired relay state
}

void TurnOnEffect()
{
	TurnOn(LED); //Drive LED high
	FlipRelay(1); //Switch relay position
	TurnOff(FET_MUTE); //Drive FET_MUTE low
	
	effectState = 1;
}

void TurnOffEffect()
{
	TurnOff(LED); //Drive LED low
	FlipRelay(0); //Switch relay position
	TurnOn(FET_MUTE); //Drive FET_MUTE high
	
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

#endif /* BASICMETHODS_H_ */
/*
* GccApplication1.cpp
*
* Created: 6/3/2014 11:32:17 AM
*  Author: MS71010
*/

#include "BasicMethods.h"

//Function prototypes
//void _delay_ms(uint16_t ms);
//void init_io();
//bool button_is_pressed();
//void toggle_led();


int main(void)
{
	InitGPIO();
	unsigned int momentaryCounter = 0;
		
	while(1)
	{
		if(ButtonIsPressed())
		{
			if(effectState == 0)
			{
				TurnOnEffect();
				_delay_ms(LOCK_INPUT_TIME);
				
				//Momentary prototype
				while (ButtonIsPressed() && momentaryCounter <= MOMENTARY_COUNTER_THRESHOLD)
				{
					momentaryCounter++;
					_delay_ms(100);
				}
				
				if(momentaryCounter > MOMENTARY_COUNTER_THRESHOLD) //If the button's been held down for long enough
				{
					//Enter momentary mode
					while (ButtonIsPressed()) //While the button's still pressed in momentary mode
					{
						_delay_ms(10); //Wait another few ms, then re-poll
					}
					
					TurnOffEffect(); //When button's no longer down, turn the effect off
					momentaryCounter = 0; //Reset momentary counter
				}

				
			}
			
			else
			{
				TurnOffEffect();
				//TurnOff(LED);
				_delay_ms(LOCK_INPUT_TIME);
				//buttonState = 0;
			}
			
		
		}
		
	}
}
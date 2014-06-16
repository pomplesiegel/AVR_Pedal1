/*
* GccApplication1.cpp
*
* Created: 6/3/2014 11:32:17 AM
*  Author: MS71010
*/

#include "BasicMethods.h"




int main(void)
{
	InitGPIO();
	

	while(1)
	{
		if(ButtonIsPressed())
		{
			if(effectState == 0)
			{
				TurnOnEffect();
				_delay_ms(LOCK_INPUT_TIME);

				
				if(MomentaryTriggerDetectedFromOff()) //If pedal is held down for long enough
				{
					EnterMomentaryMode(); //Enter momentary mode, in which holding down the pedal engages the effect until the user lets go
				}
				
			}
			
			
			
			else
			{
				//if(MomentaryTriggerDetectedFromOn()) //If pedal is held down for long enough
				//{
				//EnterMomentaryMode(); //Enter momentary mode, in which holding down the pedal engages the effect until the user lets go
				//}
				
				//else
				//{
				TurnOffEffect();
				_delay_ms(LOCK_INPUT_TIME);
				
				if(MomentaryTriggerDetectedFromOn())
				{
					WaitUntilButtonReleased();
				}
				
				//}
				
			}
			
			
			
			
		}
		
	}
}
/*
 * steps.h
 *
 *  Created on: 8 Sep 2017
 *      Author: a5015980
 */

#ifndef STEP_SELECT_H_
#define STEP_SELECT_H_

/*
 * The value of the macro STEP_SELECT controls the inclusion of sections of source code
 * for the relevant part of this lab.
 * This enables the application to be put together in a series of steps without the
 * lab attendee having to type in or cut and paste sections of code thus eliminating potential mistakes.
 *
 * STEP_SELECT == 0 : All of the application code is excluded from the build.
 * STEP_SELECT == 1 : Enables the section of code to where attendee needs to enter .open API calls.
 * STEP_SELECT == 2 : Enables the gpt callback function
 * STEP_SELECT == 3 : Enables the section of code to where attendee needs to enter .start API calls.
 * STEP_SELECT == 4 : Enables the ADC, DTC & 2nd GPT module

 */
#define STEP_SELECT   (0)

#endif /* STEP_SELECT_H_ */

#ifndef __TIMER_H
#define __TIMER_H

/** @defgroup timer timer
 * @brief This is the timer module, to handle or timer interrupt necessary applications
 * @{
 * Functions for manipulating timer
 */

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int timer_subscribe_int(void);

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_unsubscribe_int();

#endif /**@}*/

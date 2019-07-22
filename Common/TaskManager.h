/*
 * TaskManager.h
 *
 * Created: 23.08.2017 0:49:57
 *  Author: Sergey
 */ 


#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_

#include "bjdevlib_tb.h"

#define RELAY_MOMETRARY_CONTACT_CLOSURE_TIME 3 //ticks
#define POPUP_MESSAGE_TIME 30 //ticks
#define PRESET_BANK_NAME_TOGGLE_TIME 60 //ticks
#define DISCONNECT_KPA_TIME 200
#define TAP_OFF_TIME 5

/*
 * task Manager main loop. Should invoked in main loop
 */
void taskManagerLoop();

/*
 * Set task for relay to switch off during some time
 */
void setTaskRelayOff(uint8_t relayNum);

/*
 * Set task for redraw main screen after pop up message
 */
void setTaskMessageOff();

/*
 *  Set task for switching between preset name and bank name display on main screen
 */
void setTaskSwitchBetweenPresetNameAndBankName();

/*
 * If no any messages form kemper, disconnect it
 */
void setTaskDisconnectKpa();

/*
 * On/off tap 
 */
void setTaskTapOff();

#endif /* TASKMANAGER_H_ */
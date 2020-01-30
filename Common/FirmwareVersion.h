/*
 * FirmwareVersion.h
 *
 * Created: 09.01.2018 13:08:43
 *  Author: User
 */ 


#ifndef FIRMWAREVERSION_H_
#define FIRMWAREVERSION_H_

#define FIRMWARE_VERSION_STRING "3.2.6b"

#ifdef TB_12_DEVICE
#	define HARDWARE_MODEL_STRING	"BJ Devices TB-12"

#elif defined (TB_11P_DEVICE)
#	define HARDWARE_MODEL_STRING	"BJ Devices TB-11P"

#elif defined (TB_6P_DEVICE)
#	define HARDWARE_MODEL_STRING	"BJ Devices TB-6P"

#elif defined (TB_8_DEVICE)
#	define HARDWARE_MODEL_STRING	"BJ Devices TB-8"

#elif defined (TB_5_DEVICE)
#	define HARDWARE_MODEL_STRING	"BJ Devices TB-5"
#endif

#endif /* FIRMWAREVERSION_H_ */
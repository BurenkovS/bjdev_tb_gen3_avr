/*
 * ExpressionProcess.h
 *
 * Created: 24.10.2017 23:42:24
 *  Author: Sergey
 */ 


#ifndef EXPRESSIONPROCESS_H_
#define EXPRESSIONPROCESS_H_

#include <stdint.h>

#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
#define  TOTAL_EXPRESSIONS_PEDALS 3
#else
#define  TOTAL_EXPRESSIONS_PEDALS 2
#endif

void expressionProcess();
/*
 * This should be invoked after global and banks settings initialization
 */
void expressionProcessSetInitPositionsAndRange();

/*
 * 
 */
uint8_t leadToRange(uint8_t pedalNum, uint8_t pedalPosition);
uint8_t convertAccordingPedalType(uint8_t pedalPosInRange, uint8_t pedalNum);



#endif /* EXPRESSIONPROCESS_H_ */
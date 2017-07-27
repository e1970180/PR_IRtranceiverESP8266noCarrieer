#include "PR_IRtranceiverESP8266noCarrieer.h"

//*******	SENDER	******
void	PR_IRtranceiverESP8266noCarrier::senderSetup(uint8_t pin, bool senderMarkLevel, irCarriedFreq_t carriedFreq = 0) {
	_senderPin = pin;
	_senderMarkLevel = senderMarkLevel;
}

//**********************************************************  SENDER	*****************************************

void	PR_IRtranceiverESP8266noCarrier::senderBegin()	{
	receiverEnd();
	pinMode(_senderPin , OUTPUT);
	digitalWrite(_senderPin, !senderMarkLevel);
}															//prepare HW for sending
void	PR_IRtranceiverESP8266noCarrier::senderEnd()	{
	digitalWrite(_senderPin, !senderMarkLevel);	
	pinMode(_senderPin, INPUT);
}

void inline	PR_IRtranceiverESP8266noCarrier::sendMark(irMicrosec_t	t) {	//without carrier!
	
	digitalWrite(_senderPin, senderMarkLevel);			//GPIO_OUTPUT_SET(_pin, logicLevelOn);			
	if (t > 0) os_delay_us(t);
}
//static void ICACHE_FLASH_ATTR 
void inline	PR_IRtranceiverESP8266noCarrier::sendSpace(irMicrosec_t	t) {
	
	digitalWrite(_senderPin, !senderMarkLevel);		//GPIO_OUTPUT_SET(_pin, !logicLevelOn);		
	if (t > 0) os_delay_us(t);
}

void 	sendRaw(irBuferRaw_t &irSeqRaw, int16_t buferLenght) {

	uint16_t n = 0;
	if ( buferLenght % 2 != 0 ) {	//if seq is not even correct it
		buferLenght--;	
	}
	if ( buferLenght <= 2) return;	//we could't send less then Mark+Space				
	do {
		sendMark (irSeqRaw[n++]);
		sendSpace(irSeqRaw[n++]);
	} while (n < buferLenght)
}

//********************************************* 	RECIVER		*********************************

void	PR_IRtranceiverESP8266noCarrier::receiverSetup(uint8_t pin, bool receiverMarkLevel) {
	_receiverPin = pin;
	_receiverMarkLevel = senderMarkLevel;
}

void	PR_IRtranceiverESP8266noCarrier::setOnRecevedCallback((*onReceivedfunc)(irBuferRaw_t&, uint16_t  ???)) {
	_onReceivedfunc	= onReceivedfunc;
}

void inline ICACHE_FLASH_ATTR IRtimerStart() {
  _receiverTimer = micros();
}

uint32_t inline ICACHE_FLASH_ATTR IRtimerElapsed() {
	//return duration and start timer again
  uint32_t tmp = _receiverTimer;
  
  _receiverTimer = micros();
  if (tmp <= _receiverTimer)  // Check if the system timer has wrapped.
    return (_receiverTimer - tmp);  // No wrap.
  else
    return (0xFFFFFFFF - tmp + _receiverTimer);  // Has wrapped.
}



void	PR_IRtranceiverESP8266noCarrier::receiverBegin(irBuferRaw_t &irSeqRaw, uint16_t irSeqRawLenght, const irMicrosec_t maxDuration, void (*onReceivedfunc)(irBuferRaw_t&, uint16_t))	{
	
	senderEnd();
	
	_receivedCounter = 0;
	_buferLenght	= buferLenght;
	_maxDuration	= maxDuration;
	_bufer			= bufer;
	_receiverState	= WAITE;
	
	if (onReceivedfunc) setOnRecevedCallback(onReceivedfunc);
	
	attachInterrupt(_receiverPin, PR_IRreceiver_ISR, CHANGE);
}															

void	PR_IRtranceiverESP8266noCarrier::receiverEnd()	{
	_receiverState	= STOP;
	detachInterrupt(_receiverPin);	// stop ISR by pinChange;
	
}

//TODO  how to attach class funct as ISR?
void	PR_IRreceiver_ISR()	{		//ISR for pinChange
	

}

void	PR_IRtranceiverESP8266noCarrier::ISR()	{		//ISR for pinChange
	
	switch (_receiverState) {
		case	WAITE:
			IRtimerStart();		
			_receiverState = RECEIVING;		
			break;
		case	RECEIVING:	
			_bufer[_receivedCounter] = IRtimerElapsed();
			if (_receivedCounter < _buferLenght) _receivedCounter++;	//prevent bufer overflow
			break;
		default:
	}
}


void	PR_IRtranceiverESP8266noCarrier::loop() {
	
	if (_receiverState == RECEIVING) {

		uint32_t duration = micros();
		if (duration <= _receiverTimer)  // Check if the system timer has wrapped.
			duration = _receiverTimer - duration;  // No wrap.
		else
			duration = 0xFFFFFFFF - duration + _receiverTimer;  // Has wrapped.
		
		if ( (duration > _maxDuration) || (_receivedCounter => _buferLenght) )	{
			receiverEnd();
			(*_onReceivedfunc)(&_bufer, _receivedCounter); //invoke callback
		}				
	}
}

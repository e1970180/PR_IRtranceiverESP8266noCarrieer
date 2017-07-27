#ifndef PR_IRtranceiverESP8266noCarrieer_h
#define PR_IRtranceiverESP8266noCarrieer_h

    #include <arduino.h>

	typedef	irCarriedFreq_t		uint16_t;	
	typedef	irMicrosec_t		uint16_t;
	typedef	irBuferRaw_t		uint16_t;

	class PR_IRtranceiverESP8266noCarrier {
		
		public:
			PR_IRtranceiverESP8266noCarrier();

		//*******	SENDER	******		
			void	senderSetup(uint8_t pin, bool senderMarkLevel, irCarriedFreq_t carriedFreq = 0);
			void	senderBegin();															//prepare  HW for sending
			void	senderEnd();
			void	sendMark(irMicrosec_t	t);
			void	sendSpace(irMicrosec_t	t);
			//void	sendRaw(irBuferRaw_t &irSeqRaw, int16_t buferLenght);
		
		//*******	RECEIVER	******			
			void	receiverSetup(uint8_t pin, bool receiverMarkLevel);
			void	receiverBegin(irBuferRaw_t &irSeqRaw, uint16_t irSeqRawLenght, const irMicrosec_t maxDuration, void (*onReceivedfunc)(irBuferRaw_t&, uint16_t) );															//prepare HW for receining
			void	receiverEnd();			
			void	setOnRecevedCallback((*onReceivedfunc)(irBuferRaw_t&, uint16_t));
			
			void	receiverLoop();
			
		protected:
		//*******	SENDER	******
			uint8_t				_senderPin;
			bool				_senderMarkLevel;
		//*******	RECEIVER	******	
			uint8_t				_receiverPin;
			bool				_receiverMarkLevel;			
			void 				(*_onReceivedfunc)(irBuferRaw_t&, uint16_t) = NULL;
			
		enum ReceiverState {WAITE = 0, RECEIVING, STOP };	
			
			volatile ReceiverState	_receiverState;
			volatile uint16_t		_recivedCounter = 0;
			uint16_t				_buferLenght;
			irMicrosec_t			_maxDuration;
			volatile irBuferRaw_t	&_irSeqRaw;
			
			volatile uint32_t		_receiverTimer;
			
			
	};

	extern  void    callbackOnReceivedIR(irBuferRaw_t buferRaw, uint16_t length);

	
#endif
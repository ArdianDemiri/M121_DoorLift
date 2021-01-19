#include <Atmega328P.h>
#include <avrlib.h>
#include <RegisterAccess.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>

void doorOpened(const Message* msg);
void doorClosed(const Message* msg);
void doorIsClosing(const Message* msg);
void doorIsOpening(const Message* msg);


void firstStatus(const Message* msg);
void lightBarrierClose(void);
void lightBarrierOpen(void);


Fsm fsm = {.RxMask = 0xff, .CurrentState = doorOpened};
Bool CustomAvrMessageHandler(const AvrMessage* msg);

typedef enum interaction
{
	open = 0,
	close = 1,
	lightBarrierClosed = 2,
	evLightBarrierOpen = 3
} messageHandler;

int main(void)
{
	PortD.DDR = 3<<5;

	Tcnt0.TCCRA = (1) | (2 << 4) | (2 << 6);
	Tcnt0.TCCRB = 0x3;
	Tcnt0.OCRA = 0;
	Tcnt0.OCRB = 0;

	Usart_Init(250000); // higher is to fast; cannot be consumed reliably anymore!	
	TRACE("Test1");
	RegisterFsm(&fsm);
	RegisterExternalInteruptHandler(ExtInterruptSource0, ExtIntTrigger_OnRaisingEdge, lightBarrierClose);
	RegisterExternalInteruptHandler(ExtInterruptSource1, ExtIntTrigger_OnRaisingEdge, lightBarrierOpen);
	RegisterAvrMessageHandler(CustomAvrMessageHandler);
	InitializeStateEventFramework();
	return 0;
}

void lightBarrierClose(void) {
	SendMessage(1, lightBarrierClosed, 0, 0);

	/*
	prio	Priorität, mit welcher die Meldung bearbeitet werden soll
	id	Id der Meldung
	msgLow	low byte des Meldungs - Parameters
	msgHigh	upper byte des Meldungs - Parameters
	*/
}

void lightBarrierOpen(void) {
	SendMessage(1, evLightBarrierOpen, 0, 0);

	/*
	prio	Priorität, mit welcher die Meldung bearbeitet werden soll
	id	Id der Meldung
	msgLow	low byte des Meldungs - Parameters
	msgHigh	upper byte des Meldungs - Parameters
	*/
}

void firstStatus(const Message* msg) {
	TRACE("test");
}

void doorIsOpening(const Message* msg) {
	// SendMessage( 1, open, 0, 0);
	TRACE("doorIsOpening %8", msg->Id);
	if (msg->Id == lightBarrierClosed) {
		fsm.CurrentState = open;
		Tcnt0.OCRA = 0;
		Tcnt0.OCRB = 0;
	}
}

void doorIsClosing(const Message* msg) {
	// SendMessage(1, close, 0, 0);
	TRACE("doorIsClosing %8", msg->Id);
	if (msg->Id == evLightBarrierOpen) {
		fsm.CurrentState = close;
		Tcnt0.OCRA = 0;
		Tcnt0.OCRB = 0;
	}
}

void doorClosed(const Message* msg) {
	// SendMessage(1, lightBarrierClosed, 0, 0);
	// PWM starten und Ports konfigurieren
	TRACE("doorClosed %8", msg->Id);
	if (msg->Id == open) {
		fsm.CurrentState = doorIsOpening;
		Tcnt0.OCRA = 0;
		Tcnt0.OCRB = 50;
	}
}

void doorOpened(const Message* msg) {
	// SendMessage(1, lightBarrierOpen, 0, 0);
	// PWM starten und Ports konfigurieren
	TRACE("doorOpened %8", msg->Id);
	if (msg->Id == close) {
		fsm.CurrentState = doorIsClosing;
		Tcnt0.OCRA = 50;
		Tcnt0.OCRB = 0;
	}
}

Bool CustomAvrMessageHandler(const AvrMessage* msg)
{
	if (DefaultMessageHandler(msg)) return True;
	// jetzt kommt die Behandlung der Meldung
	TRACE("Message Handler %8", msg->Payload[0]);
	if (msg->Payload[0] == 0xCF) {
		SendMessage( 1, open, 0, 0);
	}
	else if (msg->Payload[0] == 0xEF) {
		SendMessage(1, close, 0, 0);
	}
	return True;
}

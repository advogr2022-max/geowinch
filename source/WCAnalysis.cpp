/*
WinchControl    Jörg Müller     WCAnalysis.cpp

Klassenimplementation GUI

Portierbarkeit: ja
*/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WCAnalysis.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

// Steuergeräte-CAN - Konfiguration
const TPCANHandle 	WCACNST_CtlUnitCAN_Device 	= PCAN_USBBUS1;
const TPCANHandle 	WCACNST_CtlUnitCAN_Device_2 = PCAN_USBBUS2;					// Alternative CAN-Schnittstelle
const TPCANBaudrate WCACNST_CtlUnitCAN_Baudrate = PCAN_BAUD_250K;

const WCACNST_CtlUnitCAN_InitInterval			= 1000;

const WCACNST_CtlUnitCAN_StatusLvl_NotAvailable = 0;	// CAN nicht verfügbar
const WCACNST_CtlUnitCAN_StatusLvl_APILoaded 	= 1;	// API geladen, CAN nicht initialisiert
const WCACNST_CtlUnitCAN_StatusLvl_Ready	 	= 2;	// CAN initialisiert, verfügbar
const WCACNST_CtlUnitCAN_StatusLvl_BusError 	= 3;	// CAN-Bus Fehler, Reinitialisierung notwendig

//---------------------------------------------------------------------------
// Konstruktor
WCAnalysis::WCAnalysis()
{
	FirstRun = true;

	V = 0;

	CtlUnitCAN_VarInit();
};

//---------------------------------------------------------------------------
// Destruktor
WCAnalysis::~WCAnalysis()
{
};

// ==========================================================================
// VERWALTUNG
// ==========================================================================
//---------------------------------------------------------------------------
// Aufräumen
void WCAnalysis::CleanUp()
{
		IOControl.CleanUp();
		CtlUnitCAN.Uninit();
		CtlUnitCAN.UnLoadAPI();
};

//---------------------------------------------------------------------------
// Initialisieren der Klasse
// erst Wenn V != 0 ist, kann mit der Klasse gearbeitet werden
void WCAnalysis::Init(WCVarControl *VarControl, WCGenOrganisationS *GenOrgan)
{
        FirstRun = true;        // der erste "Analyse"-Durchlauf ist noch nicht erfolgt

        // Interne Taktung, Starteinstellung
        Clock100ms_cnt = 0;
        LastClock = GetProcessTime_in_ms();                                     // LastClock gleich für spätere Grundinitialisierung innerhalb dieser Methode verwenden (spart mehrere clock()-Funktionsaufrufe)

        // diverse Initialisierung
        EngineStopSignal = false;
        EngineStopSignalOffDelay = false;
        EngineRevolutionAutoIsOn = false;                               

        PowerFailFunctionActive = false;

        // Prüfen, ob Zeiger-Wert i.O. ist
        try
        {
                VarControl->devel();
        }catch(...)
        {
                V = 0;
                return;
        };

        V = VarControl;

        // Initialisieren der Hilfsvariablen
        if (V != 0)
        {
                GetDepth_Time_old = LastClock;
                GetDirection_Depth_old = V->GetInfo_CurrDepth();
//                GetVelocity_Depth_old = V->GetInfo_CurrDepth();
                GetVelocity_Time_old = LastClock;
                GetVelocity_Real = V->GetInfo_CurrVelocity();
                GetTemperature_Time_old = LastClock;
        };

		CalibTension_AverageManager.SetSize(CalibTension_AverageBuffer_SIZE);   // Hilfspuffer für Zugkraftkalibrierung

/*        for (int i = 0; i < Calib_Traction_MeanSqareBuffer_SIZE; ++i)
        {
                Calib_Traction_MeanSqareBuffer[i] = 0;
        };
*/
        for (int i = 0; i < VELOCITY_CALC_BUF_SIZE; ++i)
        {
                VelocityCalcBuffer[i].TimeDiff = 0;
                VelocityCalcBuffer[i].Depth = 0;
        };
        CurrVelocityCalcBufIndex = 0;

        // Prüfen, ob Zeiger-Wert i.O. ist
        try
        {
                GenOrgan->CurrFunctionPanel;
        }catch(...)
        {
                GenOrg = 0;
                return;
        };

        GenOrg = GenOrgan;

        // Grundinitialisierung der Komponente
        IOControl.Init();

        // Schnittstelle zum Teufenrechner öffnen
        IOControl.InitDepthCalcCOM(GenOrg);

        // Daten-Ein/Ausgabe-Klasse initialisieren
        IOControl.InitInfoCOM(GenOrg);

        // Schnittstellenkarten DIO/DAC initialisieren
        IOControl.InitDIOCard(GenOrg);

		// Standardflags/-werte setzen
		LostMotionImpulseOn = false;

};

// ==========================================================================
// Steuergeräte-CAN
// ==========================================================================

// --------------------------------------------------------------------------
// Variableninitialisierung für Steuergeräte-CAN-Kommunikation
void WCAnalysis::CtlUnitCAN_VarInit() {

	clock_t Countdown;

	CtlUnitCAN_TxNominee = 0;

	CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.ID = 0x101;
	CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.LEN = 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CtlUnitCAN_MsgBuf.Msg_0x00000101.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000101.Expected = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000101.Delayed = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000101.tx_queue_ctdwn = 0xFF;

	CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.ID = 0x102;
	CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.LEN = 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CtlUnitCAN_MsgBuf.Msg_0x00000102.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000102.Expected = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000102.Delayed = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000102.tx_queue_ctdwn = 0xFF;

	CtlUnitCAN_MsgBuf.Msg_0x00000103.CANMsg.ID = 0x103;
	CtlUnitCAN_MsgBuf.Msg_0x00000103.CANMsg.LEN = 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000103.CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CtlUnitCAN_MsgBuf.Msg_0x00000103.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000103.Expected = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000103.Delayed = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000103.tx_queue_ctdwn = 0xFF;

	CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.ID = 0x104;
	CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.LEN = 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CtlUnitCAN_MsgBuf.Msg_0x00000104.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000104.Expected = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000104.Delayed = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000104.tx_queue_ctdwn = 0xFF;

	CtlUnitCAN_MsgBuf.Msg_0x00000201.CANMsg.ID = 0x201;
	CtlUnitCAN_MsgBuf.Msg_0x00000201.CANMsg.LEN = 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000201.CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CtlUnitCAN_MsgBuf.Msg_0x00000201.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000201.Expected = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000201.Delayed = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000201.tx_queue_ctdwn = 0xFF;

	CtlUnitCAN_MsgBuf.Msg_0x00000204.CANMsg.ID = 0x204;
	CtlUnitCAN_MsgBuf.Msg_0x00000204.CANMsg.LEN = 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000204.CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CtlUnitCAN_MsgBuf.Msg_0x00000204.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000204.Expected = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000204.Delayed = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000204.tx_queue_ctdwn = 0xFF;

	CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.ID = 0x301;
	CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.LEN = 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CtlUnitCAN_MsgBuf.Msg_0x00000301.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000301.Expected = true; // zyklisch
	CtlUnitCAN_MsgBuf.Msg_0x00000301.Delayed = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000301.tx_queue_ctdwn = 0xFF;

	CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.ID = 0x302;
	CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.LEN = 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CtlUnitCAN_MsgBuf.Msg_0x00000302.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000302.Expected = true; // zyklisch
	CtlUnitCAN_MsgBuf.Msg_0x00000302.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000302.tx_queue_ctdwn = 0xFF;

	CtlUnitCAN_MsgBuf.Msg_0x00000303.CANMsg.ID = 0x303;
	CtlUnitCAN_MsgBuf.Msg_0x00000303.CANMsg.LEN = 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000303.CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CtlUnitCAN_MsgBuf.Msg_0x00000303.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000303.Expected = true; // zyklisch
	CtlUnitCAN_MsgBuf.Msg_0x00000303.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000303.tx_queue_ctdwn = 0xFF;

	CtlUnitCAN_MsgBuf.Msg_0x00000304.CANMsg.ID = 0x304;
	CtlUnitCAN_MsgBuf.Msg_0x00000304.CANMsg.LEN = 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000304.CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CtlUnitCAN_MsgBuf.Msg_0x00000304.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000304.Expected = true; // zyklisch
	CtlUnitCAN_MsgBuf.Msg_0x00000304.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000304.tx_queue_ctdwn = 0xFF;

	CtlUnitCAN_MsgBuf.Msg_0x00000305.CANMsg.ID = 0x305;
	CtlUnitCAN_MsgBuf.Msg_0x00000305.CANMsg.LEN = 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000305.CANMsg.MSGTYPE = PCAN_MESSAGE_STANDARD;
	CtlUnitCAN_MsgBuf.Msg_0x00000305.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000305.Expected = true; // zyklisch
	CtlUnitCAN_MsgBuf.Msg_0x00000305.Modified = false;
	CtlUnitCAN_MsgBuf.Msg_0x00000305.tx_queue_ctdwn = 0xFF;

	CtlUnitCAN_StatusLvl = WCACNST_CtlUnitCAN_StatusLvl_NotAvailable;

	IsTimeElapsed(&CtlUnitCAN_ReInit_Time, &Countdown, WCACNST_CtlUnitCAN_InitInterval, 1, true); // CtlUnitCAN_ReInit_Time initialisieren

}

// --------------------------------------------------------------------------
// Zustandsebenensteuerung für Steuergeräte-CAN
void WCAnalysis::CtlUnitCAN_StatusLevelCtl() {

	clock_t Countdown; // Dummy

	switch (CtlUnitCAN_StatusLvl) {

		case WCACNST_CtlUnitCAN_StatusLvl_NotAvailable:

			// API laden
			if (CtlUnitCAN.LoadAPI()) {

				GenOrg->Evnt >> 27;
				CtlUnitCAN_StatusLvl = WCACNST_CtlUnitCAN_StatusLvl_APILoaded;

			} else GenOrg->Evnt << 27;

			// direkt weiter zur Initialisierung

		case WCACNST_CtlUnitCAN_StatusLvl_APILoaded:

			if (IsTimeElapsed(&CtlUnitCAN_ReInit_Time, &Countdown, WCACNST_CtlUnitCAN_InitInterval, 1, false)) {


				// Schnittstelle initialisieren
				if ((CtlUnitCAN.Init(WCACNST_CtlUnitCAN_Device, WCACNST_CtlUnitCAN_Baudrate) == PCAN_ERROR_OK ) ||	// primäre SS
					(CtlUnitCAN.Init(WCACNST_CtlUnitCAN_Device_2, WCACNST_CtlUnitCAN_Baudrate) == PCAN_ERROR_OK ))  // sekundäre SS (Ersatz)
				{
					GenOrg->Evnt >> 28;
					CtlUnitCAN_StatusLvl = WCACNST_CtlUnitCAN_StatusLvl_Ready;	// Schnittstelle für Verwendung bereit
				} else GenOrg->Evnt << 28;

				// Software Version des Steuergeräts muss nach dem
				// Initialisieren neu eingelesen werden
                // Standardwert bleibt solange aktiv
				V->SetInfo_CtlUnitSWVersion(const_cast<char*>(STD_CtlUnitSWVersion));
				V->SetControl_GetCtlUnitSWVersionOn();
			}

			break;

		case WCACNST_CtlUnitCAN_StatusLvl_BusError:

			if (IsTimeElapsed(&CtlUnitCAN_ReInit_Time, &Countdown, WCACNST_CtlUnitCAN_InitInterval, 1, false)) {

				// Schnittstelle deinitialisieren
				CtlUnitCAN.Uninit();
				CtlUnitCAN_StatusLvl = WCACNST_CtlUnitCAN_StatusLvl_APILoaded; 	// Neuinitialisierung anstoßen
			}

			break;

		case WCACNST_CtlUnitCAN_StatusLvl_Ready:
			// Schnittstelle einsatzbereit
			break;

	default:
		;
	}

}

//---------------------------------------------------------------------------
// Steuergeräte CAN-Botschaft einlesen
void WCAnalysis::GetCtlUnitCAN_I() {

	clock_t Countdown;
	TPCANStatus CanStatus;
	TPCANMsg Msg;
	bool CANCOMERR = false;	// Flag für Sign. Fehler in der CAN-Kommunikation

	// Wenn CAN nicht bereit, dann keine Abfragen notwendig
	if (CtlUnitCAN_StatusLvl != WCACNST_CtlUnitCAN_StatusLvl_Ready) return;

	do {
			// CAN-Abfrage
			CanStatus = CtlUnitCAN.Read(&Msg);

			// Botschafts-Erfassung
			switch (Msg.ID) {
			case 0x201:
				for (int i = 0; i < 8; ++i)
					CtlUnitCAN_MsgBuf.Msg_0x00000201.CANMsg.DATA[i] = Msg.DATA[i];
				CtlUnitCAN_MsgBuf.Msg_0x00000201.Expected = false;
				CtlUnitCAN_MsgBuf.Msg_0x00000201.Modified = true;
				IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000201.ModTS, &Countdown, 1000, 1, true);
				CtlUnitCAN_MsgBuf.Msg_0x00000201.Delayed = false;
				break;
			case 0x204:
				for (int i = 0; i < 8; ++i)
					CtlUnitCAN_MsgBuf.Msg_0x00000204.CANMsg.DATA[i] = Msg.DATA[i];
				CtlUnitCAN_MsgBuf.Msg_0x00000204.Expected = false;
				CtlUnitCAN_MsgBuf.Msg_0x00000204.Modified = true;
				IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000204.ModTS, &Countdown, 1000, 1, true);
				CtlUnitCAN_MsgBuf.Msg_0x00000204.Delayed = false;
				break;
			case 0x301:
				for (int i = 0; i < 8; ++i)
					CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.DATA[i] = Msg.DATA[i];
				// Expected bleibt gesetzt, da zyklische Botschaft
				CtlUnitCAN_MsgBuf.Msg_0x00000301.Modified = true;
				IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000301.ModTS, &Countdown, 1000, 1, true);
				CtlUnitCAN_MsgBuf.Msg_0x00000301.Delayed = false;
				break;
			case 0x302:
				for (int i = 0; i < 8; ++i)
					CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[i] = Msg.DATA[i];
				// Expected bleibt gesetzt, da zyklische Botschaft
				CtlUnitCAN_MsgBuf.Msg_0x00000302.Modified = true;
				IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000302.ModTS, &Countdown, 1000, 1, true);
				CtlUnitCAN_MsgBuf.Msg_0x00000302.Delayed = false;
				break;
			case 0x303:
				for (int i = 0; i < 8; ++i)
					CtlUnitCAN_MsgBuf.Msg_0x00000303.CANMsg.DATA[i] = Msg.DATA[i];
				// Expected bleibt gesetzt, da zyklische Botschaft
				CtlUnitCAN_MsgBuf.Msg_0x00000303.Modified = true;
				IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000303.ModTS, &Countdown, 1000, 1, true);
				CtlUnitCAN_MsgBuf.Msg_0x00000303.Delayed = false;
				break;
			case 0x304:
				for (int i = 0; i < 8; ++i)
					CtlUnitCAN_MsgBuf.Msg_0x00000304.CANMsg.DATA[i] = Msg.DATA[i];
				// Expected bleibt gesetzt, da zyklische Botschaft
				CtlUnitCAN_MsgBuf.Msg_0x00000304.Modified = true;
				IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000304.ModTS, &Countdown, 1000, 1, true);
				CtlUnitCAN_MsgBuf.Msg_0x00000304.Delayed = false;
				break;
			case 0x305:
				for (int i = 0; i < 8; ++i)
					CtlUnitCAN_MsgBuf.Msg_0x00000305.CANMsg.DATA[i] = Msg.DATA[i];
				// Expected bleibt gesetzt, da zyklische Botschaft
				CtlUnitCAN_MsgBuf.Msg_0x00000305.Modified = true;
				IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000305.ModTS, &Countdown, 1000, 1, true);
				CtlUnitCAN_MsgBuf.Msg_0x00000305.Delayed = false;
				break;
			default:
				;
			}
	} while ((CanStatus == PCAN_ERROR_OK) || (CanStatus == PCAN_ERROR_BUSLIGHT));

	// -------------------------------------------------------------------------
	// Ausbleibende erwartete Botschaften erkennen

	if (IsTimeElapsed(	&CtlUnitCAN_MsgBuf.Msg_0x00000201.ModTS,
						&Countdown,
						500,	// ...ms lang keine Antwort
						1,
						!CtlUnitCAN_MsgBuf.Msg_0x00000201.Expected) &&
		CtlUnitCAN_MsgBuf.Msg_0x00000201.Expected) {
		for (int i = 0; i < 8; ++i) CtlUnitCAN_MsgBuf.Msg_0x00000201.CANMsg.DATA[i] = 0;
		IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000201.ModTS, &Countdown, 1000, 1, true); // Zeitfenster zurücksetzen
		CtlUnitCAN_MsgBuf.Msg_0x00000201.Expected = false;
		CtlUnitCAN_MsgBuf.Msg_0x00000201.Delayed = true;
	};

	if (IsTimeElapsed(	&CtlUnitCAN_MsgBuf.Msg_0x00000204.ModTS,
						&Countdown,
						500,	// ...ms lang keine Antwort
						1,
						!CtlUnitCAN_MsgBuf.Msg_0x00000204.Expected) &&
		CtlUnitCAN_MsgBuf.Msg_0x00000204.Expected) {
		for (int i = 0; i < 8; ++i) CtlUnitCAN_MsgBuf.Msg_0x00000204.CANMsg.DATA[i] = 0;
		IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000204.ModTS, &Countdown, 1000, 1, true); // Zeitfenster zurücksetzen
		CtlUnitCAN_MsgBuf.Msg_0x00000204.Expected = false;
		CtlUnitCAN_MsgBuf.Msg_0x00000204.Delayed = true;
	};

	if (IsTimeElapsed(	&CtlUnitCAN_MsgBuf.Msg_0x00000301.ModTS,
						&Countdown,
						500,	// ...ms lang keine Antwort
						1,
						!CtlUnitCAN_MsgBuf.Msg_0x00000301.Expected) &&
		CtlUnitCAN_MsgBuf.Msg_0x00000301.Expected) {
		for (int i = 0; i < 8; ++i) CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.DATA[i] = 0;
		IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000301.ModTS, &Countdown, 1000, 1, true); // Zeitfenster zurücksetzen
		// Expected bleibt gesetzt, da zyklische Botschaft
		CtlUnitCAN_MsgBuf.Msg_0x00000301.Delayed = true;
	};

	if (IsTimeElapsed(	&CtlUnitCAN_MsgBuf.Msg_0x00000302.ModTS,
						&Countdown,
						5000,	// ...ms lang keine Antwort
						1,
						!CtlUnitCAN_MsgBuf.Msg_0x00000302.Expected) &&
		CtlUnitCAN_MsgBuf.Msg_0x00000302.Expected) {
		for (int i = 0; i < 8; ++i) CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[i] = 0;
		IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000302.ModTS, &Countdown, 1000, 1, true); // Zeitfenster zurücksetzen
		// Expected bleibt gesetzt, da zyklische Botschaft
		CtlUnitCAN_MsgBuf.Msg_0x00000302.Delayed = true;
	};

	if (IsTimeElapsed(	&CtlUnitCAN_MsgBuf.Msg_0x00000303.ModTS,
						&Countdown,
						2000,	// ...ms lang keine Antwort
						1,
						!CtlUnitCAN_MsgBuf.Msg_0x00000303.Expected) &&
		CtlUnitCAN_MsgBuf.Msg_0x00000303.Expected) {
		for (int i = 0; i < 8; ++i) CtlUnitCAN_MsgBuf.Msg_0x00000303.CANMsg.DATA[i] = 0;
		IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000303.ModTS, &Countdown, 1000, 1, true); // Zeitfenster zurücksetzen
		// Expected bleibt gesetzt, da zyklische Botschaft
		CtlUnitCAN_MsgBuf.Msg_0x00000303.Delayed = true;
	};

	if (IsTimeElapsed(	&CtlUnitCAN_MsgBuf.Msg_0x00000304.ModTS,
						&Countdown,
						2000,	// ...ms lang keine Antwort
						1,
						!CtlUnitCAN_MsgBuf.Msg_0x00000304.Expected) &&
		CtlUnitCAN_MsgBuf.Msg_0x00000304.Expected) {
		for (int i = 0; i < 8; ++i) CtlUnitCAN_MsgBuf.Msg_0x00000304.CANMsg.DATA[i] = 0;
		IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000304.ModTS, &Countdown, 1000, 1, true); // Zeitfenster zurücksetzen
		// Expected bleibt gesetzt, da zyklische Botschaft
		CtlUnitCAN_MsgBuf.Msg_0x00000304.Delayed = true;
	};

	if (IsTimeElapsed(	&CtlUnitCAN_MsgBuf.Msg_0x00000305.ModTS,
						&Countdown,
						500, // ...ms lang keine Antwort
						1,
						!CtlUnitCAN_MsgBuf.Msg_0x00000305.Expected)&&
		CtlUnitCAN_MsgBuf.Msg_0x00000305.Expected) {
		for (int i = 0; i < 8; ++i) CtlUnitCAN_MsgBuf.Msg_0x00000305.CANMsg.DATA[i] = 0;
		IsTimeElapsed(&CtlUnitCAN_MsgBuf.Msg_0x00000305.ModTS, &Countdown, 1000, 1, true); // Zeitfenster zurücksetzen
		// Expected bleibt gesetzt, da zyklische Botschaft
		CtlUnitCAN_MsgBuf.Msg_0x00000305.Delayed = true;
	};

	// -------------------------------------------------------------------------
	// Reaktion auf ausbleibende CAN-Botschaften

	// 201
	if (CtlUnitCAN_MsgBuf.Msg_0x00000201.Delayed) {

		V->SetInfo_CtlUnitSWVersion("---");     // Software-Version als unbekannt markieren
		V->SetControl_GetCtlUnitSWVersionOn();	// Software-Version neu abfragen
		CtlUnitCAN_MsgBuf.Msg_0x00000201.Delayed = false;
		CANCOMERR = true;
	}

	// 204
	if (CtlUnitCAN_MsgBuf.Msg_0x00000204.Delayed) {

		V->SetControl_SetDepthCalFactor_On();		// Kalibrierfaktor Teufe neu setzen
		CtlUnitCAN_MsgBuf.Msg_0x00000204.Delayed = false;
		CANCOMERR = true;
	}

	// 301, 302, 303, 304, 305
	if (
			CtlUnitCAN_MsgBuf.Msg_0x00000301.Delayed ||
			CtlUnitCAN_MsgBuf.Msg_0x00000302.Delayed ||
			CtlUnitCAN_MsgBuf.Msg_0x00000303.Delayed ||
			CtlUnitCAN_MsgBuf.Msg_0x00000304.Delayed ||
			CtlUnitCAN_MsgBuf.Msg_0x00000305.Delayed) {

		CANCOMERR = true;
	}

	// --->
	if (CanStatus == PCAN_ERROR_BUSHEAVY) {		// Bus überlastet, oder gar keine Botschaften mehr -> CAN-Modul könnte gerade ausgetauscht werden -> neu abfragen und initialisieren
		V->SetInfo_CtlUnitSWVersion("---");     // Software-Version als unbekannt markieren
		V->SetControl_GetCtlUnitSWVersionOn();	// Software-Version neu abfragen, da u.U. ein neues Gerät angeschlossen wurde.
		V->SetControl_SetDepthCalFactor_On();	// Kalibrierfaktor Teufe neu setzen (es kann nicht garantiert werden, dass dem Steuergerät der Wert bekannt ist)
	}

	if (CANCOMERR) GenOrg->Evnt << 29;
	else GenOrg->Evnt >> 29;
}

//---------------------------------------------------------------------------
// Steuergeräte CAN Sendewarteschlangenverwaltung
// Mit jedem Aufruf wird eine CAN-Botschaft nominiert, welche als nächstes
// gesendet wird. Dabei wird der Warteschlangenzähler der internen Puffer
// aktualisiert und ausgewertet.
void WCAnalysis::CtlUnitCAN_TxQueueAdmin() {

	// Jede Botschaft wird gemäß ihrer Priorität in die Sendewarteschlange
	// eingefügt, sofern ein Sendeauftrag vorhanden ist.
	// Zyklische Botschaften besitzen immer einen Sendeauftrag und werden, ohne
	// Beachtung des Flags "Modified", mit jedem Aufruf dieser Methode
	// eingeordnet.
	// Befindet sich eine Botschaft bereits in der Warteschlange, dann wird
	// lediglich dessen Priorität erhöht (Zähler dekrementiert).
	// Ist der Zähler gleich Null, dann wird dieser bis zum erfolgreichen
	// Senden der Warteschlange beibehalten.

	// Warteschlange aktualisieren
	// -----------------------------
	// Ereignisabhängige Botschaften
	if (CtlUnitCAN_MsgBuf.Msg_0x00000101.Modified) {
		switch (CtlUnitCAN_MsgBuf.Msg_0x00000101.tx_queue_ctdwn) {
		case 0xFF:
			CtlUnitCAN_MsgBuf.Msg_0x00000101.tx_queue_ctdwn = 10;
			break;
		case 0x00:
			break;
		default:
			--CtlUnitCAN_MsgBuf.Msg_0x00000101.tx_queue_ctdwn;
		}
	}

	if (CtlUnitCAN_MsgBuf.Msg_0x00000104.Modified) {
		switch (CtlUnitCAN_MsgBuf.Msg_0x00000104.tx_queue_ctdwn) {
		case 0xFF:
			CtlUnitCAN_MsgBuf.Msg_0x00000104.tx_queue_ctdwn = 10;
			break;
		case 0x00:
			break;
		default:
			--CtlUnitCAN_MsgBuf.Msg_0x00000104.tx_queue_ctdwn;
		}
	}

	// Zyklische Botschaften
	// "Modified" wird nicht ausgewertet
	switch (CtlUnitCAN_MsgBuf.Msg_0x00000102.tx_queue_ctdwn) {
	case 0xFF:
		CtlUnitCAN_MsgBuf.Msg_0x00000102.tx_queue_ctdwn = 20;
		break;
	case 0x00:
		break;
	default:
		--CtlUnitCAN_MsgBuf.Msg_0x00000102.tx_queue_ctdwn;
	}

	switch (CtlUnitCAN_MsgBuf.Msg_0x00000103.tx_queue_ctdwn) {
	case 0xFF:
		CtlUnitCAN_MsgBuf.Msg_0x00000103.tx_queue_ctdwn = 40;
		break;
	case 0x00:
		break;
	default:
		--CtlUnitCAN_MsgBuf.Msg_0x00000103.tx_queue_ctdwn;
	}

	// Warteschlange auswerten
	// -----------------------
	// Die Botschaft mit der höchsten Priorität suchen
	CtlUnitCAN_TxNominee = &CtlUnitCAN_MsgBuf.Msg_0x00000101;                   // Die erste Botschaft als Bezug einsetzen

	if (CtlUnitCAN_MsgBuf.Msg_0x00000102.tx_queue_ctdwn < CtlUnitCAN_TxNominee->tx_queue_ctdwn)
		CtlUnitCAN_TxNominee = &CtlUnitCAN_MsgBuf.Msg_0x00000102;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000103.tx_queue_ctdwn < CtlUnitCAN_TxNominee->tx_queue_ctdwn)
		CtlUnitCAN_TxNominee = &CtlUnitCAN_MsgBuf.Msg_0x00000103;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000104.tx_queue_ctdwn < CtlUnitCAN_TxNominee->tx_queue_ctdwn)
		CtlUnitCAN_TxNominee = &CtlUnitCAN_MsgBuf.Msg_0x00000104;

	// Ein Sendeauftrag vorhanden?
	if (CtlUnitCAN_TxNominee->tx_queue_ctdwn == 0xFF) CtlUnitCAN_TxNominee = 0;
}

//---------------------------------------------------------------------------
// Steuergeräte CAN-Botschaft ausgeben (senden)
void WCAnalysis::ActCtlUnitCAN_O() {

	clock_t Countdown;

	// Gesendet wird nie schneller als im 10ms Takt
	// In der Praxis ist der Thread ohnenhin langsamer, daher nur eine
	// präventive Sicherheitsmaßnahme.
	if(!IsTimeElapsed(&CtlUnitCAN_TxTrigger, &Countdown, 10, 1, FirstRun)) return;

//ACHTUNG: Fehler 29 würde komplette Kommunikation blockieren, deshalb folgender
// Absatz Ersetzt. Muss bei allen vorherigen Versionen ebenfalls gemacht werden!
/*	// Wenn CAN nicht bereit, dann ohnehin kein Senden möglich
	if ((CtlUnitCAN_StatusLvl != WCACNST_CtlUnitCAN_StatusLvl_Ready) ||
		 GenOrg->Evnt[29]) return;
*/

	// TEST5: Prüfung entfernt - Sendung unabhängig vom Ready-Status
	// if ((CtlUnitCAN_StatusLvl != WCACNST_CtlUnitCAN_StatusLvl_Ready)) return;

	CtlUnitCAN_TxQueueAdmin();

	// Sendeauftrag vorhanden ?
	if (CtlUnitCAN_TxNominee == 0) return;

	// Senden
	if (CtlUnitCAN.Write(&CtlUnitCAN_TxNominee->CANMsg) == PCAN_ERROR_OK) {
		// Nach erfolgreichem Senden als abgearbeitet kennzeichnen
		CtlUnitCAN_TxNominee->Modified = false;
		CtlUnitCAN_TxNominee->tx_queue_ctdwn = 0xFF;

		// Je nach erfolgreich gesendeter Botschaft wird ggf. eine Antwort erwartet
		if (CtlUnitCAN_TxNominee->CANMsg.ID == 0x101) CtlUnitCAN_MsgBuf.Msg_0x00000201.Expected = true;
		if (CtlUnitCAN_TxNominee->CANMsg.ID == 0x104) CtlUnitCAN_MsgBuf.Msg_0x00000204.Expected = true;
	}
}

// ==========================================================================
// ANALYSE UND REAKTION
// ==========================================================================

//---------------------------------------------------------------------------
// Nebenantrieb Überwachung
void WCAnalysis::GetHydraulicStatus()
{
		bool tmpErr; // wird derzeit nicht ausgewertet
		if (IOControl.DIPortStatus(DIOHydraulicOKNUM, false, &tmpErr) ||
			((CtlUnitCAN_MsgBuf.Msg_0x00000303.CANMsg.DATA[2] & 0x01) != 0))
		{
				V->SetInfo_HydraulicOKOn();
		}else
		{
				V->SetInfo_HydraulicOKOff();
		};
};

//---------------------------------------------------------------------------
// Speisedruck der Hydraulik erfassen
void WCAnalysis::GetHydrChargePressure() {
	V->SetInfo_HydrChargePressure(CtlUnitCAN_MsgBuf.Msg_0x00000303.CANMsg.DATA[5] & 0x7F);
}

//---------------------------------------------------------------------------
// Aktuellen Hydraulikfüllstand erfassen
void WCAnalysis::GetHydrLvl() {
	V->SetInfo_HydrLvl(CtlUnitCAN_MsgBuf.Msg_0x00000303.CANMsg.DATA[4] & 0x7F);
}

//---------------------------------------------------------------------------
// Arbeitsdruck der Hydraulik erfassen
void WCAnalysis::GetHydrWrkPressure() {

	clock_t Countdown; // nur Dummy

	// In Zeitabständen den Hydraulikdruck abfragen
	if (!IsTimeElapsed(&GetHydrWrkPressure_Time_old, &Countdown, HYDRWRKPRESSURE_AQUISITION_CLOCK, 0, FirstRun)) return;  // Bei Überlauf true; bei FirstRun Initialisierung und true

	if (FirstRun) {
		HydrWrkPressure_AverageManager.SetSize(HYDRWRKPRESSURE_AVERAGEBUFFER_SIZE);
		HydrWrkPressure_AverageManager.Clear(CtlUnitCAN_MsgBuf.Msg_0x00000303.CANMsg.DATA[0] * 3);
		HydrWrkPressure_AverageManager.SetKeepBufferContent(true);
	}else
		HydrWrkPressure_AverageManager << (CtlUnitCAN_MsgBuf.Msg_0x00000303.CANMsg.DATA[0] * 3);

	V->SetInfo_HydrWrkPressure(HydrWrkPressure_AverageManager.GetAverage());
}

//---------------------------------------------------------------------------
// Erfassen der Fehlermeldungen vom Steuergerät
void WCAnalysis::GetCtlUnitErrorMsg() {

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[0] & 0x04) GenOrg->Evnt << 30;
	else GenOrg->Evnt >> 30;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[0] & 0x08) GenOrg->Evnt << 31;
	else GenOrg->Evnt >> 31;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[0] & 0x10) GenOrg->Evnt << 32;
	else GenOrg->Evnt >> 32;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[0] & 0x20) GenOrg->Evnt << 33;
	else GenOrg->Evnt >> 33;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[0] & 0x40) GenOrg->Evnt << 34;
	else GenOrg->Evnt >> 34;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[0] & 0x80) GenOrg->Evnt << 35;
	else GenOrg->Evnt >> 35;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[1] & 0x01) GenOrg->Evnt << 36;
	else GenOrg->Evnt >> 36;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[1] & 0x02) GenOrg->Evnt << 37;
	else GenOrg->Evnt >> 37;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[1] & 0x08) GenOrg->Evnt << 38;
	else GenOrg->Evnt >> 38;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[1] & 0x10) GenOrg->Evnt << 39;
	else GenOrg->Evnt >> 39;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[1] & 0x20) GenOrg->Evnt << 40;
	else GenOrg->Evnt >> 40;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[1] & 0x40) GenOrg->Evnt << 41;
	else GenOrg->Evnt >> 41;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[2] & 0x01) GenOrg->Evnt << 42;
	else GenOrg->Evnt >> 42;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[2] & 0x02) GenOrg->Evnt << 43;
	else GenOrg->Evnt >> 43;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[1] & 0x04) GenOrg->Evnt << 44;
	else GenOrg->Evnt >> 44;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[2] & 0x04) GenOrg->Evnt << 45;
	else GenOrg->Evnt >> 45;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[2] & 0x08) GenOrg->Evnt << 46;
	else GenOrg->Evnt >> 46;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[2] & 0x10) GenOrg->Evnt << 47;
	else GenOrg->Evnt >> 47;

}

//---------------------------------------------------------------------------
// Ermittelt die SW-Version des Steuergeräts
void WCAnalysis::GetCtlUnitSWVersion() {

	char HBuf[CTLUNIT_SW_VERSION_NAMELENGTH];
	char Hstring8BitValue[4];
	int Hidx;

	if (!CtlUnitCAN_MsgBuf.Msg_0x00000201.Modified) return;

    Hidx = 0;
	HBuf[Hidx++] = CtlUnitCAN_MsgBuf.Msg_0x00000201.CANMsg.DATA[0];// Buchstabenbezeichnung
	HBuf[Hidx++] = CtlUnitCAN_MsgBuf.Msg_0x00000201.CANMsg.DATA[1];
	HBuf[Hidx++] = CtlUnitCAN_MsgBuf.Msg_0x00000201.CANMsg.DATA[2];
	HBuf[Hidx++] = 0x20;                                 	// Leerzeichen
	HBuf[Hidx++] = 0x56; 									// 'V'

	// Versionsnummer anhängen
	itoa(CtlUnitCAN_MsgBuf.Msg_0x00000201.CANMsg.DATA[3], Hstring8BitValue, 10);
	strcpy(&HBuf[Hidx], Hstring8BitValue);
	Hidx += strlen(Hstring8BitValue);                       // max. + 3 (weil 8bit Zahl)
	HBuf[Hidx++] = 0x2E;									// '.'
	itoa(CtlUnitCAN_MsgBuf.Msg_0x00000201.CANMsg.DATA[4], Hstring8BitValue, 10);
	strcpy(&HBuf[Hidx], Hstring8BitValue);
	Hidx += strlen(Hstring8BitValue);                       // max. + 3 (weil 8bit Zahl)
	HBuf[Hidx++] = 0;										// EK

	V->SetInfo_CtlUnitSWVersion(HBuf);

}

//---------------------------------------------------------------------------
// Ermitteln der gemessenen momentanen Windendrehzahlsteuerspannung (zum Steuergerät)
void WCAnalysis::GetCurrWinchRevMessVoltage()
{
        bool tmpErr;    // z.Z. nicht benutzt
		clock_t Countdown; // nur Dummy
		int HV;

        // In Zeitabständen die Temperaturen abfragen
		if (!IsTimeElapsed(&GetCurrWinchRevMessVoltage_Time_old, &Countdown, WINCHREVMESSVOLTAGE_AQUISITION_CLOCK, 0, FirstRun)) return;  // Bei Überlauf true; bei FirstRun Initialisierung und true

        HV = (IOControl.ADUIn(GenOrg->AI_WinchRevMessChannel, false, &tmpErr) * 1000) + 0.5; // +0.5 wegen exaktem Runden

        if (FirstRun)
        {
                CurrWinchRevMessVoltage_AverageManager.SetSize(WINCHREVMESSVOLTAGE_AVERAGEBUFFER_SIZE);
                CurrWinchRevMessVoltage_AverageManager.Clear(HV);
                CurrWinchRevMessVoltage_AverageManager.SetKeepBufferContent(true);
		}else
			CurrWinchRevMessVoltage_AverageManager << HV;

        V->SetInfo_CurrWinchRevMessVoltage(CurrWinchRevMessVoltage_AverageManager.GetAverage());
};

//---------------------------------------------------------------------------
// Teufe ermitteln und entsprechende Daten aktualisieren
void WCAnalysis::GetDepth()
{
        bool tmpMagneticMark, tmpMagneticMarkDirect;
        bool tmpError;
        int  tmpDepth, tmpTimeDiff;
        long newTime = GetProcessTime_in_ms();

		// "IsTimeElapsed" kann hier nicht eingesetzt werden, da u.U. noch die
        // Zeitdifferenz zwischen momentanem und letztem Zeitpunkt für spätere
        // Geschwindigkeits-Kalkulation benötigt wird

        // Überlauf korrigieren, bzw. Grundinitialisierung
		if (newTime < GetDepth_Time_old) GetDepth_Time_old = newTime;

        // Teufenabfrage mit Mindestzeitintervall
        if ((newTime - GetDepth_Time_old) >= GET_DEPTH_STEP)
        {
				tmpDepth = IOControl.GetDepth(&tmpMagneticMark, &tmpMagneticMarkDirect, &tmpTimeDiff, &tmpError);

				// Je nach Einstellung Teufe umkehren
				if (V->GetParam_ReverseDepth()) tmpDepth *= -1;

                // Weiterverfahren in Abhängigkeit,
                // ob ein Fehler bei der Teufenerfassung aufgetreten ist, oder nicht
				if (tmpError)                                                   // wenn Fehler bei der Komunikation mit dem TR erkannt, dann
                {
                        tmpDepth = V->GetInfo_CurrDepth();                      // aktuelle Teufe bleibt unangetastet
                        tmpMagneticMark = _OFF;                                 // Magnetmarkentrigger aus
                        tmpMagneticMarkDirect = _OFF;                           // Magnetmarke ist aus
                        tmpTimeDiff = newTime - GetDepth_Time_old;              // Verwendet die von dieser Methode ermittelte Zeitdifferenz
                };

                // Teufe setzen
                V->SetInfo_CurrDepth(tmpDepth);

                // Zeit für Geschwindigkeitsermittlung berechnen
#ifndef __USE_GET_DEPTH_EXTENDED
                tmpTimeDiff = newTime - GetDepth_Time_old;                      // Da in diesem Falle nicht von "IOControl.GetDepth" ermittelt, wird PC interne Zeit verwendet
#endif
                // Folgende Zeile erst hier, da vorher noch die Zeitdifferenz benötigt!!!
                GetDepth_Time_old = newTime;                                    // Aktualisieren für nächsten Durchlauf

                // Ermitteln des Indexes des Puffers für die Geschwindigkeitsermittlung
                if (FirstRun)
                {
                        tmpMagneticMark_cntdwn = 0;

                        // Beim ersten Durchlauf muss der Puffer komplett gefüllt
                        // werden, so dass die Geschwindigkeit nicht springt
                        for (int i = 0; i < VELOCITY_CALC_BUF_SIZE; ++i)
                        {
                                VelocityCalcBuffer[i].TimeDiff = tmpTimeDiff;
                                VelocityCalcBuffer[i].Depth = tmpDepth;
                        };
                };

                if (tmpMagneticMark) tmpMagneticMark_cntdwn = 2;

                // im laufenden Betrieb
                if (tmpMagneticMark_cntdwn > 0)
                {
                        --tmpMagneticMark_cntdwn;

                        // Bei auftretender Magnetmarke muss Puffer angepasst werden,
                        // damit keine Geschwindigkeitssprünge entstehen können.
                        // Durch diese Funktion entstehen zwar für 3,6s leichte Verfälschungen
                        // der Geschwindigkeit, diese sind jedoch wesentlich weniger
                        // auffällig und weniger störend als ein Geschwindigkeitssprung von mehreren 100m/h

                        long HVlong = V->GetInfo_CurrVelocity() * VELOCITY_CALC_TIMEDIFF_FOR_1MPH / 3600; // Berechnung: (m/h in mm/ms) * (Messdauer für genaue Geschwindigkeitsberechnung) = Wert für Eintrag in VelocityCalcBuffer

                        // Ermittlung der Teufe, die bei der aktuellen Geschwindigkeit,
                        // ausgehend von der korrigierten Teufe, vor der Zeitdauer
                        // VELOCITY_CALC_TIMEDIFF_FOR_1MPH hätte vorhanden sein müssen
                        if (V->GetInfo_CurrDirection() == _UPward) HVlong = tmpDepth + HVlong;
                        else HVlong = tmpDepth - HVlong;

                        // Puffer wird vollständig neu gefüllt mit aktueller Teufe und
                        // aus aktueller Geschwindigkeit ermittelten Werten.
                        // Geschwindigkeitssprünge werden so bei Teufenkorrektur
                        // durch die Magnetmarke unterbunden.
                        // CurrVelocityCalcBufIndex braucht deshalb nicht neu gesetzt werden
                        for (int i = 0; i < VELOCITY_CALC_BUF_SIZE; ++i)
                        {
                                VelocityCalcBuffer[i].TimeDiff = 0;
                                VelocityCalcBuffer[i].Depth = (int)(HVlong);
                        };
                        VelocityCalcBuffer[CurrVelocityCalcBufIndex].TimeDiff = VELOCITY_CALC_TIMEDIFF_FOR_1MPH;
                        VelocityCalcBuffer[CurrVelocityCalcBufIndex].Depth = tmpDepth;
                }else
                {       // keine Magnetmarke
                        ++CurrVelocityCalcBufIndex;
                        if (CurrVelocityCalcBufIndex >= VELOCITY_CALC_BUF_SIZE) CurrVelocityCalcBufIndex = 0;   // Reaktion auf Indexüberlauf (Ring-Puffer wird gewährleistet)
                        // Puffer für die Geschwindigkeitsermittlung aktualisieren
                        VelocityCalcBuffer[CurrVelocityCalcBufIndex].TimeDiff = tmpTimeDiff;
                        VelocityCalcBuffer[CurrVelocityCalcBufIndex].Depth = tmpDepth;
                };

                // Fehlersignalisierung
                // Wenn Kommunikation mit Teufenrechner fehlgeschlagen
				if (tmpError) GenOrg->Evnt << 2;								// Fehler 2 ein
				else GenOrg->Evnt >> 2;											// Fehler 2 aus

                // MagnetmarkenTrigger
                if (tmpMagneticMark)
                {
                        V->SetInfo_MagneticMarkOn();
                        V->SetInfo_MagneticMarkStaticOn();
                        V->SetInfo_MagneticMarkStaticLOGOn();
                } else V->SetInfo_MagneticMarkOff();

                // Magnetmarkenzustand direkt
                if (tmpMagneticMarkDirect)
                {
                        V->SetInfo_MagneticMarkDirectOn();
                        V->SetInfo_MagneticMarkStaticOn();
                        V->SetInfo_MagneticMarkStaticLOGOn();
                } else V->SetInfo_MagneticMarkDirectOff();
        };
};

//---------------------------------------------------------------------------
// Ermittelt die SW-Version des Teufenrechners
void WCAnalysis::GetDepthCalcSWVersion()
{

		if ((V->GetControl_GetDepthCalcSWVersion() != _ON) ||
			(V->GetDepthCalcSWVersion_Locked) ||                                // Abfrage blockiert
			GenOrg->Evnt[1] ||
			GenOrg->Evnt[2])															// irgendein TR-Fehler
                return;

        char HChar[DEPTHCALC_SW_VERSION_NAMELENGTH] = "";
        if (IOControl.GetDepthCalcSWVersion(HChar, DEPTHCALC_SW_VERSION_NAMELENGTH))
        {
                if (HChar[0] != 0) V->SetInfo_DepthCalcSWVersion(HChar);
                else V->SetInfo_DepthCalcSWVersion((char*)(STD_DepthCalcSWVersion));
                V->SetControl_GetDepthCalcSWVersionOff(); // Bei Erfolg kann Einstellung zurückgesetzt werden
        }else
        {
				GenOrg->Evnt << 2;												// Fehler 2 ein
                V->SetInfo_DepthCalcSWVersion((char*)(STD_DepthCalcSWVersion));
        };
};

//---------------------------------------------------------------------------
// Teufenparameter auslesen (vom Steuergerät)
void WCAnalysis::GetDepthParam()
{
	int HV;

	if (CtlUnitCAN_MsgBuf.Msg_0x00000204.Modified) {

		HV = static_cast<int>(CtlUnitCAN_MsgBuf.Msg_0x00000204.CANMsg.DATA[0] & 0x03) * 0x1000000; // gleichzeitig um 24 Bit nach links schieben
		HV += static_cast<int>(CtlUnitCAN_MsgBuf.Msg_0x00000204.CANMsg.DATA[1]) * 0x10000; // gleichzeitig um 16 Bit nach links schieben
		HV += static_cast<int>(CtlUnitCAN_MsgBuf.Msg_0x00000204.CANMsg.DATA[2]) * 0x100; // gleichzeitig um 8 Bit nach links schieben
		HV += static_cast<int>(CtlUnitCAN_MsgBuf.Msg_0x00000204.CANMsg.DATA[3]);

		if (HV != V->GetParam_CalFactDepth()) V->SetControl_SetDepthCalFactor_On(); // Bei Abweichung Kalibrierfaktor erneut senden
	}
}

//---------------------------------------------------------------------------
// Einlesen der Stati der Digitalen Eingänge
void WCAnalysis::GetDI()
{
        bool Error;
        IOControl.DIPortStatus(0, true, &Error);
};

//---------------------------------------------------------------------------
// Fahrtrichtung ermitteln
void WCAnalysis::GetDirection()
{
		int CurrDepth = V->GetInfo_CurrDepth();

        // Bei Auftreten einer Magnetmarke (Moment, wenn Teufe korrigiert wird)
        // darf keine Fahrtrichtungserkennung stattfinden, da sonst
        // Verfälschungen der Anzeige durch Teufensprünge in die
        // entgegengesetzte Fahrtrichtung auftreten, die andere
        // Programmfunktionen stören können
        if (V->GetInfo_MagneticMark() == _OFF)
        {
			if ((CurrDepth - GetDirection_Depth_old) > 0) V->SetInfo_CurrDirectionDown();
			if ((CurrDepth - GetDirection_Depth_old) < 0) V->SetInfo_CurrDirectionUp();
		};
        GetDirection_Depth_old = CurrDepth;
};

//---------------------------------------------------------------------------
// Ermittelt die momentane Sollzugkraft für das aktive Kabel entladen
void WCAnalysis::GetUnloadCableTractionReverence()
{
        if (V->GetParam_UnloadCableStretch() == _ON)
        {
                float BeginDepth, EndDepth, DepthDiff, CurrDepth;
                float TractResult;

                // Parameter erfassen
                if (V->GetParam_UnloadCableBeginLenght() > V->GetParam_UnloadCableEndLenght())
                {
                        BeginDepth = V->GetParam_UnloadCableEndLenght();
                        EndDepth = V->GetParam_UnloadCableBeginLenght();
                }else
                {
                        BeginDepth = V->GetParam_UnloadCableBeginLenght();
                        EndDepth = V->GetParam_UnloadCableEndLenght();
                };
                DepthDiff = EndDepth - BeginDepth;
                CurrDepth = V->GetInfo_CurrDepth();

                if (CurrDepth >= EndDepth) TractResult = V->GetParam_UnloadCableEndTraction();
                else if (CurrDepth <= BeginDepth) TractResult = V->GetParam_UnloadCableStartTraction();
                else
                {
                        TractResult = ((V->GetParam_UnloadCableEndTraction() - V->GetParam_UnloadCableStartTraction()) / DepthDiff * (CurrDepth - BeginDepth)) + V->GetParam_UnloadCableStartTraction();
                };

                V->SetInfo_CurrTractionReference(TractResult);
        }else
        {
				V->SetInfo_CurrTractionReference(V->GetParam_UnloadCableTraction());
		};
};

//---------------------------------------------------------------------------
// Not-Schalter-Überwachung
void WCAnalysis::GetEmergencySwitch()
{
        bool tmpErr; // wird derzeit nicht ausgewertet
		if (!IOControl.DIPortStatus(DIOEmergencySwitchNUM, false, &tmpErr) &&
			!(CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.DATA[0] & 0x02))
		{
				V->SetInfo_EmergencySwitchOn();
		}else
		{
				V->SetInfo_EmergencySwitchOff();
		};
};

//---------------------------------------------------------------------------
// Motor-Zustandsinformationen
void WCAnalysis::GetEngineInfo()
{
		bool tmpErr; // wird derzeit nicht ausgewertet
        int HV;

		// Antriebsfehler
		if (IOControl.DIPortStatus(DIOAutomotiveErrorNUM, false, &tmpErr) ||
			((CtlUnitCAN_MsgBuf.Msg_0x00000304.CANMsg.DATA[6] & 0x80) != 0) ||
			((CtlUnitCAN_MsgBuf.Msg_0x00000304.CANMsg.DATA[6] & 0x01) != 0) ||
			((CtlUnitCAN_MsgBuf.Msg_0x00000304.CANMsg.DATA[6] & 0x02) != 0) ||
			((CtlUnitCAN_MsgBuf.Msg_0x00000304.CANMsg.DATA[6] & 0x04) != 0))
		{
				GenOrg->Evnt << 18;												// Fehler 18 ein
				V->SetInfo_AutomotiveErrorOn();
		}else
		{
				GenOrg->Evnt >> 18;												// Fehler 18 aus
				V->SetInfo_AutomotiveErrorOff();
		};

		// Atrieb Ein/Aus
		if (IOControl.DIPortStatus(DIOEngineOnNUM, false, &tmpErr) ||
			(CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.DATA[0] & 0x08))
		{
				V->SetInfo_EngineOn();
		}else
		{
				V->SetInfo_EngineOff();
		};

		// Antriebskühlmitteltemperatur
		V->SetInfo_EngineCoolantTemp(CtlUnitCAN_MsgBuf.Msg_0x00000304.CANMsg.DATA[2] - 40);

		// Antriebsdrehzahl
		HV = CtlUnitCAN_MsgBuf.Msg_0x00000304.CANMsg.DATA[1] & 0x3F;
		HV <<= 8;
		V->SetInfo_EngineSpeed(HV | CtlUnitCAN_MsgBuf.Msg_0x00000304.CANMsg.DATA[0]);
};

//---------------------------------------------------------------------------
void WCAnalysis::GetIncEncError()
{
        if (V->GetParam_IncEncErrorMonit() == _OFF)
        {
				GenOrg->Evnt >> 8;												// Fehler 8 aus
				V->SetInfo_IncEncErrorOff();
				return;
		};

		if (CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[1] & 0x80)
		{
				GenOrg->Evnt << 8;												// Fehler 8 ein
				V->SetInfo_IncEncErrorOn();
		}else
		{
				GenOrg->Evnt >> 8;												// Fehler 8 aus
				V->SetInfo_IncEncErrorOff();
		};
};

//---------------------------------------------------------------------------
// Steuergerät - Fehler-Überwachung
void WCAnalysis::GetControllerError()
{
		if ((V->GetParam_ControllerErrorMonit() == _OFF) ||
			(!V->GetControl_FastRun() && !V->GetParam_ControllerErrorMonitAtSM())) {
				GenOrg->Evnt >> 17;												// Fehler 17 aus
				V->SetInfo_ControllerErrorOff();
				return;
		};

		bool tmpErr; // wird derzeit nicht ausgewertet

		if ((IOControl.DIPortStatus(DIOControllerErrorNUM, false, &tmpErr) != V->GetParam_ControllerErrorLactive()) ||
			(CtlUnitCAN_MsgBuf.Msg_0x00000302.CANMsg.DATA[0] & 0x02)) {
				GenOrg->Evnt << 17;												// Fehler 17 ein
				V->SetInfo_ControllerErrorOn();
		}else {
				GenOrg->Evnt >> 17;												// Fehler 17 aus
				V->SetInfo_ControllerErrorOff();
		};

};

//---------------------------------------------------------------------------
// Internen Takt ermitteln
void WCAnalysis::GetInternalClock()
{
		// "IsTimeElapsed" kann hier NICHT verwendet werden!
        // siehe nachflgd. Implementierung

        clock_t CurrClock = GetProcessTime_in_ms();

        if (LastClock > CurrClock) LastClock = CurrClock;
        if ((CurrClock - LastClock) < CLOCK_Base) return;

        LastClock += CLOCK_Base;                                                // ClockBase weiter

        // Takte generieren
        ++Clock100ms_cnt;

        if (Clock100ms_cnt >= 1)
        {
                Clock100ms_cnt = 0;
                GenOrg->Clock_100ms = !GenOrg->Clock_100ms;
        };
};

//---------------------------------------------------------------------------
// Fahrhebelbetätigtungs-Überwachung
void WCAnalysis::GetLeverState()
{
        bool tmpErr; // wird derzeit nicht ausgewertet
		if (IOControl.DIPortStatus(DIOLeverStateNUM, false, &tmpErr) ||
		   (CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.DATA[0] & 0x10))
        {
                V->SetInfo_LeverOn();
        }else
        {
                V->SetInfo_LeverOff();
        };
};

//---------------------------------------------------------------------------
// Freilauf-Überwachung
void WCAnalysis::GetLostMotion()
{
		bool tmpErr; // wird derzeit nicht ausgewertet

		if (((V->GetControl_FastRun() == _OFF) && 					  	// langsame Fahrstufe
//			 (V->GetControl_WinchBrakeOff() == _OFF) &&					// Windenbremse manuell gelöst
			 (IOControl.DIPortStatus(DIOLostMotionNUM, false, &tmpErr))) ||
			 (CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.DATA[0] & 0x20)) {
			V->SetInfo_LostMotionOn();
		}else {
            V->SetInfo_LostMotionOff();
        };
};

//---------------------------------------------------------------------------
// Ölfilter wechseln Signal überwachen
void WCAnalysis::GetOilStrainer()
{
        bool tmpErr; // wird derzeit nicht ausgewertet
        clock_t Countdown; // nur Dummy

		if (IOControl.DIPortStatus(DIOOilStrainerNUM, false, &tmpErr) ||
			(CtlUnitCAN_MsgBuf.Msg_0x00000303.CANMsg.DATA[2] & 0x02))
        {
                IsTimeElapsed(&OilStrainerOnTriggerTime, &Countdown, OILSTRAINER_TRIGGER_TIME, 0, true); // "EIN"-Signal triggern

                V->SetInfo_OilStrainerOn();
        }else
        {
                if (!IsTimeElapsed(&OilStrainerOnTriggerTime, &Countdown, OILSTRAINER_TRIGGER_TIME, 0, FirstRun)) return; // Bei Überlauf true; bei FirstRun Initialisierung true

                V->SetInfo_OilStrainerOff();
        };
};

//---------------------------------------------------------------------------
// Power-Fail-Signalauswertung
void WCAnalysis::GetPowerFail()
{
    bool tmpErr; // wird derzeit nicht ausgewertet
    clock_t Countdown; // nur Dummy

	if (IOControl.DIPortStatus(DIOPowerFailNUM, false, &tmpErr) ||
		(CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.DATA[0] & 0x80)) {

        // Power-OK Signal liegt an -> Normaler Btrieb
        PowerFailFunctionActive = true;
        V->SetControl_PowerFailOff();
        IsTimeElapsed(&PowerFailOnTriggerTime, &Countdown, POWERFAIL_TRIGGER_TIME, 0, true);

    } else {

        if (!PowerFailFunctionActive) {

            V->SetControl_PowerFailOff();
            return;

        }

        // Power-OK Signal liegt NICHT an -> Stromausfall
        if (!IsTimeElapsed(&PowerFailOnTriggerTime, &Countdown, POWERFAIL_TRIGGER_TIME, 0, FirstRun)) return; // Bei Überlauf true; bei FirstRun Initialisierung true
        V->SetControl_PowerFailOn();
    }

}

//---------------------------------------------------------------------------
// Einlesen der Sondersignale
void WCAnalysis::GetSpecialSignals()
{
        bool tmpErr; // wird derzeit nicht ausgewertet

		if (IOControl.DIPortStatus(DIOSpecialSignal01NUM, false, &tmpErr) ||
			(CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.DATA[1] & 0x01))
		{
				V->SetInfo_SpecialSignal01On();
        }else
        {
                V->SetInfo_SpecialSignal01Off();
        };

		if (IOControl.DIPortStatus(DIOSpecialSignal02NUM, false, &tmpErr) ||
			(CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.DATA[1] & 0x02))
        {
                V->SetInfo_SpecialSignal02On();
        }else
        {
                V->SetInfo_SpecialSignal02Off();
        };
};

//---------------------------------------------------------------------------
// Ermitteln der Temperaturwerte der verschiedenen Sensoren
// inkl. Minimum-/Maximum-Überwachung (Sinnfälligkeitstest)
void WCAnalysis::GetTemperature()
{
        bool tmpErr; // wird derzeit nicht ausgewertet
        clock_t Countdown; // nur Dummy
        int HV;
        float Voltage, Temperature;

        // In Zeitabständen die Temperaturen abfragen
		if (!IsTimeElapsed(&GetTemperature_Time_old, &Countdown, TEMPERATURE_AQUISITION_CLOCK, 0, FirstRun)) return;  // Bei Überlauf true; bei FirstRun Initialisierung und true

	   // Temperatur Teufenrechner-Sensor
	   if (GenOrg->Evnt[5] || GenOrg->Evnt[7]) { // Wenn DAC-Karte nicht verfügbar
			V->SetInfo_TemperatureSensorDepthCalc(CtlUnitCAN_MsgBuf.Msg_0x00000301.CANMsg.DATA[2] - 40);
		} else {
			// Von der PCI-1716 lesen
			Voltage = IOControl.ADUIn(GenOrg->AI_TemperatureDepthCalcChannel, false, &tmpErr);
			Temperature = (Voltage * V->GetParam_CalibTSensorDepthCalc_CaractSlope()) + V->GetParam_CalibTSensorDepthCalc_CaractZero();
			if (Temperature > TEMPERATURE_ACCEPTED_MAXIMUM) V->SetInfo_TemperatureSensorDepthCalc(TEMPERATURE_ACCEPTED_MAXIMUM);
			else if (Temperature < TEMPERATURE_ACCEPTED_MINIMUM) V->SetInfo_TemperatureSensorDepthCalc(TEMPERATURE_ACCEPTED_MINIMUM);
			else V->SetInfo_TemperatureSensorDepthCalc(Temperature);
		}

		// Temperatur Sensor A
		Voltage = IOControl.ADUIn(GenOrg->AI_TemperatureAChannel, false, &tmpErr);
		Temperature = (Voltage * V->GetParam_CalibTSensorA_CaractSlope()) + V->GetParam_CalibTSensorA_CaractZero();
		if (Temperature > TEMPERATURE_ACCEPTED_MAXIMUM) V->SetInfo_TemperatureSensorA(TEMPERATURE_ACCEPTED_MAXIMUM);
		else if (Temperature < TEMPERATURE_ACCEPTED_MINIMUM) V->SetInfo_TemperatureSensorA(TEMPERATURE_ACCEPTED_MINIMUM);
		else V->SetInfo_TemperatureSensorA(Temperature);

		// Temperatur Sensor B
		Voltage = IOControl.ADUIn(GenOrg->AI_TemperatureBChannel, false, &tmpErr);
		Temperature = (Voltage * V->GetParam_CalibTSensorB_CaractSlope()) + V->GetParam_CalibTSensorB_CaractZero();
		if (Temperature > TEMPERATURE_ACCEPTED_MAXIMUM) V->SetInfo_TemperatureSensorB(TEMPERATURE_ACCEPTED_MAXIMUM);
		else if (Temperature < TEMPERATURE_ACCEPTED_MINIMUM) V->SetInfo_TemperatureSensorB(TEMPERATURE_ACCEPTED_MINIMUM);
		else V->SetInfo_TemperatureSensorB(Temperature);

		// Öltanktemperatur in °C
        V->SetInfo_TemperatureOilTank(CtlUnitCAN_MsgBuf.Msg_0x00000303.CANMsg.DATA[3] - 50);

};

//---------------------------------------------------------------------------
// Aktualisieren der Schwellwertzustände
void WCAnalysis::GetThresholdExceedance()
{
        long HVlong;
		clock_t Countdown; // nur Dummy

		// TEUFE
        // bei Kabel entladen werden Schwellwertüberschreitungen ignoriert
        // Freigabe (weiter unten) ist immer möglich
        if ((V->GetControl_UnloadCable() == _OFF) ||
//			((V->GetInfo_LostMotion() == _OFF) || (V->GetControl_WinchBrakeOff() == _ON))
			(V->GetInfo_LostMotion() == _OFF)
		   )
        {
                // ---------------------------
                // >>> Überschreitung der Teufenschwelle
                if ((V->GetParam_MinDepth() > V->GetInfo_CurrDepth()) ||
                    (V->GetParam_MaxDepth() < V->GetInfo_CurrDepth()))
                {
                        // Verzögerung der Steuergerät-Abschaltung bei Schwellwertüberschreitung initialisieren
                        // ---
                        if ((V->GetInfo_DepthThresholdExceeded() == _OFF) &&    // nur Moment des Überschreitens der Schwelle maßgebend
                            (V->GetControl_ThresholdExceedenceStopDelay() == _OFF))// Anhaltevorgang nicht bereits wegen anderem Überschreiten der Schwelle initialisiert
                        {
                                V->SetControl_ThresholdExceedenceStopDelayOn(); // Verzögerung aktiv
                                IsTimeElapsed(&ThresholdExceedenceStopDelay_StartTime, &Countdown, THRESHOLD_EXCEEDENCE_STOP_DELAY, 0, true);
                        };
                        // ---

                        GenOrg->Evnt << 9;                      				// Fehler 9 ein
                        V->SetInfo_DepthThresholdExceededOn();
                };
        };

        // ---------------------------
        // <<< Teufenfreigabe wieder in Normalzustand
        if ((V->GetParam_MinDepth() < V->GetInfo_CurrDepth()) &&
            (V->GetParam_MaxDepth() > V->GetInfo_CurrDepth()) &&
			(V->GetControl_ClearAfterThresholdDepth() == _ON))
        {
                GenOrg->Evnt >> 9;                             					// Fehler 9 aus

                V->SetControl_ClearAfterThresholdDepthOff();

                V->SetInfo_DepthThresholdExceededOff();
        };

        // ===========================
        // ZUGKRAFT
		// bei Kabel entladen und Freilauf werden Schwellwertüberschreitungen ignoriert
		// Freigabe (weiter unten) ist immer möglich

		if ((V->GetControl_UnloadCable() == _OFF) && (V->GetInfo_LostMotion() == _OFF)) {

			// Test auf Grenzüberschreitung
			if ((V->GetInfo_CurrTraction() > V->GetParam_MaxTraction()) ||		// absolute Maximalzugkraft
				((V->GetControl_CalcCurrTractionsActive() == _ON) &&            // dynamische Grenzen aktiv
				 (((V->GetInfo_CurrDirection() == _DOWN) && (V->GetInfo_CurrTraction() < V->GetInfo_CurrTensionLimit_Lower())) || // bei Abwärtsfahrt relative Mindestzugkraft überschritten
				  ((V->GetInfo_CurrDirection() == _UPward) && (V->GetInfo_CurrTraction() > V->GetInfo_CurrTensionLimit_Upper())) // bei Aufwärtsfahrt relative Maximalzugkraft überschritten
				 )
				)
			   )
			 {
				// Verzögerung der Steuergerät-Abschaltung bei Schwellwertüberschreitung initialisieren
				// ---
				if ((V->GetInfo_TractionThresholdExceeded() == _OFF) &&    		// nur Moment des Überschreitens der Schwelle maßgebend
					(V->GetControl_ThresholdExceedenceStopDelay() == _OFF))		// Anhaltevorgang nicht bereits wegen anderem Überschreiten der Schwelle initialisiert
					{
						V->SetControl_ThresholdExceedenceStopDelayOn(); 		// Verzögerung aktiv
						IsTimeElapsed(&ThresholdExceedenceStopDelay_StartTime, &Countdown, THRESHOLD_EXCEEDENCE_STOP_DELAY, 0, true);
					};
					// ---

					GenOrg->Evnt << 10;											// Fehler 10 ein
					V->SetInfo_TractionThresholdExceededOn();

			 }
		}

		// ---------------------------
		// Nach Verzögerung zur Ausführung des Anhaltevorgangs der Winde nach Schwellwertüberschreitung
		// das Verzögerungsflag löschen
		if ((V->GetControl_ThresholdExceedenceStopDelay() == _ON) &&
			 IsTimeElapsed(&ThresholdExceedenceStopDelay_StartTime, &Countdown, THRESHOLD_EXCEEDENCE_STOP_DELAY, 0, false))
				V->SetControl_ThresholdExceedenceStopDelayOff();

		// ---------------------------
		// <<< Zugkraftüberwachung wieder in Normalzustand
		if ((V->GetParam_MaxTraction() > V->GetInfo_CurrTraction()) &&
			((V->GetControl_CalcCurrTractions() == _OFF) ||
			 ((V->GetInfo_CurrTensionLimit_Lower() < V->GetInfo_CurrTraction()) &&
			  (V->GetInfo_CurrTensionLimit_Upper() > V->GetInfo_CurrTraction()))) &&
			(V->GetControl_ClearAfterThresholdTraction() == _ON)) {

				GenOrg->Evnt >> 10;												// Fehler 10 aus
				V->SetControl_ClearAfterThresholdTractionOff();
				V->SetInfo_TractionThresholdExceededOff();
		};

};

//---------------------------------------------------------------------------
// aktuelle Zugkraft-Ermitteln
void WCAnalysis::GetTension()
{
        bool tmpErr; // wird derzeit nicht ausgewertet
        int Value;
        float VoltageValue;

		long int  N = V->GetParam_CalibTractionZero();
        int X = V->GetParam_CalibTractionX();
        int Y = V->GetParam_CalibTractionY();
		long int Scale;
		int HV;

        // ------------------
        // Aktuelle Zugkraft
        // Spannungswert ermitteln

// ToDo: PCI1716 abhängige Zugkraftwerterfassung entfernen
/*		VoltageValue = IOControl.ADUIn(GenOrg->AI_TractionChannel, false, &tmpErr); */
		VoltageValue = static_cast<float>(CtlUnitCAN_MsgBuf.Msg_0x00000305.CANMsg.DATA[0] & 0x3F) * 256;
		VoltageValue += static_cast<float>(CtlUnitCAN_MsgBuf.Msg_0x00000305.CANMsg.DATA[1]);
		VoltageValue /= 1000; // Umrechnung in V, da folgende Zeilen für Zugkraftberechnung ursprünglich für V geschrieben wurden

#ifdef __TRACTION_NOISE_DAMP
        // Imaginäre Zugkraft-Dämpfung (Filtern der Zugkraft)
        // beim ersten Programmdurchlauf
        if (FirstRun) {
            GetTraction_Dump_Time_old = GetProcessTime_in_ms();                 // Start definieren
            GetTraction_AverageManager.SetSize(GET_TRACTION_NOISE_DAMP_TIME / 100); // Puffergröße auf 100ms Takt abgestimmt
            GetTraction_AverageManager.Clear(VoltageValue);                     // Ausgangsspannung setzen
            GetTraction_AverageManager.SetKeepBufferContent(true);              // Bei Änderung der Puffergröße den Pufferinhalt weitestgehend beibehalten
        }

        // 100 ms Takt verwalten
        // IsTimeElapsed kann hier aufgrund der hier notwendigen Arbeitsweise
        // (nachholen verlorener Takte) NICHT verwendet werden
        long CurrTime = GetProcessTime_in_ms();
        if (CurrTime < GetTraction_Dump_Time_old) GetTraction_Dump_Time_old = CurrTime; // Überlauftest
        if ((CurrTime - GetTraction_Dump_Time_old) >= 100) {
            GetTraction_Dump_Time_old += 100;
            GetTraction_AverageManager << VoltageValue;
        }

        // Ist Puffer gleich 0 wird die Spannung direkt weitergegeben, ansonsten hier ausgelesen
        if (GetTraction_AverageManager.GetSize() > 0) VoltageValue = GetTraction_AverageManager.GetAverage();

#endif

		V->SetInfo_CurrTractionVoltage(VoltageValue * (float)(1000));

        // Aus Spannungswert die aktuelle Zugkraft berechnen
        Value = (float)(N) + VoltageValue * ((float)(Y) * 1000 / (float)(X));

        V->SetInfo_CurrTraction(Value);

		// Absolute Maximal-Zugkraft setzen (für internes LOG)
		if (GenOrg->internal_LOG_AbsoluteMaxTraction < V->GetInfo_CurrTraction()) GenOrg->internal_LOG_AbsoluteMaxTraction = V->GetInfo_CurrTraction();

		// --------------------------------------------------
		// Ausgabe der Zugkraft-Parameter an das Steuergerät,
		// damit es selbst die Zugkraft ermitteln kann
		Scale = Y * 1000 / X; // Zugkraftskalierung in mN/mV = N/V
		if (Scale > 65535) Scale = 65535;
		else if (Scale < 0) Scale = 0;

		// Nullpunkt [Newton] in Nullpunkt [mV] umrechnen
		N = -N * X / Y + 32768;
//		if (N < 0) N = 0;

		CtlUnitCAN_MsgBuf.Msg_0x00000103.CANMsg.DATA[0] = Scale / 256;
		CtlUnitCAN_MsgBuf.Msg_0x00000103.CANMsg.DATA[1] = Scale;
		CtlUnitCAN_MsgBuf.Msg_0x00000103.CANMsg.DATA[2] = (N / 256);
		CtlUnitCAN_MsgBuf.Msg_0x00000103.CANMsg.DATA[3] = N;

		CtlUnitCAN_MsgBuf.Msg_0x00000103.Modified = true;
};

//---------------------------------------------------------------------------
// Berechnet die dynamischen Zugkraftgrenzen
void WCAnalysis::GetDynTensionLimits() {

	// Festlegen, ob dynamische Zugkraftgrenzen tatsächlich aktiv sind
	// Das zentrale Ergebnis beeinflusst gleichzeitig die Zustands-Anzeige auf "DynamicTensionOnOffButton"
	if ((V->GetControl_CalcCurrTractions() == _ON) && (V->GetInfo_LostMotion() == _OFF))
		V->SetControl_CalcCurrTractionsActiveOn();
	else V->SetControl_CalcCurrTractionsActiveOff();


	if ((V->GetControl_CalcCurrTractionsActive() == _OFF) ||
//		(V->GetInfo_CurrVelocity() == 0) ||
		FirstRun) {

		// Bei inaktiver dynamischer Zugraftbegrenzung, Windenstillstand
		// und nicht bereits ausgelöster Schwellwertüberschreitung
		// werden die Grenzen ständig an die aktuellen Grenzen angepasst.
		// Damit stehen die Grenzen ggf. unmittelbar nach dem Einschalten
		// der Regelung zur Verfügung

		V->SetInfo_CurrTensionLimit_Lower(V->GetInfo_CurrTraction() - V->GetParam_RelTensionLimitLower());
		V->SetInfo_CurrTensionLimit_Upper(V->GetInfo_CurrTraction() + V->GetParam_RelTensionLimitUpper());

	} else if (V->GetInfo_CurrDirection() == _DOWN) {

		// Bei Abwärtsfahrt

		if ((V->GetInfo_CurrTraction() - V->GetParam_RelTensionLimitLower()) > V->GetInfo_CurrTensionLimit_Lower())
			V->SetInfo_CurrTensionLimit_Lower(V->GetInfo_CurrTraction() - V->GetParam_RelTensionLimitLower());
		V->SetInfo_CurrTensionLimit_Upper(V->GetInfo_CurrTraction() + V->GetParam_RelTensionLimitUpper());

	} else {

		// Bei Aufwärtsfahrt

		V->SetInfo_CurrTensionLimit_Lower(V->GetInfo_CurrTraction() - V->GetParam_RelTensionLimitLower());
		if ((V->GetInfo_CurrTraction() + V->GetParam_RelTensionLimitUpper()) < V->GetInfo_CurrTensionLimit_Upper())
			V->SetInfo_CurrTensionLimit_Upper(V->GetInfo_CurrTraction() + V->GetParam_RelTensionLimitUpper());
	};

}

//---------------------------------------------------------------------------
// Geschwindigkeit ermitteln
void WCAnalysis::GetVelocity()
{
        long newTime = 0;
        newTime = GetProcessTime_in_ms();

        int HV1, HV2;
        float Hfloat1;

/*
        Deaktiviert am 6.11.2008, da schon länger ohne Funktion und überflüssig
        // Beim ersten Durchlauf wird die Referenzteufe gleich der "Ist"-Teufe gesetzt
        if (FirstRun)
        {
                GetVelocity_Depth_old = V->GetInfo_CurrDepth();
// nur bei aktiver Dämpfung der internen Geschwindigkeit notwendig
//                GetVelocity_DampNoise_Time_old = newTime;                       // für Schwankungsdämpfung
                GetAnnouncedVelocity_DampNoise_Time_old = newTime;              // für Schwankungsdämpfung der Anzeige
        };
*/

        // Überlauf korrigieren, bzw. Grundinitialisierung
        if (newTime < GetVelocity_Time_old) GetVelocity_Time_old = newTime;
// nur bei aktiver Dämpfung der internen Geschwindigkeit notwendig
//        if (newTime < GetVelocity_DampNoise_Time_old) GetVelocity_DampNoise_Time_old = newTime; // für Schwankungsdämpfung
//        if (newTime < (long)(GetAnnouncedVelocity_DampNoise_Time_old)) GetAnnouncedVelocity_DampNoise_Time_old = newTime; // für Schwankungsdämpfung

        // Reale Geschwindigkeit ermitteln (so gut es geht, also in einer Breite von 3,6s)
        if ((newTime - GetVelocity_Time_old) > GET_VELOCITY_CALC_STEP)
        {
                // Berechnung Zeitdifferenz und gleichzeitig zweite Indexvariable ermitteln
                HV2 = 0;
                HV1 = CurrVelocityCalcBufIndex;
                for (int i = 0; i < (VELOCITY_CALC_BUF_SIZE - 1); ++i)          // max. alle Pufferelemente durchlaufen
                {
                        HV2 += VelocityCalcBuffer[HV1--].TimeDiff;
                        if (HV1 < 0) HV1 = VELOCITY_CALC_BUF_SIZE - 1;
                        if (HV2 >= VELOCITY_CALC_TIMEDIFF_FOR_1MPH) break;
                };

                // Berechnung Geschwindigkeit
                if (HV2 <= 0)                                                           // Division durch Null vermeiden
                {
                        Hfloat1 = 0;
                }else
                {
                        Hfloat1 = (float)(abs(VelocityCalcBuffer[CurrVelocityCalcBufIndex].Depth - VelocityCalcBuffer[HV1].Depth)) / (float)(HV2) * 3600;
                };
                V->SetInfo_CurrVelocity(Hfloat1);
        };

        // Angezeigte Geschwindigkeit ermitteln
        V->SetInfo_AnnouncedVelocity(V->GetInfo_CurrVelocity());
};

//---------------------------------------------------------------------------
// Liest alle Spannungen vom ADU-Ein
void WCAnalysis::GetVoltages()
{
        bool Error; // z.Z. nicht verwendet
        IOControl.ADUIn(0, true, &Error);
};

//---------------------------------------------------------------------------
// Parameter an den Teufenrechner übergeben
void WCAnalysis::SetDepthCalcParamm()
{
        // Parameter setzen erwünscht
		if (V->GetControl_SetDepthCalcParam() == _OFF) return;

		// Teufenparameter an Teufenrechner
		if (GenOrg->Evnt[2] == _ON) return;            							// Bei Fehler 2 werden Parameter nicht gesetzt (dadurch wird langsame COM-Port-Kommunikation nicht ausgeführt) Fehler 2 wird bei erfolgreichem Einlesen der Teufeninfo zurückgesetzt

		bool SetDeph_was_sheduled = false;                                      // merken, ob Teufe gesetzt werden soll, da durch "SetDepthCalcParam" bei erfolgreichem Schreiben zurückgesetzt
		if (V->GetControl_SetDepth_DC() == _ON) SetDeph_was_sheduled = true;

		bool Ok = IOControl.SetDepthCalcParam(V);

		// Wenn Fehler in Kommunikation
		if (!Ok)
		{
				GenOrg->Evnt << 2;												// Fehler 2 ein
				return;
		};

		// Wurde gerade die Teufe gesetzt, dann muss der Puffer für die
		// Geschwindigkeit angepasst werden, damit keine extremen
		// Geschwindigkeitswerte nach der Änderung mehr angezeigt werden
		// (riesige irreale Teufensprünge vermeiden)
		// ACHTUNG! Wird nun noch ein enormer Geschwindigkeitssprung festgestellt,
		// kann von einem Fehler bei der Übertragung der Daten zum Teufenrechner
		// ausgegangen werden.
		if (SetDeph_was_sheduled)
		{
				int HV = V->GetParam_StartDepth();
				for (int i = 0; i < VELOCITY_CALC_BUF_SIZE; ++i) VelocityCalcBuffer[i].Depth = HV; // Puffer mit der gesetzten Teufe voll schreiben
		};

		GenOrg->Evnt >> 2;														// Fehler 2 aus

};

//---------------------------------------------------------------------------
// Teufenparameter setzen (für TR siehe "SetDepthCalcParamm()")
void WCAnalysis::SetDepthParam() {

	int HVint;

	// Kalibrierfaktor / Skalierfaktor der Teufe
	HVint = V->GetParam_CalFactDepth();
	CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.DATA[0] = (HVint >> 24) & 0x03; // 4. Byte inkl. provilaktische Bereinigung
	CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.DATA[1] = HVint >> 16;
	CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.DATA[2] = HVint >> 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.DATA[3] = HVint;

	if (V->GetControl_SetDepthCalFactor()) {
		CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.DATA[0] |= 0x80;
		CtlUnitCAN_MsgBuf.Msg_0x00000104.Modified = true;
		V->SetControl_SetDepthCalFactor_Off();

		V->SetInfo_CalibDepthAllowedOff();										// Zunächst keine Neukalibrierung erlaubt
		V->SetParam_CalFactDepth_Earmarked(0);                                  // ggf. vorhandenen vorgemerkten Kal.-Faktor verwerfen
	}

	// Aktuelle Teufe
	HVint = V->GetParam_StartDepth() + 9999999; // Offset gemäß Protokoll
	CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.DATA[4] = (HVint >> 24) & 0x01; // 4. Byte inkl. provilaktische Bereinigung
	CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.DATA[5] = HVint >> 16;
	CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.DATA[6] = HVint >> 8;
	CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.DATA[7] = HVint;
	if (V->GetControl_SetDepth()) {
		CtlUnitCAN_MsgBuf.Msg_0x00000104.CANMsg.DATA[4] |= 0x80;
		CtlUnitCAN_MsgBuf.Msg_0x00000104.Modified = true;
		V->SetControl_SetDepth_Off();

		V->SetInfo_CalibDepthAllowedOn();										// Zunächst Neukalibrierung erlaubt
//		V->SetParam_CalFactDepth_Earmarked(0);									// ggf. vorhandenen vorgemerkten Kal.-Faktor verwerfen
	}

}

//---------------------------------------------------------------------------
// Aktualisieren der Sperr-Anweisungen
void WCAnalysis::SetLockStates()
{
        // Geschwindigkeitsabhängige Blockierung ermitteln
		if (V->GetInfo_CurrVelocity() > MAX_IGNORED_VELOCITY)  V->VelocityLock_Active = true;
		else if (V->GetInfo_CurrVelocity() < MAX_IGNORED_VELOCITY_OFF_THRESHOLD) V->VelocityLock_Active = false;


/*        // --------------------
		// OK / Übernehmen-Taste im Funktionspanel (Parameterbestätigung)
		if (
				(V->GetControl_UnloadCableActive() == _ON) ||                	// Kabel aktiv entladen
				(V->GetControl_AutoRunOn() == _ON) ||                           // Automatik an
				(V->VelocityLock_Active)                                        // Geschwindigkeit
		   )
				V->ParameterOKButton_Locked = true;
		else    V->ParameterOKButton_Locked = false;
*/
		// --------------------
		// Abbrechen/Verwerfen im Funktionspanel
		if (
				V->GetControl_UnloadCableActive()                            	// Kabel aktiv entladen
           )
                V->CancelButton_Locked = true;
        else    V->CancelButton_Locked = false;

        // --------------------
        // Automatik Ein/Aus
        if (
                   V->GetControl_UnloadCable()                               	// Kabel entladen
                || V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
				|| V->GetInfo_TractionThresholdExceeded()                       // Zugkraftschwelle überschritten
                || V->GetInfo_DepthThresholdExceeded()                          // Teufenschwelle überschritten
                || V->GetInfo_EmergencySwitch()                                 // Not-Aus-Schalter
                || ((V->GetInfo_CurrDepth() <= MIN_DEPTH_FOR_AUTO_UP) &&
                    (((V->GetParam_AutoRunProgramableOn() == _OFF) && (V->GetParam_Direction() == _UPward)) ||
                     ((V->GetParam_AutoRunProgramableOn() == _ON) && (V->GetParam_Direction_PrgAutom() == _UPward))))
				|| V->GetControl_PowerFail()
           )
                V->AutoRunOn_Locked = true;
        else    V->AutoRunOn_Locked = false;

		// --------------------
		// Bremse permanent lösen
		if (
				   V->GetInfo_EmergencySwitch()                                 // Not-Aus-Schalter
//				|| (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf aktiv, nicht nur Bremse gelöst
				|| V->GetInfo_LostMotion()                                      // Freilauf
				|| LostMotionImpulseOn                                          // Impuls für Freilaufrelais aktiv
				|| (V->GetControl_LostMotionImpulse() == _ON)					// Freilaufimpuls angefordert
		   )
				V->WinchBrakeOff_Locked = true;
		else    V->WinchBrakeOff_Locked = false;

		// --------------------
		// Bremse permanent lösen verfügbar
		if (
				!V->devel()
		   )
				V->BrakeOffEnable_Locked = true;
		else    V->BrakeOffEnable_Locked = false;

		// --------------------
		// cm-Impulse
		if (
				   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
				|| V->GetControl_UnloadCable()                               	// Kabel entladen
				|| V->VelocityLock_Active                                       // Geschwindigkeit
				|| V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
//				|| (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
		   )
				V->FormatCMImpulse_Locked = true;
		else    V->FormatCMImpulse_Locked = false;


		// --------------------
		// Dateiverwaltung
		if (
				   V->GetControl_UnloadCable()                               	// Kabel entladen
				|| V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
		   )
				V->FileAdministration_Locked = true;
		else    V->FileAdministration_Locked = false;

        // --------------------
        // Fahrtrichtung
        if (
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
//				|| (V->GetInfo_LostMotion()  && !V->GetControl_WinchBrakeOff()) // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
		   )
				V->Direction_Locked = true;
		else    V->Direction_Locked = false;

        // --------------------
        // Freigabe nach Grenzwertüberschreitung (Zugkraft/Teufe)
		if (                                                                    // Bei
				   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
				|| V->VelocityLock_Active                                       // Geschwindigkeit
				|| (V->GetControl_ThresholdExceedenceStopDelay() == _ON)        // Verzögerung des Ausschaltens des Steuergeräts bei Schwellwertüberschreitung
		   )
				V->Reset_Locked = true;
		else    V->Reset_Locked = false;

		// --------------------
		// Freilauffunktion aktivierbar
		if (
				!V->devel()														// Entwicklermodus
			) {
			V->LostMotionEnable_Locked = true;
		} else {
			V->LostMotionEnable_Locked = false;
		}

		// --------------------
		// Geschwindigkeit
		if (                                                                    // Bei
				V->GetControl_UnloadCable()                                  	// Kabel entladen
				|| V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
//				|| (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
		   )
				V->Velocity_Locked = true;
		else    V->Velocity_Locked = false;

		// --------------------
		// Geschwindigkeitseinstellungen für Automatikfahrt, dynamisch
		if (
				V->GetParam_AutoRunProgramableOn() == _ON
		   )
				V->SetAutoVelocityXButton_Locked = true;
		else    V->SetAutoVelocityXButton_Locked = false;

        // --------------------
        // Kabel entladen ein/aus
        if (                                                                    // Bei
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
// 30.07.2015 				|| V->GetControl_UnloadCableActive()                         	// Kabel aktiv entladen
//				|| (!V->GetInfo_LostMotion() && !V->GetControl_UnloadCable())	// Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
//				|| !V->GetControl_UnloadCable()									// Kabel Entladen aktiv
// 30.07.2015 				|| V->GetControl_WinchBrakeOff()                                // Nur Bremse gelöst
// 30.07.2015               || V->VelocityLock_Active                                       // Geschwindigkeit
                || V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
           )
                V->UnloadCable_Locked = true;
        else    V->UnloadCable_Locked = false;

        // --------------------
        // Kabel aktiv entladen ein/aus
        if (                                                                    // Bei
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || !V->GetControl_UnloadCable()                              	// Kabel entladen
				|| V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
				|| V->GetInfo_EmergencySwitch()                                 // Not-Aus-Schalter
				|| V->GetControl_PowerFail()
           )
                V->UnloadCableActive_Locked = true;
        else    V->UnloadCableActive_Locked = false;

        // --------------------
        // Kabel entladen: Anfangszugkraft (für stretch.)
        if (                                                                    // Bei
                V->GetControl_CalibrateOn() ||                                  // Kalibrieren ein (generell)
                (
						V->GetParam_UnloadCableStretch() &&                  	// Stretching
						V->GetControl_UnloadCableActive()                    	// Aktiv Kabel entladen
				)

		   )
				V->UnloadCableStartTraction_Locked = true;
		else    V->UnloadCableStartTraction_Locked = false;

		// --------------------
		// Kabel entladen: Endzugkraft (für stretch.)/ ansonst. Sollzugkraft
		if (                                                                    // Bei
				V->GetControl_CalibrateOn() ||                                  // Kalibrieren ein (generell)
				(
						V->GetParam_UnloadCableStretch() &&                  	// Stretching
						V->GetControl_UnloadCableActive()                    	// Aktiv Kabel entladen
                )
           )
                V->UnloadCableEndTraction_Locked = true;
        else    V->UnloadCableEndTraction_Locked = false;

        // --------------------
        // Kabel entladen: Anfangslänge (für stretch.)
        if (
                V->GetControl_CalibrateOn() ||                                  // Kalibrieren ein (generell)
                (
                        V->GetParam_UnloadCableStretch() &&                  	// Stretching
                        V->GetControl_UnloadCableActive()                    	// Aktiv Kabel entladen
                )
           )
                V->UnloadCableBeginLenght_Locked = true;
        else    V->UnloadCableBeginLenght_Locked = false;

        // --------------------
        // Kabel entladen: Endlänge (für stretch.)
        if (
                V->GetControl_CalibrateOn() ||                                  // Kalibrieren ein (generell)
                (
                        V->GetParam_UnloadCableStretch() &&                  	// Stretching
                        V->GetControl_UnloadCableActive()                    	// Aktiv Kabel entladen
                )
           )
                V->UnloadCableEndLenght_Locked = true;
        else    V->UnloadCableEndLenght_Locked = false;


        // --------------------
        // Kabel entladen: Soll-Zugkraft
        if (
                V->GetControl_UnloadCableActive() &&                         	// Aktiv entladen
                !V->GetParam_UnloadCableStretch()                            	// Stretching
           )
                V->UnloadCableTraction_Locked = true;
        else    V->UnloadCableTraction_Locked = false;

        // --------------------
        // Kabel entladen: stretching
        if (                                                                    // Bei
                V->GetControl_CalibrateOn() ||                                  // Kalibrieren ein (generell)
                V->GetControl_UnloadCableActive()                            	// Kabel aktiv entladen
           )
                V->UnloadCableStrech_Locked = true;
        else    V->UnloadCableStrech_Locked = false;

        // --------------------
        // Kalibrierung generell
        if (                                                                    // Bei
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || V->VelocityLock_Active                                       // Geschwindigkeit
 //               || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
           )
                V->CalibrateOn_Locked = true;                                   // Blockieren
        else    V->CalibrateOn_Locked = false;                                  // ansonsten frei

        // --------------------
        // Kalibrieren: Teufe
        if (                                                                    // Bei
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || V->VelocityLock_Active                                       // Geschwindigkeit
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
		   )
        {
                   V->CalibrateDepth_Locked = true;
                   V->SetDepthCalFactor_DC_Locked = true;
                   V->CalFactDepth_Locked = true;
        } else
        {
                   V->CalibrateDepth_Locked = false;
                   V->SetDepthCalFactor_DC_Locked = false;
                   V->CalFactDepth_Locked = false;
        };

		// --------------------
		// Kalibrieren: Teufe Kabellänge eingeben
		if (
			V->GetControl_MagneticMarkOn() ||									// Magnetmarkenkorr. aktiv
			(V->GetInfo_CalibDepthAllowed() && !V->GetInfo_CalibDepthCableOk()) || // Teufenkalibrierung erlaubt, aber Kabel nicht ausreichend herausgezogen
			V->VelocityLock_Active)												// Geschwindigkeitsblockierung
			V->CalFactDepth_Earmarked_Locked = true;
		else V->CalFactDepth_Earmarked_Locked = false;

		// --------------------
		// Kalibrieren: Teufe Parameter setzen
		if (
				!V->GetControl_CalibrateDepth()                              	// keine Teufenkalibrierung
//               || V->GetControl_UnloadCable()                               	// Kabel entladen
				|| (V->GetParam_CalFactDepth_Earmarked() <= 0)                 	// kein gültiger Eichfaktor
//				 || V->GetInfo_LostMotion()                                      // Freilauf
				|| V->VelocityLock_Active										// Geschwindigkeitsblockierung
		   )
				   V->CalibDepthSetParm_Locked = true;
        else V->CalibDepthSetParm_Locked = false;

        // --------------------
        // Kalibrieren: Zugkraft
        if (                                                                    // Bei
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || V->VelocityLock_Active                                       // Geschwindigkeit
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
           )
        {
                V->CalibrateTraction_Locked = true;
                V->CalibTractionX_Locked = true;
                V->CalibTractionY_Locked = true;
                V->CalibTractionZero_Locked = true;
        } else
        {
                V->CalibrateTraction_Locked = false;
                V->CalibTractionX_Locked = false;
                V->CalibTractionY_Locked = false;
                V->CalibTractionZero_Locked = false;
        };

        // --------------------
        // Kalibrieren: Zugkraft Berechnung der Parameter für die Zugkraftkalibrierung
        if (                                                                    // Bei
                !V->GetControl_CalibrateTraction()                              // keine Zugkraftkalibrierung
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || !V->GetInfo_CalibTractionValue1Ok()                          // keinem Wert 1
                || !V->GetInfo_CalibTractionValue2Ok()                          // keinem Wert 2
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
           )
                V->CalibTractionCalculateParam_Locked = true;
        else    V->CalibTractionCalculateParam_Locked = false;

        // --------------------
        // Kalibrieren: Zugkraft 1. Wert ermitteln
        if (                                                                    // Bei
                !V->GetControl_CalibrateTraction()                              // keine Zugkraftkalibrierung
                || V->GetControl_UnloadCable()                               	// Kabel entladen
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
           )
				V->CalibTension_GetVal1_Locked = true;
		else    V->CalibTension_GetVal1_Locked = false;

        // --------------------
        // Kalibrieren: Zugkraft 2. Wert ermitteln
        if (                                                                    // Bei
                !V->GetControl_CalibrateTraction()                              // keine Zugkraftkalibrierung
                || V->GetControl_UnloadCable()                               	// Kabel entladen
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
		   )
				V->CalibTension_GetVal2_Locked = true;
        else    V->CalibTension_GetVal2_Locked = false;

        // --------------------
        // Kalibrieren: Zugkraft ermittelte Werte setzen
        if (                                                                    // Bei
                !V->GetControl_CalibrateTraction()                              // keine Zugkraftkalibrierung
                || V->GetControl_UnloadCable()                               	// Kabel entladen
             || !V->GetInfo_CalibTractionValue1Ok()                             // keinem Wert 1
             || !V->GetInfo_CalibTractionValue2Ok()                             // keinem Wert 2
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
           )
                V->CalibTractionSetParam_Locked = true;
        else    V->CalibTractionSetParam_Locked = false;

        // --------------------
        // Kalibrieren: Zugkraft Nullwert setzen
        if (                                                                    // Bei
                !V->GetControl_CalibrateTraction()                              // keine Zugkraftkalibrierung
                || V->GetControl_UnloadCable()                               	// Kabel entladen
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
           )
                V->CalibTractionSetZero_Locked = true;
        else    V->CalibTractionSetZero_Locked = false;

        // --------------------
        // Kalibrieren: Zugkraft 1. Wert
        if (                                                                    // Bei
                !V->GetControl_CalibrateTraction()                              // keine Zugkraftkalibrierung
                || V->GetControl_UnloadCable()                               	// Kabel entladen
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
           )
				V->CalibTractionValue1_Locked = true;
		else    V->CalibTractionValue1_Locked = false;

        // --------------------
        // Kalibrieren: Zugkraft 2. Wert
		if (                                                                    // Bei
                !V->GetControl_CalibrateTraction()                              // keine Zugkraftkalibrierung
                || V->GetControl_UnloadCable()                               	// Kabel entladen
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
           )
                V->CalibTractionValue2_Locked = true;
        else    V->CalibTractionValue2_Locked = false;

         // --------------------
        // Leerlauf Impuls
        if(
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || (V->GetInfo_EmergencySwitch() == _ON)                        // Not-Aus-Schalter
				|| (V->GetControl_FastRun() == _OFF)                            // Wenn langsame Fahrt eingeschaltet ist
				|| V->GetControl_WinchBrakeOff()								// Windenbremse ist manuell gelöst
		  )
                V->LostMotionImpulse_Locked = true;
        else    V->LostMotionImpulse_Locked = false;

        // --------------------
        // LOG-Datei-Typ
        if (
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || V->VelocityLock_Active                                       // Geschwindigkeit
           )
                V->LOGFileType_Locked = true;
        else    V->LOGFileType_Locked = false;

        // --------------------
        // Magnetmarken ein /aus
        if (
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
				|| V->GetControl_UnloadCable()                               	// Kabel entladen
				|| V->VelocityLock_Active                                       // Geschwindigkeit
				|| (V->GetControl_CalibrateOn() && !V->GetControl_MagneticMarkOn()) // Kalibrieren ein (generell)
//				|| (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
		   )
				V->MagneticMarkOn_Locked = true;
		else    V->MagneticMarkOn_Locked = false;

		// --------------------
		// Magnetmarkenzwangssynchronisaton
		if (
				   V->GetControl_AutoRunOn()
				|| V->GetControl_UnloadCable()
				|| V->VelocityLock_Active
				|| V->GetControl_CalibrateOn()
//				|| (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())
				|| V->GetInfo_LostMotion()                                      // Freilauf
		   ) {
				V->MMSyncForceOn_Locked = true;
				V->MMSyncForceRangeLim_Locked = true;
		   }
		else {
				V->MMSyncForceOn_Locked = false;
				V->MMSyncForceRangeLim_Locked = false;
		}

		// --------------------
		// Maximale Zugkraft (für Fahrt)
        if (
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || V->VelocityLock_Active                                       // Geschwindigkeit
                || V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
           )
                V->MaxDepth_Locked = true;
        else    V->MaxDepth_Locked = false;

        // --------------------
        // Mindest-/Maximal-Teufe
        if (
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || V->VelocityLock_Active                                       // Geschwindigkeit
                || V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
           )
        {
                V->MinDepth_Locked = true;
                V->MaxDepth_Locked = true;
        }else
        {
                V->MinDepth_Locked = false;
                V->MaxDepth_Locked = false;
        };

        // --------------------
        // Motordrehzahl änderbar "Ein"/"Aus"
        if (
                   !V->devel()                                                  // Entwicklermodus
           )
        {
                V->EngineRevolutionChangeable_Locked = true;
        }else
        {
                V->EngineRevolutionChangeable_Locked = false;
        };

        // --------------------
        // Motor-Start-Stop-Funktion aktivierbar
        if (
				   !V->devel()                                                  // Entwicklermodus
		   )
		{
				V->EngineStartStopEnable_Locked = true;
		}else
		{
				V->EngineStartStopEnable_Locked = false;
		};

		// --------------------
        // Motor "Ein"
        if (
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->VelocityLock_Active                                       // Geschwindigkeit
                || (V->GetControl_EngineStop() == _ON)                          // Motor "Aus" gedrückt
                || EngineStopSignal                                             // Motor "Aus" intern noch aktiv
				|| ((V->GetInfo_EngineOn() == _ON) && (V->GetControl_EngineStart() == _OFF)) // Wenn Motor läuft und "Start"-Taste losgelassen wurde
		   )
		{
				V->EngineStart_Locked = true;
		}else
		{
				V->EngineStart_Locked = false;
		};

        // --------------------
        // Motor "Aus"
        if (
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->VelocityLock_Active                                       // Geschwindigkeit
                || (V->GetControl_EngineStart() == _ON)                         // Motor "Ein"
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf "Ein"
				|| V->GetInfo_LostMotion()                                      // Freilauf
           )
        {
                V->EngineStop_Locked = true;
        }else
        {
                V->EngineStop_Locked = false;
        };

        // --------------------
        // Parameter-Menü
        if (                                                                    // Bei
                V->GetControl_UnloadCable()                                  	// Kabel entladen
                || V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
           )
                V->ParameterMenu_Locked = true;
        else    V->ParameterMenu_Locked = false;

        // --------------------
        // Programmautomatik
        if (
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || V->VelocityLock_Active                                       // Geschwindigkeit
                || V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
           ) {
                V->AutoRunProgramableOn_Locked = true;
                V->AutoRunProgramFileName_Locked = true;
        } else {
                V->AutoRunProgramableOn_Locked = false;
                V->AutoRunProgramFileName_Locked = false;
        }

        // --------------------
        // Programmautomatik Ablaufprogrammpunkt
        if (
                   !V->GetParam_AutoRunProgramableOn()                          // Programmautomatik aus
                || V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || V->VelocityLock_Active                                       // Geschwindigkeit
                || V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
           )
                V->SetAutoRunProgramableIndex_Locked = true;
        else    V->SetAutoRunProgramableIndex_Locked = false;


        // --------------------
        // Schnelle / Langsame Fahrt
        if (
                   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->GetInfo_LeverOn()                                         // Steuerhebel "Ein"
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || V->VelocityLock_Active                                       // Geschwindigkeit
				|| V->GetControl_WinchBrakeOff()                                // Windenbremse gelöst (Sonst könnte ein evtl. vorhandenes Schaltgetriebe Schaden nehmen)
				|| V->GetInfo_LostMotion()                                      // Freilauf
		   )
                V->FastRun_Locked = true;
        else    V->FastRun_Locked = false;

        // --------------------
        // Sondersignal-Modus
        if (
                   !V->devel()
                || V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
                || V->GetInfo_LeverOn()                                         // Steuerhebel "Ein"
                || V->GetControl_UnloadCable()                               	// Kabel entladen
                || V->VelocityLock_Active                                       // Geschwindigkeit
           )
                V->SpecialSignalMode_Locked = true;
		else    V->SpecialSignalMode_Locked = false;

		// --------------------
		// Kabel-/Seilöler Dauerbetrieb
		if (!V->GetParam_LineLubAvailable())
			 V->LineLubPermanentOperation_Locked = true;
		else V->LineLubPermanentOperation_Locked = false;

		// --------------------
		// Kabel-/Seilöler Intensität
		if (!V->GetParam_LineLubAvailable())
			 V->LineLubIntensity_Locked = true;
		else V->LineLubIntensity_Locked = false;

		// --------------------
		// Start-Teufe (im Teufenrechner zu setzende Teufe)
		if (
				   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
				|| V->GetControl_UnloadCable()                               	// Kabel entladen
				|| V->VelocityLock_Active                                       // Geschwindigkeit
//				|| (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
		   )
		{
				V->SetDepth_DC_Locked = true;
				V->StartDepth_Locked = true;
		} else
		{
				V->SetDepth_DC_Locked = false;
				V->StartDepth_Locked = false;
		};

		// --------------------
		// Steuergerätefehler in der langsamen Fahrstufe erfassen
		if (
				!V->devel()
		   )
				V->ControllerErrorMonitAtSM_Locked = true;
		else    V->ControllerErrorMonitAtSM_Locked = false;

		// --------------------
		// Teufe umkehren
		if (
				   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
				|| V->GetControl_UnloadCable()                               	// Kabel entladen
				|| V->VelocityLock_Active                                       // Geschwindigkeit
//				|| (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
			) {
				V->ReverseDepth_Locked = true;
		} else {
				V->ReverseDepth_Locked = false;
		}

		// --------------------
		// System-Menü

        // --------------------
        // Menü-Aufruf
        if (
                V->GetControl_UnloadCable()                                  	// Kabel entladen
                || V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
           )
                V->SystemMenu_Locked = true;
        else    V->SystemMenu_Locked = false;

        // --------------------
        // System herunterfahren
		if (
				!(	 V->GetInfo_EmergencySwitch()
				  || V->GetControl_PowerFail()
				 ) &&
				 (
					 V->GetControl_AutoRunOn()                                  // Automatik ein/aus
				  || V->GetControl_UnloadCable()                           		// Kabel entladen
				  || V->GetControl_CalibrateOn()                             	// Kalibrieren ein (generell)
				  || V->VelocityLock_Active                                     // Geschwindigkeit
				  || V->GetInfo_LostMotion()           							// Freilauf
				 )
		   )
				V->SystemShutdownButton_Locked = true;
		else
				V->SystemShutdownButton_Locked = false;

		// --------------------
		// System-Neustart
		if (
				!(	 V->GetInfo_EmergencySwitch()
				  || V->GetControl_PowerFail()
				 ) &&
				 (
					 V->GetControl_AutoRunOn()                                  // Automatik ein/aus
				  || V->GetControl_UnloadCable()                               	// Kabel entladen
				  || V->GetControl_CalibrateOn()                                // Kalibrieren ein (generell)
				  || V->VelocityLock_Active                                     // Geschwindigkeit
				  || V->GetInfo_LostMotion()                                    // Freilauf
				 )
		   )
				V->SystemRestartButton_Locked = true;
		else
				V->SystemRestartButton_Locked = false;

		// --------------------
		// Programm-Beendigung
		if (
				!(	 V->GetInfo_EmergencySwitch()
				  || V->GetControl_PowerFail()
				 ) &&
				 (
					 V->GetControl_AutoRunOn()                                  // Automatik ein/aus
				  || V->GetControl_UnloadCable()                               	// Kabel entladen
				  || V->GetControl_CalibrateOn()                                // Kalibrieren ein (generell)
				  || V->VelocityLock_Active                                     // Geschwindigkeit
				  || V->GetInfo_LostMotion()                                    // Freilauf
				 )
		   )
				V->TerminationButton_Locked = true;
		else
				V->TerminationButton_Locked = false;

		// --------------------
		// Datum Uhrzeit Setzen
		if (
				V->GetControl_AutoRunOn()                                       // Automatik ein/aus
				|| V->GetControl_UnloadCable()                               	// Kabel entladen
				|| V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
				|| V->VelocityLock_Active                                       // Geschwindigkeit
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
		   )
				V->SystemDateTimeAcceptButton_Locked = true;
		else
				V->SystemDateTimeAcceptButton_Locked = false;

		// --------------------
		// TR-Software-Version
		if (
				   V->GetControl_AutoRunOn()                                    // Automatik ein/aus
				|| V->GetControl_UnloadCable()                               	// Kabel entladen
				|| V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
				|| V->VelocityLock_Active                                       // Geschwindigkeit
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
		   )
				V->GetDepthCalcSWVersion_Locked = true;
		else    V->GetDepthCalcSWVersion_Locked = false;

		// --------------------
		// Windenbremse intern automatisch gesteuert
		if (
				V->GetControl_AutoRunOn()                                       // Automatik ein/aus
				|| V->GetControl_UnloadCable()                               	// Kabel entladen
				|| V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
				|| V->VelocityLock_Active                                       // Geschwindigkeit
				|| V->GetInfo_LostMotion()                                      // Freilauf
			)
				V->WinchBrakeInternAutoReleaseHysteresisPerc_Locked = true;
		else	V->WinchBrakeInternAutoReleaseHysteresisPerc_Locked = false;

		// --------------------
		// Windennummer
		if (
				   V->GetControl_AutoRunOn()                                    // Automatik-Fahrt
				|| V->GetControl_UnloadCable()                               	// Kabel entladen
				|| V->VelocityLock_Active                                       // Geschwindigkeit
				|| V->GetControl_CalibrateOn()                                  // Kalibrieren ein (generell)
//                || (V->GetInfo_LostMotion() && !V->GetControl_WinchBrakeOff())  // Freilauf
				|| V->GetInfo_LostMotion()                                      // Freilauf
//				|| (V->GetParam_WinchCnt() < 2)                                 // Zu verwaltende Windenanzahl
		   )
                V->WinchNo_Locked = true;
        else    V->WinchNo_Locked = false;

};

//---------------------------------------------------------------------------
// Setzen des Programm-Modus
void WCAnalysis::SetProgramMode()
{
        // 2 = "Programm-Ende", dann wird keine Änderung mehr zugelassen
        if (V->GetInfo_ProgramMode() != 2)
        {
                // Automatik-Betrieb
                if (V->GetControl_AutoRunOn() == _ON)
                {
                        V->SetInfo_ProgramMode(4);
                        return;
                };

                // Kalibrieren
                if (V->GetControl_CalibrateOn() == _ON)
                {
                        // Teufenrechner kalibrieren
						if (V->GetControl_CalibrateDepth() == _ON)
                        {
                                V->SetInfo_ProgramMode(6);
                                return;
                        };

                        // Zugkraftmesser kalibrieren
                        if (V->GetControl_CalibrateTraction() == _ON)
                        {
                                V->SetInfo_ProgramMode(7);
                                return;
                        };

                        // Kalibrieren allgemein
                        V->SetInfo_ProgramMode(5);
                        return;
                };

                // Kabel entladen
                if (V->GetControl_UnloadCable() == _ON)
                {
                        V->SetInfo_ProgramMode(8);
                        return;
                };

                // Passiver Zustand (Freilauf ein)
                if ((V->GetInfo_LostMotion() == _ON) &&
                    (V->GetControl_UnloadCable() == _OFF)
//                    && (V->GetControl_WinchBrakeOff() == _OFF)
				   )
                {
                        V->SetInfo_ProgramMode(9);
                        return;
                };

                // Manueller Betrieb
                V->SetInfo_ProgramMode(3);
        };
};

//---------------------------------------------------------------------------
// Automatik-Reaktion
void WCAnalysis::ActAutomatic()
{
	int SetAutoVelocity;
	bool SetAutoDirection;
	long int SetDriveFrequency;

	// Bei Bedarf Automatik abschalten
	if ((V->GetInfo_EmergencySwitch()              == _ON) ||
		(V->GetInfo_LeverOn()                      == _ON) ||
		(V->GetInfo_TractionThresholdExceeded()    == _ON) ||
		(V->GetInfo_DepthThresholdExceeded()       == _ON) ||
		(V->GetControl_PowerFail()                 == _ON) ||
		((V->GetControl_AutoRunOn()                == _ON) &&
		 ((V->GetInfo_CurrDepth() <= MIN_DEPTH_FOR_AUTO_UP) &&
		  (((V->GetParam_AutoRunProgramableOn() == _OFF) && (V->GetParam_Direction() == _UPward)) ||
		   ((V->GetParam_AutoRunProgramableOn() == _ON)&& (V->GetParam_Direction_PrgAutom() == _UPward)))))) {
		V->SetControl_AutoRunOff();
	};

	// ---------------------------------------------
	if (V->GetControl_AutoRunOn() == _ON) {

		// Automatik-Aktiv-Signal
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] |= 0x04;

		// Unterscheidung der Sollgeschwindigkeit und Fahrtrichtung nach Programmautomatik oder "klassischer" Automatikfahrt
		// Wird auch bei Geschwindigkeitsvorgabe über CAN-Bus verwendet
		if (V->GetParam_AutoRunProgramableOn()) {
			SetAutoVelocity = V->GetParam_Velocity_PrgAutom();
			SetAutoDirection = V->GetParam_Direction_PrgAutom();
		} else {
			SetAutoVelocity = V->GetParam_Velocity();
			SetAutoDirection = V->GetParam_Direction();
		}

		// Geschwindigkeitsvorgabe über CAN-Bus
		SetDriveFrequency = SetAutoVelocity * pow10(DEPTHCALC_CALFACT_DIGITS) / V->GetParam_CalFactDepth(); // f=v/calfactor = (1 m/h) / (1 mm/Imp).
		SetDriveFrequency = SetDriveFrequency * 10 / 36;		// Einheit anpassen da v in m/h

		if (SetAutoDirection == _UPward) SetDriveFrequency *= -1; // Aufwärts -> negativer Wert
		SetDriveFrequency += 10000;

		if (SetDriveFrequency < 0) SetDriveFrequency = 0;       // Wertebereich auf +-10000Hz (entspr. Übertragungswert 0..20000) begrenzt
		else if (SetDriveFrequency > 20000) SetDriveFrequency = 20000;

		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[6] = static_cast<unsigned char>(SetDriveFrequency / 256);
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[7] = static_cast<unsigned char>(SetDriveFrequency);

	}else {
		// Automatik-Aktiv-Signal
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] &= ~0x04;

		// Geschwindigkeit 0Hz
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[6] = 0x27;
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[7] = 0x10; // entspricht 10000
	};
};

//---------------------------------------------------------------------------
// Aktionen zum Kalibrieren der Teufe
void WCAnalysis::ActCalibrationDepth()
{
	// Wird auch im Normalen Betrieb geprüft

	// Unter bestimmten Bedingungen ist das Kalibrieren der Teufe unterbunden
	if (V->GetControl_MagneticMarkOn() ||   // MM-Korrektur verfälscht Teufe für Kalibrierwertermittlung
		GenOrg->Evnt[1] ||					// irgendein TR-Fehler
		GenOrg->Evnt[2] ||
		V->GetInfo_IncEncError()			// Teufenencoder-Fehler
		) {
		V->SetInfo_CalibDepthAllowedOff();	// Einschalten erfolgt an anderer Stelle beim Setzen der Teufe durch den Bediener
	}

	if (!V->GetInfo_CalibDepthAllowed())
		V->SetParam_CalFactDepth_Earmarked(0);// Potetiellen Kalibrierwert zurücksetzen

	// Abgewickelte Seil-/Kabellänge für Teufenkalibrierung ausreichend?
	(abs(V->GetInfo_CurrDepth() - V->GetParam_StartDepth()) < STD_CalibDepthMinLenght) ?
		V->SetInfo_CalibDepthCableOkOff() : V->SetInfo_CalibDepthCableOkOn();

	// Parameter übernehmen
	// erfolgt durch Aktivierung des Setzens in "WCAnalysis::SetDepthParam()" und "WCAnalysis::SetDepthCalcParamm()"
}

//---------------------------------------------------------------------------
// Aktionen zum Kalibrieren der Zugkraft
void WCAnalysis::ActCalibrationTension()
{

	clock_t CountDown; //Dummy

	// permanent Zugkraftwerte sammeln für Mittelwertbildung
	if (IsTimeElapsed(&CalibTension_AverageBuffer_FillTrigger, &CountDown, CalibTension_AverageBuffer_CLOCK, 0, false))
		CalibTension_AverageManager << V->GetInfo_CurrTractionVoltage();


	if (V->GetControl_CalibrateTraction() == _ON) {

		// Ersten Wert erfassen
		if (V->GetControl_CalibTension_GetVal1() == _ON) {
			V->SetParam_CalibTension_MeasuringVal(0, CalibTension_AverageManager.GetAverage());
			V->SetControl_CalibTension_GetVal1Off();
			V->SetInfo_CalibTractionValue1OkOn();
		}

		// Zweiten Wert erfassen
		if (V->GetControl_CalibTension_GetVal2() == _ON) {
			V->SetParam_CalibTension_MeasuringVal(1, CalibTension_AverageManager.GetAverage());
			V->SetControl_CalibTension_GetVal2Off();
			V->SetInfo_CalibTractionValue2OkOn();
		}

		// Berechnen und abspeichern
		if (V->GetControl_CalibTractionSetParam() == _ON) {

			CalibTension_MeasuringValDiff = V->GetParam_CalibTension_MeasuringVal(1) - V->GetParam_CalibTension_MeasuringVal(0);
			CalibTension_TensionDiff = V->GetParam_CalibTension_Tension(1) - V->GetParam_CalibTension_Tension(0);

			// Nur wenn beide Werte Unterschiede aufweisen, erfolgt eine vollständige Zuweisung ansonsten erfolgt nur ein 0-Abgleich
			if ((CalibTension_MeasuringValDiff != 0) && (CalibTension_TensionDiff != 0)) {
				V->SetParam_CalibTractionX(CalibTension_MeasuringValDiff);
				V->SetParam_CalibTractionY(CalibTension_TensionDiff);
				V->SetParam_CalibTractionZero(V->GetParam_CalibTractionValue2() - (V->GetParam_CalibTension_MeasuringVal(1) * V->GetParam_CalibTractionY() / V->GetParam_CalibTractionX()));
			}else {
				// Nur Nullpunkt setzen
				V->SetParam_CalibTractionZero(-(V->GetParam_CalibTension_MeasuringVal(0) * V->GetParam_CalibTractionY() / V->GetParam_CalibTractionX()));
			};

			V->SetInfo_CalibTractionValue1OkOff();
			V->SetInfo_CalibTractionValue2OkOff();
			V->SetControl_CalibTractionSetParamOff();

			GenOrg->NewLOG = true;												// Neue Protokolldatei beginnen, damit Parameter im Header übernommen werden

			// Daten in die Parameter-Datei übernehmen
			if (V->WriteData() == _BAD) {
				GenOrg->Evnt << 12;												// Fehler 12 ein
			}else {
				GenOrg->Evnt >> 11;
				GenOrg->Evnt >> 12;												// Fehler 12 aus
			}
		}

		// Nullabgleich
		if (V->GetControl_CalibTractionSetZero() == _ON) {
			V->SetParam_CalibTractionZero(-(CalibTension_AverageManager.GetAverage() * V->GetParam_CalibTractionY() / V->GetParam_CalibTractionX()));
			V->SetControl_CalibTractionSetZeroOff();
			GenOrg->NewLOG = true;												// Neue Protokolldatei beginnen, damit Parameter im Header übernommen werden

			// Daten in die Parameter-Datei übernehmen
			if (V->WriteData() == _BAD) GenOrg->Evnt << 12;						// Fehler 12 ein
			else {
				GenOrg->Evnt >> 11;												// Fehler 11 aus
				GenOrg->Evnt >> 12;												// Fehler 12 aus
			}
		}
	}
}


//---------------------------------------------------------------------------
// Identifizieren des Steuergeräts anfordern
// Gleichzeitig identifiziert sich WinCtrl selbst
void WCAnalysis::ActCtlUnitCANIdent() {

	if (V->GetControl_GetCtlUnitSWVersion() == _OFF) return;

	CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[0] = 0x57; // 'W'
	CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[1] = 0x43; // 'C'
	CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[2] = __VERSION_MAIN;
	CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[3] = __VERSION_MID;
	CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[4] = __VERSION_REV;
	CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[5] = 0;
	CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[6] = 0;
	CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[7] = 0;

	CtlUnitCAN_MsgBuf.Msg_0x00000101.Modified = true;

	V->SetControl_GetCtlUnitSWVersionOff();

}

//---------------------------------------------------------------------------
// Kabel-entladen-Reaktion
void WCAnalysis::ActUnloadCable()
{
	// Bei Not-Aus-Schalter und PowerFail wird Zugkraftkontrolle ausgeschaltet
	if ((V->GetInfo_EmergencySwitch() == _ON) ||
		(V->GetControl_PowerFail() == _ON))
		V->SetControl_UnloadCableActiveOff();

	// Sollzugkraft aktualisieren
	if (V->GetControl_UnloadCable() == _ON) {

		GetUnloadCableTractionReverence();	// Sollzugkraft ermitteln

    	// Sollzugkraft auf den CAN-Bus legen
		if (V->GetInfo_CurrTractionReference() < 0) { // Erlaubte Zugkraft zwischen 0N
			CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[4] = 0;
			CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[5] = 0;
		} else if (V->GetInfo_CurrTractionReference() > 655350) { // und 65535daN
			CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[4] = 0xFF;
			CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[5] = 0xFF;
		} else { // wenn innerhalb des erlaubten Bereichs, echten Wert zuweisen
			CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[4] = static_cast<unsigned char>(V->GetInfo_CurrTractionReference() / 10 / 256); // Umrechnung in daN und obere 8 Bit übergeben
			CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[5] = static_cast<unsigned char>(V->GetInfo_CurrTractionReference() / 10); // Umrechnung in daN und untere 8 Bit übergeben
		}
	} else {
		V->SetControl_UnloadCableActiveOff();                        // Sicherheitshalber wird automatische Zugkraftreglung hier nochmals abgeschaltet (normalerweise sollte das vor Beenden des Kabel-Entladens bereits geschehen sein)
	};

	// Aktiv Kabel entladen ausgewählt
	if (V->GetControl_UnloadCableActive() == _ON)
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] |= 0x10;
	else
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] &= ~0x10;
};

//---------------------------------------------------------------------------
// Zustände an digitalen Ausgang übertragen
void WCAnalysis::ActDO()
{
		IOControl.DOOut();
};

//---------------------------------------------------------------------------
// Motor-Drehzahl-Reglung
void WCAnalysis::ActEngineRevolution()
{
		long actualTime;
		int  HV_EngineRevolution;

		// Automatische Drehzahlsteuerung prüfen u. ggf. anpassen
		// ------------------------------------------------------
		// Ausgelöste Automatik einschalten
        if ((V->GetControl_EngineRevolutionAutoUpDown() != 0) && !EngineRevolutionAutoIsOn)
        {
                EngineRevolutionAutoStartTime = GetProcessTime_in_ms();
                EngineRevolutionAutoIsOn = true;
        };

        // Bei aktiver Motordrehzahlautomatik durchführen
        if (EngineRevolutionAutoIsOn)
        {
                actualTime = GetProcessTime_in_ms();                            // Überlauftest und Reaktion weiter unten

                // aktive Automatik abschalten
                if ((V->GetControl_EngineRevolutionUpDown() != 0) ||                   // bei manueller Drehzahlsteuerung
                    ((actualTime - EngineRevolutionAutoStartTime) >= (const long)(ENGINE_REVOLUTION_AUTO_UPDOWN_DELAY)) || // wenn Zeit abgelaufen
                    (actualTime < EngineRevolutionAutoStartTime)                       // Überlauf der Timervariable
                   )
                {
                        V->SetControl_EngineRevolutionAutoHold();
                        EngineRevolutionAutoIsOn = false;
                };
        };

        // Einstellen der Art der Drehzahländerung
        // ------------------------------------------------------
        // Manuelle Drehzahlregelung hat immer Vorrang
        (V->GetControl_EngineRevolutionUpDown() != 0) ? (HV_EngineRevolution = V->GetControl_EngineRevolutionUpDown()) : (HV_EngineRevolution = V->GetControl_EngineRevolutionAutoUpDown());

        // Ausführen der Drehzahländerung
        // ------------------------------------------------------
        if (HV_EngineRevolution == -1)
        {
				IOControl.DOPortOff(DIOEngineRevolutionUpNUM);
				CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] &= ~0x04;
				IOControl.DOPortOn(DIOEngineRevolutionDownNUM);
				CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] |= 0x08;
		}else if (HV_EngineRevolution == 1)
		{
				IOControl.DOPortOff(DIOEngineRevolutionDownNUM);
				CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] &= ~0x08;
				IOControl.DOPortOn(DIOEngineRevolutionUpNUM);
				CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] |= 0x04;
		}else
		{
				IOControl.DOPortOff(DIOEngineRevolutionUpNUM);
				CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] &= ~0x04;
				IOControl.DOPortOff(DIOEngineRevolutionDownNUM);
				CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] &= ~0x08;
		};
};

//---------------------------------------------------------------------------
// Motor-an-Ausgang-Reaktion
void WCAnalysis::ActEngineStart()
{
        if ((V->GetControl_EngineStart() == _ON) && !EngineStopSignal)
        {
                // Start-Verzögerung bearbeiten und auswerten
                if (EngineStartDelayCounter < ENGINE_STARTSTOP_DELAY)
                {
                        if (EngineStartDelayTrigger != GenOrg->Clock_100ms)
                        {
                                EngineStartDelayTrigger = GenOrg->Clock_100ms;
                                EngineStartDelayCounter += 100;
                        };
						IOControl.DOPortOff(DIOEngineStartNUM);
						CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] &= ~0x01;

				}else
				{       // Verzögerung abgelaufen
						IOControl.DOPortOn(DIOEngineStartNUM);
						CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] |= 0x01;
				};
		}else
		{
				EngineStartDelayCounter = 0;                                    // Start-Verzögerungszähler zurückgesetzt

				IOControl.DOPortOff(DIOEngineStartNUM);
				CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] &= ~0x01;
		};
};

//---------------------------------------------------------------------------
// Motor-aus-Ausgang-Reaktion
void WCAnalysis::ActEngineStop()
{

        //Stopp-Signal starten
        if ((V->GetControl_EngineStop() == _ON) && (V->GetControl_EngineStart() == _OFF))
        {
                // Stopp-Verzögerung bearbeiten und auswerten
                if (EngineStopDelayCounter < ENGINE_STARTSTOP_DELAY)
                {
                        if (EngineStopDelayTrigger != GenOrg->Clock_100ms)
                        {
                                EngineStopDelayTrigger = GenOrg->Clock_100ms;
                                EngineStopDelayCounter += 100;
                        };
                }else
                {       // Verzögerung abgelaufen
                        EngineStopSignal = true;                                // Motor soll nun stoppen
                        EngineStopSignalOffDelay = 0;                           // Motor-Stop-Zeitzähler zurücksetzen
                        V->SetControl_EngineRevolutionAutoDown();               // Drehzahl runter, wenn Motor aus
                };

        }else
        {
                EngineStopDelayCounter = 0;
        };

        // Motor Stopp-Signal
        if (EngineStopSignal)
        {
                if (EngineStopSignalOffDelay < ENGINE_STOPSIGNALOFF_DELAY)
                {
                        if (EngineStopSignalOffDelayTrigger != GenOrg->Clock_100ms)
                        {
                                EngineStopSignalOffDelayTrigger = GenOrg->Clock_100ms;
                                EngineStopSignalOffDelay += 100;
                        };
						IOControl.DOPortOn(DIOEngineStopNUM);
						CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] |= 0x02;
				}else
				{
                        EngineStopSignalOffDelay = 0;
                        EngineStopSignal = false;                               // Motor-Stopp-Relais fällt beim nächsten Schleifendurchlauf ab
                };
        }else
        {
                // EngineStopSignal muss auch hier noch ausgeschaltet werden, da sonst folgendes:
                // Motor läuft und wird durch Taste ausgeschaltet
                // -> D+-Signal verschwindet, EngineStopSignal ist aber noch true
                // -> Nach Motor-Start ist D+-Signal wieder da und EngineStopSignal noch true
                // -> Motor wird sofort automatisch wieder ausgeschaltet
                EngineStopSignal = false;
				IOControl.DOPortOff(DIOEngineStopNUM);
				CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] &= ~0x02;
        };
};

//---------------------------------------------------------------------------
// Reaktionen auf diverse Fehler (Resets usw.)
void WCAnalysis::ActErrorReactions()
{

	clock_t Countdown; // nur Dummy

	// Wenn Fehler 2 ein (Kommunikation mit TR fehlgeschlagen)
	if (GenOrg->Evnt[2] == _ON) {
		V->SetControl_GetDepthCalcSWVersionOn();                        		// SW-Version muss neu ausgelesen werden

		// Alle TR Parameter setzen (außer Teufe)
		V->SetControl_SetDepthCalFactor_DC_On();
		V->SetControl_SetFormatCMImpulseOn();
		V->SetControl_SetMagneticMarkOn();

		V->SetControl_SetDepthCalcParamOn();                            		// Parameter des TR müssen neu gesetzt werden
	};

};

//---------------------------------------------------------------------------
// Externe Informationsausgabe
void WCAnalysis::ActExtInfoOut()
{
        char CheckByte = IOControl.CheckInfoOutPort();
        unsigned char InfoOutString[10];
        int FormattedDepth = abs(V->GetInfo_CurrDepth() / 10);                  // Teufe in cm nur positiv
        int FormattedTraction = fabs((float)(V->GetInfo_CurrTraction()) / __GFORCE);  // Zugkraft in kp nur positiv
        int HV;

        switch (CheckByte)
        {
                case INFOOUT_IN_1:      // Teufe und Zugkraft ausgeben
                                        InfoOutString[0] = INFOOUT_OUT_1;
                                        // Teufe eintragen
                                        InfoOutString[1] = (unsigned char) (FormattedDepth & 0xFF);
                                        InfoOutString[2] = (unsigned char)((FormattedDepth & 0xFF00)    / 0x100);
                                        InfoOutString[3] = (unsigned char)((FormattedDepth & 0xFF0000)  / 0x10000);


                                        // Zugkraft Formatieren und eintragen
                                   /*   Ausgabeprotokoll bis Version 1.1.3 Octber 12, 2005 0
                                        HV = FormattedTraction % 10;            // 1er
                                        InfoOutString[4] = (unsigned char)((HV & 0x0F) * 0x10);
                                        FormattedTraction /= 10;                // 10er
                                        HV = FormattedTraction % 10;
                                        InfoOutString[4] |= (unsigned char)(HV & 0x0F);
                                        FormattedTraction /= 10;                // 100er
                                        HV = FormattedTraction % 10;
                                        InfoOutString[5] = (unsigned char)((HV & 0x0F) * 0x10);
                                        FormattedTraction /= 10;                // 1000er
                                        HV = FormattedTraction % 10;
                                        InfoOutString[5] |= (unsigned char)(HV & 0x0F);
                                   */
                                        HV = FormattedTraction % 10;            // 1er
                                        InfoOutString[4] = (unsigned char)(HV & 0x0F);
                                        FormattedTraction /= 10;                // 10er
                                        HV = FormattedTraction % 10;
                                        InfoOutString[4] |= (unsigned char)((HV & 0x0F) * 0x10);
                                        FormattedTraction /= 10;                // 100er
                                        HV = FormattedTraction % 10;
                                        InfoOutString[5] = (unsigned char)(HV & 0x0F);
                                        FormattedTraction /= 10;                // 1000er
                                        HV = FormattedTraction % 10;
                                        InfoOutString[5] |= (unsigned char)((HV & 0x0F) * 0x10);
                                        IOControl.WriteToInfoOutPort(InfoOutString, 6);
                                        break;

                case INFOOUT_IN_2:      // Zugkraft ausgeben
                                        InfoOutString[0] = INFOOUT_OUT_2;

                                        // Zugkraft Formatieren und eintragen
                                   /*   Ausgabeprotokoll bis Version 1.1.3 Octber 12, 2005 0
                                        HV = FormattedTraction % 10;            // 1er
                                        InfoOutString[1] = (unsigned char)((HV & 0x0F) * 0x10);
                                        FormattedTraction /= 10;                // 10er
                                        HV = FormattedTraction % 10;
                                        InfoOutString[1] |= (unsigned char)(HV & 0x0F);
                                        FormattedTraction /= 10;                // 100er
                                        HV = FormattedTraction % 10;
                                        InfoOutString[2] = (unsigned char)((HV & 0x0F) * 0x10);
                                        FormattedTraction /= 10;                // 1000er
                                        HV = FormattedTraction % 10;
                                        InfoOutString[2] |= (unsigned char)(HV & 0x0F);
                                   */
                                        HV = FormattedTraction % 10;            // 1er
                                        InfoOutString[1] = (unsigned char)(HV & 0x0F);
                                        FormattedTraction /= 10;                // 10er
                                        HV = FormattedTraction % 10;
                                        InfoOutString[1] |= (unsigned char)((HV & 0x0F) * 0x10);
                                        FormattedTraction /= 10;                // 100er
                                        HV = FormattedTraction % 10;
                                        InfoOutString[2] = (unsigned char)(HV & 0x0F);
                                        FormattedTraction /= 10;                // 1000er
                                        HV = FormattedTraction % 10;
                                        InfoOutString[2] |= (unsigned char)((HV & 0x0F) * 0x10);
                                        IOControl.WriteToInfoOutPort(InfoOutString, 3);

                                        break;

                default:
                                        break;
        };
};

//---------------------------------------------------------------------------
// Schnell/Langsam-Umschaltungs-Reaktion
void WCAnalysis::ActFastRun()
{
		// Ausgang je nach Bedarf setzen                                        // Schnell wenn:
		if ((V->GetControl_FastRun() == _ON)||                                  // Schnell ein
//            ((V->GetInfo_LostMotion() == _ON) && (V->GetControl_WinchBrakeOff() == _OFF)
			(V->GetInfo_LostMotion() == _ON)									// Freilauf ein
		   )
		{
				IOControl.DOPortOff(DIOFastRunNUM);
				CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] &= ~0x08;
		}else
		{
				IOControl.DOPortOn(DIOFastRunNUM);
				CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] |= 0x08;
		};
};

//---------------------------------------------------------------------------
// Hydraulische Bremse
void WCAnalysis::ActHydraulicBrake()
{
	// Ausgang je nach Bedarf setzen
	if (V->GetParam_HydraulicBrake())
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] |= 0x20;
	else
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] &= ~0x20;
}

//---------------------------------------------------------------------------
// Freilauf-Impuls-Reaktion bzw. Ausführung der Windenbremse
void WCAnalysis::ActLostMotion()
{
	if (V->GetControl_LostMotionImpulse() == _ON) {
		// Nur wenn folgende Bedingungen eingehalten werden,
		// wird Freilaufimpuls ausgelöst.
		if ( V->GetControl_WinchBrakeOff() == _OFF &&
			(V->GetControl_FastRun() == _ON)) {

				IOControl.DOPortOn(DIOLostMotionImpulseNUM);
				CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] |= 0x10;
				LostMotionImpulsLenght = 0;
				LostMotionImpulseOn = true;
		}

		// Initiierungs-Impuls wird in jedem Falle wieder abgeschaltet,
		// auch wenn keine Hardware-Reaktion aufgrund obiger Zustände erfolgt.
		V->SetControl_LostMotionImpulseOff();
	};

	if (LostMotionImpulseOn) {
		if (GenOrg->Clock_100ms != LostMotionImpulseTrigger) {      // Positive Flanke des Taktes
			++LostMotionImpulsLenght;                               // Zähler erhöhen
			LostMotionImpulseTrigger = GenOrg->Clock_100ms;         // Positive Flanke erkannt und abgearbeitet
		};

		if (LostMotionImpulsLenght >= IMPULSE_LENGHT_LostMotion) {
			LostMotionImpulseTrigger = GenOrg->Clock_100ms;         // Reset und Relay abschalten
			LostMotionImpulseOn = false;
			LostMotionImpulsLenght = 0;

			IOControl.DOPortOff(DIOLostMotionImpulseNUM);
			CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] &= ~0x10;
		};
	};
};

//---------------------------------------------------------------------------
// MM-Zwangssynchronisation ausführen
void WCAnalysis::ActMMSyncForce()
{
	if (FirstRun) {
		MMSyncForceAwait = false;
	}

// Bei erzwungener Magnetmarkensynchronisation auch Steuergerät Teufe Setzen
	if (V->GetControl_MMSyncForceActiveOn()) {

			if (!V->GetInfo_MagneticMark() &&
				 !V->GetInfo_MagneticMarkDirect())
                 MMSyncForceAwait = true;

			if (MMSyncForceAwait &&
				(V->GetInfo_MagneticMark() ||
				 V->GetInfo_MagneticMarkDirect())) {

				MMSyncForceAwait = false;
                V->SetControl_MMSyncForceCounter(V->GetControl_MMSyncForceCounter() + 10);

				V->SetParam_StartDepth(V->GetControl_MMSyncForceCounter() * 1000);
				V->SetControl_SetDepth_DC_On();
				V->SetControl_SetDepthCalcParamOn();
			}

			if (V->GetControl_MMSyncForceCounter() >= V->GetParam_MMSyncForceRangeLim()) V->SetControl_MMSyncForceActiveOff();

	};

}

//---------------------------------------------------------------------------
// Selbsthalteschaltung aktivieren
void WCAnalysis::ActPowerLock()
{
	IOControl.DOPortOn(DIOPowerLockNUM);
	CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] |= 0x02;
}

//---------------------------------------------------------------------------
// Steuert die Einstellungen der Programmautomatik
void WCAnalysis::ActPrgAutomatic()
{
    int HV, HV2;
    bool HVbool;

    if ((V->GetControl_AutoRunOn() == _ON) &&
        (V->GetParam_AutoRunProgramableOn() == _ON)) {

        // ----- Ablaufprogrammpunkt einlesen
        if (PrgAutom_ReadStep) {

            // Geschwindigkeit
            HV = GenOrg->PrgAutomatic.GetVelocity(GenOrg->PrgAutoIndex);
            if (HV < 0) { // bei Fehler
                V->SetParam_Velocity_PrgAutom(0);   // wie Stopp-Eintrag behandeln
            } else {
                V->CheckParam_Velocity_PrgAutom(HV, &HV2);
                V->SetParam_Velocity_PrgAutom(HV2);
            }

            // Zielteufe-Vorauswertung
            if (V->GetParam_Velocity_PrgAutom() != 0) {
                HV = GenOrg->PrgAutomatic.GetTargetDepth(GenOrg->PrgAutoIndex);
                if (HV < 0) {// bei Fehler
                    V->SetParam_Velocity_PrgAutom(0);   // wie Stopp-Eintrag behandeln
                }
            }

            // Fahrtrichtung ignorieren ?
            if (V->GetParam_Velocity_PrgAutom() != 0) {
                GenOrg->PrgAutomatic.GetLastError();
                HVbool = GenOrg->PrgAutomatic.DirectionIgnored(GenOrg->PrgAutoIndex);
                if (GenOrg->PrgAutomatic.GetLastError() != 0) { // bei Fehler
                    V->SetParam_Velocity_PrgAutom(0);   // wie Stopp-Eintrag behandeln
                }
            }

            // Fahrtrichtung festlegen
            if (V->GetParam_Velocity_PrgAutom() != 0) {
                // Falls Fahrtrichtung ignoriert
                // dann die tatsächliche Fahrtrichtung festlegen
                if (HVbool) {
                    if (V->GetInfo_CurrDepth() > GenOrg->PrgAutomatic.GetTargetDepth(GenOrg->PrgAutoIndex)) V->SetParam_Direction_PrgAutomUp();
                    else V->SetParam_Direction_PrgAutomDown();
                } else { // ansonsten Fahrtrichtung einlesen und setzen
                    GenOrg->PrgAutomatic.GetLastError();
                    HVbool = GenOrg->PrgAutomatic.GetDirection(GenOrg->PrgAutoIndex);
                    if (GenOrg->PrgAutomatic.GetLastError() != 0) { // bei Fehler
                        V->SetParam_Velocity_PrgAutom(0);   // wie Stopp-Eintrag behandeln
                    } else {
                        if (HVbool == _UPward) V->SetParam_Direction_PrgAutomUp();
                        else V->SetParam_Direction_PrgAutomDown();
                    }
                }
            }
            PrgAutom_ReadStep = false;
        }

        // ---- Automatik steuern
        // Ablaufprogrammpunkt STOPP
        if (V->GetParam_Velocity_PrgAutom() == 0) {
            V->SetControl_AutoRunOff(); // Automatik anhalten
			++GenOrg->PrgAutoIndex;     // Ablaufprogrammpunkt weiterzählen, jedoch nicht über die maximale Anzahl hinausgehen
            if (GenOrg->PrgAutoIndex > GenOrg->PrgAutomatic.GetIndexCount()) GenOrg->PrgAutoIndex = GenOrg->PrgAutomatic.GetIndexCount();
        } else {
            HV = GenOrg->PrgAutomatic.GetTargetDepth(GenOrg->PrgAutoIndex);
            if (((V->GetParam_Direction_PrgAutom() == _UPward) && (V->GetInfo_CurrDepth() <= HV)) ||
                ((V->GetParam_Direction_PrgAutom() == _DOWN) && (V->GetInfo_CurrDepth() >= HV))) {
                ++GenOrg->PrgAutoIndex;     // Ablaufprogrammpunkt weiterzählen, jedoch nicht über die maximale Anzahl hinausgehen
                if (GenOrg->PrgAutoIndex > GenOrg->PrgAutomatic.GetIndexCount()) GenOrg->PrgAutoIndex = GenOrg->PrgAutomatic.GetIndexCount();
                PrgAutom_ReadStep = true;   // Neuen Ablaufprogrammpunkt einlesen
            }
        }

    } else PrgAutom_ReadStep = true;

    if (V->GetParam_AutoRunProgramableOn() == _OFF) GenOrg->PrgAutoIndex = -1;

}


//---------------------------------------------------------------------------
// Signal-Ausgang (Hupe) Reaktion
void WCAnalysis::ActSignalHorn()
{
        if (V->GetControl_SignalHornOn() == _ON)
        {
			IOControl.DOPortOn(DIOSignalHornNUM);
			CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] |= 0x80;

			// TEST5: zyklisches Senden der Identifikations-Botschaft 0x101 alle 500ms
			clock_t Countdown;
			if (IsTimeElapsed(&ActSignalHorn_101_Time_old, &Countdown, 500, 0, FirstRun))
			{
				CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[0] = 0x57; // 'W'
				CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[1] = 0x43; // 'C'
				CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[2] = __VERSION_MAIN;
				CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[3] = __VERSION_MID;
				CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[4] = __VERSION_REV;
				CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[5] = 0;
				CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[6] = 0;
				CtlUnitCAN_MsgBuf.Msg_0x00000101.CANMsg.DATA[7] = 0;
				CtlUnitCAN_MsgBuf.Msg_0x00000101.Modified = true;
			}
		}else
		{
			IOControl.DOPortOff(DIOSignalHornNUM);
			CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] &= ~0x80;
		};
};


//---------------------------------------------------------------------------
// Sonderfunktion ausführen
void WCAnalysis::ActSpecialFunc(){

	// -----------------------------------
	// Sonderfunktion 1 (Kabel-/Seilöler)
	if (!V->GetParam_LineLubAvailable()) V->SetControl_SpecialFunc1Off();

	// Kabel- Seilöler (Pumpe und Schaltventil)
	if (V->GetControl_SpecialFunc1() == _ON) {
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[3] = V->GetParam_LineLubIntensity(); // Sonderausgang 1 PWM-Signal am Steuergerät mit Tastgrad 1 ausgeben
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] |= 0x40;
	}
	else {
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[3] = 0;
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[1] &= ~0x40;
	}


}

//---------------------------------------------------------------------------
// Auswertung der Sondersignale
void WCAnalysis::ActSpecialSignals()
{
		clock_t Countdown; // nur Dummy

		// Notwendige ereignisabhängige Resets
		// Reset für 1. Variante
		// .....................
		if (FirstRun) {
				ActSpecialSignals_V01_EventON = false;                          // Ereignis aus
				GenOrg->Evnt >> 22;												// Fehlermeldung aus
				GenOrg->Evnt >> 24;												// Fehlermeldung aus
				GenOrg->Evnt >> 25;												// Fehlermeldung aus
				GenOrg->Evnt >> 26;												// Fehlermeldung aus
		};


		// Bestehende durch Sondersignale ausgelöste Fehler und Zustände,
		// deren auslösender Signalmodus inaktiv ist müssen gelöscht werden
		// Außerdem diverse Flags zurücksetzen
		switch (V->GetParam_SpecialSignalMode()) {
			case 1:
				// Sondersignalmodus Version 2 - inaktiv
				GenOrg->Evnt >> 24;
				GenOrg->Evnt >> 25;
				// Sondersignalmodus Version 3 - inaktiv
				V->SetInfo_OilStrainer2Off();
				// Sondersignalmodus Version 4 - inaktiv
				GenOrg->Evnt >> 26;
				// Sondersignalmodus Version 5 - inaktiv
				V->SetInfo_SpecialSignalExtBrakeOff();
				V->SetParam_ExternalBrakeAvailableOff();
				break;
			case 2:
				// Sondersignalmodus Version 1 - inaktiv
				GenOrg->Evnt >> 22;
				ActSpecialSignals_V01_EventON = false;                          // Ereignis aus
				// Sondersignalmodus Version 3 - inaktiv
				V->SetInfo_OilStrainer2Off();
				// Sondersignalmodus Version 4 - inaktiv
				GenOrg->Evnt >> 26;
				// Sondersignalmodus Version 5 - inaktiv
				V->SetInfo_SpecialSignalExtBrakeOff();
				V->SetParam_ExternalBrakeAvailableOff();
				break;
			case 3:
				// Sondersignalmodus Version 1 - inaktiv
				GenOrg->Evnt >> 22;
				ActSpecialSignals_V01_EventON = false;                          // Ereignis aus
				// Sondersignalmodus Version 2 - inaktiv
				GenOrg->Evnt >> 24;
				GenOrg->Evnt >> 25;
				// Sondersignalmodus Version 4 - inaktiv
				GenOrg->Evnt >> 26;
				// Sondersignalmodus Version 5 - inaktiv
				V->SetInfo_SpecialSignalExtBrakeOff();
				V->SetParam_ExternalBrakeAvailableOff();
				break;
			case 4:
				// Sondersignalmodus Version 1 - inaktiv
				GenOrg->Evnt >> 22;
				ActSpecialSignals_V01_EventON = false;                          // Ereignis aus
				// Sondersignalmodus Version 2 - inaktiv
				GenOrg->Evnt >> 24;
				GenOrg->Evnt >> 25;
				// Sondersignalmodus Version 3 - inaktiv
				V->SetInfo_OilStrainer2Off();
				// Sondersignalmodus Version 5 - inaktiv
				V->SetInfo_SpecialSignalExtBrakeOff();
				V->SetParam_ExternalBrakeAvailableOff();
				break;
			case 5:
				// Sondersignalmodus Version 1 - inaktiv
				GenOrg->Evnt >> 22;
				ActSpecialSignals_V01_EventON = false;                          // Ereignis aus
				// Sondersignalmodus Version 2 - inaktiv
				GenOrg->Evnt >> 24;
				GenOrg->Evnt >> 25;
				// Sondersignalmodus Version 3 - inaktiv
				V->SetInfo_OilStrainer2Off();
				// Sondersignalmodus Version 4 - inaktiv
				GenOrg->Evnt >> 26;
				break;

		default:
				// Sondersignalmodus Version 1 - inaktiv
				GenOrg->Evnt >> 22;
				ActSpecialSignals_V01_EventON = false;                          // Ereignis aus
				// Sondersignalmodus Version 2 - inaktiv
				GenOrg->Evnt >> 24;
				GenOrg->Evnt >> 25;
				// Sondersignalmodus Version 3 - inaktiv
				V->SetInfo_OilStrainer2Off();
				// Sondersignalmodus Version 4 - inaktiv
				GenOrg->Evnt >> 26;
				// Sondersignalmodus Version 5 - inaktiv
				V->SetInfo_SpecialSignalExtBrakeOff();
				V->SetParam_ExternalBrakeAvailableOff();
		}

		// Eigentliche Auswerteung
		switch (V->GetParam_SpecialSignalMode())
		{
				case 1:
						// 1. Variante: Umschaltung Schaltgetriebe, Positionsschalterüberwachung
						// .....................................................................
						if (      (V->GetInfo_SpecialSignal01() == V->GetInfo_SpecialSignal02())// beide an, oder beide aus
								||(V->GetInfo_SpecialSignal01() != V->GetControl_FastRun())     // Schnelle Fahrt detektiert, aber nicht erwartet
								||(V->GetInfo_SpecialSignal02() == V->GetControl_FastRun())     // Langsame Fahrt detektiert, aber schnelle Fahrt erwartet
						   )
						{
								if (!GenOrg->Evnt[22])
								{
										// Triggern der Verzögerung
										if (!ActSpecialSignals_V01_EventON)                     // Ereignis neu (Flag noch nicht umgeschaltet)
												IsTimeElapsed(&ActSpecialSignals_Time_old, &Countdown, SPECIALSIGNAL_V01_DELAY, 0, true);

										// Verzögerung abgelaufen? -> Meldung ein
										if (IsTimeElapsed(&ActSpecialSignals_Time_old, &Countdown, SPECIALSIGNAL_V01_DELAY, 0, false)) {
												GenOrg->Evnt << 22;
										}
								};

								ActSpecialSignals_V01_EventON = true;
						} else
						{
								if (GenOrg->Evnt[22])
								{
										// Triggern der Verzögerung
										if (ActSpecialSignals_V01_EventON)                      // Ereignis neu (Flag noch nicht umgeschaltet)
												IsTimeElapsed(&ActSpecialSignals_Time_old, &Countdown, SPECIALSIGNAL_V01_DELAY, 0, true);

										// Verzögerung abgelaufen? -> Meldung ein
										if (IsTimeElapsed(&ActSpecialSignals_Time_old, &Countdown, SPECIALSIGNAL_V01_DELAY, 0, false)) {
												GenOrg->Evnt >> 22;
										}
								};

								ActSpecialSignals_V01_EventON = false;
						};
						break;

				case 2:
						// 2. Variante: 2 Fehler externer Apparaturen
						// .....................................................................
						// Signal 1 (lowaktiv) auswerten
						if (V->GetInfo_SpecialSignal01()) {
							// Fehler aktivieren
							if (!GenOrg->Evnt[24]) {
								GenOrg->Evnt << 24;
								V->SetControl_ResetSpecialSignalError1FlagOff();// Fehler muss durch Anwender Quitiert werden
							}
						} else
						{
							// Fehler deaktivieren
							if (V->GetControl_ResetSpecialSignalError1Flag())
								GenOrg->Evnt >> 24;
						};

                        // Signal 2 (lowaktiv) auswerten
                        if (V->GetInfo_SpecialSignal02()) {
                            // Fehler aktivieren
							if (!GenOrg->Evnt[25]) {
								GenOrg->Evnt << 25;
								V->SetControl_ResetSpecialSignalError2FlagOff();    // Fehler muss durch Anwender Quitiert werden
							}
						} else
						{
							// Fehler deaktivieren
							if (V->GetControl_ResetSpecialSignalError2Flag())
								GenOrg->Evnt >> 25;
						};
						break;

				case 3:
						// 3. Variante: "Ölfilter wechseln" an Sondersignal 1 /
						// .....................................................................
						// Signal 1 (highaktiv) auswerten
						if (V->GetInfo_SpecialSignal01()) {
							V->SetInfo_OilStrainer2On();
						} else {
							V->SetInfo_OilStrainer2Off();
						}
						break;
				case 4:
						// 4. Variante: "Mast ungesichert" an Sondersignal 1 oder 2
						// .....................................................................
						// Signal 1 und 2 (highaktiv) auswerten
						if (V->GetInfo_SpecialSignal01()&&
							V->GetInfo_SpecialSignal02()) {
							GenOrg->Evnt >> 26;
						} else {
							GenOrg->Evnt << 26;
						}
						break;
				case 5:
						// 5. Variante: "Externe Bremse gelöst" an Sondersignal 1
						// .....................................................................
						V->SetParam_ExternalBrakeAvailableOn();
						// Signal 1 (highaktiv) auswerten
						if (V->GetInfo_SpecialSignal01()) {
							V->SetInfo_SpecialSignalExtBrakeOn();
						} else {
							V->SetInfo_SpecialSignalExtBrakeOff();
						}
						break;
				default:
						// 0. Variante: Überwachung Sondersignale aus
						// ---------------------------------------------------------------------

						break;
		};
};

//---------------------------------------------------------------------------
// Ausgabe der Zugkraft als Spannung für KEDR
// Nur für KEDR! Zugkraft der Ausgabe synchron zur Anzeige von WinchControl.
void WCAnalysis::ActTensionOut_KEDR()
{
		float HVfloat = (float)(TENSIONOUT_KEDR_MAX_Voltage - TENSIONOUT_KEDR_MIN_Voltage) * (float)(V->GetInfo_CurrTraction()) / (float)(TENSIONOUT_KEDR_MAX_Tension - TENSIONOUT_KEDR_MIN_Tension);
		if (HVfloat > TENSIONOUT_KEDR_MAX_Voltage) HVfloat = TENSIONOUT_KEDR_MAX_Voltage;
		else if (HVfloat < TENSIONOUT_KEDR_MIN_Voltage) HVfloat = TENSIONOUT_KEDR_MIN_Voltage;
		IOControl.ADUOut(AO_TensionOutChannel, HVfloat / 1000, false, false);
};

//---------------------------------------------------------------------------
// Ausgabe der Spannungen an den DAU
void WCAnalysis::ActVoltages()
{
        // Parameter für Kanal und Spannung ignorieren
        // deshalb für beide 0 eingetragen, da sowieso egal
        IOControl.ADUOut(0, 0, true, true);
};

//---------------------------------------------------------------------------
// Steuerung der Windenbremse durch eine interne Automatik
void WCAnalysis::ActWinchBrakeInternAuto()
{
	bool HV_BrakeFixate = false;
	bool HV_BrakeRelease = false;

	float HV_CTL_UpThresh;			// Schwelle für Grenze zum Bremse Lösen aufwärts [mV]
	float HV_CTL_DwnThresh;     	// Schwelle für Grenze zum Bremse Lösen abwärts [mV]
	float HV_CTL_HystUp;      	 	// Hysterese Bremse Lösen/Feststellen aufwärts (damit Bremse an Grenzstelle nicht flackert)[mV]
	float HV_CTL_HystDwn;    		// Hysterese Bremse Lösen/Feststellen abwärts (damit Bremse an Grenzstelle nicht flackert)[mV]
	float HV_CTL_Sig;				// Steuerungssignal [mV]

	clock_t Countdown;				// Dummy

	// Initialisierung persistenter Hilfsvariablen
	// ---------------------------------------------
	if (FirstRun) {
		WinchBrakeInternAuto_Released = false;
		WinchBrakeInternAuto_StoppExpected = false;
		WinchBrakeInternAuto_ReleaseAllowed = false;
		WinchBrakeInternAuto_StartingAidRefPos = 2500;
	}

	// Windenbremse ist ausgeschaltet, intere Automatikbremse  inaktiv
	// ----------------------------------------------------------------
	if ((V->GetParam_WinchBrakeInternAuto() == _OFF) ||
		(V->GetControl_WinchBrakeOff() == _ON)){
		WinchBrakeInternAuto_StoppExpected = false;
		WinchBrakeInternAuto_ReleaseAllowed = false;
		V->SetInfo_CurrWinchBrakeInternAutoStatus(0);
		return;
	}

	// Status-Signalisierung für Anzeige aktualisieren
	if (!WinchBrakeInternAuto_Released)	V->SetInfo_CurrWinchBrakeInternAutoStatus(1);
	else if (WinchBrakeInternAuto_StoppExpected) V->SetInfo_CurrWinchBrakeInternAutoStatus(3);
	else V->SetInfo_CurrWinchBrakeInternAutoStatus(2);


	// Berechnen der Schwellwerte und Hysteresen für das Steuerungssignal
	// -------------------------------------------------------------------
	// ---
	// Hysterese
	HV_CTL_HystDwn     = 	(AO_WinchRevMaxVoltage - 2500)
						  / 100
						  * V->GetParam_WinchBrakeInternAutoReleaseHysteresisPerc();

	// ---
	HV_CTL_HystUp	   = 	(2500 - AO_WinchRevMinVoltage)
						  / 100
						  * V->GetParam_WinchBrakeInternAutoReleaseHysteresisPerc();
	// ---
	// Abwickeln: Ansteuerungsspannung > V->GetParam_DriveVelocity0mph())
	HV_CTL_DwnThresh   = (
						   (AO_WinchRevMaxVoltage - 2500)
						   / 100
						   * V->GetParam_WinchBrakeInternAutoReleasePerc()
						 )
						 + 2500;

	// Hysterese bei bereits gelöster Bremse berücksichtigen
	if (WinchBrakeInternAuto_Released) HV_CTL_DwnThresh -= HV_CTL_HystDwn;

	// Hysterese bei angezogener Bremse berücksichtigen
	if (!WinchBrakeInternAuto_Released) HV_CTL_DwnThresh += HV_CTL_HystDwn;

	// ---
	// Aufwickeln: Ansteuerungsspannung < V->GetParam_DriveVelocity0mph())
	HV_CTL_UpThresh     = 2500
						 - (
							  (2500 - AO_WinchRevMinVoltage)
							/ 100
							* V->GetParam_WinchBrakeInternAutoReleasePerc()
						   );
	// Anfahrhilfe berücksichtigen
	if (HV_CTL_UpThresh > WinchBrakeInternAuto_StartingAidRefPos) HV_CTL_UpThresh = WinchBrakeInternAuto_StartingAidRefPos;

	// Hysterese bei bereits gelöster Bremse berücksichtigen
	if (WinchBrakeInternAuto_Released) HV_CTL_UpThresh += HV_CTL_HystUp;

	// Hysterese bei bereits festgestellter Bremse berücksichtigen
	if (!WinchBrakeInternAuto_Released) HV_CTL_UpThresh -= HV_CTL_HystUp;

	// Aktuelle Steuerspannungsposition erfassen
	HV_CTL_Sig = V->GetInfo_CurrWinchRevMessVoltage();

	// Logik für Anziehen/Lösen der Bremse
	// -----------------------------------------------------------
	// Zustand: Bremse festgestellt
	if (!WinchBrakeInternAuto_Released) {

		WinchBrakeInternAuto_StoppExpected = false;		// Bremsen wird nicht mehr erwartet

		// Bremse lösen erlauben
		if ((V->GetInfo_EmergencySwitch() == _OFF) &&
			(V->GetControl_WinchWork() == _ON)){

			if (((HV_CTL_Sig < HV_CTL_DwnThresh) &&         // Steuerungsspannung im aktuellen Bremsbereich
				 (HV_CTL_Sig > HV_CTL_UpThresh)) ||
				(V->GetControl_AutoRunOn() == _ON)          // Automatikfahrt aktiviert
				) {

				WinchBrakeInternAuto_ReleaseAllowed = true;
			}
		}
	}

	// ---
	// Zustand: Bremse gelöst
	if (WinchBrakeInternAuto_Released) {

		WinchBrakeInternAuto_ReleaseAllowed = false;	// Erlaubnis zum Lösen nicht mehr notwändig

		// Bremsen erwartet wenn
		if (!WinchBrakeInternAuto_StoppExpected &&
			(HV_CTL_Sig < HV_CTL_DwnThresh) &&          // Steuerungsspannung im aktuellen Bremsbereich war
			(HV_CTL_Sig > HV_CTL_UpThresh) &&
			(V->GetControl_AutoRunOn() == _OFF)) {      // keine Automatik aktiv ist

			WinchBrakeInternAuto_StoppExpected = true;
			IsTimeElapsed(&WinchBrakeInternAuto_StoppExpect_elapsed, &Countdown, WINCHBRAKEINTERNAUTO_VELO_HLDLNGTH, 0, true); // Triggern der Abwartezeit
		}

		// Wird in Vorwärtsfahrt ausgelenkt, dann wird Bremsen nicht erwartet
		if (HV_CTL_Sig > HV_CTL_DwnThresh) WinchBrakeInternAuto_StoppExpected = false;
	}

	// ---
	// Bremse lösen
	if (WinchBrakeInternAuto_ReleaseAllowed &&
		((HV_CTL_Sig > HV_CTL_DwnThresh) ||
		 (HV_CTL_Sig < HV_CTL_UpThresh)
		)
	   ) {
	   HV_BrakeRelease = true;
	}

	// ---
	// Bremse anziehen
	if (WinchBrakeInternAuto_StoppExpected){
		if (V->GetInfo_CurrVelocity() < WINCHBRAKEINTERNAUTO_VELOCITY_THRES) {
			if (IsTimeElapsed(&WinchBrakeInternAuto_StoppExpect_elapsed, &Countdown, WINCHBRAKEINTERNAUTO_VELO_HLDLNGTH, 0, false)) { // Geschwindigkeitsschwelle muss eine festgelegte Zeit unterschritten sein, ehe reagiert wird
				WinchBrakeInternAuto_StartingAidRefPos = V->GetInfo_CurrWinchRevMessVoltage();
				HV_BrakeFixate = true;
			}
		} else IsTimeElapsed(&WinchBrakeInternAuto_StoppExpect_elapsed, &Countdown, WINCHBRAKEINTERNAUTO_VELO_HLDLNGTH, 0, true); // Triggern der Abwartezeit
	}

	// Notausschalter und Windenfreigabe
	// dominieren die Automatische Bremse,
	// -----------------------------------------------------------
	if ((V->GetInfo_EmergencySwitch() == _ON) ||
	   (V->GetControl_WinchWork() == _OFF) ||
	   FirstRun) {   // FirstRun nur zur Sicherheit, theoretisch sollte das nicht notwändig sein, da Programmstart nie mit gelöster Bremse stattfindet
	   HV_BrakeFixate = true;
	}

	// Bremsbetätigung ausführen
	// -----------------------------------------------------------
	if (HV_BrakeRelease) {
		IOControl.DOPortOn(DIOReleaseBrakeNUM);
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] |= 0x40;
		WinchBrakeInternAuto_Released = true;
	}

	if (HV_BrakeFixate) { // Feststellen wird nach Lösen ausgeführt, so wird sichergestellt, dass versehentliches Lösen rückgängig gemacht wird
		IOControl.DOPortOff(DIOReleaseBrakeNUM);
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] &= ~0x40;
		WinchBrakeInternAuto_Released = false;
	}
}

//---------------------------------------------------------------------------
// Steuerung der Windenbremse
void WCAnalysis::ActWinchBrakeOff()
{
	bool HV_BrakeFixate = false;
	bool HV_BrakeRelease = false;

	// Impuls zur Bremsensteuerung erfolgt ?
	// ---------------------------------------------
	if (V->GetControl_WinchBrakeOffImpulse() == _ON) {

		V->SetControl_WinchBrakeOffImpulse_Off();

		if (V->GetControl_WinchBrakeOff() == _ON) HV_BrakeFixate = true;		// Bremse anziehen
		else HV_BrakeRelease = true;                                      		// Bremse lösen
	}

	// Bei Kabel entladen (Zugkraftreglung) muss Bremse gelöst sein.
	// Das geschieht bereits, wenn die Zugkraft noch nicht aktiv geregelt wird
	// Die Bremse muss dann manuell wieder angezogen werden
	if ((V->GetControl_UnloadCable() == _ON) &&
		(V->GetControl_WinchBrakeOff() == _OFF)) HV_BrakeRelease = true;

	// Notausschalter und Windenfreigabe
	// dominieren alle Zustände und Nutzereingaben
	// -----------------------------------------------------------
	if ((V->GetInfo_EmergencySwitch() == _ON) ||
	   (V->GetControl_WinchWork() == _OFF)){
		if (V->GetControl_WinchBrakeOff() ==_ON) {
			HV_BrakeFixate = true;			// Bremse anziehen
			HV_BrakeRelease = false;
        }
	}

	// Ausführen des Umschaltens
	// ---------------------------------------------
	// Bremse lösen
	if (HV_BrakeRelease) {
		IOControl.DOPortOn(DIOReleaseBrakeNUM);
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] |= 0x40;
		V->SetControl_WinchBrakeOff_On();
		WinchBrakeInternAuto_Released = true;   // zur Übergabe an Routine für interne Automatikbremse
		WinchBrakeInternAuto_StoppExpected = false;
	}

	// Bremse anziehen
	if (HV_BrakeFixate) {
		// Ist die interne Automatikbremse aktiviert,
		// dann steuert diese Funktion das reale Anziehen der Bremse
		// und es wird hier nicht ausgeführt
		if (V->GetParam_WinchBrakeInternAuto() == _OFF) {
			IOControl.DOPortOff(DIOReleaseBrakeNUM);
			CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] &= ~0x40;
			WinchBrakeInternAuto_Released = false;
		}
		V->SetControl_WinchBrakeOff_Off();
	}
};

//---------------------------------------------------------------------------
// Ansteuerung für Ventile ein
void WCAnalysis::ActController()
{
	long HV_clock;

	// Hallo über Signal WinchClear
	// -----------------------------------------------------------------
	// Signal Triggern
	if (V->GetControl_Hello_WinchClear()) {
		V->SetControl_Hello_WinchClearOff();
		IsTimeElapsed(&ActController_Hello_WinchClear_StartTime, &HV_clock, HELLO_WINCHCLEAR_DURATION, 0, true);
        ActController_Hello_WinchClear_Active = true;
	}

	if (ActController_Hello_WinchClear_Active) {
		if (IsTimeElapsed(&ActController_Hello_WinchClear_StartTime, &HV_clock, HELLO_WINCHCLEAR_DURATION, 0, false)) {
			ActController_Hello_WinchClear_Active = false;
		}
	}
	// -----------------------------------------------------------------

	// Schwellwerte und andere Zustände Prüfen und bei Bedarf Fahrt anhalten
	if ((
			ActController_Hello_WinchClear_Active ||
			(V->GetControl_UnloadCable() == _ON) ||
			(V->GetControl_ThresholdExceedenceStopDelay() == _ON) ||
			(
				(
					(V->GetInfo_DepthThresholdExceeded()            == _OFF) ||
					(V->GetControl_ClearAfterThresholdDepth()       == _ON)  ||
					(V->GetControl_AutoRunOn()                      == _ON)
				) &&
				(
					(V->GetInfo_TractionThresholdExceeded()         == _OFF) ||
					(V->GetControl_ClearAfterThresholdTraction()    == _ON)  ||
					(V->GetControl_AutoRunOn()                      == _ON)
				)
			)
		) &&
		(V->GetInfo_SpecialSignalExtBrake() ||
		 (V->GetParam_ExternalBrakeAvailable() == _OFF)
		)
	   ) {
		IOControl.DOPortOn(DIOControllerNUM);
		V->SetControl_WinchOn();
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] |= 0x20;

	   } else {

		IOControl.DOPortOff(DIOControllerNUM);
		V->SetControl_WinchOff();
		CtlUnitCAN_MsgBuf.Msg_0x00000102.CANMsg.DATA[0] &= ~0x20;

	   };
};

//---------------------------------------------------------------------------
// Liest Hardware und Werte aus, analysiert Daten, setzt Werte steuert Hardware
// Hauptverwaltung
void WCAnalysis::Analyze()
{
        if (V == 0) return;                                                     // wenn keine Werte gelesen oder gesetzt werden können

#ifdef __USE_THREAD_CLOCK
        // --------------------
        // Takt
        GetInternalClock();
#endif

		// Verwaltung
		CtlUnitCAN_StatusLevelCtl();

        // --------------------
        // einfache Informationsbeschaffung
		GetVoltages();  // Spannungen sollten immer vor der Auswertung dieser gelesen werden
        GetDI();        // Stati aller digitaler Eingänge vonder Hardware einlesen

		GetCtlUnitCAN_I();

        GetCtlUnitErrorMsg();

        GetHydraulicStatus();
        GetHydrChargePressure();
    	GetHydrLvl();
		GetHydrWrkPressure();
		GetDepth();
		GetCtlUnitSWVersion();
		GetDepthCalcSWVersion();
		GetDepthParam();
		GetDirection();
        GetEmergencySwitch();
        GetEngineInfo();
#ifndef __IGNORE_INCENCERROR
        GetIncEncError();
#endif
        GetControllerError();
        GetLeverState();
        GetLostMotion();
        GetOilStrainer();
		GetPowerFail();
        GetSpecialSignals();
		GetTemperature();
		GetTension();
        GetDynTensionLimits();
		GetCurrWinchRevMessVoltage();
        GetThresholdExceedance();
        GetVelocity();

    	// Alle empfangenen CAN-Nachrichten sollten hier abgearbeitet sein
		CtlUnitCAN_MsgBuf.Msg_0x00000201.Modified = false;
		CtlUnitCAN_MsgBuf.Msg_0x00000204.Modified = false;
		CtlUnitCAN_MsgBuf.Msg_0x00000301.Modified = false;
		CtlUnitCAN_MsgBuf.Msg_0x00000302.Modified = false;
		CtlUnitCAN_MsgBuf.Msg_0x00000303.Modified = false;
		CtlUnitCAN_MsgBuf.Msg_0x00000304.Modified = false;
		CtlUnitCAN_MsgBuf.Msg_0x00000305.Modified = false;

		ActMMSyncForce(); // Ausführung an dieser Stelle, damit zwischen Ereigniserkennung und TR-Aktualisierung möglichst keine Zeit vergeht

		// --------------------
        // Spezielle Datenübertragung
		SetDepthParam();		// vor Aufruf von "SetDepthCalcParamm()", damit Flags nicht zurückgesetzt werden
		SetDepthCalcParamm();

        // --------------------
        // Sperre
        SetLockStates();

        // --------------------
        // Programmmodus setzen
        SetProgramMode();

        // Auswertung der "IST"-Zustände und Steuerung
		ActCtlUnitCANIdent();
		ActPowerLock();
        ActSignalHorn();
        ActSpecialFunc();
		ActAutomatic();
        ActPrgAutomatic();
        ActEngineStart();
        ActEngineStop();
		ActEngineRevolution();
        ActErrorReactions();
        ActExtInfoOut();
        ActFastRun();
        ActHydraulicBrake();
		ActController();
        ActWinchBrakeInternAuto();
		ActWinchBrakeOff();  // erst WinchBrakeOff bearbeiten, dann UnloadCable, dann LostMotion
        ActUnloadCable();
		ActLostMotion();
		ActCalibrationDepth();
		ActCalibrationTension();
		ActSpecialSignals();
		ActTensionOut_KEDR();

		ActCtlUnitCAN_O(); // CAN-Botschaften senden

		ActVoltages();
        ActDO();

        FirstRun = false;
};


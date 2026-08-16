/*
WinchControl    Jörg Müller     WCIO.cpp

Klassenimplementation Ein-/Ausgabe-Klasse

Portierbarkeit: nein
*/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WCIO.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
WCIO::WCIO()
{
        Init();
};

//---------------------------------------------------------------------------
WCIO::~WCIO()
{
};

//---------------------------------------------------------------------------
// VERWALTUNG PRIVATE
//---------------------------------------------------------------------------
// Öffnet die angegebene serielle Schnittstelle
// Rückgabewert ist das ermittelte SS-Handle
HANDLE WCIO::OpenCOMHandle(HANDLE hComm, char *Port, _COMMTIMEOUTS *newctmo, _DCB *newdcb, _COMMTIMEOUTS *oldctmo, _DCB *olddcb)
{
		// evtl. geöffnette Schnittstelle schließen
		CloseCOMHandle(hComm, 0, 0);

		try
		{
				// Schnittstelle öffnen
				hComm = CreateFile(Port,
								   GENERIC_READ | GENERIC_WRITE,
								   0,
								   0,
								   OPEN_EXISTING,
								   0,
								   0);

		}catch(...)
        {
                hComm = INVALID_HANDLE_VALUE;
                return hComm;
        };

        if (hComm != INVALID_HANDLE_VALUE)
        {
               // Alte Parameter sichern
               GetCommTimeouts(hComm, oldctmo);
               GetCommState(hComm, olddcb);

                SetCommTimeouts(hComm, newctmo);                                // Timeout setzen
               SetCommState(hComm, newdcb);                                     // Steuer-Parameter setzen
        };

        return hComm;
};

//---------------------------------------------------------------------------
// Schließt die angegebene serielle Schnittstelle
// oldctmo und olddcb werden vor dem Freigeben des Handles gesetzt, so kann ein
// vorheriger Zustand wieder hergestellt werden
// oldctmo und olddcb können auch auf 0 Zeigen und werden in diesem Falle
// ignoriert.
void WCIO::CloseCOMHandle(HANDLE hComm, _COMMTIMEOUTS *oldctmo, _DCB *olddcb)
{
        try
        {
                if (hComm != INVALID_HANDLE_VALUE)
                {
                        // Alte Parameter wieder setzen
                        try
                        {
                                if (oldctmo != 0) SetCommTimeouts(hComm, oldctmo);
                        }catch(...){};
                        try
                        {
                                if (olddcb != 0) SetCommState(hComm, olddcb);
                        }catch(...){};

                        // Input-Puffer löschen, falls vorhanden
                        PurgeComm(hComm, PURGE_RXABORT);

                        CloseHandle(hComm);
                };

        }catch(...){};

        hComm = INVALID_HANDLE_VALUE;
};

//---------------------------------------------------------------------------
// VERWALTUNG
//---------------------------------------------------------------------------
// Initialisiert die Klasse
// Wird bei dynamischer Speicherallokierung vom Konstruktor aufgerufen.
// Deshalb braucht in diesem Fall init nicht explizit aufgerufen werden.
void WCIO::Init()
{
//-------------------------------
#ifndef DAQNAVI_ARCH
		// Spannungseingang-Pufferinitialisierung
		for (int i = 0; i < (sizeof(AINVoltageBuffer) / sizeof(AINVoltageBuffer[0])); ++i) AINVoltageBuffer[i] = 0;

		// Spannungsausgang-Pufferinitialisierung
		for (int i = 0; i < (sizeof(AOutVoltageBuffer) / sizeof(AOutVoltageBuffer[0])); ++i) AOutVoltageBuffer[i] = 0;

		// Digitaleingangs-Pufferinitialisierung
		for (int i = 0; i < (sizeof(DIStatusBuffer) / sizeof(DIStatusBuffer[0])); ++i) DIStatusBuffer[i] = false;

		// Digitalausgangs-Pufferinitialisierung
		for (int i = 0; i < (sizeof(DOSetBuffer) / sizeof(DOSetBuffer[0])); ++i) DOSetBuffer[i] = false;
#endif //DAQNAVI_ARCH
//-------------------------------
		InfoOutH = INVALID_HANDLE_VALUE;
		InfoOutOk = false;

		DepthCalcH = INVALID_HANDLE_VALUE;
		DepthCalcOk = false;

//-------------------------------
#ifndef DAQNAVI_ARCH
		PCI1762_VarInit();                                                      // Initialisieren der Variablen und Handles für Advantech PCI-1762
		PCI1716_VarInit();                                                      // Initialisieren der Variablen und Handles für Advantech PCI-1716
#endif //DAQNAVI_ARCH
//-------------------------------

};

// Initialisieren der Schnittstelle für Informationsausgabe
void WCIO::InitInfoCOM(WCGenOrganisationS *GenOrgan)
{
		// Prüfen, ob Zeiger-Wert i.O. ist
		if (GenOrgan == 0) return;
		try
        {
                GenOrgan->CurrFunctionPanel;                                    // wenn's funktioniert, zeigt Zeiger auch auf gült. Struktur
        }catch(...)
        {
                return;
        };

        // Parameter der Schnittstelle einstellen
        // COM - Port initialisieren
        InfoOutctmoNew.ReadIntervalTimeout = __HW_InfoOut_ReadTOInt;     // Timeout - Einstellungen
        InfoOutctmoNew.ReadTotalTimeoutConstant = __HW_InfoOut_ReadTOConst;
        InfoOutctmoNew.ReadTotalTimeoutMultiplier = 0;
        InfoOutctmoNew.WriteTotalTimeoutMultiplier = __HW_InfoOut_WriteTOConst;
        InfoOutctmoNew.WriteTotalTimeoutConstant = 0;

        // Steuer-Einstellung der SS
        BuildCommDCB("baud=38400 parity=N data=8 stop=1", &InfoOutdcbCPortNew); // Dummy-DCB, damit alle Werte vordefiniert werden
        InfoOutdcbCPortNew.BaudRate = GenOrgan->InfoOut_Baud;
        InfoOutdcbCPortNew.ByteSize = GenOrgan->InfoOut_DataBit;
        InfoOutdcbCPortNew.StopBits = GenOrgan->InfoOut_StopBit;
        InfoOutdcbCPortNew.Parity = GenOrgan->InfoOut_Parity;

        // Schnittstelle zum Teufenrechner öffnen
        InfoOutH = OpenCOMHandle(InfoOutH, GenOrgan->InfoOut_Port, &InfoOutctmoNew, &InfoOutdcbCPortNew, &InfoOutctmoOld, &InfoOutdcbCPortOld);
        if (InfoOutH != INVALID_HANDLE_VALUE)
        {
				GenOrgan->Evnt >> 3;											// Fehler 3 aus
				InfoOutOk = true;
		}else
		{
				GenOrgan->Evnt << 3;											// Fehler 3 ein
                InfoOutOk = false;
        };
};

//---------------------------------------------------------------------------
// Initialisieren der Schnittstelle zum Teufenrechner
void WCIO::InitDepthCalcCOM(WCGenOrganisationS *GenOrgan)
{
        // Prüfen, ob Zeiger-Wert i.O. ist
        if (GenOrgan == 0) return;
        try
        {
                GenOrgan->CurrFunctionPanel;                                    // wenn's funktioniert, zeigt Zeiger auch auf gült. Struktur
        }catch(...)
        {
                return;
        };

        // Parameter der Schnittstelle einstellen
        // COM - Port initialisieren
        DepthCalcctmoNew.ReadIntervalTimeout = __HW_DepthCalc_ReadTOInt;        // Timeout - Einstellungen
        DepthCalcctmoNew.ReadTotalTimeoutConstant = __HW_DepthCalc_ReadTOConst;
        DepthCalcctmoNew.ReadTotalTimeoutMultiplier = 0;
        DepthCalcctmoNew.WriteTotalTimeoutMultiplier = __HW_DepthCalc_WriteTOConst;
        DepthCalcctmoNew.WriteTotalTimeoutConstant = 0;

        // Steuer-Einstellung der SS
        BuildCommDCB("baud=38400 parity=N data=8 stop=1", &DepthCalcdcbCPort_New); // Dummy-DCB, damit alle Werte vordefiniert werden
        DepthCalcdcbCPort_New.BaudRate = GenOrgan->DepthCalc_Baud;
        DepthCalcdcbCPort_New.ByteSize = GenOrgan->DepthCalc_DataBit;
        DepthCalcdcbCPort_New.StopBits = GenOrgan->DepthCalc_StopBit;
        DepthCalcdcbCPort_New.Parity = GenOrgan->DepthCalc_Parity;

        // Schnittstelle zum Teufenrechner öffnen
        DepthCalcH = OpenCOMHandle(DepthCalcH, GenOrgan->DepthCalc_Port, &DepthCalcctmoNew, &DepthCalcdcbCPort_New, &DepthCalcctmoOld, &DepthCalcdcbCPort_Old);
        if (DepthCalcH != INVALID_HANDLE_VALUE)
        {
				GenOrgan->Evnt >> 2;											// Fehler 2 aus
				DepthCalcOk = true;
		}else
		{
				GenOrgan->Evnt << 2;											// Fehler 2 ein
                DepthCalcOk = true;
        };

        DepthCalc.SetHandle(DepthCalcH);
};

//---------------------------------------------------------------------------
// Initialisieren der DIO-Relay-Card
void WCIO::InitDIOCard(WCGenOrganisationS *GenOrgan)
{
//        short PCI1716Error_MSG, PCI1762Error_MSG;
        bool PCI1716_located = false;
        bool PCI1762_located = false;

//-------------------------------
#ifdef DAQNAVI_ARCH
		if (PCI1716.DeviceCnt() > 0) PCI1716_located = true;
#ifdef __ENABLE_ADV_PCI1762
		if (PCI1762.DeviceCnt() > 0) PCI1762_located = true;
#endif // __ENABLE_ADV_PCI1762
#endif // DAQNAVI_ARCH
//-------------------------------
#ifndef DAQNAVI_ARCH
		// Benötigte Karten lokalisieren
		PCI1716_located = PCI1716_CardFind();
#ifdef __ENABLE_ADV_PCI1762
		PCI1762_located = PCI1762_CardFind();
#endif //  __ENABLE_ADV_PCI1762
#endif // DAQNAVI_ARCH
//-------------------------------

		if (PCI1716_located) GenOrgan->Evnt >> 5;								// Fehler 5 aus
		else GenOrgan->Evnt << 5;												// Fehler 5 ein

#ifdef __ENABLE_ADV_PCI1762
		if (PCI1762_located) GenOrgan->Evnt >> 4;								// Fehler 4 aus
		else GenOrgan->Evnt << 4;												// Fehler 4 ein
#endif //  __ENABLE_ADV_PCI1762

//-------------------------------
#ifdef DAQNAVI_ARCH
		// Kartenzugang öffnen
		if (PCI1716_located) {
			if (PCI1716.Open(0)) GenOrgan->Evnt >> 7;							// Fehler 7 aus
			else GenOrgan->Evnt << 7;											// Fehler 7 ein
		}

#ifdef __ENABLE_ADV_PCI1762
		// Kartenzugang öffnen
		if (PCI1762_located) {
			if (PCI1762.Open(0)) GenOrgan->Evnt >> 6;							// Fehler 6 aus
			else GenOrgan->Evnt << 6;											// Fehler 6 ein
		}
#endif //  __ENABLE_ADV_PCI1762

#endif // DAQNAVI_ARCH
//-------------------------------
#ifndef DAQNAVI_ARCH
		// Kartenzugang öffnen
		if (PCI1716_located) {
			if (PCI1716_CardOpen()) GenOrgan->Evnt >> 7;						// Fehler 7 aus
			else GenOrgan->Evnt << 7;											// Fehler 7 ein
		}

#ifdef __ENABLE_ADV_PCI1762
		// Kartenzugang öffnen
		if (PCI1762_located) {
			if (PCI1762_CardOpen()) GenOrgan->Evnt >> 6;						// Fehler 6 aus
			else GenOrgan->Evnt << 6;											// Fehler 6 ein
		}
#endif //  __ENABLE_ADV_PCI1762

#endif // DAQNAVI_ARCH
//-------------------------------
		ConfigDIODAC(GenOrgan);

};

//---------------------------------------------------------------------------
// Konfiguriert die Analogkarte neu
// Rückgabewert: true bei Erfolg, false bei Fehler
bool WCIO::ConfigDIODAC(WCGenOrganisationS *GenOrgan)
{
//-------------------------------
#ifndef DAQNAVI_ARCH
		short           PCI1716_AInGains[8];
		unsigned short  PCI1716_AOUT_Channels[2];
		unsigned short  PCI1716_AOUT_RefSrc[2];
		float           PCI1716_AOUT_MinValue[2];
		float           PCI1716_AOUT_MaxValue[2];

		// PCI-1716 initialisiert
		for (int i = 0; i < 8; ++i) PCI1716_AInGains[i] = 0;
		PCI1716_AInGains[GenOrgan->AI_TractionChannel] = GenOrgan->AI_TractionGain;
		PCI1716_AInGains[GenOrgan->AI_WinchRevMessChannel] = GenOrgan->AI_WinchRevMessGain;
		PCI1716_AInGains[GenOrgan->AI_TemperatureDepthCalcChannel] = GenOrgan->AI_TemperatureDepthCalcGain;
		PCI1716_AInGains[GenOrgan->AI_TemperatureAChannel] = GenOrgan->AI_TemperatureAGain;
		PCI1716_AInGains[GenOrgan->AI_TemperatureBChannel] = GenOrgan->AI_TemperatureBGain;

		// Windenansteuerung Geschwindigkeit
		PCI1716_AOUT_Channels[0] = GenOrgan->AO_WinchRevChannel;
		PCI1716_AOUT_RefSrc[0]   = GenOrgan->AO_WinchRevReference;
		PCI1716_AOUT_MinValue[0] = GenOrgan->AO_WinchRevCfgMinVoltage / 1000;
		PCI1716_AOUT_MaxValue[0] = GenOrgan->AO_WinchRevCfgMaxVoltage / 1000;

		// Ausgabe Zugkraftwert nach Definition
		PCI1716_AOUT_Channels[1] = GenOrgan->AO_TensionOutChannel;
		PCI1716_AOUT_RefSrc[1]   = GenOrgan->AO_TensionOutReference;
		PCI1716_AOUT_MinValue[1] = GenOrgan->AO_TensionOutCfgMinVoltage / 1000;
		PCI1716_AOUT_MaxValue[1] = GenOrgan->AO_TensionOutCfgMaxVoltage / 1000;

		if (!PCI1716_Config(PCI1716_AInGains, 8, PCI1716_AOUT_Channels, PCI1716_AOUT_RefSrc, PCI1716_AOUT_MinValue, PCI1716_AOUT_MaxValue, 2)) return false;
		return true;
#endif // DAQNAVI_ARCH
//-------------------------------

//-------------------------------
#ifdef DAQNAVI_ARCH
		int	 HVAIOCfgRange;
		bool HVError = false;

		// Ausgabe Zugkraftwert nach Definition
		switch (GenOrgan->AO_DAQNavi_TensionOutRange) {
			case 0:	HVAIOCfgRange = V_0To5; break;
			case 1: HVAIOCfgRange = V_0To10; break;
			case 2: HVAIOCfgRange = V_Neg5To5; break;
			case 3: ;
		default:
			HVAIOCfgRange = V_Neg10To10;
		}
		if (!PCI1716.SetAOutVRange(GenOrgan->AO_TensionOutChannel, HVAIOCfgRange)) {
			HVError = true;
		}

		// Windenansteuerung Geschwindigkeit
		switch (GenOrgan->AO_DAQNavi_WinchRevCfgRange) {
			case 0:	HVAIOCfgRange = V_0To5; break;
			case 1: HVAIOCfgRange = V_0To10; break;
			case 2: HVAIOCfgRange = V_Neg5To5; break;
			case 3: ;
		default:
			HVAIOCfgRange = V_Neg10To10;
		}
		if (!PCI1716.SetAOutVRange(GenOrgan->AO_WinchRevChannel, HVAIOCfgRange)) {
			HVError = true;
		}

		// Zugkraftsignal
		switch (GenOrgan->AI_DAQNavi_TensionRange) {
			case 0: HVAIOCfgRange = V_0To5; break;
			case 1: HVAIOCfgRange = V_0To10; break;
			case 2: HVAIOCfgRange = V_Neg5To5; break;
			case 4: HVAIOCfgRange = V_0To2pt5; break;
			case 5: HVAIOCfgRange = V_Neg2pt5To2pt5; break;
			case 3: ;
		default:
			HVAIOCfgRange = V_Neg10To10;
		}
		if (!PCI1716.SetAInVRange(GenOrgan->AI_TractionChannel, HVAIOCfgRange)) {
        	HVError = true;
		}

		// Windensteuerungsspannung (Messung vom HSG)
		switch (GenOrgan->AI_DAQNavi_WinchRevMessRange) {
			case 0: HVAIOCfgRange = V_0To5; break;
			case 1: HVAIOCfgRange = V_0To10; break;
			case 2: HVAIOCfgRange = V_Neg5To5; break;
			case 4: HVAIOCfgRange = V_0To2pt5; break;
			case 5: HVAIOCfgRange = V_Neg2pt5To2pt5; break;
			case 3: ;
		default:
			HVAIOCfgRange = V_Neg10To10;
		}
		if (!PCI1716.SetAInVRange(GenOrgan->AI_WinchRevMessChannel, HVAIOCfgRange)) {
			HVError = true;
		}

		// Temperaturmessung TR
		switch (GenOrgan->AI_DAQNavi_TemperatureDepthCalcRange) {
			case 0: HVAIOCfgRange = V_0To5; break;
			case 1: HVAIOCfgRange = V_0To10; break;
			case 2: HVAIOCfgRange = V_Neg5To5; break;
			case 4: HVAIOCfgRange = V_0To2pt5; break;
			case 5: HVAIOCfgRange = V_Neg2pt5To2pt5; break;
			case 3: ;
		default:
			HVAIOCfgRange = V_Neg10To10;
		}
		if (!PCI1716.SetAInVRange(GenOrgan->AI_TemperatureDepthCalcChannel, HVAIOCfgRange)) {
			HVError = true;
		}

		// Temperaturmessung Kanal A
		switch (GenOrgan->AI_DAQNavi_TemperatureARange) {
			case 0: HVAIOCfgRange = V_0To5; break;
			case 1: HVAIOCfgRange = V_0To10; break;
			case 2: HVAIOCfgRange = V_Neg5To5; break;
			case 4: HVAIOCfgRange = V_0To2pt5; break;
			case 5: HVAIOCfgRange = V_Neg2pt5To2pt5; break;
			case 3: ;
		default:
			HVAIOCfgRange = V_Neg10To10;
		}
		if (!PCI1716.SetAInVRange(GenOrgan->AI_TemperatureAChannel, HVAIOCfgRange)) {
			HVError = true;
		}

		// Temperaturmessung Kanal B
		switch (GenOrgan->AI_DAQNavi_TemperatureBRange) {
			case 0: HVAIOCfgRange = V_0To5; break;
			case 1: HVAIOCfgRange = V_0To10; break;
			case 2: HVAIOCfgRange = V_Neg5To5; break;
			case 4: HVAIOCfgRange = V_0To2pt5; break;
			case 5: HVAIOCfgRange = V_Neg2pt5To2pt5; break;
			case 3: ;
		default:
			HVAIOCfgRange = V_Neg10To10;
		}
		if (!PCI1716.SetAInVRange(GenOrgan->AI_TemperatureBChannel, HVAIOCfgRange)) {
			HVError = true;
		}

	return HVError;
#endif // DAQNAVI_ARCH
};

//---------------------------------------------------------------------------
// Aufräumen
void WCIO::CleanUp()
{
        // Exakte Aktuelle Teufe in TR-CMOS-RAM schreiben
        DepthCalc.SaveDepthToCMOS();

        // InfoCOM freigeben
        CloseCOMHandle(InfoOutH, &InfoOutctmoOld, &InfoOutdcbCPortOld);
        InfoOutOk = false;

        // Port zum Teufenrechner wieder freigeben
        CloseCOMHandle(DepthCalcH, &DepthCalcctmoOld, &DepthCalcdcbCPort_Old);
        DepthCalcOk = false;

//-------------------------------
#ifdef DAQNAVI_ARCH
	PCI1716.Close();

#ifdef __ENABLE_ADV_PCI1762
	PCI1762.Close();
#endif //  __ENABLE_ADV_PCI1762

#endif DAQNAVI_ARCH
//-------------------------------
#ifndef DAQNAVI_ARCH
		PCI1762_Close();
		PCI1716_Close();
#endif DAQNAVI_ARCH
//-------------------------------

};

//---------------------------------------------------------------------------
// HARDWARE ZUGRIFF
//---------------------------------------------------------------------------

// TEUFENRECHNER
//---------------------------------------------------------------------------
// Fragt die aktuelle Teufe vom Teufenrechner ab
int WCIO::GetDepth(bool *MagneticMark, bool *MagneticMarkDirect, int *TimeSinceLastQuery, bool *Error)
{
//		if (!DepthCalcOk) Application->ProcessMessages(); // zur Geschwindigkeitsreduzierung der Threads, wenn COM-Port Zugriff wegfällt, damit Anwendung bedienbar bleibt

#ifndef __USE_GET_DEPTH_EXTENDED
        // *TimeSinceLastQuery bleibt unangetastet
        *MagneticMarkDirect = false;                                             // Element wir nicht verwendet
        return DepthCalc.GetDepth(MagneticMark, Error);
#endif

#ifdef __USE_GET_DEPTH_EXTENDED
		return DepthCalc.GetDepthExtended(MagneticMark, MagneticMarkDirect, TimeSinceLastQuery, Error);
#endif
};

//---------------------------------------------------------------------------
// Software-Version des Teufenrechners ermitteln
bool WCIO::GetDepthCalcSWVersion(char *Version, int VersionLength)
{
		bool OK;

        // Da Teufenrechner für diese Operation länger braucht, muss hier
        // vorübergehend ein größerer Timeout eingeräumt werden
        int tmpReadTimeoutInterval = DepthCalcctmoNew.ReadIntervalTimeout;      // momentane Werte merken
        int tmpReadTimeoutConstant = DepthCalcctmoNew.ReadTotalTimeoutConstant;

        DepthCalcctmoNew.ReadIntervalTimeout = __HW_DepthCalc_Long_ReadTOConst;
        DepthCalcctmoNew.ReadTotalTimeoutConstant =  __HW_DepthCalc_Long_ReadTOConst;

        SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout auf voherigen Wert zurücksetzen

        OK = DepthCalc.GetSWVersion(Version, VersionLength);

        DepthCalcctmoNew.ReadIntervalTimeout = tmpReadTimeoutInterval;          // voherige Werte wieder aufnehmen
        DepthCalcctmoNew.ReadTotalTimeoutConstant = tmpReadTimeoutConstant;

        SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout auf voherigen Wert zurücksetzen

        return OK;
};


//---------------------------------------------------------------------------
// explizit Eichfaktor setzen
bool WCIO::SetDepthCalcCalFact(int CalFactor)
{
		bool Ok = true;

        // Da Teufenrechner für diese Operation länger braucht, muss hier
        // vorübergehend ein größerer Timeout eingeräumt werden
        int tmpReadTimeoutInterval = DepthCalcctmoNew.ReadIntervalTimeout;      // momentane Werte merken
        int tmpReadTimeoutConstant = DepthCalcctmoNew.ReadTotalTimeoutConstant;

        DepthCalcctmoNew.ReadIntervalTimeout = __HW_DepthCalc_Long_ReadTOConst;
        DepthCalcctmoNew.ReadTotalTimeoutConstant =  __HW_DepthCalc_Long_ReadTOConst;

        SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout setzen

        DepthCalcctmoNew.ReadIntervalTimeout = tmpReadTimeoutInterval;          // voherige Werte wieder aufnehmen
        DepthCalcctmoNew.ReadTotalTimeoutConstant = tmpReadTimeoutConstant;

//        Ok = DepthCalc.SetCalibFactor(CalFactor);
        Ok = DepthCalc.SetCalibFactor2(CalFactor);                              // 23.01.2006 Setzen des Eichfaktors, ohne Beeinflussung des aktuellen Teufenwertes

        SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout auf voherigen Wert zurücksetzen

        return Ok;
};

//---------------------------------------------------------------------------
// explizit Teufe setzen
bool WCIO::SetDepthCalcDepth(int Depth)
{
		bool Ok = true;

        // Da Teufenrechner für diese Operation länger braucht, muss hier
        // vorübergehend ein größerer Timeout eingeräumt werden
        int tmpReadTimeoutInterval = DepthCalcctmoNew.ReadIntervalTimeout;      // momentane Werte merken
        int tmpReadTimeoutConstant = DepthCalcctmoNew.ReadTotalTimeoutConstant;

        DepthCalcctmoNew.ReadIntervalTimeout = __HW_DepthCalc_Long_ReadTOConst;
        DepthCalcctmoNew.ReadTotalTimeoutConstant =  __HW_DepthCalc_Long_ReadTOConst;

        SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout setzen

        DepthCalcctmoNew.ReadIntervalTimeout = tmpReadTimeoutInterval;          // voherige Werte wieder aufnehmen
        DepthCalcctmoNew.ReadTotalTimeoutConstant = tmpReadTimeoutConstant;

        Ok = DepthCalc.SetStartDepth(Depth);

        SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout auf voherigen Wert zurücksetzen

        return Ok;
};

//---------------------------------------------------------------------------
// explizit Magnetmarke setzen
bool WCIO::SetDepthCalcMagneticMark(bool MagneticMark)
{
		bool Ok = true;

		// Da Teufenrechner für diese Operation länger braucht, muss hier
		// vorübergehend ein größerer Timeout eingeräumt werden
		int tmpReadTimeoutInterval = DepthCalcctmoNew.ReadIntervalTimeout;      // momentane Werte merken
		int tmpReadTimeoutConstant = DepthCalcctmoNew.ReadTotalTimeoutConstant;

		DepthCalcctmoNew.ReadIntervalTimeout = __HW_DepthCalc_Long_ReadTOConst;
		DepthCalcctmoNew.ReadTotalTimeoutConstant =  __HW_DepthCalc_Long_ReadTOConst;

		SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout setzen

		DepthCalcctmoNew.ReadIntervalTimeout = tmpReadTimeoutInterval;          // voherige Werte wieder aufnehmen
		DepthCalcctmoNew.ReadTotalTimeoutConstant = tmpReadTimeoutConstant;

		if (MagneticMark == _ON) Ok = DepthCalc.ActivateMagneticMark();
		else Ok = DepthCalc.DeactivateMagneticMark();

		SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout auf voherigen Wert zurücksetzen

		return Ok;
};

//---------------------------------------------------------------------------
// Konfiguriert den Teufenrechner
bool WCIO::SetDepthCalcParam(WCVarControl *V)
{
		bool Ok = true;

		// Da Teufenrechner für diese Operation länger braucht, muss hier
		// vorübergehend ein größerer Timeout eingeräumt werden
		int tmpReadTimeoutInterval = DepthCalcctmoNew.ReadIntervalTimeout;      // momentane Werte merken
		int tmpReadTimeoutConstant = DepthCalcctmoNew.ReadTotalTimeoutConstant;

		DepthCalcctmoNew.ReadIntervalTimeout = __HW_DepthCalc_Long_ReadTOConst;
		DepthCalcctmoNew.ReadTotalTimeoutConstant =  __HW_DepthCalc_Long_ReadTOConst;

		SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout setzen

		DepthCalcctmoNew.ReadIntervalTimeout = tmpReadTimeoutInterval;          // voherige Werte wieder aufnehmen
		DepthCalcctmoNew.ReadTotalTimeoutConstant = tmpReadTimeoutConstant;

		// --- Startteufe setzen erwünscht ?
		if (V->GetControl_SetDepth_DC() == _ON) {

			// Wenn Teufenumkehrung aktiv, dann muss die Startteufe auch umgekehrt an den TR gesendet werden
			if (V->GetParam_ReverseDepth() == _ON) Ok = DepthCalc.SetStartDepth(V->GetParam_StartDepth() * -1);
			else Ok = DepthCalc.SetStartDepth(V->GetParam_StartDepth());

			if (!Ok) {
				SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout auf voherigen Wert zurücksetzen
				return false;
			}

			// wenn erfolgreich gesetzt (hier angekommen),
			// dann braucht Wert nicht noch einmal gesetzt werden,
			// und Flag kann ausgeschaltet werden
			V->SetControl_SetDepth_DC_Off();
		}

		// --- Magnetmarke ein/aus
		if (V->GetControl_SetMagneticMark() == _ON) {

			if (V->GetControl_MagneticMarkOn() == _ON) Ok = DepthCalc.ActivateMagneticMark();
			else Ok = DepthCalc.DeactivateMagneticMark();

			if (!Ok) {
				SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout auf voherigen Wert zurücksetzen
				return false;
			}

			// wenn erfolgreich gesetzt (hier angekommen),
			// dann braucht Wert nicht noch einmal gesetzt werden,
			// und Flag kann ausgeschaltet werden
			V->SetControl_SetMagneticMarkOff();
		}


		// --- Format für externe cm-Impulse setzen
		if (V->GetControl_SetFormatCMImpulse() == _ON) {

			switch (V->GetParam_FormatCMImpulse()) {
				case _KARAT:    Ok = DepthCalc.SetExternalCMImpulseFormat('K');
								break;

				case _KARAT2:   Ok = DepthCalc.SetExternalCMImpulseFormat('S');
								break;

				case _KARAT2N:  Ok = DepthCalc.SetExternalCMImpulseFormat('N');
								break;

				case _ONYX:     Ok = DepthCalc.SetExternalCMImpulseFormat('O');
								break;

				case _KEDR:     Ok = DepthCalc.SetExternalCMImpulseFormat('G');
								break;

				case _KEDRN:    Ok = DepthCalc.SetExternalCMImpulseFormat('P');
								break;

				default:        Ok = DepthCalc.SetExternalCMImpulseFormat('K');
								break;
			}

#ifndef __DEF_IGNORE_EXT_CMIMPULSE_FORMAT_SET_RESPONSE
			if (!Ok) {
				SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout auf voherigen Wert zurücksetzen
				return false;
			}
#endif

			// wenn erfolgreich gesetzt (hier angekommen),
			// dann braucht Wert nicht noch einmal gesetzt werden,
			// und Flag kann ausgeschaltet werden
			V->SetControl_SetFormatCMImpulseOff();
		}

		// --- Eichfaktor setzen
//        if (V->GetControl_SetDepthCalFactor() == _ON) Ok = DepthCalc.SetCalibFactor(V->GetParam_CalFactDepth());
		if (V->GetControl_SetDepthCalFactor_DC() == _ON) {
			Ok = DepthCalc.SetCalibFactor2(V->GetParam_CalFactDepth()); // 23.01.2006 Setzen des Eichfaktors, ohne Beeinflussung des aktuellen Teufenwertes

            if (!Ok) {
                    SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout auf voherigen Wert zurücksetzen
                    return false;
            }

            // wenn erfolgreich gesetzt (hier angekommen),
            // dann braucht Wert nicht noch einmal gesetzt werden,
            // und Flag kann ausgeschaltet werden
            V->SetControl_SetDepthCalFactor_DC_Off();
        }

        // ---

        SetCommTimeouts(DepthCalcH, &DepthCalcctmoNew);                         // Timeout auf voherigen Wert zurücksetzen

        V->SetControl_SetDepthCalcParamOff();

        return true;
};

//---------------------------------------------------------------------------
// DIO-RELAY-CARD
//---------------------------------------------------------------------------
// setzt den angegebenen Digitalausgang auf High
// Rückgabewert ist true bei Erfolg, false bei Misserfolg
bool WCIO::DOPortOn(short Num)
{
#ifdef __ENABLE_ADV_PCI1762

//-------------------------------
#ifdef DAQNAVI_ARCH
	return PCI1762.SetH(Num, false);
#endif //DAQNAVI_ARCH
//-------------------------------
#ifndef DAQNAVI_ARCH
	DOSetBuffer[Num] = 1;
	return true;
#endif //DAQNAVI_ARCH
//-------------------------------

#endif // __ENABLE_ADV_PCI1762

#ifndef __ENABLE_ADV_PCI1762
	return true;
#endif // __ENABLE_ADV_PCI1762

};

//---------------------------------------------------------------------------
// setzt den angegebenen Digitalausgang auf Low
// Rückgabewert ist true bei Erfolg, false bei Misserfolg
bool WCIO::DOPortOff(short Num)
{
#ifdef __ENABLE_ADV_PCI1762

//-------------------------------
#ifdef DAQNAVI_ARCH
	return PCI1762.SetL(Num, false);
#endif //DAQNAVI_ARCH
//-------------------------------
#ifndef DAQNAVI_ARCH
	DOSetBuffer[Num] = 0;
	PT_DioWriteBit DORelayPort;
	return true;
#endif DAQNAVI_ARCH
//-------------------------------

#endif // __ENABLE_ADV_PCI1762

#ifndef __ENABLE_ADV_PCI1762
	return true;
#endif // __ENABLE_ADV_PCI1762

};

//---------------------------------------------------------------------------
// Ausgabe des internen Puffers an DO-Hardware
// Rückgabewert
bool WCIO::DOOut()
{
#ifdef __ENABLE_ADV_PCI1762

#ifndef __DEMO_MODE
	//-------------------------------
	#ifdef DAQNAVI_ARCH
	return PCI1762.WriteAll();
	#endif //DAQNAVI_ARCH
	//-------------------------------
	#ifndef DAQNAVI_ARCH
		return PCI1762_DOUT(DOSetBuffer, sizeof(DOSetBuffer)/sizeof(DOSetBuffer[0]));
	#endif DAQNAVI_ARCH
	//-------------------------------
#endif

#endif // __ENABLE_ADV_PCI1762

#ifndef __ENABLE_ADV_PCI1762
	return true;
#endif // __ENABLE_ADV_PCI1762

};


//---------------------------------------------------------------------------
// fragt den Status des angegebenen Einganges ab
// Rückgabewert ist der Status des jeweiligen Bits
// FromHardware: wenn auf true gesetzt, dann werden die DI-Werte neu eingelesen,
//               wenn auf false gesetzt, dann werden die Puffer-Werte ausgelesen und ausgegeben.
//               Dies ermöglicht den Aufruf der Methode, um zeitgleich Zustände zu erfassen.
// Error gibt an, ob der Wert festgestellt werden konnte (false) oder nicht (true)
bool WCIO::DIPortStatus(short Num, bool FromHardware, bool *Error)
{
#ifdef __ENABLE_ADV_PCI1762

//-------------------------------
#ifdef DAQNAVI_ARCH
	if (FromHardware) *Error = PCI1762.ReadAll();
	return PCI1762.DIn(Num);
#endif //DAQNAVI_ARCH
//-------------------------------
#ifndef DAQNAVI_ARCH
	// Bei Anforderung neu einlesen
	if (FromHardware) {
		*Error = !PCI1762_DIN(DIStatusBuffer, sizeof(DIStatusBuffer)/sizeof(DIStatusBuffer[0]));
	};
	return DIStatusBuffer[Num];
#endif //DAQNAVI_ARCH
//-------------------------------

#endif // __ENABLE_ADV_PCI1762

#ifndef __ENABLE_ADV_PCI1762
	return false;
#endif // __ENABLE_ADV_PCI1762

};

//---------------------------------------------------------------------------
// Gibt die am angegebenen Kanal anliegende Spannung zurück
// *Error ist bei Erfolg false, bei Fehler true
// FromHardware: wenn auf true gesetzt, dann werden die ADU-Werte neu eingelesen,
//               wenn auf false gesetzt, dann werden die Puffer-Werte ausgelesen und ausgegeben.
//               Dies ermöglicht den Aufruf der Methode, um zeitgleich anliegende Spannungen zu erfassen.
float WCIO::ADUIn(unsigned short Channel, bool FromHardware, bool *Error)
{
//-------------------------------
#ifdef DAQNAVI_ARCH
	if (FromHardware) *Error = !PCI1716.ReadAll();
	return PCI1716.AIn(Channel);
#endif //DAQNAVI_ARCH
//-------------------------------
#ifndef DAQNAVI_ARCH
	if (FromHardware) {
		if (PCI1716_AIN(AINVoltageBuffer, sizeof(AINVoltageBuffer)/sizeof(AINVoltageBuffer[0]))) {
			*Error = false;
			return AINVoltageBuffer[Channel];
		};
	} else return AINVoltageBuffer[Channel];
	*Error = true;
	return 0;
#endif // DAQNAVI_ARCH
//-------------------------------
};

//---------------------------------------------------------------------------
// Gibt die angegebene Spannung aus
// Rückgabewerte: true wenn OK, false bei irgendeinem Fehler, d.h. aber nicht, dass alles schiefgegangen ist
// ToHardware   ... sorgt dafür, dass der Puffer mit den Spannungswerten über die Hardware ausgegeben wird
// IgnoreChannel... sorgt dafür, dass die Parameter "Channel" und "Voltage" ignoriert werden
bool WCIO::ADUOut(unsigned short Channel, float Voltage, bool IgnoreChannel, bool ToHardware)
{
		bool OK = true;
//-------------------------------
#ifdef DAQNAVI_ARCH
	if (!IgnoreChannel)
		if (!PCI1716.AOut(Channel, Voltage, false)) OK = false;
	if (ToHardware)
#ifndef __DEMO_MODE
		if (!PCI1716.WriteAll()) OK = false;
#endif

#endif //DAQNAVI_ARCH
//-------------------------------
#ifndef DAQNAVI_ARCH

	// Kanal in den Puffer schreiben und dabei Puffergröße beachten
	if (!IgnoreChannel)
	{
		if (Channel < (sizeof(AOutVoltageBuffer)/sizeof(AOutVoltageBuffer[0]))) AOutVoltageBuffer[Channel] = Voltage;
		else OK = false;
	}

	if (ToHardware)
	{
#ifndef __DEMO_MODE
		if (!PCI1716_AOUT(AOutVoltageBuffer, PCI1716_MAXOUTCHANNELNUM)) OK = false;
#endif
	};
#endif //DAQNAVI_ARCH
//-------------------------------

	return OK;
};

//---------------------------------------------------------------------------
// EXTERNE INFO-SS
// Fragt den Eingang des Informationsausgabeprts ab und gibt das jeweils eingelesene Byte zurück
char WCIO::CheckInfoOutPort()
{
		if (!InfoOutOk)
		{
//				Application->ProcessMessages();                                 // zur Geschwindigkeitsreduzierung der Threads, wenn COM-Port Zugriff wegfällt, damit Anwendung bedienbar bleibt
				return 0;
		};

		DWORD ReadedBytes;
		char ByteBuffer = 0;

		ReadFile(InfoOutH, &ByteBuffer, 1, &ReadedBytes, NULL);
		return ByteBuffer;
};

//---------------------------------------------------------------------------
// Sendet die angegebenen Daten an den Informations-Ausgabe-Port
// InfoData       ... Zeiger auf die zu sendenden Daten
// InfoDataLength ... Länge des Arrays
// Rückgabewert :
bool WCIO::WriteToInfoOutPort(unsigned char *InfoData, unsigned int InfoDataLength)
{
		bool OK = true;
		for (unsigned int i = 0; i < InfoDataLength; ++i)
		{
				OK = TransmitCommChar(InfoOutH, InfoData[i]);                   // jeweiliges Zeichen senden
				if (!OK) break;
		};
		return OK;
};

//---------------------------------------------------------------------------


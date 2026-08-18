/*
WinchControl    Jörg Müller     GUIMain.cpp

Klassenimplementation GUI

Portierbarkeit: nein
*/
//---------------------------------------------------------------------------
#include <vcl.h>

#pragma hdrstop

#include "constants.h"
#include "Labelling.h"
#include "Splash.h"
#include "DevelF.h"
#include <dir.h>
#include <memory>
#include <Mmsystem.h>

#include "GUIMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//#pragma link "VirtualKeyBoard"
//#pragma link "DirectionViewVert01"
#pragma link "TractionViewHor01"
#pragma link "CSPIN"
//#pragma link "VirtualKeyboard2"
//#pragma link "AnalogMeter01"
//#pragma link "DepthVisualisation01"
//#pragma link "LED01"
//#pragma link "CGAUGES"
#pragma link "AnalogMeter01"
#pragma link "CGAUGES"
#pragma link "DepthVisualisation01"
#pragma link "LED01"
//#pragma link "VirtualKeyboard2"
#pragma link "DirectionViewVert02"
//#pragma link "VirtualKeyBoard2"
#pragma link "VirtualKeyBoard2"
#pragma link "Thermometer01"
#pragma link "SpeedView01"
#pragma link "GaugeVert01"
#pragma link "VirtKeypad"
#pragma link "JMAdvLabeledEdit"
#pragma resource "*.dfm"
TMainForm *MainForm;

// Interne Status-Informationen nur für dieses Formular
bool    ApplicationClose = false;
int     TerminationTimer = TIMEOUT_Termination;

// Magnetmarken-Ansicht (Lampen-Ausschaltverzögerung)
clock_t MagneticMarkViewTrigger = 0;

// Felder zur Datums- u. Uhrzeit-Einstellung
bool    ActualizeSystemDateMaskEdit = false;
bool    ActualizeSystemTimeMaskEdit = false;

// Flags zur Darstellungsoptimierung
// Beim neu Zeichnen von Bildern kommt es zum Flackern der Darstellung.
// Ändert sich im gleichen Moment der Bildinhalt, fällt das Flackern nicht auf.
// Mit Hilfe folgender Flags soll sichergestellt werden, dass die Darstellung nur bei tatsächlichem Bedarf aktualisiert wird
// ACHTUNG, in "TMainForm::MainFormResize" sollten die Flags auf Standard zurückgesetzt werden, damit die betroffenen
// Darstellungen bei Größenänderung des Programmfensters sicher aktualisiert werden.
int Status_EmergencySwitchSymbolLamp = -1;
int Status_AutomotiveErrorSymbolLamp = -1;
int Status_ChangeOilFilterSymbolLamp = -1;
int Status_AuxiallyDriveSymbolLamp = -1;
int Status_MagneticMarkOnOffSymbolLamp = -1;
int Status_DynamicTensionOnOffSymbolLamp = -1;

int Status_FastRunButton = -1;
int Status_CurrDirectionView = -1;

// Standardschriftgrößen
int FONTSIZE_SMALL = 8;
int FONTSIZE_NORMAL = 10;
int FONTSIZE_BIG = 12;

// Standard-Tastengrößen
int BTN_SIZE_H = 14;
int BTN_SIZE_W = 4 * BTN_SIZE_H;

// Blockierungsflags, die verhindern sollen, dass mehrere Instanzen einer Methode aufgerufen werden können
bool ShowExtCMImpulseFormatImage_IsRunning = false;

// Hilfsflags zur Soundausgabe
bool SND_Alarm = false;
bool SND_Warning = false;

// Hilfsvariable
int AutoRunProgramItemList_TmpIndex = -1;

clock_t PowerFailStartTime;
bool PowerFailStartTime_Reset = true;

/*
// ==========================================================================
// HILFSMETHODEN
// ==========================================================================
// Dämpft veränderliche Werte, indem sich der Istwert dem Sollwert annähert
// (durch stetiges Aufrufen dieser Methode mit den jeweiligen Folgewerten,
// wird nach einer einstellbaren Verzögerung der Sollwert erreicht)
// Wie bei I-Regler.
// CurrVal      ... der bisherige Wert.
// TargetVal    ... Der zu erreichende Soll-Wert. In diesem wird auch das Ergebnis der Methode geliefert.
// Delay        ... Die Verzögerungszeit
// LastTime     ... Die beim letzten Aufruf vorhandene Zeit (Zeitbasis für Berechnung)
// Ist der Zeitunterschied gleich 0, wird *TargetVal = CurrVal.
void TMainForm::DampNoise(float CurrVal, float *TargetVal, int Delay, unsigned int *LastTime)
{
        // Imaginäre Zugkraft-Dämpfung (Filtern der Zugkraft)
        unsigned int HVuint = GetProcessTime_in_ms();                           // momentane Zeit
        unsigned int Steps = 0;                                                 // noch zu ermittelne Annäherungsstufen

        if ((HVuint - *LastTime) != 0)
        {
                // Anzahl der Annäherungsstufen erfassen
                Steps = Delay / (HVuint - *LastTime);
                if (Steps == 0) Steps = 1;                                      // wenn mehr als die erlaubte Zeit vergangen ist, dann wird der Wert direkt gesetzt
                *LastTime = HVuint;                                             // für nächsten Durchgang merken

                *TargetVal = CurrVal + ((*TargetVal - CurrVal) / Steps);

        }else *TargetVal = CurrVal;
};

// --------------------------------------------------------------------------
// ermittelt die seit Prozessbeginn verbrauchte CPU-Zeit in ms
long TMainForm::GetProcessTime_in_ms()
{
        return ((float)(clock()) * (float)(1000) / (float)(CLOCKS_PER_SEC));
};
*/
// ==========================================================================
// PROGRAMM-VERWALTUNG
// ==========================================================================
// Konstruktor
__fastcall TMainForm::TMainForm(TComponent* Owner)
        : TForm(Owner)
{
        UnicodeString HString;

        // --------------------------
        // Grundeinstellungen
        DisplayThread = 0;
        ControlThread = 0;

        DevelForm = 0;

        // Zähler für Thread-Starts zurücksetzen
        ControlThread_STARTUP_CNT = 0;                                          // sind diese Zähler > 0, dann sind sie aktiv,
        DisplayThread_STARTUP_CNT = 0;                                          // wenn kleiner 0, werden sie vom "Timer" ignoriert

        // --------------------------
        // Takte beginnen alle mit High(true)
        GenOrgan.Clock_100ms = _HIGH;
        Clock100ms_cnt = 0;

#ifdef __USE_THREAD_CLOCK
		Timer->Interval = 1000; // Timer braucht bloß Uhrzeit anzeigen
#endif

#ifndef __USE_THREAD_CLOCK
        Timer->Interval = CLOCK_Base;  // Timer muss internen Takt bereitstellen
#endif

		TerminationTimer = TIMEOUT_Termination / Timer->Interval;

/*  Variante mit STRControl am 29.04.2011 deaktiviert
        // Countdown für Abschaltung bei Spannungsversorgungs-Fehler
        PowerFailCountdown->Duration = POWERFAIL_SHUTDOWN_DELAY * 1000;         // Countdown-Dauer in ms
        PowerFailCountdown->EventName = EVENT_NAME_POWERFAIL;
        PowerFailCountdown->FastExamination = true;
        PowerFailCountdown->AutoReset = true;
        PowerFailCountdown->Enabled = true;                                     // Überwachung ist aktiv
        PowerFailCountdown->Reset();
*/

        // --------------------------
        // Initialisierung der Programm-Organisations-Struktur
        // Aktuelles Arbeitsverzeichnis setzen
        SetWorkingDir();

        // Potokollpfad festlegen
        strncpy(GenOrgan.LOGPath, LOG_Path, sizeof(LOG_Path) / sizeof(LOG_Path[0]));
        GenOrgan.LOGPath[0] = GenOrgan.CurrDrive[0]; // Laufwerksbuchstaben austauschen

        strncpy(GenOrgan.LOGPathinternal, LOG_Path_internal, sizeof(LOG_Path_internal) / sizeof(LOG_Path_internal[0]));
        GenOrgan.LOGPathinternal[0] = GenOrgan.CurrDrive[0]; // Laufwerksbuchstaben austauschen

        // Schnittstellen-Grundkonfiguration
        GenOrgan.DepthCalc_Port = (char*)__HW_DepthCalc_Port;                   // Standard-Port für die Kommunikation mit dem Teufenrechner
        GenOrgan.DepthCalc_DataBit = __HW_DepthCalc_DataBit;
        GenOrgan.DepthCalc_StopBit = __HW_DepthCalc_StopBit;
        GenOrgan.DepthCalc_Parity = __HW_DepthCalc_Parity;
        GenOrgan.DepthCalc_Baud = __HW_DepthCalc_Baud;

        GenOrgan.InfoOut_Port = (char*)__HW_InfoOut_Port;                       // Standard-Port für die zusätzliche Informationsausgabe
        GenOrgan.InfoOut_DataBit = __HW_InfoOut_DataBit;
        GenOrgan.InfoOut_StopBit = __HW_InfoOut_StopBit;
        GenOrgan.InfoOut_Parity = __HW_InfoOut_Parity;
        GenOrgan.InfoOut_Baud = __HW_InfoOut_Baud;
                
        // Mess-/Steuerkarten-Konfiguration Grundeinstellung
        GenOrgan.AI_TractionChannel = AI_TractionChannel;
        GenOrgan.AI_TractionGain = AI_TractionGain;
        GenOrgan.AI_DAQNavi_TensionRange = AI_DAQNavi_TensionRange;
		GenOrgan.AI_WinchRevMessChannel = AI_WinchRevMessChannel;
		GenOrgan.AI_WinchRevMessGain = AI_WinchRevMessGain;
        GenOrgan.AI_DAQNavi_WinchRevMessRange = AI_DAQNavi_WinchRevMessRange;
        GenOrgan.AI_TemperatureDepthCalcChannel = AI_TemperatureDepthCalcChannel;
		GenOrgan.AI_TemperatureDepthCalcGain = AI_TemperatureDepthCalcGain;
		GenOrgan.AI_DAQNavi_TemperatureDepthCalcRange = AI_DAQNavi_TemperatureDepthCalcRange;
		GenOrgan.AI_TemperatureAChannel = AI_TemperatureAChannel;
		GenOrgan.AI_TemperatureAGain = AI_TemperatureAGain;
        GenOrgan.AI_DAQNavi_TemperatureARange = AI_DAQNavi_TemperatureARange;
		GenOrgan.AI_TemperatureBChannel = AI_TemperatureBChannel;
        GenOrgan.AI_TemperatureBGain = AI_TemperatureBGain;
        GenOrgan.AI_DAQNavi_TemperatureBRange = AI_DAQNavi_TemperatureBRange;

        GenOrgan.AO_WinchRevChannel = AO_WinchRevChannel;
        GenOrgan.AO_WinchRevReference = AO_WinchRevReference;
        GenOrgan.AO_WinchRevMinVoltage = AO_WinchRevMinVoltage;
        GenOrgan.AO_WinchRevMaxVoltage = AO_WinchRevMaxVoltage;
        GenOrgan.AO_WinchRevZeroBy = AO_WinchRevZeroBy;
		GenOrgan.AO_WinchRevCfgMinVoltage = AO_WinchRevCfgMinVoltage;
		GenOrgan.AO_WinchRevCfgMaxVoltage = AO_WinchRevCfgMaxVoltage;
		GenOrgan.AO_DAQNavi_WinchRevCfgRange = AO_DAQNavi_WinchRevCfgMaxVoltage;

		GenOrgan.AO_TensionOutChannel = AO_TensionOutChannel;
        GenOrgan.AO_TensionOutReference = AO_TensionOutReference;
        GenOrgan.AO_TensionOutMinVoltage = AO_TensionOutMinVoltage;
		GenOrgan.AO_TensionOutMaxVoltage = AO_TensionOutMaxVoltage;
		GenOrgan.AO_TensionOutCfgMinVoltage = AO_TensionOutCfgMinVoltage;
		GenOrgan.AO_TensionOutCfgMaxVoltage = AO_TensionOutCfgMaxVoltage;
		GenOrgan.AO_DAQNavi_TensionOutRange = AO_DAQNavi_TensionOutRange;

		GenOrgan.AO_TensionCtrl_ExtReg_Channel = AO_TensionCtrl_ExtReg_Channel;
		GenOrgan.AO_TensionCtrl_ExtReg_Reference = AO_TensionCtrl_ExtReg_Reference;
		GenOrgan.AO_TensionCtrl_ExtReg_MinVoltage = AO_TensionCtrl_ExtReg_MinVoltage;
		GenOrgan.AO_TensionCtrl_ExtReg_MaxVoltage = AO_TensionCtrl_ExtReg_MaxVoltage;
		GenOrgan.AO_TensionCtrl_ExtReg_CfgMinVolt = AO_TensionCtrl_ExtReg_CfgMinVolt;
		GenOrgan.AO_TensionCtrl_ExtReg_CfgMaxVolt = AO_TensionCtrl_ExtReg_CfgMaxVolt;
        GenOrgan.AO_DAQNavi_TensionCtrl_ExtReg_Range = AO_DAQNavi_TensionCtrl_ExtReg_Range;

		// Initialisierung der Ereignismeldungsverwaltung
		if (!InitEventMgr()) {
				MessageDlg(L"WinchControl event message management could not be established", mtError, TMsgDlgButtons() << mbOK, 0);
				Application->ProcessMessages();
				Application->Terminate();
		}

		// --------------------------
		if (VarControl.Init((char*)__ParameterFile, (char*)__ParameterTmpFile, (char*)__ParameterBackupFile, (char*)__InternalParameterFile) == _BAD) {
				GenOrgan.Evnt << 11;		// Fehler 11 ein
		};

        // Starten des schreibens der LOG-Datei
        GenOrgan.NewLOG = true;
        GenOrgan.LOG_Lng = __CURR_LNG;
        GenOrgan.LOG_LOCATION[0] = 0;
        GenOrgan.LOG_WELL[0] = 0;
        GenOrgan.LOG_NAME[0] = 0;
		GenOrgan.LOG_TEAM[0] = 0;
        GenOrgan.LOG_WINCHMACHINERY[0] = 0;
		GenOrgan.LOG_COMMENT[0] = 0;

        // Füllstatus des Datenspeichers auf "Platz vorhanden" setzen
        GenOrgan.LOGDiskFree = true;                                            // für offizielle LOG-Daten
        GenOrgan.LOGInternalDiskFree = true;                                    // für interne LOG-Daten

        // Sprach-Einstellung
        GenOrgan.Lng = __CURR_LNG;

        // --------------------------
        // Einlesen der Kommandozeilen-Parameter und Übernehmen von Einstellungen
        GetCmdParameter();
		SetDevelMode();
		// --------------------------
        // Einlesen der Standardwerte/Setzen der Standard-Werte
//TODO
        // --------------------------
        // Einlesen der Daten aus der Konfigurationsdatei
//TODO
        // --------------------------
        // Anwenden der Sprachdaten, Zuweisen der Beschriftungen
		Font->Charset = LNG_CODE[GenOrgan.Lng];
		VirtualKeyboard21->MakeKBLayout(GenOrgan.Lng);
		SetLabelling();

        // --------------------------
		// Panel->Darstellung
		GenOrgan.CurrFunctionPanel = STARTUP_FunctionPanel;
		GenOrgan.FavoriteFunctionPanel =STARTUP_FunctionPanel;
		ShowCurrFunctionPanel();

// ToDo: Nach bewähren der neuen Methode "AcousticSignalOn"-Flag vollständig entfernen
        // Akustische Signalisierung einschalten
//		GenOrgan.AcousticSignalOn = true;

#ifdef  __TRACTION_VIEW_NOISE_DAMP
        oldTractionViewDampTime = GetProcessTime_in_ms();
        TractionView_AverageManager.SetSize(GET_TRACTION_VIEW_NOISE_DAMP_TIME / 100);
        TractionView_AverageManager.SetKeepBufferContent(true);                 // Bei Änderung der Puffergröße den Pufferinhalt weitestgehend beibehalten
#endif

        // --------------------------
        // Dynamische Instanzen der untergeordneten Klassen bilden
        if (VarControl.devel())
        {
                try
                {
                        DevelForm = new TDevelForm(this);
                }catch(...)
                {
                        DevelForm = 0;
                };
        };

		DisplayThreadTerminates = false;                                        // nur DisplayThread kann diese Variable setzen
        ControlThreadTerminates = false;

        try
        {
            ControlThread = new WCControlThread(true);
        }catch(...) {
				MessageDlg(ERRMSG_START_FAILURE[GenOrgan.Lng], mtError, TMsgDlgButtons() << mbOK, 0);

				Application->ProcessMessages();
				Application->Terminate();
		}

        try
        {
                DisplayThread = new WCDisplayThread(true);
        }catch(...)
        {
                if (ControlThread != 0) ControlThread->Terminate();

                MessageDlg(ERRMSG_START_FAILURE[GenOrgan.Lng], mtError, TMsgDlgButtons() << mbOK, 0);

                Application->ProcessMessages();
				Application->Terminate();
        };

		// --------------------------
		// Teufenrechner und Steuergerät müssen bei Systemstart aktualisiert werden
        VarControl.SetControl_GetDepthCalcSWVersionOn();
		VarControl.SetControl_SetDepthCalFactor_On();
		VarControl.SetControl_SetDepthCalFactor_DC_On();
		VarControl.SetControl_SetDepthCalcParamOn();
        VarControl.SetControl_SetFormatCMImpulseOn();
        VarControl.SetControl_SetMagneticMarkOn();

        // --------------------------
        // Verfügbare Ablaufprogramme für die Programmautomatik erfassen
        GetAutoRunProgramList();
        // Standardeinstellung für evtl. eingestelltes Ablaufprogramm laden
        // Unbedingt nach "GetAutoRunProgramList()"!!! siehe deren Implementierung
        GenOrgan.PrgAutoIndex = 0;  // Ablaufprogrammindex zurücksetzen
        HString = VarControl.GetParam_AutoRunProgramFileName();
		HString = HString + __AutoRunPrgFileExtention;
		if (!GenOrgan.PrgAutomatic.Read(HString.w_str())) {
            // Kann eingestelltes Programm nicht geladen werden, dann ggf. Programmautomatik deaktivieren
            VarControl.SetParam_AutoRunProgramableOff();
        }

        // --------------------------
        ActualizeAllInformations();

        // Bildschirmschoner und Ruhezustand abschalten
        SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, false, NULL, SPIF_SENDCHANGE);
        SystemParametersInfo(SPI_SETPOWEROFFACTIVE, false, NULL, SPIF_SENDCHANGE);
}

// --------------------------------------------------------------------------
// Setzt das aktuelle Arbeitsverzeichnis
// Dabei wird als Arbeitsverzeichnis der Pfad der Programmdatei ermittelt
void __fastcall TMainForm::SetWorkingDir()
{
        AnsiString WorkDir = ParamStr(0);                                       // Programm-Pfad + Start-Datei
        AnsiString WorkDrive = WorkDir;
        bool WorkDriveOK = false;

        // Arbeitsverzeichnis ermitteln
        WorkDir.SetLength(WorkDir.LastDelimiter("\\" ) - 1);                    // Start-Datei abschneiden (ab letzem \ alles weg)
        chdir(WorkDir.c_str());

        // Laufwerk ermitteln
        if (WorkDrive.Length() >= (StdDriveLength - 1)) {
            if (WorkDrive.IsDelimiter("aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuuvVwWxXyYzZ", 1)) {
                if (WorkDrive.IsDelimiter(":", 2)) {
                    if (WorkDrive.IsDelimiter("\\", 3)) {
                        WorkDrive.SetLength(StdDriveLength - 1);
                        strcpy(GenOrgan.CurrDrive, WorkDrive.c_str());
                        WorkDriveOK = true;
                    }
                }
            }
        }

        if (!WorkDriveOK) {
            strcpy(GenOrgan.CurrDrive, __StdDrive);
        }

};

// --------------------------------------------------------------------------
// Ermitteln der Kommandozeilenparameter vom Programmstart
void __fastcall TMainForm::GetCmdParameter()
{
        for (int i = 0; i <= ParamCount(); ++i)
        {
                if (ParamStr(i).c_str()[0] == CMDP_Entry)
                {
                        // ---------------
                        // Entwicklermodus
                        if (ParamStr(i).Pos(CMDP_DEVEL) == 2)
                        {
                                VarControl.SetControl_DevelOn();
                        };

                        // ---------------
                        // Programmsprache
                        // deutsch
                        if (ParamStr(i).Pos(CMDP_LANGUAGE_GERMAN) == 2)
                        {
                                GenOrgan.LOG_Lng = __GERMAN;
                                GenOrgan.Lng = __GERMAN;
                        };

                        // russisch
                        if (ParamStr(i).Pos(CMDP_LANGUAGE_RUSSIAN) == 2)
                        {
                                GenOrgan.LOG_Lng = __RUSSIAN;
                                GenOrgan.Lng = __RUSSIAN;
                        };

                        // englisch
                        if (ParamStr(i).Pos(CMDP_LANGUAGE_ENGLISH) == 2)
                        {
                                GenOrgan.LOG_Lng = __ENGLISH;
                                GenOrgan.Lng = __ENGLISH;
                        };
                        // ---------------
                };
        };
};

// --------------------------------------------------------------------------
// Darstellung und Einstellmöglichkeiten abh. vom Entwicklermodus anpassen
void __fastcall TMainForm::SetDevelMode()
{
		if (VarControl.devel())                                                 // Entwicklermodus ein
		{
				BorderStyle = bsSizeable;                                       // Am Hauptformular können Größenänderungen vorgenommen werden
				DevelFormButton->Visible = true;                                // Die Menütaste "Beenden" ist vorhanden

				// Erscheinung setzen (alles sichtbar)
				for (int i = 0; i < ERRORARRAYSIZE; ++i) {
					GenOrgan.Evnt.SetSilentOff(i);
				}
//                WinchBrakeOffSpeedButton->Visible = true;                       // Bremse kann manuell gelöst werden
		}else                                                                   // Entwicklermodus aus
		{
				BorderStyle = bsNone;
				DevelFormButton->Visible = false;
		}
				// Erscheinung setzen (still oder auffällig)
				for (int i = 0; i < ERRORARRAYSIZE; ++i) {

					(ERRORHIDE[i] == _ON) ?
					GenOrgan.Evnt.SetSilent(i) :
					GenOrgan.Evnt.SetSilentOff(i);

//                WinchBrakeOffSpeedButton->Visible = false;
		};

};

// --------------------------------------------------------------------------
// Ereignismeldungsverwaltung initialisieren
// Rückgabewert:
// true ... alles i.O.
// false ... wenn ein Fehler aufgetreten ist
bool __fastcall TMainForm::InitEventMgr() {

    short HV;
	// Alle verfügbaren Ereignisse "anmelden"
	if (GenOrgan.Evnt.Add(ERRORARRAYSIZE) != EVNT_OK) return false;

	try {
		for (int i = 0; i < ERRORARRAYSIZE; i++) {

			// Typ setzen (Fehler oder Warnung)
			(ERRORONLYWARNING[i] == _ON) ?
			HV = GenOrgan.Evnt.SetType(i, EVNTOBJTYPE_WARNING) :
			HV = GenOrgan.Evnt.SetType(i, EVNTOBJTYPE_ERROR);
			if (HV != EVNT_OK) return false;

			// Erscheinung setzen (still oder auffällig)
			(ERRORHIDE[i] == _ON) ?
			HV = GenOrgan.Evnt.SetSilent(i) :
			HV = GenOrgan.Evnt.SetSilentOff(i);

			if (HV != EVNT_OK) return false;
		}

	} catch (...) {
		return false;
	}

	return true;
}

// --------------------------------------------------------------------------
// Fenstereinstellungen für den Demo-Modus
void __fastcall TMainForm::SetDemoMode()
{
#ifdef __DEMO_MODE
	BorderStyle = bsSizeable;                                       			// Am Hauptformular können Größenänderungen vorgenommen werden
	WindowState = wsNormal;
	Height = 600;
	Width = 800;
#endif
#ifndef __DEMO_MODE
	WindowState = wsMaximized;
#endif

}

//---------------------------------------------------------------------------
// Löscht, wenn enthalten, die dem Fehlercode-zugeordnete Fehlermeldung aus der Meldungsliste
void __fastcall TMainForm::DelErrorMessage(int Code)
{
		AnsiString CodeString = CONST__BRACKET01LEFT + IntToStr(Code) + CONST__BRACKET01RIGHT;;

		for (int i = 0; i < MessageListBox->Lines->Count; ++i)
		{
				// Wenn String mit CodeString beginnt, dann gefunden und löschen
				if (MessageListBox->Lines->Strings[i].Pos(CodeString) == 1)
				{
						MessageListBox->Lines->Delete(i);
				};
		};

        GenOrgan.Evnt.SetShownOff(Code);
};

//---------------------------------------------------------------------------
// Hängt die zum Fehler-Code gehörige Meldung an die Meldungsliste an
void __fastcall TMainForm::AddErrorMessage(int Code)
{
		// Ermittlung der Code-Nr. aus "BitCode" wird sicherheitshalber
		// nicht durchgeführt, da die Logarithmus-Auswertung fehleranfälliger
		// ist und entsprechende Implementierungssfehler schneller zum
		// Programmabsturz führen können
		AnsiString CodeString = CONST__BRACKET01LEFT + IntToStr(Code) + CONST__BRACKET01RIGHT + CONST__SEPARATOR01;

		switch (Code) {

		case 1		: 	CodeString = CodeString + ERRMSG_DEPTHCALC_PORT_OPEN_FAILURE[GenOrgan.Lng] + CONST__SEPARATOR01 + GenOrgan.DepthCalc_Port;
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 2		: 	CodeString = CodeString + ERRMSG_DEPTHCALC_COMM_FAILURE[GenOrgan.Lng] + CONST__SEPARATOR01 + GenOrgan.DepthCalc_Port;
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 3		:	CodeString = CodeString + ERRMSG_INFOOUT_PORT_OPEN_FAILURE[GenOrgan.Lng] + CONST__SEPARATOR01 + GenOrgan.InfoOut_Port;
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 4		:	CodeString = CodeString + ERRMSG_DIO_RELAY_CARD_NOT_FOUND[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 5		:	CodeString = CodeString + ERRMSG_DAC_CARD_NOT_FOUND[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 6		:	CodeString = CodeString + ERRMSG_DIO_RELAY_CARD_NO_MEMORY[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 7		:	CodeString = CodeString + ERRMSG_DAC_CARD_NO_MEMORY[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 8		:	CodeString = CodeString + ERRMSG_INC_ENC_ERROR[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 9		:	CodeString = CodeString + ERRMSG_DEPTHRANGE_EXCEEDED[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 10		:	CodeString = CodeString + ERRMSG_MAXTENSION_EXCEEDED[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 11		:	CodeString = CodeString + ERRMSG_COULDNT_READ_PARAM[GenOrgan.Lng] + CONST__SEPARATOR01 + __ParameterFile;
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 12		:	CodeString = CodeString + ERRMSG_COULDNT_WRITE_PARAM[GenOrgan.Lng] + CONST__SEPARATOR01 + __ParameterFile;
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 13		:	CodeString = CodeString + ERRMSG_LOG_DATA_FAIL[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 14		:	CodeString = CodeString + ERRMSG_COPY_LOG_FILE_FAIL[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 15		:	CodeString = CodeString + ERRMSG_PROGRAM_INSTANCE_EXISTS[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 16		:	CodeString = CodeString + ERRMSG_COULDNT_WRITE_SPECIAL_PARAM[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 17		:	CodeString = CodeString + ERRMSG_CONTROLLERERROR[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 18		:	CodeString = CodeString + ERRMSG_AUTOMOTIVEERROR[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 19		:	CodeString = CodeString + ERRMSG_DELETE_LOG_FILE_FAIL[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 20		:	CodeString = CodeString + ERRMSG_LOG_DISKSPACE_FAIL[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 21		:	CodeString = CodeString + ERRMSG_LOG_INTERNAL_DISKSPACE_FAIL[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 22		:	CodeString = CodeString + ERRMSG_GEAR_SHIFT_FAIL[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 23		:	CodeString = CodeString + ERRMSG_POWERFAIL[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 24		:	CodeString = CodeString + ERRMSG_ERROR_SIGNAL_1_FROM_EXT_EQUIPMENT[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 25		:	CodeString = CodeString + ERRMSG_ERROR_SIGNAL_2_FROM_EXT_EQUIPMENT[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 26		:	CodeString = CodeString + ERRMSG_RIG_UNSECURED[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 27		:	CodeString = CodeString + ERRMSG_CTL_CAN_LIBNOTFOUND[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 28		:	CodeString = CodeString + ERRMSG_CTL_CAN_INTERFACE_INIT_FAILURE[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 29		:	CodeString = CodeString + ERRMSG_CTL_CAN_INTERFACE_COM_ERROR[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 30		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_BRAKE[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 31		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_SPOOLER_HOR_CLUTCH[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 32		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_SPOOLER_TELESCOP_RUNIN[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 33		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_SPOOLER_VERT[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 34		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_SPOOLER_HOR_LEFT[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 35		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_SPOOLER_HOR_RIGHT[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 36		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_FREEWHEEL[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 37		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_FREEWHEEL_INV[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 38		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_PUMP1_FWD[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 39		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_PUMP1_REVERSE[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 40		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_PUMP2_FWD[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 41		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_PUMP2_REVERSE[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 42		:	CodeString = CodeString + ERRMSG_LEVER_FAULTY[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 43		:	CodeString = CodeString + ERRMSG_SPOOLER_HOR_LEVER_FAULTY[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 44		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_SLOWMOTION[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 45		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_SPECIAL1[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 46		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_SPECIAL2[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		case 47		:	CodeString = CodeString + ERRMSG_OUTPUT_FAULTY_HYDRAULIC_ACTIVE[GenOrgan.Lng];
						GenOrgan.Evnt.SetShown(Code);
						break;

		default		:	return;

		};

		MessageListBox->Lines->Append(CodeString);
// ToDo: Nach bewähren der neuen Methode "AcousticSignalOn"-Flag vollständig entfernen
//		GenOrgan.AcousticSignalOn = true;
};

//---------------------------------------------------------------------------
// Ermittelt, wie die Anzeige für den angegebenen Fehlerzustand aktualisiert werden muss
// Parameter:           Code     	der zu prüfende Fehlercode
// Rückgabewert:        _IGNORE     Die Fehleranzeige ist aktuell, es muss nichts unternommen werden
//                      _SHOW       Der betreffende Fehler muss der Anzeige hinzugefügt werden
//                      _HIDE       Der betreffende Fehler muss aus der Anzeige entfernt werden
short __fastcall TMainForm::DetermErrorMessageNotif(int Code)
{
	// Ist Fehler nicht mehr in Fehlerliste, wird aber noch angezeigt ?
	if (!GenOrgan.Evnt[Code] ||
		 (GenOrgan.Evnt.Silent(Code) &&
		 GenOrgan.Evnt.Shown(Code)))
			return _HIDE;

	// Ist Fehler in Fehlerliste, ist anzuzeigender Fehler und wird nicht angezeigt ?
	// Hier wird auch beachtet, ob der Fehler vor dem Anwender verborgen bleiben soll
	if (GenOrgan.Evnt[Code] &&
		!GenOrgan.Evnt.Silent(Code) &&
		!GenOrgan.Evnt.Shown(Code))
			return _SHOW;

    return _IGNORE;
}

//---------------------------------------------------------------------------
// Anzeigen des Formulars
void __fastcall TMainForm::FormShow(TObject *Sender)
{
        // Statische Layout-Einstellungen
        MakeStaticLayout();

        // Anpassung der Objekte an die Dimensionen des Formulars
        MakeLayout();


#ifdef __SHOW_SPLASHSCREEN
        // Splash-Formular anzeigen
        SplashForm = 0;

        try
        {
                SplashForm = new TSplashForm(this);

				SplashForm->SplashText = (AnsiString)(__PROGNAME) + " " + __VERSION;

	#ifdef __DEMO_MODE
				SplashForm->WindowState = wsNormal;
				SplashForm->Height = Height;
				SplashForm->Width = Width;
				SplashForm->Left = 0;
				SplashForm->Top = 0;

            	SplashForm->SplashText += " DEMO";
	#endif // __DEMO_MODE

				SplashForm->Delay = SplashScreenDelay;
				SplashForm->ShowModal();
        }catch(...)
        {
                SplashForm = 0;
        };

                // SplashScreen wieder abschalten
        if (SplashForm != 0)
        {
                try
                {
                        delete SplashForm;
                }catch(...){};

                SplashForm = 0;
        };

#endif //__SHOW_SPLASHSCREEN



        // =====================================================

        // TEST5: íàäïèñü "ÒÅÑÒÎÂÀß ÂÅÐÑÈß" + êíîïêà "Âûõîä"

        // (íàä íàäïèñüþ "Ðó÷íîå óïðàâëåíèå" / ProgramModeLabel)

        // =====================================================

        try {

                TLabel *TestVersionLabel = new TLabel(this);

                TestVersionLabel->Parent = ProgramModeLabel->Parent;

                TestVersionLabel->Caption = L"ÒÅÑÒÎÂÀß ÂÅÐÑÈß";

                TestVersionLabel->Font->Color = clBlack;

                TestVersionLabel->Font->Style = TestVersionLabel->Font->Style << fsBold;

                TestVersionLabel->Font->Height = ProgramModeLabel->Font->Height;

                TestVersionLabel->AutoSize = true;

                TestVersionLabel->Left = ProgramModeLabel->Left;

                TestVersionLabel->Top = ProgramModeLabel->Top - TestVersionLabel->Height - 2;

                if (TestVersionLabel->Top < 0) TestVersionLabel->Top = 0;

                TestVersionLabel->Visible = true;



                TButton *TestExitButton = new TButton(this);

                TestExitButton->Parent = ProgramModeLabel->Parent;

                TestExitButton->Caption = L"Âûõîä";

                TestExitButton->Font->Height = ProgramModeLabel->Font->Height;

                TestExitButton->Left = TestVersionLabel->Left + TestVersionLabel->Width + 8;

                TestExitButton->Top = TestVersionLabel->Top;

                TestExitButton->Height = TestVersionLabel->Height + 6;

                TestExitButton->OnClick = ExitButtonClick;

                TestExitButton->Visible = true;

        } catch(...) { /* TEST5: íå êðèòè÷íî, åñëè íå ñîçäàëîñü */ };

}

//---------------------------------------------------------------------------
// Layout-Anpassung der im Hauptformular enthaltenen Objekte an die Größe des Formulars
// abhängig vom Sender werden die entsprechenden Elemente in Ihrer größenabhängigen Anordnung aktualisiert
void __fastcall TMainForm::MainFormResize(TObject *Sender)
{
        // Zeichensatz
        Font->Charset = LNG_CODE[GenOrgan.Lng];

        // Bei jeglicher Größenänderung werden pauschal alle Symbole neu gezeichnet, um keine Layout-Fehler zu erhalten (z.B. Symbole passen nicht auf Taste)
        Status_EmergencySwitchSymbolLamp = -1;
        Status_AutomotiveErrorSymbolLamp = -1;
        Status_ChangeOilFilterSymbolLamp = -1;
        Status_AuxiallyDriveSymbolLamp = -1;
        Status_MagneticMarkOnOffSymbolLamp = -1;
        Status_DynamicTensionOnOffSymbolLamp = -1;

        Status_FastRunButton = -1;
		Status_CurrDirectionView = -1;

		// Sender-abhängige Ansichtsaktualisierung
		if ((Sender == CalibDepthPanel) && (CalibDepthPanel->Visible)) MakeCalibDepthPanelLayout();
		else if ((Sender == CalibTractionPanel) && (CalibTractionPanel->Visible)) MakeCalibTractionPanelLayout();
		else if ((Sender == ChoicePanel) && (ChoicePanel->Visible)) MakeChoicePanelLayout();
		else if ((Sender == DialogPanel) && (DialogPanel->Visible)) MakeDialogPanelLayout();
		else if ((Sender == UnloadCablePanel) && (UnloadCablePanel->Visible)) MakeUnloadCablePanelLayout();
		else if ((Sender == FileAdminPanel) && (FileAdminPanel->Visible)) MakeFileAdminPanelLayout();
		else if ((Sender == InfoPanel) && (InfoPanel->Visible)) MakeInfoPanelLayout();
		else if ((Sender == MainForm) && (MainForm->Visible)) MakeLayout();
		else if ((Sender == MessagePanel) && MessagePanel->Visible) MakeMessagePanelLayout();
		else if ((Sender == NormalViewPanel) && (NormalViewPanel->Visible)) MakeNormalViewPanelLayout();
		else if ((Sender == ParameterPanel) && (ParameterPanel->Visible)) MakeParameterPanelLayout();
		else if ((Sender == SystemPanel) && (SystemPanel->Visible)) MakeSystemPanelLayout();
		else if ((Sender == MainMenuPanel) && MainMenuPanel->Visible) MakeMainMenuPanelLayout();

		// Fenstergrößeninfo erfassen
		GenOrgan.MainForm_ClientWidth = ClientWidth;
		GenOrgan.MainForm_ClientHeight = ClientHeight;

};

//---------------------------------------------------------------------------
// Taste Beenden gedrückt
void __fastcall TMainForm::ExitButtonClick(TObject *Sender)
{
        VarControl.SetInfo_ProgramMode(2);      // Programm-Ende

        // Threads beenden
        if (DisplayThread != 0) DisplayThread->Terminate();
        if (ControlThread != 0) ControlThread->Terminate();

        // Anwendung darf geschlossen werden, Bedienung nicht mehr möglich
        MainForm->Enabled = false;
        ApplicationClose = true;
};

//---------------------------------------------------------------------------
// Anwendung beenden angefordert (ALT+F4 mit z.B.)
void __fastcall TMainForm::CloseQuery(TObject *Sender, bool &CanClose)
{
        CanClose = false;

        ExitButtonClick(this);
}

//---------------------------------------------------------------------------
void __fastcall TMainForm::TimerEvent(TObject *Sender)
{
        // Uhrzeit aktualisieren
        ShowTime();
        ShowSystemDateTime();                                                   // im Systemmenü

        // Von Programmbeginn an wartende Threads starten
		if (ControlThread_STARTUP_CNT < ControlThread_STARTUP_DELAY) {
			ControlThread_STARTUP_CNT += Timer->Interval;
			if ((ControlThread_STARTUP_CNT >= ControlThread_STARTUP_DELAY) && (ControlThread != 0)) ControlThread->Resume();
		};

		if (DisplayThread_STARTUP_CNT < DisplayThread_STARTUP_DELAY) {
				DisplayThread_STARTUP_CNT += Timer->Interval;
				if ((DisplayThread_STARTUP_CNT >= DisplayThread_STARTUP_DELAY) && (DisplayThread != 0)) DisplayThread->Resume();
		};

        // Anwendung beenden
        if (ApplicationClose)
        {
				// Entwickler-Fenster SCHLIEßEN
                if (DevelForm != 0) try {
                        delete DevelForm;
                }catch(...){};
                DevelForm = 0;

                --TerminationTimer;
                // Programm-Ende, wenn
                // - Threads Ihre Arbeit für beendet erklären
                // - Timeout erreicht ist
				if ((ControlThreadTerminates && DisplayThreadTerminates) ||
					(TerminationTimer == 0)) {

						// Thread-Speicherfreigabe, falls einer "hängt"
						if (!ControlThreadTerminates) {
							try {
//                                ControlThread->Suspend();
								delete ControlThread;
							}catch(...) {
							}
						};

						if (!DisplayThreadTerminates) {
							try {
//                                DisplayThread->Suspend();
								delete DisplayThread;
							}catch(...) {
}
						};

                        Application->Terminate();
                };

        };

#ifndef __USE_THREAD_CLOCK
        // Takte generieren
        ++Clock100ms_cnt;

        if (Clock100ms_cnt == 1)
        {
                Clock100ms_cnt = 0;
                GenOrgan.Clock_100ms = !GenOrgan.Clock_100ms;
        };
#endif

};

// ==========================================================================
// GUI-LAYOUT
// ==========================================================================
// Layout-Einstellungen, die während der Programmlaufzeit statisch sind
void __fastcall TMainForm::MakeStaticLayout()
{
        DoubleBuffered = true;

        // Panel, die zur Entwurfszeit, zur besseresn Erkennbarkeit,
        // noch ihre Caption besitzen, diese aber nicht angezeigt werden sollen
//        AutoRunProgramItemListPanel->Caption = "";
    	AutoRunProgramItemListPanel->ShowCaption = false;
		CalibDepthEdit_GroupBox->Caption = L"";
//		CalibDepthFramePanel->Caption = "";
		CalibDepthFramePanel->ShowCaption = false;
//		CalibDepthPanel->Caption = "";
		CalibDepthPanel->ShowCaption = false;
//		CalibTractionPanel->Caption = "";
		CalibTractionPanel->ShowCaption = false;
//		ChoicePanel->Caption = "";
		ChoicePanel->ShowCaption = false;
//		ControlLampPanel->Caption = "";
		ControlLampPanel->ShowCaption = false;
        CommonCtlPanel->ShowCaption = false;
//		CurrTractionView->Caption = "";
		CurrTractionView->ShowCaption = false;
//		DepthVisualisation->Caption = "";
		DepthVisualisation->ShowCaption = false;
//		DialogPanel->Caption = "";
		DialogPanel->ShowCaption = false;
//		UnloadCablePanel->Caption = "";
		UnloadCablePanel->ShowCaption = false;
//		UnloadCableHelpPanel->Caption = "";
		UnloadCableHelpPanel->ShowCaption = false;
//		UnloadCableTopHelpPanel->Caption = "";
		UnloadCableTopHelpPanel->ShowCaption = false;
//		EngineCtlPanel->Caption = "";
		EngineCtlPanel->ShowCaption = false;
		EngineCoolantTemperatureView->ShowCaption = true; // Optikmaßnahme, damit nebeneinander liegende Anzeigen die gleichen Abmessungen haben
		EngineCoolantTemperatureView->Caption = L"";
		EngineSpeedView->ShowCaption = false;
//		FileAdminPanel->Caption = "";
		FileAdminPanel->ShowCaption = false;
//		LOGFileCharacteristicsHelpPanel->Caption = "";
		LOGFileCharacteristicsHelpPanel->ShowCaption = false;
//		HydraulicPanel->Caption = "";
		HydraulicCtlPanel->ShowCaption = true;	// Optikmaßnahme, damit nebeneinander liegende Anzeigen die gleichen Abmessungen haben
		HydraulicCtlPanel->Caption = L"";
    	HydrLvlView->ShowCaption = true;
		HydrTemperatureView->ShowCaption = true;	// Optikmaßnahme, damit nebeneinander liegende Anzeigen die gleichen Abmessungen haben
		HydrTemperatureView->Caption = L"";
		HydrWrkPressureView->ShowCaption = true;	// Optikmaßnahme, damit nebeneinander liegende Anzeigen die gleichen Abmessungen haben
		HydrWrkPressureView->Caption = L"";
//		InfoPanel->Caption = "";
		InfoPanel->ShowCaption = false;
//		LOGFileAdminButtonPanel->Caption = "";
		LOGFileAdminButtonPanel->ShowCaption = false;
//		LOGFileAdminHelpPanel1->Caption = "";
		LOGFileAdminHelpPanel1->ShowCaption = false;
//		LOGFileAdminHelpPanel2->Caption = "";
		LOGFileAdminHelpPanel2->ShowCaption = false;
//		LOGFileButtonPanel->Caption = "";
		LOGFileButtonPanel->ShowCaption = false;
//		LOGFilePanel->Caption = "";
		LOGFilePanel->ShowCaption = false;
//		LogFileLanguageFormatPanel->Caption = "";
		LogFileLanguageFormatPanel->ShowCaption = false;
//		LogFileTensionUnitPanel->Caption = "";
		LogFileTensionUnitPanel->ShowCaption = false;
//		LOGFileTypePanel->Caption = "";
		LOGFileTypePanel->ShowCaption = false;
//		MagneticMarkPanel->Caption = "";
		MagneticMarkPanel->ShowCaption = false;
//		MainMenuPanel->Caption = "";
		MainMenuPanel->ShowCaption = false;
        MainCtlSubPanel->ShowCaption = false;
//		MessagePanel->Caption = "";
		MessagePanel->ShowCaption = false;
//		MessageButtonPanel->Caption = "";
		MessageButtonPanel->ShowCaption = false;
//		NormalViewPanel->Caption = "";
		NormalViewPanel->ShowCaption = false;
//		NormalViewHelpPanel1->Caption = "";
		NormalViewHelpPanel1->ShowCaption = false;
//		NormalViewHelpPanel2->Caption = "";
		NormalViewHelpPanel2->ShowCaption = false;
//		ParameterPanel->Caption = "";
		ParameterPanel->ShowCaption = false;
		PLineLubTunePanel->ShowCaption = false;
//		SystemPanel->Caption = "";
		SystemPanel->ShowCaption = false;
//		SystemHelpPanel->Caption = "";
		SystemHelpPanel->ShowCaption = false;
//		SystemHelpPanel1->Caption = "";
		SystemHelpPanel1->ShowCaption = false;
//		WinchDrivePanel->Caption = "";
		WinchDriveCtlPanel->ShowCaption = false;
		LOGFileCommentsMemo->Clear();
		XtraCtlPagePanel->ShowCaption = false;
        XtraEngineCtlPanel->ShowCaption = false;

		// GUI-Form
		SetDemoMode();
		Top = 0;
		Left = 0;
		BorderWidth = 0;
		Constraints->MinWidth = LAYOUT_GUI_MinWidth;
		Constraints->MinHeight = LAYOUT_GUI_MinHeight;

		SetFocusedControl(this); // experimentell ab V 2.2.3 (15.12.09) Taskleiste soll nicht mehr sichtbar sein

		// MessageLight
		MessageLight->BorderWidth = LAYOUT_GUI_Margin;

		// ------------------------
		// ControlLampPanel
		ControlLampPanel->BorderWidth = LAYOUT_GUI_Margin;

		AlarmSoundOnOffButton->NumGlyphs = 2;
		AutomotiveError_DeadButton->NumGlyphs = 2;
		AutomotiveError_DeadButton->Flat = true;
		AutomotiveError_DeadButton->Enabled = false;
		AuxiallyDrive_DeadButton->NumGlyphs = 2;
		AuxiallyDrive_DeadButton->Flat = true;
		AuxiallyDrive_DeadButton->Enabled = false;
		ChangeOilFilter_DeadButton->NumGlyphs = 2;
		ChangeOilFilter_DeadButton->Flat = true;
		ChangeOilFilter_DeadButton->Enabled = false;
		DynamicTensionOnOffButton->NumGlyphs = 2;
		EmergencySwitch_DeadButton->NumGlyphs = 2;
		EmergencySwitch_DeadButton->Flat = true;
		EmergencySwitch_DeadButton->Enabled = false;
		MagneticMarkOnOffButton->NumGlyphs = 2;

		PowerFailCdResetSpeedButton->Font->Style = TFontStyles() << fsBold;

		SpecialFuncButton->NumGlyphs = 2;
		SpecialFuncButton->Caption = L"";

		MainMenuSpeedButton->NumGlyphs = 2;
		MainMenuSpeedButton->Flat = false;
		MainMenuSpeedButton->Enabled = true;

		// ------------------------
		// Control-Panel
		// Randbreiten
		MainCtlPanel->BorderWidth = LAYOUT_GUI_Margin;
		MainViewPanel->BorderWidth = LAYOUT_GUI_Margin;
//		MainCtlSubPanel->BorderWidth = LAYOUT_GUI_Margin;
		MagneticMarkPanel->BorderWidth = LAYOUT_GUI_Margin;
		EngineCtlPanel->BorderWidth = LAYOUT_GUI_Margin;

		MainViewHelpPanel1->BorderWidth = 2 * LAYOUT_GUI_Margin;
		MainViewHelpPanel1->Align = alClient;

		// Tension
		CurrTractionView->Align = alClient;
		CurrTractionView->Margins->Top = LAYOUT_GUI_Margin;
		CurrTractionView->Margins->Bottom = LAYOUT_GUI_Margin;
		CurrTractionView->AlignWithMargins = true;
		CurrTractionView->TranspCol = (TColor)LAYOUT_CurrTractionView_TranspCol;
		CurrTractionView->MeterBackCol = (TColor)LAYOUT_CurrTractionView_MeterBackCol;
		CurrTractionView->MeterFrameCol = clWindowFrame;

		CommonCtlPanel->Align = alClient;

		StartStopButton->Align = alTop;
		StartStopButton->AlignWithMargins = true;
		StartStopButton->Margins->Top = LAYOUT_GUI_Margin;
		StartStopButton->Margins->Bottom = 0;
		StartStopButton->Margins->Left = LAYOUT_GUI_Margin;
		StartStopButton->Margins->Right = LAYOUT_GUI_Margin;

		ResetButton->Align = alTop;
		ResetButton->AlignWithMargins = true;
		ResetButton->Margins->Top = LAYOUT_GUI_Margin;
		ResetButton->Margins->Bottom = 0;
		ResetButton->Margins->Left = LAYOUT_GUI_Margin;
		ResetButton->Margins->Right = LAYOUT_GUI_Margin;

		SignalHornButton->NumGlyphs = 2;
		SignalHornButton->Caption = L"";
		SignalHornButton->Align = alTop;
		SignalHornButton->AlignWithMargins = true;
		SignalHornButton->Margins->Top = LAYOUT_GUI_Margin;
		SignalHornButton->Margins->Bottom = 0;
		SignalHornButton->Margins->Left = LAYOUT_GUI_Margin;
		SignalHornButton->Margins->Right = LAYOUT_GUI_Margin;

		// Schriften-Style
		CurrDepthLabel->Font->Style = CurrDepthLabel->Font->Style << fsBold;

		// ------------------------
		// Xtra-Control-Panel

		XtraCtlPageScroller->Control = XtraCtlPagePanel;

//		XtraCtlPagePanel->BorderWidth = LAYOUT_GUI_Margin;
		XtraEngineCtlPanel->BorderWidth = LAYOUT_GUI_Margin;
		HydraulicCtlPanel->BorderWidth = LAYOUT_GUI_Margin;
		WinchDriveCtlPanel->BorderWidth = LAYOUT_GUI_Margin;

		// Engine
		EngineSpeedView->Align = alLeft;
		EngineSpeedView->TranspCol = (TColor)LAYOUT_EngineSpeedView_TranspCol;
		EngineSpeedView->MeterBackCol = LAYOUT_MeterBgColor;
		EngineSpeedView->MeterFrameCol = clWindowFrame;

		EngineCoolantTemperatureView->Align = alLeft;

		EngineCoolantTemperatureView->NormalColor = COLOR_TEMPERATURE_NORMAL;
		EngineCoolantTemperatureView->WarnColor = COLOR_TEMPERATURE_COLD;
		EngineCoolantTemperatureView->AlertColor = COLOR_TEMPERATURE_HOT;
		EngineCoolantTemperatureView->BackgroundColor = clWhite;
		EngineCoolantTemperatureView->TransparentBg = true;

		EngineCoolantTemperatureView->ScaleMin = TEMPERATURE_ENGINECOOLANT_MIN;
		EngineCoolantTemperatureView->ScaleMax = TEMPERATURE_ENGINECOOLANT_MAX;
		EngineCoolantTemperatureView->ScaleFrequency = TEMPERATURE_ENGINECOOLANT_SCALEFREQ;

		// Hydraulik
		HydrWrkPressureView->Align = alLeft;
		HydrWrkPressureView->BevelOuter = bvNone;

		HydrWrkPressureView->TranspCol = (TColor)LAYOUT_HydrWrkPressureView_TranspCol;
		HydrWrkPressureView->MeterBackCol = LAYOUT_MeterBgColor;
		HydrWrkPressureView->MeterFrameCol = clWindowFrame;

		HydrTemperatureView->Align = alLeft;
		HydrTemperatureView->BevelOuter = bvNone;

		HydrTemperatureView->NormalColor = COLOR_TEMPERATURE_NORMAL;
		HydrTemperatureView->AlertColor = COLOR_TEMPERATURE_HOT;
		HydrTemperatureView->WarnColor = COLOR_TEMPERATURE_COLD;
		HydrTemperatureView->BackgroundColor = clWhite;
		HydrTemperatureView->TransparentBg = true;

		HydrTemperatureView->ScaleMin = TEMPERATURE_HYDRTANK_MIN;
		HydrTemperatureView->ScaleMax = TEMPERATURE_HYDRTANK_MAX;
		HydrTemperatureView->ScaleFrequency = TEMPERATURE_HYDRTANK_SCALFREQ;

		HydrLvlView->Align = alLeft;
		HydrLvlView->BevelOuter = bvNone;

		HydrLvlView->NormalColor = COLOR_OK;
		HydrLvlView->AlertColor = COLOR_ALERT;
		HydrLvlView->WarnColor = COLOR_ATTENTION;
		HydrLvlView->BackgroundColor = clWhite;
		HydrLvlView->TransparentBg = true;

		HydrLvlView->ScaleMin = 0;
		HydrLvlView->ScaleMax = 100;
		HydrLvlView->ScaleFrequency = 25;

		// Winde
		FastRunButton->Caption = "";

		// ------------------------
		// Funktions-Panel
		FunctionPanel->BorderWidth = LAYOUT_GUI_Margin;
		KeyBoardPanel->BorderWidth = LAYOUT_GUI_Margin;

		// ------------------------
		// NormalViewPanel
		NormalViewPanel->BorderWidth = LAYOUT_GUI_Margin;
		DepthVisualisation->BorderWidth = LAYOUT_GUI_Margin;
		DepthVisualisation->TubeFrontColor = (TColor)LAYOUT_DepthVisualisation_FrontColor;
		DepthVisualisation->TubeBorderColor = (TColor)LAYOUT_DepthVisualisation_BorderColor;
		NormalViewHelpPanel2->BorderWidth = LAYOUT_GUI_Margin;
        AutoRunProgramItemListPanel->BorderWidth = LAYOUT_GUI_Margin;
		AutoRunProgramItemListGroupBox->Margins->Bottom = LAYOUT_GUI_Margin;
        AutoRunProgramItemListGroupBox->Margins->Top = LAYOUT_GUI_Margin;
        AutoRunProgramItemListGroupBox->Margins->Left = LAYOUT_GUI_Margin;
        AutoRunProgramItemListGroupBox->Margins->Right = LAYOUT_GUI_Margin;

        // ------------------------
        // ParameterPanel
        ParameterPanel->BorderWidth = LAYOUT_GUI_Margin;
        WinchTabSheet->BorderWidth = LAYOUT_GUI_Margin;
        RunTabSheet->BorderWidth = LAYOUT_GUI_Margin;
        CableTabSheet->BorderWidth = LAYOUT_GUI_Margin;
        DepthCalcTabSheet->BorderWidth = LAYOUT_GUI_Margin;
        SuperVisionTabSheet->BorderWidth = LAYOUT_GUI_Margin;
		ViewTabSheet->BorderWidth = LAYOUT_GUI_Margin;
		AdditionalFuncTabSheet->BorderWidth = LAYOUT_GUI_Margin;

#ifdef __ENABLE_PROGRAMAUTOMATIC
		PProgramAutomGroupBox->Visible = true;
#else
		PProgramAutomGroupBox->Visible = false;
#endif

        // Zuerst Sichtbare Registerkarte
        ParamPageControl->ActivePage = WinchTabSheet;

		PMinDepthEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		PMinDepthEdit->AutoLayout = false;
		PMinDepthEdit->Gap = LAYOUT_GUI_Margin;
		PMinDepthEdit->Type = 3; // Eingabewert float
		PMinDepthEdit->Margins->Top = LAYOUT_GUI_Margin;
		PMinDepthEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		PMinDepthEdit->AlignWithMargins = true;

		PMaxDepthEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		PMaxDepthEdit->AutoLayout = false;
		PMaxDepthEdit->Gap = LAYOUT_GUI_Margin;
		PMaxDepthEdit->Type = 3; // Eingabewert float
		PMaxDepthEdit->Margins->Top = LAYOUT_GUI_Margin;
		PMaxDepthEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		PMaxDepthEdit->AlignWithMargins = true;

		PVelocityEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		PVelocityEdit->AutoLayout = false;
		PVelocityEdit->Gap = LAYOUT_GUI_Margin;
		PVelocityEdit->Type = 1; // Eingabewert int
		PVelocityEdit->Margins->Top = LAYOUT_GUI_Margin;
		PVelocityEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		PVelocityEdit->AlignWithMargins = true;

		PChangeDirectionButton->Margins->Top = LAYOUT_GUI_Margin;
		PChangeDirectionButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PChangeDirectionButton->AlignWithMargins = true;

		PProgramAutomCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PProgramAutomCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PProgramAutomCheckBox->AlignWithMargins = true;

		PProgramAutomFileListBox->Margins->Top = LAYOUT_GUI_Margin;
		PProgramAutomFileListBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PProgramAutomFileListBox->AlignWithMargins = true;

		PProgramAutomPrgNameLabel->Margins->Top = LAYOUT_GUI_Margin;
		PProgramAutomPrgNameLabel->Margins->Bottom = LAYOUT_GUI_Margin;
		PProgramAutomPrgNameLabel->AlignWithMargins = true;

		PWinchBrakeInternalAutoCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PWinchBrakeInternalAutoCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PWinchBrakeInternalAutoCheckBox->AlignWithMargins = true;

		PEngineRevolutionChangeableCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PEngineRevolutionChangeableCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PEngineRevolutionChangeableCheckBox->AlignWithMargins = true;

		PEngineStartStopEnableCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PEngineStartStopEnableCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PEngineStartStopEnableCheckBox->AlignWithMargins = true;

		PLostMotionEnableCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PLostMotionEnableCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PLostMotionEnableCheckBox->AlignWithMargins = true;

		PBrakeOffEnableCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PBrakeOffEnableCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PBrakeOffEnableCheckBox->AlignWithMargins = true;

		PHydraulikBrakeEnableCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PHydraulikBrakeEnableCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PHydraulikBrakeEnableCheckBox->AlignWithMargins = true;

		PMaxTensionEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		PMaxTensionEdit->AutoLayout = false;
		PMaxTensionEdit->Gap = LAYOUT_GUI_Margin;
		PMaxTensionEdit->Type = 1; // Eingabewert int
		PMaxTensionEdit->Margins->Top = LAYOUT_GUI_Margin;
		PMaxTensionEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		PMaxTensionEdit->AlignWithMargins = true;

		PRelTensionLimitLowerEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		PRelTensionLimitLowerEdit->AutoLayout = false;
		PRelTensionLimitLowerEdit->Gap = LAYOUT_GUI_Margin;
		PRelTensionLimitLowerEdit->Type = 1; // Eingabewert int
		PRelTensionLimitLowerEdit->Margins->Top = LAYOUT_GUI_Margin;
		PRelTensionLimitLowerEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		PRelTensionLimitLowerEdit->AlignWithMargins = true;

		PRelTensionLimitUpperEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		PRelTensionLimitUpperEdit->AutoLayout = false;
		PRelTensionLimitUpperEdit->Gap = LAYOUT_GUI_Margin;
		PRelTensionLimitUpperEdit->Type = 1; // Eingabewert int
		PRelTensionLimitUpperEdit->Margins->Top = LAYOUT_GUI_Margin;
		PRelTensionLimitUpperEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		PRelTensionLimitUpperEdit->AlignWithMargins = true;

		PCalibTensionCalibInfoLabel->Margins->Top = LAYOUT_GUI_Margin;
		PCalibTensionCalibInfoLabel->Margins->Bottom = LAYOUT_GUI_Margin;
		PCalibTensionCalibInfoLabel->AlignWithMargins = true;

		PCalibTensionXEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		PCalibTensionXEdit->AutoLayout = false;
		PCalibTensionXEdit->Gap = LAYOUT_GUI_Margin;
		PCalibTensionXEdit->Type = 1; // Eingabewert int
		PCalibTensionXEdit->Margins->Top = LAYOUT_GUI_Margin;
		PCalibTensionXEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		PCalibTensionXEdit->AlignWithMargins = true;

		PCalibTensionYEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		PCalibTensionYEdit->AutoLayout = false;
		PCalibTensionYEdit->Gap = LAYOUT_GUI_Margin;
		PCalibTensionYEdit->Type = 1; // Eingabewert int
		PCalibTensionYEdit->Margins->Top = LAYOUT_GUI_Margin;
		PCalibTensionYEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		PCalibTensionYEdit->AlignWithMargins = true;

		PCalibTensionZeroEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		PCalibTensionZeroEdit->AutoLayout = false;
		PCalibTensionZeroEdit->Gap = LAYOUT_GUI_Margin;
		PCalibTensionZeroEdit->Type = 1; // Eingabewert int
		PCalibTensionZeroEdit->Margins->Top = LAYOUT_GUI_Margin;
		PCalibTensionZeroEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		PCalibTensionZeroEdit->AlignWithMargins = true;

		PSetDepthEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		PSetDepthEdit->AutoLayout = false;
		PSetDepthEdit->Gap = LAYOUT_GUI_Margin;
		PSetDepthEdit->Type = 3; // Eingabewert float
		PSetDepthEdit->Margins->Top = LAYOUT_GUI_Margin;
		PSetDepthEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		PSetDepthEdit->AlignWithMargins = true;

		PDepthCalFactorEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		PDepthCalFactorEdit->AutoLayout = false;
		PDepthCalFactorEdit->Gap = LAYOUT_GUI_Margin;
		PDepthCalFactorEdit->Type = 3; // Eingabewert float
		PDepthCalFactorEdit->Margins->Top = LAYOUT_GUI_Margin;
		PDepthCalFactorEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		PDepthCalFactorEdit->AlignWithMargins = true;

		PMMSyncForceRangeLimEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		PMMSyncForceRangeLimEdit->AutoLayout = false;
		PMMSyncForceRangeLimEdit->Gap = LAYOUT_GUI_Margin;
		PMMSyncForceRangeLimEdit->Type = 1; // Eingabewert int
		PMMSyncForceRangeLimEdit->Margins->Top = LAYOUT_GUI_Margin;
		PMMSyncForceRangeLimEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		PMMSyncForceRangeLimEdit->AlignWithMargins = true;

		PKARATRadioButton->Margins->Top = LAYOUT_GUI_Margin;
		PKARATRadioButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PKARATRadioButton->AlignWithMargins = true;

		PKARAT2RadioButton->Margins->Top = LAYOUT_GUI_Margin;
		PKARAT2RadioButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PKARAT2RadioButton->AlignWithMargins = true;

		PKARAT2NRadioButton->Margins->Top = LAYOUT_GUI_Margin;
		PKARAT2NRadioButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PKARAT2NRadioButton->AlignWithMargins = true;

		PONYXRadioButton->Margins->Top = LAYOUT_GUI_Margin;
		PONYXRadioButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PONYXRadioButton->AlignWithMargins = true;

		PKEDRRadioButton->Margins->Top = LAYOUT_GUI_Margin;
		PKEDRRadioButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PKEDRRadioButton->AlignWithMargins = true;

		PKEDRNRadioButton->Margins->Top = LAYOUT_GUI_Margin;
		PKEDRNRadioButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PKEDRNRadioButton->AlignWithMargins = true;

		PIncEncErrorMonitCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PIncEncErrorMonitCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PIncEncErrorMonitCheckBox->AlignWithMargins = true;

		PControllerErrorMonitCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PControllerErrorMonitCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PControllerErrorMonitCheckBox->AlignWithMargins = true;

		PControllerErrorLactiveCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PControllerErrorLactiveCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PControllerErrorLactiveCheckBox->AlignWithMargins = true;

		PControllerErrorAtSMCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PControllerErrorAtSMCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PControllerErrorAtSMCheckBox->AlignWithMargins = true;

		PSpecialSignalMode0RadioButton->Margins->Top = LAYOUT_GUI_Margin;
		PSpecialSignalMode0RadioButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PSpecialSignalMode0RadioButton->AlignWithMargins = true;

		PSpecialSignalMode1RadioButton->Margins->Top = LAYOUT_GUI_Margin;
		PSpecialSignalMode1RadioButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PSpecialSignalMode1RadioButton->AlignWithMargins = true;

		PSpecialSignalMode2RadioButton->Margins->Top = LAYOUT_GUI_Margin;
		PSpecialSignalMode2RadioButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PSpecialSignalMode2RadioButton->AlignWithMargins = true;

		PSpecialSignalMode3RadioButton->Margins->Top = LAYOUT_GUI_Margin;
		PSpecialSignalMode3RadioButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PSpecialSignalMode3RadioButton->AlignWithMargins = true;

		PSpecialSignalMode4RadioButton->Margins->Top = LAYOUT_GUI_Margin;
		PSpecialSignalMode4RadioButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PSpecialSignalMode4RadioButton->AlignWithMargins = true;

		PSpecialSignalMode5RadioButton->Margins->Top = LAYOUT_GUI_Margin;
		PSpecialSignalMode5RadioButton->Margins->Bottom = LAYOUT_GUI_Margin;
		PSpecialSignalMode5RadioButton->AlignWithMargins = true;

		PLineLubAvailableCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PLineLubAvailableCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PLineLubAvailableCheckBox->AlignWithMargins = true;

		PLineLubTunePanel->Margins->Top = LAYOUT_GUI_Margin;
		PLineLubTunePanel->Margins->Bottom = LAYOUT_GUI_Margin;
		PLineLubTunePanel->AlignWithMargins = true;

		PLineLubPermanentOperationCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		PLineLubPermanentOperationCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;
		PLineLubPermanentOperationCheckBox->AlignWithMargins = true;

		PLineLubIntensitySlider->Margins->Top = LAYOUT_GUI_Margin;
		PLineLubIntensitySlider->Margins->Bottom = LAYOUT_GUI_Margin;
		PLineLubIntensitySlider->AlignWithMargins = true;
		PLineLubIntensitySlider->Frequency = 10;
		PLineLubIntensitySlider->LineSize = 1;
		PLineLubIntensitySlider->PageSize = 10;
		PLineLubIntensitySlider->Min = LIM_PAR_LineLubIntensity_MIN;
		PLineLubIntensitySlider->Max = LIM_PAR_LineLubIntensity_MAX;

		// ------------------------
        // CalibDepthPanel
		CalibDepthPanel->BorderWidth = LAYOUT_GUI_Margin;

		CalibDepthInfoLabel->AlignWithMargins = true;

		CalibDepthEdit_GroupBox->AlignWithMargins = true;
		CalibDepthEdit_GroupBox->Margins->Top = LAYOUT_GUI_Margin;
		CalibDepthEdit_GroupBox->Margins->Bottom = LAYOUT_GUI_Margin;

		CalibDepthHint1_SwitchOffMM_Label->AlignWithMargins = true;
		CalibDepthHint2_SetDepth_Label->AlignWithMargins = true;
		CalibDepthHint3_MoveLine_Label->AlignWithMargins = true;
		CalibDepthHint4_InsertCorrectDepth_Label->AlignWithMargins = true;

		CalibDepth_DepthEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		CalibDepth_DepthEdit->AutoLayout = true;
		CalibDepth_DepthEdit->Gap = LAYOUT_GUI_Margin;
		CalibDepth_DepthEdit->Type = 3; // Eingabewert float
		CalibDepth_DepthEdit->Margins->Top = LAYOUT_GUI_Margin;
		CalibDepth_DepthEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		CalibDepth_DepthEdit->AlignWithMargins = true;

		// ------------------------
		// CalibTractionPanel
		CalibTractionPanel->BorderWidth = LAYOUT_GUI_Margin;

		CalibTension_Tension1Edit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		CalibTension_Tension1Edit->AutoLayout = true;
		CalibTension_Tension1Edit->Gap = LAYOUT_GUI_Margin;
		CalibTension_Tension1Edit->Type = 1; // Eingabewert int
		CalibTension_Tension1Edit->Margins->Top = LAYOUT_GUI_Margin;
		CalibTension_Tension1Edit->Margins->Bottom = LAYOUT_GUI_Margin;
		CalibTension_Tension1Edit->AlignWithMargins = true;

		CalibTension_Tension2Edit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		CalibTension_Tension2Edit->AutoLayout = true;
		CalibTension_Tension2Edit->Gap = LAYOUT_GUI_Margin;
		CalibTension_Tension2Edit->Type = 1; // Eingabewert int
		CalibTension_Tension2Edit->Margins->Top = LAYOUT_GUI_Margin;
		CalibTension_Tension2Edit->Margins->Bottom = LAYOUT_GUI_Margin;
		CalibTension_Tension2Edit->AlignWithMargins = true;

		// ------------------------
        // ChoicePanel
        ChoicePanel->BorderWidth = LAYOUT_GUI_Margin;

        // ------------------------
        // DialogPanel
        DialogPanel->BorderWidth = LAYOUT_GUI_Margin;

        // ------------------------
        // UnloadCablePanel
        UnloadCablePanel->BorderWidth = LAYOUT_GUI_Margin;

		UnloadCableSetTensionEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		UnloadCableSetTensionEdit->AutoLayout = true;
		UnloadCableSetTensionEdit->Gap = LAYOUT_GUI_Margin;
		UnloadCableSetTensionEdit->Type = 1; // Eingabewert int
		UnloadCableSetTensionEdit->Margins->Top = LAYOUT_GUI_Margin;
		UnloadCableSetTensionEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		UnloadCableSetTensionEdit->AlignWithMargins = true;

		UnloadCabelStretchCheckBox->AlignWithMargins = true;
		UnloadCabelStretchCheckBox->Margins->Top = LAYOUT_GUI_Margin;
		UnloadCabelStretchCheckBox->Margins->Right = LAYOUT_GUI_Margin;
		UnloadCabelStretchCheckBox->Margins->Bottom = LAYOUT_GUI_Margin;

		UnloadCableStretchStartLenghtEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		UnloadCableStretchStartLenghtEdit->AutoLayout = false;
		UnloadCableStretchStartLenghtEdit->Gap = LAYOUT_GUI_Margin;
		UnloadCableStretchStartLenghtEdit->Type = 3; // Eingabewert float
		UnloadCableStretchStartLenghtEdit->Margins->Top = LAYOUT_GUI_Margin;
		UnloadCableStretchStartLenghtEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		UnloadCableStretchStartLenghtEdit->AlignWithMargins = true;

		UnloadCableStretchEndLenghtEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		UnloadCableStretchEndLenghtEdit->AutoLayout = false;
		UnloadCableStretchEndLenghtEdit->Gap = LAYOUT_GUI_Margin;
		UnloadCableStretchEndLenghtEdit->Type = 3; // Eingabewert float
		UnloadCableStretchEndLenghtEdit->Margins->Top = LAYOUT_GUI_Margin;
		UnloadCableStretchEndLenghtEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		UnloadCableStretchEndLenghtEdit->AlignWithMargins = true;

		UnloadCableStretchStartTensionEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		UnloadCableStretchStartTensionEdit->AutoLayout = false;
		UnloadCableStretchStartTensionEdit->Gap = LAYOUT_GUI_Margin;
		UnloadCableStretchStartTensionEdit->Type = 1; // Eingabewert int
		UnloadCableStretchStartTensionEdit->Margins->Top = LAYOUT_GUI_Margin;
		UnloadCableStretchStartTensionEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		UnloadCableStretchStartTensionEdit->AlignWithMargins = true;

		UnloadCableStretchEndTensionEdit->HighlightColor = COLOR_EDITFIELD_BACKKGR_INVALID;
		UnloadCableStretchEndTensionEdit->AutoLayout = false;
		UnloadCableStretchEndTensionEdit->Gap = LAYOUT_GUI_Margin;
		UnloadCableStretchEndTensionEdit->Type = 1; // Eingabewert int
		UnloadCableStretchEndTensionEdit->Margins->Top = LAYOUT_GUI_Margin;
		UnloadCableStretchEndTensionEdit->Margins->Bottom = LAYOUT_GUI_Margin;
		UnloadCableStretchEndTensionEdit->AlignWithMargins = true;

		// ------------------------
        // FileAdminPanel
        FileAdminPanel->BorderWidth = LAYOUT_GUI_Margin;

        // LOGFilePanel
        LOGFilePanel->BorderWidth = LAYOUT_GUI_Margin;
        LOGFileCharacteristicsTabSheet->BorderWidth = LAYOUT_GUI_Margin;
        LOGFileFileAdminTabSheet->BorderWidth = LAYOUT_GUI_Margin;
        LOGFileButtonPanel->BorderWidth = LAYOUT_GUI_Margin;
        LOGFileLocationMaskEdit->MaxLength = LOG_LOCATION_LENGHT - 1;
        LOGFileWellMaskEdit->MaxLength = LOG_WELL_LENGHT - 1;
        LOGFileNameMaskEdit->MaxLength = LOG_NAME_LENGHT - 1;
		LOGFileTeamMaskEdit->MaxLength = LOG_TEAM_LENGTH - 1;
        LOGFileWinchMachineryMaskEdit->MaxLength = LOG_WINCHMACHINERY_LENGTH - 1;
		LOGFileCommentsMemo->MaxLength = LOG_COMMENT_LENGHT - 1;

        LOGFileFileAdminTabSheet->BorderWidth = LAYOUT_GUI_Margin;
        LOGFileAdminHelpPanel1->BorderWidth = LAYOUT_GUI_Margin;
        LOGFileAdminHelpPanel2->BorderWidth = LAYOUT_GUI_Margin;
        LOGFileAdminButtonPanel->BorderWidth = LAYOUT_GUI_Margin;

        // ------------------------
        // InfoPanel
        InfoPanel->BorderWidth = LAYOUT_GUI_Margin;

		// ------------------------
		// SystemPanel
		SystemPanel->BorderWidth = LAYOUT_GUI_Margin;
		SystemHelpPanel->BorderWidth = LAYOUT_GUI_Margin;
		SystemDateMaskEdit->EditMask = FORMAT_Date_Mask;
		SystemTimeMaskEdit->EditMask = FORMAT_Time_Mask;

		SystemTempDepthCalcLabel->Transparent = true;
		SystemTempALabel->Transparent = true;
		SystemTempBLabel->Transparent = true;

        // ------------------------
		// Message-Panel
		MessagePanel->BorderWidth = LAYOUT_GUI_Margin;

		// ------------------------
		// MainMenuPanel
		MainMenuPanel->BorderWidth = LAYOUT_GUI_Margin;

#ifdef __NO_ENGINE_REVOLUTION_BUTTONS
        EngineRevolutionDownButton->Visible = false;
        EngineRevolutionUpButton->Visible = false;
        PEngineRevolutionChangeableCheckBox->Visible = false;
#endif

#ifdef __BLOCK_WINCH_CHOISE
        PWinchCfgListBox->Enabled = false;
#endif

        SystemTemperatureGroupBox->Visible = false;
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Objekte
void __fastcall TMainForm::MakeLayout()
{
		// Standardschriftgrößen festlegen
		FONTSIZE_SMALL = ClientHeight * LAYOUT_STD_SMALLFONT_HEIGHT_PERC / 100;
		FONTSIZE_NORMAL = ClientHeight * LAYOUT_STD_FONT_HEIGHT_PERC / 100;
		FONTSIZE_BIG = ClientHeight * LAYOUT_STD_BIGFONT_HEIGHT_PERC / 100;
		Font->Height = FONTSIZE_NORMAL;

		// Standard-Tasten-Abmessung festlegen
		BTN_SIZE_H = ClientHeight * LAYOUT_STD_BTN_HEIGHT_PERC / 100;
		BTN_SIZE_W = BTN_SIZE_H * 4;

		// -------------------------------------------------------------------
		// MELDUNGS-BEREICH
		MakeControlLampPanelLayout();

		// -------------------------------------------------------------------
		// HAUPT-KONTROLL-ANZEIGE
		MainCtlPanel->Width = MessageLight->ClientWidth * LAYOUT_ControlPanel_WIDTH_PERC / 100;
		MakeMainCtlPanelLayout();
		// -------------------------------------------------------------------

		// FUNKTIONS-BEREICH
//		KeyBoardPanel->Height = FunctionPanel->ClientWidth / LAYOUT_KeyBoardPanelProp_HEIGHT;
//		VirtKeypad1->Height = FunctionPanel->ClientWidth / LAYOUT_KeyBoardPanelProp_HEIGHT;
		KeyBoardPanel->Height = ClientHeight / LAYOUT_SubCtlPanel_PROP_HEIGHT;
		VirtKeypad1->Height = ClientHeight / LAYOUT_SubCtlPanel_PROP_HEIGHT;
		MakeCalibDepthPanelLayout();
		MakeCalibTractionPanelLayout();
		MakeChoicePanelLayout();
		MakeDialogPanelLayout();
		MakeUnloadCablePanelLayout();
		MakeFileAdminPanelLayout();
		MakeInfoPanelLayout();
		MakeNormalViewPanelLayout();
		MakeParameterPanelLayout();
		MakeXtraCtlPageScrollerLayout();
		MakeSystemPanelLayout();
		MakeMainMenuPanelLayout();
        MakeMessagePanelLayout();
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von ControlPanel
void __fastcall TMainForm::MakeMainCtlPanelLayout()
{
		// HAUPTANZEIGE
		MainViewPanel->Height = MainCtlPanel->ClientHeight * LAYOUT_MainViewPanel_HEIGHT_PERC / 100;
// ToDo: Schriftanpassung kann entfernt werden, sobald sich Routinen bewehrt haben
//        TimeLabel->Height = MainViewHelpPanel1->ClientHeight * LAYOUT_TimeLabel_HEIGHT_PERC / 100;
//		TimeLabel->Font->Height = TimeLabel->Height;
//		ProgramModeLabel->Height = MainViewHelpPanel1->ClientHeight * LAYOUT_ProgramModeLabel_HEIGHT_PERC / 100;
//		ProgramModeLabel->Font->Height = ProgramModeLabel->Height;

		TimeLabel->Font->Height = FONTSIZE_SMALL;

		// Teufenanzeige
		CurrDepthBox->Height = MainViewHelpPanel1->ClientHeight * LAYOUT_CurrDepthBox_HEIGHT_PERC / 100;
		CurrDepthBox->Font->Height = FONTSIZE_SMALL;
		CurrDepthLabel->Top = 0;
		CurrDepthLabel->Height = MagneticMarkPanel->Height;
		CurrDepthLabel->Left = LAYOUT_GUI_Margin;
		CurrDepthLabel->Width = MagneticMarkPanel->Width - (2 * LAYOUT_GUI_Margin);
		CurrDepthLabel->Font->Height = CurrDepthLabel->Height;

		// Geschwindigkeitsanzeige
        SpeedView->Font->Height = FONTSIZE_SMALL;
		SpeedView->Height = MainViewHelpPanel1->ClientHeight * LAYOUT_CurrVelocityBox_HEIGHT_PERC / 100;
		SpeedView->RedrawCurrSpeedView();
		SpeedView->RedrawAdjView();

//		CurrTractionView->Height = MainViewPanel->ClientHeight - CurrVelocityBox->Top - CurrVelocityBox->Height - (6 * LAYOUT_GUI_Margin);
//		CurrTractionView->Height = MainViewPanel->ClientHeight - SpeedView->Top - SpeedView->Height - (6 * LAYOUT_GUI_Margin);

		// Start/Stop-Taste
//		StartStopButton->Height = ControlPanel->ClientHeight * LAYOUT_StartStopButton_HEIGHT_PERC / 100;
//		StartStopButton->Width = ControlPanel->ClientWidth - (2 * LAYOUT_GUI_Margin);
// ToDo: Schriftanpassung kann entfernt werden, sobald sich Routinen bewehrt haben
//		StartStopButton->Font->Height = -StartStopButton->Height / LAYOUT_StartStopFontProp_Height;
//		StartStopButton->Top = MainViewPanel->Top + MainViewPanel->Height + LAYOUT_GUI_Margin;
//		StartStopButton->Left = LAYOUT_GUI_Margin;

		// Hauptsteuerelemente-Container
		MainCtlSubPanel->Height = ClientHeight / LAYOUT_SubCtlPanel_PROP_HEIGHT;
// ToDo: Schriftanpassung kann entfernt werden, sobald sich Routinen bewehrt haben
//		MainCtlPageControl->Font->Height = LAYOUT_MainCtlPageControl_FontProp_Height * MainCtlPageControl->Height / 100;


		CommonCtlGroupBox->Font->Height = FONTSIZE_SMALL;
		// Alle Positionsangaben relativ zu CommonCtlPanel

		StartStopButton->Height = BTN_SIZE_H;
		StartStopButton->Font->Height = FONTSIZE_SMALL;

		ResetButton->Height	= BTN_SIZE_H;
		ResetButton->Font->Height = FONTSIZE_SMALL;

		SignalHornButton->Height = BTN_SIZE_H;
		SignalHornButton->Font->Height = FONTSIZE_SMALL;


		EngineCtlGroupBox->Font->Height = FONTSIZE_SMALL;
		// Alle Positionsangaben relativ zu EngineCtlPanel

		EngineRevolutionUpButton->Height = BTN_SIZE_H;
		EngineRevolutionUpButton->Width = BTN_SIZE_H * 2;
		EngineRevolutionUpButton->Left = LAYOUT_GUI_Margin;
		EngineRevolutionUpButton->Top = LAYOUT_GUI_Margin;
		EngineRevolutionUpButton->Font->Height = FONTSIZE_SMALL;
		EngineRevolutionUpButton->Visible = VarControl.GetParam_EngineRevolutionChangeable();

		EngineRevolutionDownButton->Height = BTN_SIZE_H;
		EngineRevolutionDownButton->Width = BTN_SIZE_H * 2;
		EngineRevolutionDownButton->Left = EngineRevolutionUpButton->Left + EngineRevolutionUpButton->Width + LAYOUT_GUI_Margin;
		EngineRevolutionDownButton->Top = EngineRevolutionUpButton->Top;
		EngineRevolutionDownButton->Font->Height = FONTSIZE_SMALL;
		EngineRevolutionDownButton->Visible = VarControl.GetParam_EngineRevolutionChangeable();

		EngineButton->Height = BTN_SIZE_H;
		EngineButton->Width = BTN_SIZE_H * 2;
		EngineButton->Left = LAYOUT_GUI_Margin;
		EngineButton->Top = EngineRevolutionUpButton->Top + EngineRevolutionUpButton->Height + LAYOUT_GUI_Margin;
		EngineButton->Font->Height = FONTSIZE_SMALL;
		EngineButton->Visible = VarControl.GetParam_EngineStartStopEnable();

		EngineStopButton->Height = BTN_SIZE_H;
		EngineStopButton->Width = BTN_SIZE_H * 2;
		EngineStopButton->Left = EngineButton->Left + EngineButton->Width + LAYOUT_GUI_Margin;
		EngineStopButton->Top = EngineButton->Top;
		EngineStopButton->Font->Height = FONTSIZE_SMALL;
		EngineStopButton->Visible = VarControl.GetParam_EngineStartStopEnable();

		EngineCtlPanel->ClientWidth = EngineStopButton->Left + EngineStopButton->Width;
		EngineCtlGroupBox->Width = EngineCtlPanel->Width + 2 * LAYOUT_GUI_Margin;
		EngineCtlGroupBox->Visible = VarControl.GetParam_EngineRevolutionChangeable() | VarControl.GetParam_EngineStartStopEnable();

		// Taste Freigabe nach Grenzwertüberschreitung
//		ResetButton->Height = WinchDrivePanel->ClientHeight * LAYOUT_ResetButton_HEIGHT_PERC / 100;
//        ClearAfterThresholdButton->Width = ControlPanel->ClientWidth * LAYOUT_ClearAfterThresholdButton_WIDTH_PERC / 100;
//		ResetButton->Width = ResetButton->Height * LAYOUT_ResetButtonProp_WIDTH;
//		ResetButton->Top = WinchDrivePanel->ClientHeight - LAYOUT_GUI_Margin - ResetButton->Height;
//		ResetButton->Left = WinchDrivePanel->ClientWidth - LAYOUT_GUI_Margin - ResetButton->Width;

		// Glyphs
		try
		{
				// Speicher allokieren
				std::auto_ptr<Graphics::TBitmap> DummyImage1(new Graphics::TBitmap());
				std::auto_ptr<Graphics::TBitmap> DummyImage2(new Graphics::TBitmap());

				// Motor "Ein" Bitmap
				DummyImage1->LoadFromResourceName((int)HInstance, "ENGINE_START");
				DummyImage2->Height = abs(EngineButton->Font->Height);
				DummyImage2->Width = 2 * DummyImage2->Height;
				DummyImage2->Canvas->StretchDraw(Rect(0, 0, DummyImage2->Width, DummyImage2->Height), DummyImage1.get());
				EngineButton->Glyph->Assign(DummyImage2.get());
				EngineButton->NumGlyphs = 2;

				// Motor "Aus" Bitmap
				DummyImage1->LoadFromResourceName((int)HInstance, "ENGINE_STOP");
				DummyImage2->Height = abs(EngineStopButton->Font->Height);
				DummyImage2->Width = 2 * DummyImage2->Height;
				DummyImage2->Canvas->StretchDraw(Rect(0, 0, DummyImage2->Width, DummyImage2->Height), DummyImage1.get());
				EngineStopButton->Glyph->Assign(DummyImage2.get());
				EngineStopButton->NumGlyphs = 2;

				// Drehzahl erhöhen Bitmap
				DummyImage1->LoadFromResourceName((int)HInstance, "GREENPLUS");
				DummyImage2->Height = abs(EngineRevolutionUpButton->Font->Height);
				DummyImage2->Width = 2 * DummyImage2->Height;
				DummyImage2->Canvas->StretchDraw(Rect(0, 0, DummyImage2->Width, DummyImage2->Height), DummyImage1.get());
				EngineRevolutionUpButton->Glyph->Assign(DummyImage2.get());
				EngineRevolutionUpButton->NumGlyphs = 2;
//                EngineRevolutionUpButton->Margin = (EngineRevolutionUpButton->Height - abs(EngineRevolutionUpButton->Font->Height)) / 2;

				// Drehzahl verringern Bitmap
				DummyImage1->LoadFromResourceName((int)HInstance, "GREENMINUS");
				DummyImage2->Height = abs(EngineRevolutionDownButton->Font->Height);
				DummyImage2->Width = 2 * DummyImage2->Height;
				DummyImage2->Canvas->StretchDraw(Rect(0, 0, DummyImage2->Width, DummyImage2->Height), DummyImage1.get());
				EngineRevolutionDownButton->Glyph->Assign(DummyImage2.get());
				EngineRevolutionDownButton->NumGlyphs = 2;
//                EngineRevolutionDownButton->Margin = (EngineRevolutionDownButton->Height - abs(EngineRevolutionDownButton->Font->Height)) / 2;

				// ------
				// SignalHorn-Taste

				DummyImage2->Height = (SignalHornButton->Height - (2 * LAYOUT_GUI_Margin));
				DummyImage2->Width = 2 * DummyImage2->Height;

				DummyImage1->LoadFromResourceName((int)HInstance, "ACOUSTICSIGNAL");
				DummyImage2->Canvas->StretchDraw(Rect(0, 0, DummyImage2->Width, DummyImage2->Height), DummyImage1.get());
				SignalHornButton->Glyph->Assign(DummyImage2.get());
				SignalHornButton->NumGlyphs = 2;

/*					if (DummyImage2->Height > LAYOUT_ControlPanelButtonSymbolSize) DummyImage2->Height = LAYOUT_ControlPanelButtonSymbolSize; // optimale Darstellung anstreben, wenn Taste groß genug

				DummyImage2->Height = SignalHornButton->ClientHeight - (2 * LAYOUT_GUI_Margin);
				DummyImage2->Width = 2 * DummyImage2->Height;
				DummyImage2->Canvas->StretchDraw(Rect(0, 0, DummyImage2->Width, DummyImage2->Height), DummyImage1.get());
				SignalHornButton->Glyph->Assign(DummyImage2.get());
				SignalHornButton->NumGlyphs = 2;
*/


		}catch(...)
		{
		};

		MainCtlPanel->Refresh();
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von XtraCtlPageScroller
void __fastcall TMainForm::MakeXtraCtlPageScrollerLayout()
{
	XtraCtlPageScroller->Height = ClientHeight / LAYOUT_SubCtlPanel_PROP_HEIGHT;

	// -------------
	XtraEngineCtlGroupBox->Font->Height = FONTSIZE_SMALL;
	// Alle Positionsangaben relativ zu XtraEngineCtlPanel

	EngineSpeedView->Width = XtraEngineCtlPanel->ClientHeight;
	EngineCoolantTemperatureView->Width = XtraEngineCtlPanel->ClientHeight / 5;

	XtraEngineCtlPanel->ClientWidth = EngineCoolantTemperatureView->Left + EngineCoolantTemperatureView->Width;
	XtraEngineCtlGroupBox->Width = 2 * LAYOUT_GUI_Margin + XtraEngineCtlPanel->Width;

	// -------------
	HydraulicCtlGroupBox->Font->Height = FONTSIZE_SMALL;
	// Alle Positionsangaben relativ zu HydraulicCtlPanel
	HydrWrkPressureView->Width = HydraulicCtlPanel->ClientHeight;
	HydrTemperatureView->Width = HydraulicCtlPanel->ClientHeight / 5;
	HydrLvlView->Width = HydraulicCtlPanel->ClientHeight / 5;

	HydraulicCtlPanel->ClientWidth = HydrLvlView->Left + HydrLvlView->Width;
	HydraulicCtlGroupBox->Width = 2 * LAYOUT_GUI_Margin + HydraulicCtlPanel->Width;

	// -------------
	WinchDriveCtlGroupBox->Font->Height = FONTSIZE_SMALL;
	// Alle Positionsangaben relativ zu WinchDriveCtlPanel
	// Freilauf
	LostMotionImpulseButton->Height = BTN_SIZE_H;
	LostMotionImpulseButton->Width = BTN_SIZE_H * 2;
	LostMotionImpulseButton->Top = LAYOUT_GUI_Margin;
	LostMotionImpulseButton->Left = LAYOUT_GUI_Margin;
	LostMotionImpulseButton->Font->Height = FONTSIZE_SMALL;

	// Bremse lösen
	WinchBrakeOffSpeedButton->Height = BTN_SIZE_H;
	WinchBrakeOffSpeedButton->Width = BTN_SIZE_H * 2;
	WinchBrakeOffSpeedButton->Top = LostMotionImpulseButton->Top + LostMotionImpulseButton->Height + LAYOUT_GUI_Margin;
	WinchBrakeOffSpeedButton->Left = LAYOUT_GUI_Margin;
	WinchBrakeOffSpeedButton->Font->Height = FONTSIZE_SMALL;
	WinchBrakeOffSpeedButton->Visible = VarControl.GetParam_BrakeOffEnable();

	// Schnell/Langsam
	FastRunButton->Height = BTN_SIZE_H;
	FastRunButton->Width = BTN_SIZE_H * 2;
	FastRunButton->Top = WinchBrakeOffSpeedButton->Top + WinchBrakeOffSpeedButton->Height + LAYOUT_GUI_Margin;
	FastRunButton->Left = LAYOUT_GUI_Margin;
	FastRunButton->Font->Height = FONTSIZE_SMALL;

	WinchDriveCtlPanel->ClientWidth = FastRunButton->Left + FastRunButton->Width;
	WinchDriveCtlGroupBox->Width = 2 * LAYOUT_GUI_Margin + WinchDriveCtlPanel->Width;

	XtraCtlPagePanel->Width = XtraEngineCtlGroupBox->Left + HydraulicCtlGroupBox->Width + WinchDriveCtlGroupBox->Width;
	XtraCtlPageScroller->Refresh();

}

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von ControlLampPanel
void __fastcall TMainForm::MakeControlLampPanelLayout()
{
        int HV, HV2;

		ControlLampPanel->Width = (MessageLight->ClientWidth * 8 / 100);

		// Die verwendeten Symbole sind für
		// LAYOUT_ControlLampPanelSymbolSize x LAYOUT_ControlLampPanelSymbolSize Punkte optimiert.
		// Deshalb wird das Lampenfeld dahingehend angepasst.
		// Verfügbaren Platz in "ControlLampHelpPanel" feststellen
		HV2 = LAYOUT_ControlLampPanelSymbolSize + (2 * LAYOUT_GUI_Margin);      // Für Symbol idealerweise Benötigter Platz
		HV = ControlLampPanel->ClientWidth - (2 * LAYOUT_GUI_Margin);       	// 2 * LAYOUT_GUI_Margin = Abstand von Symbol/Button zum Rand
		if (HV > HV2) ControlLampPanel->Width -= HV - HV2;                      // Abziehen der Differenz zw. vorhandenem und benötigten Platz
		HV = ControlLampPanel->ClientWidth - (2 * LAYOUT_GUI_Margin);

		// Dimensionierung der Elemente
		AlarmSoundOnOffButton->Width			= HV;
		AlarmSoundOnOffButton->Height			= HV;
		EmergencySwitch_DeadButton->Width       = HV;
		EmergencySwitch_DeadButton->Height      = HV;
        AutomotiveError_DeadButton->Width       = HV;
        AutomotiveError_DeadButton->Height      = HV;
        ChangeOilFilter_DeadButton->Width       = HV;
        ChangeOilFilter_DeadButton->Height      = HV;
        AuxiallyDrive_DeadButton->Width         = HV;
		AuxiallyDrive_DeadButton->Height        = HV;
        MagneticMarkOnOffButton->Width          = HV;
        MagneticMarkOnOffButton->Height         = HV;
        DynamicTensionOnOffButton->Width        = HV;
        DynamicTensionOnOffButton->Height       = HV;
		PowerFailCdResetSpeedButton->Width      = HV;
		PowerFailCdResetSpeedButton->Height     = HV;
		SpecialFuncButton->Width				= HV;
		SpecialFuncButton->Height				= HV;
		MainMenuSpeedButton->Width				= HV;
        MainMenuSpeedButton->Height				= HV;

        // Anordnung der Elemente
		AlarmSoundOnOffButton->Left				= LAYOUT_GUI_Margin;
		AlarmSoundOnOffButton->Top				= LAYOUT_GUI_Margin;
		EmergencySwitch_DeadButton->Left        = LAYOUT_GUI_Margin;
		EmergencySwitch_DeadButton->Top         = AlarmSoundOnOffButton->Height + (2 * LAYOUT_GUI_Margin);
		AutomotiveError_DeadButton->Left        = LAYOUT_GUI_Margin;
		AutomotiveError_DeadButton->Top         = EmergencySwitch_DeadButton->Top + EmergencySwitch_DeadButton->Height + (2 * LAYOUT_GUI_Margin);
        ChangeOilFilter_DeadButton->Left        = LAYOUT_GUI_Margin;
        ChangeOilFilter_DeadButton->Top         = AutomotiveError_DeadButton->Top + AutomotiveError_DeadButton->Height + (2 * LAYOUT_GUI_Margin);
        AuxiallyDrive_DeadButton->Left          = LAYOUT_GUI_Margin;
        AuxiallyDrive_DeadButton->Top           = ChangeOilFilter_DeadButton->Top + ChangeOilFilter_DeadButton->Height + (2 * LAYOUT_GUI_Margin);
        MagneticMarkOnOffButton->Left           = LAYOUT_GUI_Margin;
        MagneticMarkOnOffButton->Top            = AuxiallyDrive_DeadButton->Top + AuxiallyDrive_DeadButton->Height + (2 * LAYOUT_GUI_Margin);
        DynamicTensionOnOffButton->Left         = LAYOUT_GUI_Margin;
        DynamicTensionOnOffButton->Top          = MagneticMarkOnOffButton->Top + MagneticMarkOnOffButton->Height + (2 * LAYOUT_GUI_Margin);
		PowerFailCdResetSpeedButton->Left       = LAYOUT_GUI_Margin;
		PowerFailCdResetSpeedButton->Top        = DynamicTensionOnOffButton->Top + DynamicTensionOnOffButton->Height + (2 * LAYOUT_GUI_Margin);
		MainMenuSpeedButton->Left				= LAYOUT_GUI_Margin;
		MainMenuSpeedButton->Top				= ControlLampPanel->ClientHeight - MainMenuSpeedButton->Height - (3 * LAYOUT_GUI_Margin);
		SpecialFuncButton->Left					= LAYOUT_GUI_Margin;
		SpecialFuncButton->Top					= PowerFailCdResetSpeedButton->Top + PowerFailCdResetSpeedButton->Height + (2 * LAYOUT_GUI_Margin);

		PowerFailCdResetSpeedButton->Font->Height = PowerFailCdResetSpeedButton->Height / 2;

		SpecialFuncButton->Visible 				= VarControl.GetParam_LineLubAvailable();


		// Glyphs
		try {
			TSpeedButton *HButton;

			// Hilfsbilder vorbereiten
			std::auto_ptr<Graphics::TBitmap> DummyImageOriginal(new Graphics::TBitmap());
			std::auto_ptr<Graphics::TBitmap> DummyImageComplete(new Graphics::TBitmap());

			// ------
			// Hauptmenü-Taste
			HButton = MainMenuSpeedButton;

			// Hilfsbilder Zuschneiden
			DummyImageComplete->Height = (HButton->Height - (2 * LAYOUT_GUI_Margin));
			DummyImageComplete->Width = 2 * DummyImageComplete->Height;

			// Je nach Button-Größe wird das passendste Bild geladen
			if (DummyImageComplete->Height < 32)
				DummyImageOriginal->LoadFromResourceName((int)HInstance, "MAINMENU_16X16_2");
			else if (DummyImageComplete->Height < 64)
				DummyImageOriginal->LoadFromResourceName((int)HInstance, "MAINMENU_32X32_2");
			else
				DummyImageOriginal->LoadFromResourceName((int)HInstance, "MAINMENU_64X64_2");

			// Bilder einfügen
			DummyImageComplete->Canvas->StretchDraw(Rect(0, 0, DummyImageComplete->Width, DummyImageComplete->Height), DummyImageOriginal.get());

			// An Komponente übergeben
			HButton->Glyph->Assign(DummyImageComplete.get());

			// ------
			// Arlarmsound-Taste
			HButton = AlarmSoundOnOffButton;

			DummyImageComplete->Height = (HButton->Height - (2 * LAYOUT_GUI_Margin));
			DummyImageComplete->Width = 2 * DummyImageComplete->Height;

			DummyImageOriginal->LoadFromResourceName((int)HInstance, "ALARMSOUNDOFF");
			DummyImageComplete->Canvas->StretchDraw(Rect(0, 0, DummyImageComplete->Width, DummyImageComplete->Height), DummyImageOriginal.get());
			HButton->Glyph->Assign(DummyImageComplete.get());

			// ------
			// Sonderfunktions-Taste
			HButton = SpecialFuncButton;

			// Hilfsbilder Zuschneiden
			DummyImageComplete->Height = (HButton->Height - (2 * LAYOUT_GUI_Margin));
			DummyImageComplete->Width = 2 * DummyImageComplete->Height;

			// Je nach Button-Größe wird das passendste Bild geladen
			if (DummyImageComplete->Height < 32)
				DummyImageOriginal->LoadFromResourceName((int)HInstance, "LINELUBRICATOR_16X16_2");
			else if (DummyImageComplete->Height < 64)
				DummyImageOriginal->LoadFromResourceName((int)HInstance, "LINELUBRICATOR_32X32_2");
			else
				DummyImageOriginal->LoadFromResourceName((int)HInstance, "LINELUBRICATOR_64X64_2");

			// Bilder einfügen
			DummyImageComplete->Canvas->StretchDraw(Rect(0, 0, DummyImageComplete->Width, DummyImageComplete->Height), DummyImageOriginal.get());

			// An Komponente übergeben
			HButton->Glyph->Assign(DummyImageComplete.get());

		} catch(...) {};


		ControlLampPanel->Refresh();
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von CalibDepthPanel
void __fastcall TMainForm::MakeCalibDepthPanelLayout()
{
	int HV;

	// Schriftgrößen
	// Welche von der Standardschriftgröße abweichen
	CalibDepthCaptionLabel->Font->Height = FONTSIZE_BIG;
	CalibDepthInfoLabel->Font->Height = FONTSIZE_SMALL;

	// Welche separat zu setzen sind
	CalibDepth_DepthEdit->Font = CalibDepthFramePanel->Font;
	CalibDepthInfo_ImpulseCount_Label->Font->Height = FONTSIZE_SMALL;
	CalibDepthInfo_ActiveCalFact_Label->Font->Height = FONTSIZE_SMALL;
	CalibDepthInfo_EarmarkedCalFact_Label->Font->Height = FONTSIZE_SMALL;
	CalibDepthInfo_EarmarkedCalFact_Label->Font->Style = TFontStyles() << fsBold;

	// Boxhöhe inhaltsabhängig
	CalibDepth_DepthEdit->Margins->Top = CalibDepth_DepthEdit->Font->Height;
	CalibDepthEdit_GroupBox->ClientHeight = CalibDepth_DepthEdit->Top + CalibDepth_DepthEdit->Height + CalibDepthEdit_GroupBox->Font->Height;

	// Tasten
	CalibDepthAbortButton->Font->Height = FONTSIZE_BIG;
	CalibDepthAbortButton->Height = BTN_SIZE_H;
	CalibDepthAbortButton->Width = BTN_SIZE_H;
	CalibDepthAbortButton->Top = CalibDepthFramePanel->ClientHeight - CalibDepthAbortButton->Height;
	CalibDepthAbortButton->Left = CalibDepthFramePanel->ClientWidth - CalibDepthAbortButton->Width - LAYOUT_GUI_Margin;

	CalibDepthAcceptButton->Height = BTN_SIZE_H;
	CalibDepthAcceptButton->Width = BTN_SIZE_W;
	CalibDepthAcceptButton->Top = CalibDepthAbortButton->Top;
	CalibDepthAcceptButton->Left = CalibDepthAbortButton->Left - CalibDepthAcceptButton->Width - LAYOUT_GUI_Margin;
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von CalibTractionPanel
void __fastcall TMainForm::MakeCalibTractionPanelLayout()
{
        int HV;

		// Schriftgrößen
		CalibTractionCaptionLabel->Font->Height = FONTSIZE_BIG;

		CalibTractionCompleteBox->Font->Height = FONTSIZE_SMALL;
		CalibTractionMessure1Button->Font->Height = FONTSIZE_NORMAL;
		CalibTractionMessure2Button->Font->Height = FONTSIZE_NORMAL;
		CalibTension_Tension1Edit->Font = CalibTractionMessure1Button->Font;
		CalibTension_Tension2Edit->Font = CalibTension_Tension1Edit->Font;
		CalibTractionCompleteSetButton->Font->Height = FONTSIZE_NORMAL;

		CalibTractionZeroBox->Font->Height = FONTSIZE_SMALL;
		CalibTractionGetZeroButton->Font->Height = FONTSIZE_NORMAL;

		CalibTractionAbortButton->Font->Height = FONTSIZE_BIG;

		// Abmessungen
		// Tastenhöhen/Eingabefeld-Höhen
		CalibTractionMessure1Button->Height = BTN_SIZE_H;
		CalibTension_Tension1Edit->EditFieldWidth = BTN_SIZE_W;
		CalibTractionMessure2Button->Height = BTN_SIZE_H;
		CalibTension_Tension2Edit->EditFieldWidth = BTN_SIZE_W;
		CalibTractionCompleteSetButton->Height = BTN_SIZE_H;
		CalibTractionGetZeroButton->Height = BTN_SIZE_H;
		CalibTractionAbortButton->Height = BTN_SIZE_H;

		// Tastenbreiten/Eingabefeld-Breiten
		CalibTractionMessure1Button->Width = BTN_SIZE_W;
		CalibTractionMessure2Button->Width = BTN_SIZE_W;
		CalibTractionCompleteSetButton->Width = BTN_SIZE_W;
		CalibTractionGetZeroButton->Width = BTN_SIZE_W;
		CalibTractionAbortButton->Width = BTN_SIZE_H;

		// Positionierung der Objekte

		// Kalibrierung komplett
		HV = abs(CalibTractionCompleteBox->Font->Height);

		CalibTractionCompleteInfoLabel->AlignWithMargins = true;
		CalibTractionCompleteInfoLabel->Margins->Left = HV;
		CalibTractionCompleteInfoLabel->Margins->Right = HV;
		CalibTractionCompleteInfoLabel->Margins->Top = HV;
		CalibTractionCompleteInfoLabel->Margins->Bottom = HV;

		CalibTractionMessure1Button->Top = CalibTractionCompleteInfoLabel->Top + CalibTractionCompleteInfoLabel->Height + LAYOUT_GUI_Margin;
		CalibTractionMessure1Button->Left = HV;

		CalibTractionMessure2Button->Top = CalibTractionMessure1Button->Top + CalibTractionMessure1Button->Height + LAYOUT_GUI_Margin;
		CalibTractionMessure2Button->Left = HV;

		CalibTension_Tension1Edit->Top = CalibTractionMessure1Button->Top + (CalibTractionMessure1Button->Height - CalibTension_Tension1Edit->Height) / 2;
		CalibTension_Tension1Edit->Left = CalibTractionMessure1Button->Left + CalibTractionMessure1Button->Width + LAYOUT_GUI_Margin;

		CalibTension_Tension2Edit->Top = CalibTractionMessure2Button->Top + (CalibTractionMessure2Button->Height - CalibTension_Tension2Edit->Height) / 2;
		CalibTension_Tension2Edit->Left = CalibTractionMessure2Button->Left + CalibTractionMessure2Button->Width + LAYOUT_GUI_Margin;

		CalibTractionCompleteSetButton->Top = CalibTractionMessure2Button->Top + CalibTractionMessure2Button->Height + LAYOUT_GUI_Margin;
		CalibTractionCompleteSetButton->Left = CalibTractionCompleteBox->ClientWidth - CalibTractionCompleteSetButton->Width - HV;

		// Kalibrierung Nullpunkt
		HV = abs(CalibTractionZeroBox->Font->Height);

		CalibTractionZeroInfoLabel->AlignWithMargins = true;
		CalibTractionZeroInfoLabel->Margins->Left = HV;
		CalibTractionZeroInfoLabel->Margins->Right = HV;
		CalibTractionZeroInfoLabel->Margins->Top = HV;
		CalibTractionZeroInfoLabel->Margins->Bottom = HV;

		CalibTractionGetZeroButton->Top = CalibTractionZeroInfoLabel->Top + CalibTractionZeroInfoLabel->Height + LAYOUT_GUI_Margin;
		CalibTractionGetZeroButton->Left = HV;

		CalibTractionAbortButton->Top = CalibTractionPanel->ClientHeight - CalibTractionAbortButton->Height - LAYOUT_GUI_Margin;
		CalibTractionAbortButton->Left = CalibTractionPanel->ClientWidth - CalibTractionAbortButton->Width - LAYOUT_GUI_Margin;

		// Höhe der umschließenden Boxen ist abhängig vom Inhalt
		CalibTractionCompleteBox->ClientHeight = CalibTractionCompleteSetButton->Top + CalibTractionCompleteSetButton->Height + abs(CalibTractionCompleteBox->Font->Height);
		CalibTractionZeroBox->ClientHeight = CalibTractionGetZeroButton->Top + CalibTractionGetZeroButton->Height + abs(CalibTractionZeroBox->Font->Height);

		CalibTractionPanel->Refresh();
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von FunctionPanel
void __fastcall TMainForm::MakeChoicePanelLayout()
{
		// ChoicePanel

		// Schriftgröße
		ChoiceCaptionLabel->Font->Height = FONTSIZE_BIG;

		// Tastendimensionen
		ChoiceButton1->Height = BTN_SIZE_H;
		ChoiceButton2->Height = BTN_SIZE_H;
		ChoiceButton3->Height = BTN_SIZE_H;
		ChoiceButton4->Height = BTN_SIZE_H;
		ChoiceButton5->Height = BTN_SIZE_H;

		ChoiceButton1->Width = BTN_SIZE_H;
		ChoiceButton2->Width = BTN_SIZE_H;
		ChoiceButton3->Width = BTN_SIZE_H;
		ChoiceButton4->Width = BTN_SIZE_H;
		ChoiceButton5->Width = BTN_SIZE_H;

		// Bildschirmaufteilung vertikal
		ChoiceCaptionLabel->Top = 2 * LAYOUT_GUI_Margin;
		ChoiceCaptionLabel->Left = 2 * LAYOUT_GUI_Margin;

		ChoiceButton1->Top = ChoiceCaptionLabel->Top + ChoiceCaptionLabel->Height + BTN_SIZE_H;
		ChoiceLabel1->Top = ChoiceButton1->Top + (ChoiceButton1->Height - ChoiceLabel1->Height) / 2;

		ChoiceButton2->Top = ChoiceButton1->Top + 2 * BTN_SIZE_H;
		ChoiceLabel2->Top = ChoiceButton2->Top + (ChoiceButton2->Height - ChoiceLabel2->Height) / 2;

		ChoiceButton3->Top = ChoiceButton2->Top + 2 * BTN_SIZE_H;
		ChoiceLabel3->Top = ChoiceButton3->Top + (ChoiceButton3->Height - ChoiceLabel3->Height) / 2;

		ChoiceButton4->Top = ChoiceButton3->Top + 2 * BTN_SIZE_H;
		ChoiceLabel4->Top = ChoiceButton4->Top + (ChoiceButton4->Height - ChoiceLabel4->Height) / 2;

		ChoiceButton5->Top = ChoiceButton4->Top + 2 * BTN_SIZE_H;
		ChoiceLabel5->Top = ChoiceButton5->Top + (ChoiceButton5->Height - ChoiceLabel5->Height) / 2;

		// Bildschirmaufteilung horizontal
		ChoiceButton1->Left = ChoiceButton1->Width;
		ChoiceButton2->Left = ChoiceButton2->Width;
		ChoiceButton3->Left = ChoiceButton3->Width;
		ChoiceButton4->Left = ChoiceButton4->Width;
		ChoiceButton5->Left = ChoiceButton5->Width;
		ChoiceLabel1->Left = ChoiceButton1->Left + ChoiceButton1->Width + LAYOUT_GUI_Margin;
		ChoiceLabel2->Left = ChoiceButton2->Left + ChoiceButton2->Width + LAYOUT_GUI_Margin;
		ChoiceLabel3->Left = ChoiceButton3->Left + ChoiceButton3->Width + LAYOUT_GUI_Margin;
		ChoiceLabel4->Left = ChoiceButton4->Left + ChoiceButton4->Width + LAYOUT_GUI_Margin;
		ChoiceLabel5->Left = ChoiceButton5->Left + ChoiceButton5->Width + LAYOUT_GUI_Margin;

		// Abbruchtaste
		ChoiceAbortButton->Height = BTN_SIZE_H;
		ChoiceAbortButton->Width = BTN_SIZE_W;
		ChoiceAbortButton->Left = ChoicePanel->ClientWidth - ChoiceAbortButton->Width - LAYOUT_GUI_Margin;
		ChoiceAbortButton->Top = ChoicePanel->ClientHeight - ChoiceAbortButton->Height - LAYOUT_GUI_Margin;

		ChoicePanel->Refresh();
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von DialogPanel
void __fastcall TMainForm::MakeDialogPanelLayout()
{
//        DialogPanel->Font->Height = DialogPanel->ClientHeight * LAYOUT_DialogPanelFont_HEIGHT_PERC / 100;
        // Überschrift
		DialogCaptionLabel->Font->Height = FONTSIZE_BIG;
//        DialogCaptionLabel->Top = DialogPanel->Height / 4;

        // Informationstext/Frage
        DialogInfoLabel->Top = (DialogPanel->Height / 2) - DialogInfoLabel->Height;

        // Tasten
		DialogOkSpeedButton->Height = BTN_SIZE_H;
		DialogOkSpeedButton->Width = BTN_SIZE_W;
		DialogCancelSpeedButton->Height = BTN_SIZE_H;
        DialogCancelSpeedButton->Width = BTN_SIZE_W;

        DialogOkSpeedButton->Left = (DialogPanel->Width / 2) - DialogOkSpeedButton->Width - LAYOUT_GUI_Margin;
        DialogOkSpeedButton->Top = DialogPanel->Height * 3 / 4;

        DialogCancelSpeedButton->Left = (DialogPanel->Width / 2) + LAYOUT_GUI_Margin;
        DialogCancelSpeedButton->Top = DialogPanel->Height * 3 / 4;
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von UnloadCablePanel
void __fastcall TMainForm::MakeUnloadCablePanelLayout()
{
        int HV;

		// Schriftgrößen
		UnloadCablePanelCaptionLabel->Font->Height = FONTSIZE_BIG;
		UnloadCableTopHelpPanel->Font->Height = FONTSIZE_NORMAL;
		UnloadCableSetTensionEdit->Font = UnloadCableTopHelpPanel->Font;
		UnloadCableStretchScrollBox->Font->Height = FONTSIZE_NORMAL;
		UnloadCableStretchGroupBox->Font->Height = FONTSIZE_SMALL;
		UnloadCabelStretchCheckBox->Font->Height = FONTSIZE_NORMAL;
		UnloadCableStretchStartLenghtEdit->Font = UnloadCableStretchScrollBox->Font;
		UnloadCableStretchEndLenghtEdit->Font = UnloadCableStretchScrollBox->Font;
		UnloadCableStretchStartTensionEdit->Font = UnloadCableStretchScrollBox->Font;
		UnloadCableStretchEndTensionEdit->Font = UnloadCableStretchScrollBox->Font;

		// Buttons und dessen Panel
		UnloadCableExitButton->Height = BTN_SIZE_H;
		UnloadCableExitButton->Width = BTN_SIZE_H;
		UnloadCableExitButton->Top = LAYOUT_GUI_Margin;
		UnloadCableExitButton->Left = UnloadCableHelpPanel->ClientWidth - UnloadCableExitButton->Width - LAYOUT_GUI_Margin;
		UnloadCableExitButton->Font->Height = FONTSIZE_BIG;

		UnloadCableHelpPanel->ClientHeight = 2 * UnloadCableExitButton->Top + UnloadCableExitButton->Height;

		UnloadCableMinTractionUpDown->Left = UnloadCableSetTensionEdit->Left + UnloadCableSetTensionEdit->Width + LAYOUT_GUI_Margin;
		UnloadCableMinTractionUpDown->Top = UnloadCableSetTensionEdit->Top;
		UnloadCableMinTractionUpDown->Height = UnloadCableSetTensionEdit->Height;
		UnloadCableMinTractionUpDown->Width = UnloadCableMinTractionUpDown->Height * 4;

		// UnloadCableStretchGroupBox
		UnloadCableStretchScrollBox->VertScrollBar->Position = 0;
		UnloadCableStretchScrollBox->HorzScrollBar->Position = 0;

		// Abmessungen
		UnloadCabelStretchCheckBox->Height = abs(UnloadCabelStretchCheckBox->Font->Height);

		// Breiteste Überschrift finden un Eingabefeld Positionieren
		HV = UnloadCableStretchStartLenghtEdit->CaptionWidth;
		if (HV < UnloadCableStretchEndLenghtEdit->CaptionWidth) HV = UnloadCableStretchEndLenghtEdit->CaptionWidth;
		if (HV < UnloadCableStretchStartTensionEdit->CaptionWidth) HV = UnloadCableStretchStartTensionEdit->CaptionWidth;
		if (HV < UnloadCableStretchEndTensionEdit->CaptionWidth) HV = UnloadCableStretchEndTensionEdit->CaptionWidth;
		HV += LAYOUT_GUI_Margin;
		UnloadCableStretchStartLenghtEdit->EditFieldLeftPos = HV;
		UnloadCableStretchStartLenghtEdit->EditFieldWidth = UnloadCableStretchStartLenghtEdit->Height * 4;
		UnloadCableStretchEndLenghtEdit->EditFieldLeftPos = HV;
		UnloadCableStretchEndLenghtEdit->EditFieldWidth = UnloadCableStretchEndLenghtEdit->Height * 4;
		UnloadCableStretchStartTensionEdit->EditFieldLeftPos = HV;
		UnloadCableStretchStartTensionEdit->EditFieldWidth = UnloadCableStretchStartTensionEdit->Height * 4;
		UnloadCableStretchEndTensionEdit->EditFieldLeftPos = HV;
		UnloadCableStretchEndTensionEdit->EditFieldWidth = UnloadCableStretchEndTensionEdit->Height * 4;

		// Größe des Containers festlegen
		UnloadCableStretchGroupBox->ClientHeight = UnloadCableStretchEndTensionEdit->Top + UnloadCableStretchEndTensionEdit->Height + 2 * LAYOUT_GUI_Margin;

		UnloadCablePanel->Refresh();
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von FileAdminPanel
void __fastcall TMainForm::MakeFileAdminPanelLayout()
{
		int HV;
		TFontCharset oldCharset;

		// Schriftgrößen
		LOGFileCaptionLabel->Font->Height = FONTSIZE_BIG;

		LOGFileCommentsGroupBox->Font->Height = FONTSIZE_SMALL;
		LOGFileCommentsMemo->Font->Height = FONTSIZE_NORMAL;

		LOGFileLanguageFormatGroupBox->Font->Height = FONTSIZE_SMALL;
//		LogFileLanguageFormatPanel->Font->Height = FONTSIZE_NORMAL;

		LOGFileTypeGroupBox->Font->Height = FONTSIZE_SMALL;
//		LOGFileTypePanel->Font->Height = FONTSIZE_NORMAL;

		LOGFileTensionUnitGroupBox->Font->Height = FONTSIZE_SMALL;
//		LogFileTensionUnitPanel->Font->Height = FONTSIZE_NORMAL;

		LOGFileAdminFileListBox->Font->Height = FONTSIZE_SMALL;

		LOGFileAdminLanguageGroupBox->Font->Height = FONTSIZE_SMALL;


		// Dimensionierung und Anordnung Tasten
		LOGFileCancelButton->Height = BTN_SIZE_H;
		LOGFileCancelButton->Width = BTN_SIZE_W;
		LOGFileOkButton->Height = BTN_SIZE_H;
		LOGFileOkButton->Width = BTN_SIZE_W;
		LOGFileCancelButton->Top = LAYOUT_GUI_Margin;
		LOGFileCancelButton->Left = LOGFileButtonPanel->ClientWidth - LOGFileCancelButton->Width;
		LOGFileOkButton->Top = LOGFileCancelButton->Top;
		LOGFileOkButton->Left = LOGFileCancelButton->Left - LOGFileOkButton->Width - LAYOUT_GUI_Margin;
		LOGFileButtonPanel->Height = LOGFileOkButton->Height + 2 * LAYOUT_GUI_Margin;

		// -----------------------------
		// LOGFileCoreDataTabSheet

		LOGFileCharacteristicsScrollBox->VertScrollBar->Margin = 2000;			// Trick, damit die vertikale Bildlaufleiste sichtbar ist und deren Breite im Layout berücksichtigt wird

		// Ausrichtung oben Links
		LOGFileLocationLabel->Left = 0;
		LOGFileLocationLabel->Top = 0;
		LOGFileWellLabel->Left = 0;
		LOGFileWellLabel->Top = LOGFileLocationLabel->Top + LOGFileLocationMaskEdit->Height + LAYOUT_GUI_Margin;
		LOGFileNameLabel->Left = 0;
		LOGFileNameLabel->Top = LOGFileWellLabel->Top + LOGFileWellMaskEdit->Height + LAYOUT_GUI_Margin;
		LOGFileTeamLabel->Left = 0;
		LOGFileTeamLabel->Top = LOGFileNameLabel->Top + LOGFileNameMaskEdit->Height + LAYOUT_GUI_Margin;
		LOGFileWinchMachineryLabel->Left = 0;
		LOGFileWinchMachineryLabel->Top = LOGFileTeamLabel->Top + LOGFileTeamMaskEdit->Height + LAYOUT_GUI_Margin;

		// Ausrichtung 2. Spalte und Breiteneinstellung
		HV = LOGFileLocationLabel->Width;
		if (HV < LOGFileWellLabel->Width) HV = LOGFileWellLabel->Width;
		if (HV < LOGFileNameLabel->Width) HV = LOGFileNameLabel->Width;
		if (HV < LOGFileTeamLabel->Width) HV = LOGFileTeamLabel->Width;
		if (HV < LOGFileWinchMachineryLabel->Width) HV = LOGFileWinchMachineryLabel->Width;
		LOGFileLocationMaskEdit->Left = HV + LOGFileLocationLabel->Left + LAYOUT_GUI_Margin;
		LOGFileLocationMaskEdit->Top = LOGFileLocationLabel->Top;
		LOGFileLocationMaskEdit->Width = LOGFileCharacteristicsScrollBox->ClientWidth - HV - LOGFileWellLabel->Left - LOGFileCharacteristicsScrollBox->VertScrollBar->Size - 2 * LAYOUT_GUI_Margin;
		LOGFileWellMaskEdit->Left = HV + LOGFileWellLabel->Left + LAYOUT_GUI_Margin;
		LOGFileWellMaskEdit->Top = LOGFileWellLabel->Top;
		LOGFileWellMaskEdit->Width = LOGFileLocationMaskEdit->Width;
		LOGFileNameMaskEdit->Left = HV + LOGFileNameLabel->Left + LAYOUT_GUI_Margin;
		LOGFileNameMaskEdit->Top = LOGFileNameLabel->Top;
		LOGFileNameMaskEdit->Width = LOGFileLocationMaskEdit->Width;
		LOGFileTeamMaskEdit->Left = HV + LOGFileTeamLabel->Left + LAYOUT_GUI_Margin;
		LOGFileTeamMaskEdit->Top = LOGFileTeamLabel->Top;
		LOGFileTeamMaskEdit->Width = LOGFileLocationMaskEdit->Width;
		LOGFileWinchMachineryMaskEdit->Left = HV + LOGFileWinchMachineryLabel->Left + LAYOUT_GUI_Margin;
		LOGFileWinchMachineryMaskEdit->Top = LOGFileWinchMachineryLabel->Top;
		LOGFileWinchMachineryMaskEdit->Width = LOGFileLocationMaskEdit->Width;

		LOGFileCharacteristicsHelpPanel->Height = LOGFileWinchMachineryMaskEdit->Top + LOGFileWinchMachineryMaskEdit->Height + LAYOUT_GUI_Margin;

		// LOGFileCommentsGroupBox
		LOGFileCommentsGroupBox->Height = 4 * abs(LOGFileCommentsMemo->Font->Height);
//		LOGFileCommentsGroupBox->Width = LOGFileCharacteristicsScrollBox->ClientWidth - LOGFileCharacteristicsScrollBox->VertScrollBar->Size - LAYOUT_GUI_Margin;
//		LOGFileCommentsGroupBox->Left = 0;
//		LOGFileCommentsGroupBox->Top = LOGFileWinchMachineryMaskEdit->Top + LOGFileWinchMachineryMaskEdit->Height + LAYOUT_GUI_Margin;

		// LOGFileLanguageFormatGroupBox
//		LOGFileLanguageFormatGroupBox->Top = LOGFileCommentsGroupBox->Top + LOGFileCommentsGroupBox->Height  + LAYOUT_GUI_Margin;
//		LOGFileLanguageFormatGroupBox->Left = 0;
//		LOGFileLanguageFormatGroupBox->Width = LOGFileCharacteristicsScrollBox->ClientWidth - LOGFileCharacteristicsScrollBox->VertScrollBar->Size - LAYOUT_GUI_Margin;

		HV = (LogFileLanguageFormatPanel->Width / 3);
		LOGFileLanguageGermanRadioButton->Height = abs(LOGFileLanguageGermanRadioButton->Font->Height);
		LOGFileLanguageGermanRadioButton->Top = 0;
		LOGFileLanguageGermanRadioButton->Left = 0;
		LOGFileLanguageGermanRadioButton->Width = HV;
		LOGFileLanguageRussianRadioButton->Height = abs(LOGFileLanguageRussianRadioButton->Font->Height);
		LOGFileLanguageRussianRadioButton->Top = 0;
		LOGFileLanguageRussianRadioButton->Left = LOGFileLanguageGermanRadioButton->Left + HV;
		LOGFileLanguageRussianRadioButton->Width = HV;
		LOGFileLanguageEnglishRadioButton->Height = abs(LOGFileLanguageEnglishRadioButton->Font->Height);
		LOGFileLanguageEnglishRadioButton->Top = 0;
		LOGFileLanguageEnglishRadioButton->Left = LOGFileLanguageGermanRadioButton->Left + (2 * HV);
		LOGFileLanguageEnglishRadioButton->Width = HV;

		LOGFileLanguageFormatGroupBox->Height = LogFileLanguageFormatPanel->Top + (2 * LOGFileLanguageGermanRadioButton->Height);

		// LOGFileTypeGroupBox
//		LOGFileTypeGroupBox->Left = 0;
//		LOGFileTypeGroupBox->Width = LOGFileCharacteristicsScrollBox->ClientWidth - LOGFileCharacteristicsScrollBox->VertScrollBar->Size - LAYOUT_GUI_Margin;
//		LOGFileTypeGroupBox->Top = LOGFileLanguageFormatGroupBox->Top + LOGFileLanguageFormatGroupBox->Height + LAYOUT_GUI_Margin;

		HV = (LOGFileTypePanel->ClientWidth / 3);
		LOGFileTypeGeoSysV2RadioButton->Top = 0;
		LOGFileTypeGeoSysV2RadioButton->Left = 0;
		LOGFileTypeGeoSysV2RadioButton->Height = abs(LOGFileTypeGeoSysV2RadioButton->Font->Height);
        LOGFileTypeGeoSysV2RadioButton->Width = HV;
		LOGFileTypeGeoSysV1RadioButton->Top = 0;
		LOGFileTypeGeoSysV1RadioButton->Left = LOGFileTypeGeoSysV2RadioButton->Left + HV;
		LOGFileTypeGeoSysV1RadioButton->Height = abs(LOGFileTypeGeoSysV1RadioButton->Font->Height);
		LOGFileTypeGeoSysV1RadioButton->Width = HV;
		LOGFileTypeLAS2RadioButton->Top = 0;
		LOGFileTypeLAS2RadioButton->Left = LOGFileTypeGeoSysV1RadioButton->Left + HV;
		LOGFileTypeLAS2RadioButton->Height = abs(LOGFileTypeLAS2RadioButton->Font->Height);
		LOGFileTypeLAS2RadioButton->Width = HV;

		LOGFileTypeGroupBox->Height = LOGFileTypePanel->Top + (2 * LOGFileTypeLAS2RadioButton->Height);

		// LOGFileTypeGroupBox
//		LOGFileTensionUnitGroupBox->Left = 0;
//		LOGFileTensionUnitGroupBox->Width = LOGFileCharacteristicsScrollBox->ClientWidth - LOGFileCharacteristicsScrollBox->VertScrollBar->Size - LAYOUT_GUI_Margin;
//		LOGFileTensionUnitGroupBox->Top = LOGFileTypeGroupBox->Top + LOGFileTypeGroupBox->Height + LAYOUT_GUI_Margin;

		HV = LogFileTensionUnitPanel->ClientWidth / 3;
		LOGFileTensionUnit_kN_RadioButton->Top = 0;
		LOGFileTensionUnit_kN_RadioButton->Left = 0;
		LOGFileTensionUnit_kN_RadioButton->Height = abs(LOGFileTensionUnit_kN_RadioButton->Font->Height);
		LOGFileTensionUnit_kN_RadioButton->Width = HV;
		LOGFileTensionUnit_kp_RadioButton->Top = 0;
		LOGFileTensionUnit_kp_RadioButton->Left = LOGFileTensionUnit_kN_RadioButton->Left + HV;
		LOGFileTensionUnit_kp_RadioButton->Height = abs(LOGFileTensionUnit_kp_RadioButton->Font->Height);
		LOGFileTensionUnit_kp_RadioButton->Width = HV;

		LOGFileTensionUnitGroupBox->Height = LogFileTensionUnitPanel->Top + (2 * LOGFileTensionUnit_kN_RadioButton->Height);

		LOGFileCharacteristicsScrollBox->VertScrollBar->Margin = 0;				// wieder zurücksetzen

		// -----------------------------
		// LOGFileAdminTabSheet

		LOGFileAdminScrollBox->VertScrollBar->Margin = 2000;					// Trick, damit die vertikale Bildlaufleiste sichtbar ist und deren Breite im Layout berücksichtigt wird

		// Tasten
		LOGFileAdminCancelButton->Height = BTN_SIZE_H;
		LOGFileAdminCancelButton->Width = BTN_SIZE_W;
		LOGFileAdminCancelButton->Left = LOGFileAdminButtonPanel->Width - LOGFileAdminCancelButton->Width - LAYOUT_GUI_Margin;
		LOGFileAdminButtonPanel->Height = LOGFileAdminCancelButton->Height + 2 * LAYOUT_GUI_Margin;

		// Dateiauswahl/Verwalten
		LOGFileAdminDestinationComboBox->Width = BTN_SIZE_W;
		LOGFileAdminCopyButton->Height = BTN_SIZE_H;
		LOGFileAdminCopyButton->Width = BTN_SIZE_W;
		LOGFileAdminDeleteButton->Height = BTN_SIZE_H;
		LOGFileAdminDeleteButton->Width = BTN_SIZE_W;

		HV = LOGFileAdminDestinationLabel->Width;
		if (HV < LOGFileAdminDestinationComboBox->Width) HV = LOGFileAdminDestinationComboBox->Width;
		HV += LOGFileAdminDeleteButton->Width + LAYOUT_GUI_Margin;              // noch eine Tastenbreite mehr
		LOGFileAdminFileListBox->Width = LOGFileAdminHelpPanel1->Width - HV - (3 * LAYOUT_GUI_Margin);
		LOGFileAdminDestinationLabel->Left = LOGFileAdminFileListBox->Left + LOGFileAdminFileListBox->Width + LAYOUT_GUI_Margin;
		LOGFileAdminDestinationLabel->Top = LOGFileAdminFileListBox->Top;
		LOGFileAdminDestinationComboBox->Left = LOGFileAdminDestinationLabel->Left;
		LOGFileAdminCopyButton->Left = LOGFileAdminDestinationComboBox->Left;
		LOGFileAdminDestinationComboBox->Top = LOGFileAdminDestinationLabel->Top + LOGFileAdminDestinationLabel->Height + LAYOUT_GUI_Margin;
		LOGFileAdminCopyButton->Top = LOGFileAdminDestinationComboBox->Top + LOGFileAdminDestinationComboBox->Height + LAYOUT_GUI_Margin;
		LOGFileAdminDeleteButton->Left = LOGFileAdminCopyButton->Left + LOGFileAdminCopyButton->Width + LAYOUT_GUI_Margin;
		LOGFileAdminDeleteButton->Top = LOGFileAdminCopyButton->Top;
		ProgressGauge->Height = BTN_SIZE_H / 2;
		ProgressGauge->Width = BTN_SIZE_W;
		ProgressGauge->Left = LOGFileAdminCopyButton->Left;
		ProgressGauge->Top = LOGFileAdminCopyButton->Top + LOGFileAdminCopyButton->Height + LAYOUT_GUI_Margin;
		LOGFileAdminHelpPanel1->Height = LOGFileAdminCopyButton->Top + LOGFileAdminCopyButton->Height + ProgressGauge->Height + (2 * LAYOUT_GUI_Margin);

		// Dateiansicht
		LOGFileAdminGermanRadioButton->Height = abs(LOGFileAdminGermanRadioButton->Font->Height);
		LOGFileAdminRussianRadioButton->Height = abs(LOGFileAdminRussianRadioButton->Font->Height);
		LOGFileAdminEnglishRadioButton->Height = abs(LOGFileAdminEnglishRadioButton->Font->Height);
		HV = LOGFileAdminLanguageGroupBox->Width / 3 - abs(2 * LOGFileAdminLanguageGroupBox->Font->Height);
		LOGFileAdminGermanRadioButton->Width = HV;
		LOGFileAdminRussianRadioButton->Width = HV;
		LOGFileAdminEnglishRadioButton->Width = HV;
		LOGFileAdminGermanRadioButton->Left = abs(LOGFileAdminLanguageGroupBox->Font->Height);
		LOGFileAdminRussianRadioButton->Left = LOGFileAdminGermanRadioButton->Left + HV;
		LOGFileAdminEnglishRadioButton->Left = LOGFileAdminGermanRadioButton->Left + (2* HV);
		HV = abs(LOGFileAdminLanguageGroupBox->Font->Height);
		LOGFileAdminGermanRadioButton->Top = HV;
		LOGFileAdminRussianRadioButton->Top = HV;
		LOGFileAdminEnglishRadioButton->Top = HV;
		LOGFileAdminLanguageGroupBox->Height = LOGFileAdminGermanRadioButton->Top + LOGFileAdminGermanRadioButton->Height + (HV / 2);

		oldCharset = LOGFileAdminCurrFileMemo->Font->Charset;
		LOGFileAdminCurrFileMemo->ParentFont = true;
		LOGFileAdminCurrFileMemo->Font->Pitch = TFontPitch::fpFixed;
		LOGFileAdminCurrFileMemo->Font->Charset = oldCharset;
		LOGFileAdminCurrFileMemo->Font->Height = FONTSIZE_SMALL;
		LOGFileAdminCurrFileMemo->Font->Name = LAYOUT_LOGFileAdminCurrFileMemo_Font;
		// -----------------------------

		LOGFileAdminScrollBox->VertScrollBar->Margin = 0;						// wieder zurücksetzen

		FileAdminPanel->Refresh();
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von InfoPanel
void __fastcall TMainForm::MakeInfoPanelLayout()
{
		InfoOkButton->Height = BTN_SIZE_H;
		InfoOkButton->Width = BTN_SIZE_W;
		InfoOkButton->Top = InfoPanel->ClientHeight - InfoOkButton->Height - 2 * LAYOUT_GUI_Margin;
		InfoOkButton->Left = (InfoPanel->ClientWidth / 2) - (InfoOkButton->Width / 2);

		InfoCaptionLabel->Font->Height = FONTSIZE_BIG;
		InfoMemo->Height = InfoOkButton->Top - InfoMemo->Top - (2 * LAYOUT_GUI_Margin);

        InfoPanel->Refresh();
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von MessagePanel
void __fastcall TMainForm::MakeMessagePanelLayout()
{
		// Meldungsfenster
		MessagePanel->Refresh();

		// Tastendimmensionen
		MessagePanelOkButton->Height = BTN_SIZE_H;
		MessagePanelOkButton->Width = BTN_SIZE_W;
		MessagePanelOkButton->Left = MessageButtonPanel->ClientWidth - MessagePanelOkButton->Width - LAYOUT_GUI_Margin;
		MessagePanelOkButton->Top = LAYOUT_GUI_Margin;
		MessageButtonPanel->Height = MessagePanelOkButton->Height + 2 * LAYOUT_GUI_Margin;
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von NormalViewPanel
void __fastcall TMainForm::MakeNormalViewPanelLayout()
{
        int HV = 0, HVButtonWidth = 0;

// ToDo: Wenn sich Schriftanpassung bewehrt hat, entfernen
//		NormalViewPanel->Font->Height = LAYOUT_Label_HEIGHT_PERC * (LAYOUT_Entry_HEIGHT_PERC * NormalViewPanel->Height / 100) / 100;

		// Schriftgrößen
		AutoRunProgramItemListGroupBox->Font->Height = FONTSIZE_SMALL;

        // -----------------------
        // HelpPanel
        NormalViewHelpPanel2->Width = LAYOUT_DepthVisualisation_WIDTH_PERC * NormalViewHelpPanel1->ClientWidth / 100;

        // -----------------------
        // Teufenvisualisierung
        DepthVisualisation->Digits = VIEW_DEPTHACCURACY;

        // -----------------------
        // Programmautomatik Ablaufprogramminformationen
        AutoRunProgramItemListGroupBox->Height = NormalViewHelpPanel2->ClientHeight / 100 * LAYOUT_AutoRunProgramListPanel_HEIGHT_PERC;

		// -----------------------
		// Signalisierungsbezeichner

		// Positionierung Symbol-Beschriftungen
        // Am rechten Rand orientieren (längste Beschriftung suchen)
        HV = EmergencySwitchLabel->Width;
        if (HV < AutomotiveErrorLabel->Width)           HV = AutomotiveErrorLabel->Width;
        if (HV < ChangeOilFilterLabel->Width)           HV = ChangeOilFilterLabel->Width;
        if (HV < AuxiallyDriveLabel->Width)             HV = AuxiallyDriveLabel->Width;
        if (HV < MagneticMarkOnOffLabel->Width)         HV = MagneticMarkOnOffLabel->Width;
        if (HV < DynamicTensionOnOffLabel->Width)       HV = DynamicTensionOnOffLabel->Width;
        if (HV < PowerFailCdLabel->Width)               HV = PowerFailCdLabel->Width;
		if (HV < SpecialFuncLabel->Width) 				HV = SpecialFuncLabel->Width;
		HV = NormalViewHelpPanel1->ClientWidth - HV -  (2 * LAYOUT_GUI_Margin);

        // Falls Text in NormalViewHelpPanel2 hineinragen sollte, dann an dessen Grenze orientieren
        // an (NormalViewHelpPanel2 (mit Teufenvisualisierung) orientiert
        if (HV < NormalViewHelpPanel2->Left + NormalViewHelpPanel2->Width) HV = NormalViewHelpPanel2->Left + NormalViewHelpPanel2->Width;

        EmergencySwitchLabel->Top = EmergencySwitch_DeadButton->Top;
        EmergencySwitchLabel->Left = HV;
        AutomotiveErrorLabel->Top = AutomotiveError_DeadButton->Top;
        AutomotiveErrorLabel->Left = HV;
        ChangeOilFilterLabel->Top = ChangeOilFilter_DeadButton->Top;
        ChangeOilFilterLabel->Left = HV;
        AuxiallyDriveLabel->Top = AuxiallyDrive_DeadButton->Top;
        AuxiallyDriveLabel->Left = HV;
        MagneticMarkOnOffLabel->Top = MagneticMarkOnOffButton->Top;
        MagneticMarkOnOffLabel->Left = HV;
        DynamicTensionOnOffLabel->Top = DynamicTensionOnOffButton->Top;
        DynamicTensionOnOffLabel->Left = HV;
        PowerFailCdLabel->Top = PowerFailCdResetSpeedButton->Top;
		PowerFailCdLabel->Left = HV;
		SpecialFuncLabel->Top = SpecialFuncButton->Top;
        SpecialFuncLabel->Left = HV;

		// -----------------------
		NormalViewPanel->Refresh();
};

//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von ParameterPanel
void __fastcall TMainForm::MakeParameterPanelLayout()
{
		int HV = 0;

// ToDo: Wenn sich Schriftanpassung bewehrt hat, entfernen
//		ParamPageControl->Font->Height = ParamPageControl->Height * LAYOUT_ParamPageControlFont_HEIGHT_PERC / 100;
		// --------------------------
		// WinchTabSheet

		// Scroll-Position auf oben links setzen, sonst werden die Koordinaten
		// der folgenden sich im Clientbereich befindlichen Objekte nicht
		// absolut gesetzt (Koordinaten sind relativ zur momentanen Position
		// der Scrollkomponenten)
		WinchTabSheetScrollBox->HorzScrollBar->Position = 0;
		WinchTabSheetScrollBox->VertScrollBar->Position = 0;

		// Größe der Scroll-Balken
		// ist abhängig von der Schrifthöhe, da diese auch für Tasten- und Eingabefenster herangezogen wird
		WinchTabSheetScrollBox->HorzScrollBar->Size = 2 * abs(WinchTabSheet->Font->Height);
		WinchTabSheetScrollBox->VertScrollBar->Size = WinchTabSheetScrollBox->HorzScrollBar->Size;

//		PWinchCfgListBox->Left = PMultipleWinchCfgCheckBox->Left;
//		PWinchCfgListBox->Top = PMultipleWinchCfgCheckBox->Top + PMultipleWinchCfgCheckBox->Height;
//		PWinchCfgListBox->Width = WinchTabSheetScrollBox->ClientWidth;
		PWinchCfgListBox->ClientHeight = abs(PWinchCfgListBox->Font->Height) * _WinchCnt;

		// Tasten
    	WinchTabSheetExitButton->Font->Height = FONTSIZE_BIG;
		WinchTabSheetExitButton->Height = BTN_SIZE_H;
		WinchTabSheetExitButton->Width = BTN_SIZE_H;
		WinchTabSheetExitButton->Top = WinchTabSheet->ClientHeight - WinchTabSheetExitButton->Height - LAYOUT_GUI_Margin;
		WinchTabSheetExitButton->Left = WinchTabSheet->ClientWidth - WinchTabSheetExitButton->Width - LAYOUT_GUI_Margin;

		WinchTabSheetScrollBox->Height = WinchTabSheetExitButton->Top - (2 * LAYOUT_GUI_Margin);

		// --------------------------
		// RunTabSheet

		// Scroll-Position auf oben links setzen, sonst werden die Koordinaten
		// der folgenden sich im Clientbereich befindlichen Objekte nicht
		// absolut gesetzt (Koordinaten sind relativ zur momentanen Position
		// der Scrollkomponenten)
		RunTabSheetScrollBox->HorzScrollBar->Position = 0;
		RunTabSheetScrollBox->VertScrollBar->Position = 0;

		// Größe der Scroll-Balken
		// ist abhängig von der Schrifthöhe, da diese auch für Tasten- und Eingabefenster herangezogen wird
		RunTabSheetScrollBox->HorzScrollBar->Size = 2 * abs(RunTabSheet->Font->Height);
		RunTabSheetScrollBox->VertScrollBar->Size = RunTabSheetScrollBox->HorzScrollBar->Size;

		// Schriftgröße
		PProgramAutomGroupBox->Font->Height = FONTSIZE_SMALL;
		PProgramAutomCheckBox->Font->Height = FONTSIZE_NORMAL;
		PProgramAutomFileListBox->Font->Height = FONTSIZE_NORMAL;
		PProgramAutomPrgNameLabel->Font->Height = FONTSIZE_NORMAL;

		PMinDepthEdit->Font = RunTabSheetScrollBox->Font;
		PMaxDepthEdit->Font = RunTabSheetScrollBox->Font;
		PVelocityEdit->Font = RunTabSheetScrollBox->Font;

		// Breitestes Label suchen und linke Position festlegen
		HV = PMinDepthEdit->CaptionWidth;
		if (HV < PMaxDepthEdit->CaptionWidth) HV = PMaxDepthEdit->CaptionWidth;
		if (HV < PVelocityEdit->CaptionWidth) HV = PVelocityEdit->CaptionWidth;
		HV += LAYOUT_GUI_Margin;
		PMinDepthEdit->EditFieldLeftPos = HV;
		PMinDepthEdit->EditFieldWidth = PMinDepthEdit->Height * 4;
		PMaxDepthEdit->EditFieldLeftPos = HV;
		PMaxDepthEdit->EditFieldWidth = PMaxDepthEdit->Height * 4;
		PVelocityEdit->EditFieldLeftPos = HV;
		PVelocityEdit->EditFieldWidth = PVelocityEdit->Height * 4;

		PChangeDirectionButton->Height = BTN_SIZE_H;

		PProgramAutomCheckBox->Height = abs(PProgramAutomCheckBox->Font->Height);
		PProgramAutomFileListBox->Height = 4 * abs(PProgramAutomFileListBox->Font->Height);
		PProgramAutomGroupBox->Height = PProgramAutomPrgNameLabel->Top + PProgramAutomPrgNameLabel->Height + LAYOUT_GUI_Margin;

		PWinchBrakeInternalAutoCheckBox->Height = abs(PWinchBrakeInternalAutoCheckBox->Font->Height);;
		PEngineRevolutionChangeableCheckBox->Height = abs(PEngineRevolutionChangeableCheckBox->Font->Height);;
		PEngineStartStopEnableCheckBox->Height = abs(PEngineStartStopEnableCheckBox->Font->Height);;
		PLostMotionEnableCheckBox->Height = abs(PLostMotionEnableCheckBox->Font->Height);;
		PBrakeOffEnableCheckBox->Height = abs(PBrakeOffEnableCheckBox->Font->Height);
		PHydraulikBrakeEnableCheckBox->Height = abs(PHydraulikBrakeEnableCheckBox->Font->Height);

		// Tasten
		RunTabSheetExitButton->Font->Height = FONTSIZE_BIG;
		RunTabSheetExitButton->Height = BTN_SIZE_H;
		RunTabSheetExitButton->Width = BTN_SIZE_H;
		RunTabSheetExitButton->Top = RunTabSheet->ClientHeight - RunTabSheetExitButton->Height - LAYOUT_GUI_Margin;
        RunTabSheetExitButton->Left = RunTabSheet->ClientWidth - RunTabSheetExitButton->Width - LAYOUT_GUI_Margin;

		RunTabSheetScrollBox->Height = RunTabSheetExitButton->Top - (2 * LAYOUT_GUI_Margin);

        // --------------------------
        // CableTabSheet

		// Scroll-Position auf oben links setzen, sonst werden die Koordinaten
        // der folgenden sich im Clientbereich befindlichen Objekte nicht
        // absolut gesetzt (Koordinaten sind relativ zur momentanen Position
        // der Scrollkomponenten)
		CableTabSheetScrollBox->HorzScrollBar->Position = 0;
        CableTabSheetScrollBox->VertScrollBar->Position = 0;

        // Größe der Scroll-Balken
        // ist abhängig von der Schrifthöhe, da diese auch für Tasten- und Eingabefenster herangezogen wird
        CableTabSheetScrollBox->HorzScrollBar->Size = 2 * abs(CableTabSheet->Font->Height);
        CableTabSheetScrollBox->VertScrollBar->Size = CableTabSheetScrollBox->HorzScrollBar->Size;

		// Schriftgröße
		PMaxTensionEdit->Font = CableTabSheetScrollBox->Font;
		PRelTensionLimitLowerEdit->Font = CableTabSheetScrollBox->Font;
		PRelTensionLimitUpperEdit->Font = CableTabSheetScrollBox->Font;

		// Breitestes Label der ersten Spalte suchen und linke Position festlegen
		HV = PMaxTensionEdit->CaptionWidth;
		if (HV < PRelTensionLimitLowerEdit->CaptionWidth) HV = PRelTensionLimitLowerEdit->CaptionWidth;
		if (HV < PRelTensionLimitUpperEdit->CaptionWidth) HV = PRelTensionLimitUpperEdit->CaptionWidth;
		HV += LAYOUT_GUI_Margin;
		PMaxTensionEdit->EditFieldLeftPos = HV;
		PMaxTensionEdit->EditFieldWidth = PMaxTensionEdit->Height * 4;
		PRelTensionLimitLowerEdit->EditFieldLeftPos = HV;
		PRelTensionLimitLowerEdit->EditFieldWidth = PRelTensionLimitLowerEdit->Height * 4;
		PRelTensionLimitUpperEdit->EditFieldLeftPos = HV;
		PRelTensionLimitUpperEdit->EditFieldWidth = PRelTensionLimitUpperEdit->Height * 4;

        // Tasten
		CableTabSheetExitButton->Font->Height = FONTSIZE_BIG;
		CableTabSheetExitButton->Height = BTN_SIZE_H;
		CableTabSheetExitButton->Width = BTN_SIZE_H;
		CableTabSheetExitButton->Top = CableTabSheet->ClientHeight - CableTabSheetExitButton->Height - LAYOUT_GUI_Margin;
		CableTabSheetExitButton->Left = CableTabSheet->ClientWidth - CableTabSheetExitButton->Width - LAYOUT_GUI_Margin;

		CableTabSheetScrollBox->Height = CableTabSheetExitButton->Top - (2 * LAYOUT_GUI_Margin);

        // Box mit Kalibrier-Werten
		// Schriftgrößen
		PCalibTensionGroupBox->Font->Height = FONTSIZE_SMALL;
		PCalibTensionCalibInfoLabel->Font = CableTabSheetScrollBox->Font;
		PCalibTensionXEdit->Font = CableTabSheetScrollBox->Font;
		PCalibTensionYEdit->Font = CableTabSheetScrollBox->Font;
		PCalibTensionZeroEdit->Font = CableTabSheetScrollBox->Font;

		// Breiteste Überschrift linke Position der Eingabefelder
		HV = PCalibTensionXEdit->CaptionWidth;
		if (HV < PCalibTensionYEdit->CaptionWidth) HV = PCalibTensionYEdit->CaptionWidth;
		if (HV < PCalibTensionZeroEdit->CaptionWidth) HV = PCalibTensionZeroEdit->CaptionWidth;
		HV += LAYOUT_GUI_Margin;
		PCalibTensionXEdit->EditFieldLeftPos = HV;
		PCalibTensionYEdit->EditFieldLeftPos = HV;
		PCalibTensionZeroEdit->EditFieldLeftPos = HV;

		PCalibTensionXEdit->Width = PCalibTensionXEdit->Height * 4;
		PCalibTensionYEdit->Width = PCalibTensionYEdit->Height * 4;
		PCalibTensionZeroEdit->Width = PCalibTensionZeroEdit->Height * 4;

		// Höhe der Box
		PCalibTensionGroupBox->Height = PCalibTensionZeroEdit->Top + PCalibTensionZeroEdit->Height + LAYOUT_GUI_Margin;

        // --------------------------

        // DepthCalcTabSheet

		// Scroll-Position auf oben links setzen, sonst werden die Koordinaten
		// der folgenden sich im Clientbereich befindlichen Objekte nicht
		// absolut gesetzt (Koordinaten sind relativ zur momentanen Position
		// der Scrollkomponenten)
		DepthCalcTabSheetScrollBox->HorzScrollBar->Position = 0;
		DepthCalcTabSheetScrollBox->VertScrollBar->Position = 0;

		// Größe der Scroll-Balken
		// ist abhängig von der Schrifthöhe, da diese auch für Tasten- und Eingabefenster herangezogen wird
		DepthCalcTabSheetScrollBox->HorzScrollBar->Size = 2 * (DepthCalcTabSheet->Font->Height);
		DepthCalcTabSheetScrollBox->VertScrollBar->Size = DepthCalcTabSheetScrollBox->HorzScrollBar->Size;

		// Schriftgrößen
		PSetDepthEdit->Font = DepthCalcTabSheet->Font;
		PDepthCalFactorEdit->Font = DepthCalcTabSheet->Font;
		PMMSyncForceRangeLimEdit->Font = DepthCalcTabSheet->Font;
		Ext_cm_Imp_GroupBox->Font->Height = FONTSIZE_SMALL;
		PKARATRadioButton->Font->Height = FONTSIZE_NORMAL;
		PKARAT2RadioButton->Font->Height = FONTSIZE_NORMAL;
		PKARAT2NRadioButton->Font->Height = FONTSIZE_NORMAL;
		PONYXRadioButton->Font->Height = FONTSIZE_NORMAL;
		PKEDRRadioButton->Font->Height = FONTSIZE_NORMAL;
		PKEDRNRadioButton->Font->Height = FONTSIZE_NORMAL;

		// Breitestes Label der ersten Spalte suchen und linke Position festlegen
		HV = PSetDepthEdit->CaptionWidth;
		if (HV < PDepthCalFactorEdit->CaptionWidth) HV = PDepthCalFactorEdit->CaptionWidth;
		if (HV < PMMSyncForceRangeLimEdit->CaptionWidth) HV = PMMSyncForceRangeLimEdit->CaptionWidth;
		HV += LAYOUT_GUI_Margin;
		PSetDepthEdit->EditFieldLeftPos = HV;
		PSetDepthEdit->EditFieldWidth = PSetDepthEdit->Height * 4;
		PDepthCalFactorEdit->EditFieldLeftPos = HV;
		PDepthCalFactorEdit->EditFieldWidth = PDepthCalFactorEdit->Height * 4;
		PMMSyncForceRangeLimEdit->EditFieldLeftPos = HV;
		PMMSyncForceRangeLimEdit->EditFieldWidth = PMMSyncForceRangeLimEdit->Height * 4;

		PKARATRadioButton->Top = 2 * (Ext_cm_Imp_GroupBox->Font->Height);
		PKARATRadioButton->Height = abs(PKARATRadioButton->Font->Height);
		PKARATRadioButton->Left = LAYOUT_GUI_Margin;
		PKARATRadioButton->Width = Ext_cm_Imp_GroupBox->ClientWidth / 2;
		PKARAT2RadioButton->Top = PKARATRadioButton->Top + PKARATRadioButton->Height + LAYOUT_GUI_Margin;
		PKARAT2RadioButton->Height = abs(PKARAT2RadioButton->Font->Height);
		PKARAT2RadioButton->Left = PKARATRadioButton->Left;
        PKARAT2RadioButton->Width = Ext_cm_Imp_GroupBox->ClientWidth / 2;
		PKARAT2NRadioButton->Top = PKARAT2RadioButton->Top + PKARAT2RadioButton->Height + LAYOUT_GUI_Margin;
        PKARAT2NRadioButton->Height = abs(PKARAT2NRadioButton->Font->Height);
        PKARAT2NRadioButton->Left = PKARATRadioButton->Left;
        PKARAT2NRadioButton->Width = Ext_cm_Imp_GroupBox->ClientWidth / 2;
		PONYXRadioButton->Top = PKARAT2NRadioButton->Top + PKARAT2NRadioButton->Height + LAYOUT_GUI_Margin;
        PONYXRadioButton->Height = abs(PONYXRadioButton->Font->Height);
        PONYXRadioButton->Left = PKARATRadioButton->Left;
        PONYXRadioButton->Width = Ext_cm_Imp_GroupBox->ClientWidth / 2;
		PKEDRRadioButton->Top = PONYXRadioButton->Top + PONYXRadioButton->Height + LAYOUT_GUI_Margin;
        PKEDRRadioButton->Height = abs(PKEDRRadioButton->Font->Height);
        PKEDRRadioButton->Left = PKARATRadioButton->Left;
        PKEDRRadioButton->Width = Ext_cm_Imp_GroupBox->ClientWidth / 2;
		PKEDRNRadioButton->Top = PKEDRRadioButton->Top + PKEDRRadioButton->Height + LAYOUT_GUI_Margin;
        PKEDRNRadioButton->Height = abs(PKEDRNRadioButton->Font->Height);
        PKEDRNRadioButton->Left = PKARATRadioButton->Left;
        PKEDRNRadioButton->Width = Ext_cm_Imp_GroupBox->ClientWidth / 2;

        Ext_cm_Imp_GroupBox->Height = PKEDRNRadioButton->Top + PKEDRNRadioButton->Height + LAYOUT_GUI_Margin;

        Ext_cm_Imp_Image->Width = (Ext_cm_Imp_GroupBox->ClientWidth / 2) - PKARATRadioButton->Left - LAYOUT_GUI_Margin - (2 * Ext_cm_Imp_GroupBox->Font->Height);
        Ext_cm_Imp_Image->Height = Ext_cm_Imp_GroupBox->ClientHeight - (4 * Ext_cm_Imp_GroupBox->Font->Height);
        Ext_cm_Imp_Image->Left = PKARATRadioButton->Left + PKARATRadioButton->Width + LAYOUT_GUI_Margin;
        Ext_cm_Imp_Image->Top = 2 * Ext_cm_Imp_GroupBox->Font->Height;

        // Tasten
		DepthCalcTabSheetExitButton->Font->Height = FONTSIZE_BIG;
		DepthCalcTabSheetExitButton->Height = BTN_SIZE_H;
		DepthCalcTabSheetExitButton->Width = BTN_SIZE_H;
		DepthCalcTabSheetExitButton->Top = DepthCalcTabSheet->ClientHeight - DepthCalcTabSheetExitButton->Height - LAYOUT_GUI_Margin;
		DepthCalcTabSheetExitButton->Left = DepthCalcTabSheet->ClientWidth - DepthCalcTabSheetExitButton->Width - LAYOUT_GUI_Margin;

		DepthCalcTabSheetScrollBox->Height = DepthCalcTabSheetExitButton->Top - (2 * LAYOUT_GUI_Margin);
        // --------------------------

        // SuperVisionTabSheet

        // Scroll-Position auf oben links setzen, sonst werden die Koordinaten
        // der folgenden sich im Clientbereich befindlichen Objekte nicht
        // absolut gesetzt (Koordinaten sind relativ zur momentanen Position
        // der Scrollkomponenten)
        SuperVisionTabSheetScrollBox->HorzScrollBar->Position = 0;
        SuperVisionTabSheetScrollBox->VertScrollBar->Position = 0;

		// Schriftgrößen
		PControllerErrorGroupBox->Font->Height = FONTSIZE_SMALL;
		PControllerErrorMonitCheckBox->Font->Height = FONTSIZE_NORMAL;
		PControllerErrorLactiveCheckBox->Font->Height = FONTSIZE_NORMAL;
		PControllerErrorAtSMCheckBox->Font->Height = FONTSIZE_NORMAL;

		SpecialSignalModeGroupBox->Font->Height = FONTSIZE_SMALL;
		PSpecialSignalMode0RadioButton->Font->Height = FONTSIZE_NORMAL;
		PSpecialSignalMode1RadioButton->Font->Height = FONTSIZE_NORMAL;
		PSpecialSignalMode2RadioButton->Font->Height = FONTSIZE_NORMAL;
		PSpecialSignalMode3RadioButton->Font->Height = FONTSIZE_NORMAL;
		PSpecialSignalMode4RadioButton->Font->Height = FONTSIZE_NORMAL;
		PSpecialSignalMode5RadioButton->Font->Height = FONTSIZE_NORMAL;

        // Größe der Scroll-Balken
		// ist abhängig von der Schrifthöhe, da diese auch für Tasten- und Eingabefenster herangezogen wird
		SuperVisionTabSheetScrollBox->HorzScrollBar->Size = 2 * (SuperVisionTabSheet->Font->Height);
		SuperVisionTabSheetScrollBox->VertScrollBar->Size = SuperVisionTabSheetScrollBox->HorzScrollBar->Size;

		PIncEncErrorMonitCheckBox->Height = abs(PIncEncErrorMonitCheckBox->Font->Height);
		PControllerErrorMonitCheckBox->Height = abs(PControllerErrorMonitCheckBox->Font->Height);
		PControllerErrorLactiveCheckBox->Height = abs(PControllerErrorLactiveCheckBox->Font->Height);
		PControllerErrorAtSMCheckBox->Height = abs(PControllerErrorAtSMCheckBox->Font->Height);
		PSpecialSignalMode0RadioButton->Height = (PSpecialSignalMode0RadioButton->Font->Height);
		PSpecialSignalMode1RadioButton->Height = (PSpecialSignalMode1RadioButton->Font->Height);
		PSpecialSignalMode2RadioButton->Height = (PSpecialSignalMode2RadioButton->Font->Height);
		PSpecialSignalMode3RadioButton->Height = (PSpecialSignalMode3RadioButton->Font->Height);
		PSpecialSignalMode4RadioButton->Height = (PSpecialSignalMode4RadioButton->Font->Height);
		PSpecialSignalMode5RadioButton->Height = (PSpecialSignalMode5RadioButton->Font->Height);

		/*		PIncEncErrorMonitCheckBox->Left = 0;
		PIncEncErrorMonitCheckBox->Top = 0;
		PIncEncErrorMonitCheckBox->Width = SuperVisionTabSheetScrollBox->ClientWidth / 2;

		PControllerErrorGroupBox->Left = 0;
		PControllerErrorGroupBox->Top = PIncEncErrorMonitCheckBox->Top + PIncEncErrorMonitCheckBox->Height + ( 4 * LAYOUT_GUI_Margin);
		PControllerErrorGroupBox->Width = SuperVisionTabSheetScrollBox->ClientWidth;

		PControllerErrorMonitCheckBox->Left = LAYOUT_GUI_Margin;
		PControllerErrorMonitCheckBox->Top = abs(PControllerErrorGroupBox->Font->Height) + (2 * LAYOUT_GUI_Margin);
		PControllerErrorMonitCheckBox->Width = PControllerErrorGroupBox->ClientWidth - (2 * LAYOUT_GUI_Margin);

		PControllerErrorLactiveCheckBox->Left = PControllerErrorMonitCheckBox->Left;
		PControllerErrorLactiveCheckBox->Top = PControllerErrorMonitCheckBox->Top +  PControllerErrorMonitCheckBox->Height + (2 * LAYOUT_GUI_Margin);
		PControllerErrorLactiveCheckBox->Width = PControllerErrorGroupBox->ClientWidth - (2 * LAYOUT_GUI_Margin);

		PControllerErrorAtSMCheckBox->Left = PControllerErrorLactiveCheckBox->Left;
		PControllerErrorAtSMCheckBox->Top = PControllerErrorLactiveCheckBox->Top + PControllerErrorLactiveCheckBox->Height + (2 * LAYOUT_GUI_Margin);
		PControllerErrorAtSMCheckBox->Width = PControllerErrorGroupBox->ClientWidth - (2 * LAYOUT_GUI_Margin);
*/
		PControllerErrorGroupBox->ClientHeight = PControllerErrorAtSMCheckBox->Top + PControllerErrorAtSMCheckBox->Height + (2 * LAYOUT_GUI_Margin);

/*		SpecialSignalModeGroupBox->Left = 0;
		SpecialSignalModeGroupBox->Top = PControllerErrorGroupBox->Top + PControllerErrorGroupBox->Height + ( 4 * LAYOUT_GUI_Margin);
		SpecialSignalModeGroupBox->Width = SuperVisionTabSheetScrollBox->ClientWidth;

		PSpecialSignalMode0RadioButton->Left = LAYOUT_GUI_Margin;
		PSpecialSignalMode0RadioButton->Top = abs(SpecialSignalModeGroupBox->Font->Height) + (2 * LAYOUT_GUI_Margin);
		PSpecialSignalMode0RadioButton->Width = SpecialSignalModeGroupBox->ClientWidth - (2 * LAYOUT_GUI_Margin);

		PSpecialSignalMode1RadioButton->Left = LAYOUT_GUI_Margin;
		PSpecialSignalMode1RadioButton->Top = PSpecialSignalMode0RadioButton->Top + PSpecialSignalMode0RadioButton->Height + LAYOUT_GUI_Margin;
		PSpecialSignalMode1RadioButton->Width = SpecialSignalModeGroupBox->ClientWidth - (2 * LAYOUT_GUI_Margin);

		PSpecialSignalMode2RadioButton->Left = LAYOUT_GUI_Margin;
		PSpecialSignalMode2RadioButton->Top = PSpecialSignalMode1RadioButton->Top + PSpecialSignalMode1RadioButton->Height + LAYOUT_GUI_Margin;
		PSpecialSignalMode2RadioButton->Width = SpecialSignalModeGroupBox->ClientWidth - (2 * LAYOUT_GUI_Margin);

		PSpecialSignalMode3RadioButton->Left = LAYOUT_GUI_Margin;
		PSpecialSignalMode3RadioButton->Top = PSpecialSignalMode2RadioButton->Top + PSpecialSignalMode2RadioButton->Height + LAYOUT_GUI_Margin;
		PSpecialSignalMode3RadioButton->Width = SpecialSignalModeGroupBox->ClientWidth - (2 * LAYOUT_GUI_Margin);

		PSpecialSignalMode4RadioButton->Left = LAYOUT_GUI_Margin;
		PSpecialSignalMode4RadioButton->Top = PSpecialSignalMode3RadioButton->Top + PSpecialSignalMode3RadioButton->Height + LAYOUT_GUI_Margin;
		PSpecialSignalMode4RadioButton->Width = SpecialSignalModeGroupBox->ClientWidth - (2 * LAYOUT_GUI_Margin);

		PSpecialSignalMode5RadioButton->Left = LAYOUT_GUI_Margin;
		PSpecialSignalMode5RadioButton->Top = PSpecialSignalMode4RadioButton->Top + PSpecialSignalMode4RadioButton->Height + LAYOUT_GUI_Margin;
		PSpecialSignalMode5RadioButton->Width = SpecialSignalModeGroupBox->ClientWidth - (2 * LAYOUT_GUI_Margin);
*/
		SpecialSignalModeGroupBox->ClientHeight = PSpecialSignalMode5RadioButton->Top + PSpecialSignalMode5RadioButton->Height + (2 * LAYOUT_GUI_Margin);


		// Tasten
		SuperVisionTabSheetExitButton->Font->Height = FONTSIZE_BIG;
		SuperVisionTabSheetExitButton->Height = BTN_SIZE_H;
		SuperVisionTabSheetExitButton->Width = BTN_SIZE_H;
		SuperVisionTabSheetExitButton->Top = SuperVisionTabSheet->ClientHeight - SuperVisionTabSheetExitButton->Height - LAYOUT_GUI_Margin;
		SuperVisionTabSheetExitButton->Left = SuperVisionTabSheet->ClientWidth - SuperVisionTabSheetExitButton->Width - LAYOUT_GUI_Margin;

		SuperVisionTabSheetScrollBox->Height = SuperVisionTabSheetExitButton->Top - (2 * LAYOUT_GUI_Margin);

		// --------------------------
		// ViewTabSheet
		// Tasten
		ViewTabSheetExitButton->Font->Height = FONTSIZE_BIG;
		ViewTabSheetExitButton->Height = BTN_SIZE_H;
		ViewTabSheetExitButton->Width = BTN_SIZE_H;
		ViewTabSheetExitButton->Top = ViewTabSheet->ClientHeight - ViewTabSheetExitButton->Height - LAYOUT_GUI_Margin;
		ViewTabSheetExitButton->Left = ViewTabSheet->ClientWidth - ViewTabSheetExitButton->Width - LAYOUT_GUI_Margin;

		ViewTabSheetScrollBox->Height = ViewTabSheetExitButton->Top - (2 * LAYOUT_GUI_Margin);

		// Zugkraftanzeige
		// ---
		PTractionViewLabel->Top = LAYOUT_GUI_Margin;
		PTractionViewLabel->Left = LAYOUT_GUI_Margin;

		PTractionViewComboBox->Top = PTractionViewLabel->Top + PTractionViewLabel->Height + LAYOUT_GUI_Margin;
		PTractionViewComboBox->Left = LAYOUT_GUI_Margin;
		PTractionViewComboBox->Width = ViewTabSheetScrollBox->ClientWidth - LAYOUT_GUI_Margin;

		// Geschwindigkeitsanzeige
		// ---
		PVelocityViewLabel->Top = PTractionViewComboBox->Top + PTractionViewComboBox->Height + 3 * LAYOUT_GUI_Margin;
		PVelocityViewLabel->Left = LAYOUT_GUI_Margin;

		PVelocityViewComboBox->Top = PVelocityViewLabel->Top + PVelocityViewLabel->Height + LAYOUT_GUI_Margin;
		PVelocityViewComboBox->Left = LAYOUT_GUI_Margin;
		PVelocityViewComboBox->Width = ViewTabSheetScrollBox->ClientWidth - LAYOUT_GUI_Margin;

		// Teufe umkehren
		// ---
		PReverseDepthCheckBox->Left = LAYOUT_GUI_Margin;
		PReverseDepthCheckBox->Top = PVelocityViewComboBox->Top + PVelocityViewComboBox->Height + 3 * LAYOUT_GUI_Margin;
		PReverseDepthCheckBox->Width = ViewTabSheetScrollBox->ClientWidth - LAYOUT_GUI_Margin;

		// --------------------------
		// AdditionalFuncTabSheet

		// Tasten
		AdditionalFuncTabSheetExitButton->Font->Height = FONTSIZE_BIG;
		AdditionalFuncTabSheetExitButton->Height = BTN_SIZE_H;
		AdditionalFuncTabSheetExitButton->Width = BTN_SIZE_H;
		AdditionalFuncTabSheetExitButton->Top = AdditionalFuncTabSheet->ClientHeight - AdditionalFuncTabSheetExitButton->Height - LAYOUT_GUI_Margin;
		AdditionalFuncTabSheetExitButton->Left = AdditionalFuncTabSheet->ClientWidth - AdditionalFuncTabSheetExitButton->Width - LAYOUT_GUI_Margin;

		AdditionalFuncTabSheetScrollBox->Height = AdditionalFuncTabSheetExitButton->Top - (2 * LAYOUT_GUI_Margin);

		// --------------------------
		ParameterPanel->Refresh();
};

// Organisiert die Bildschirmaufteilung der Unter-Objekte von SystemPanel
void __fastcall TMainForm::MakeSystemPanelLayout()
{
        int HV;

    	// Schriftgrößen
		SystemHelpPanel1->Font->Height = FONTSIZE_SMALL;

		SystemDateLabel->Font->Height = FONTSIZE_NORMAL;
		SystemTimeLabel->Font->Height = FONTSIZE_NORMAL;
		SystemDateMaskEdit->Font->Height = FONTSIZE_NORMAL;
		SystemTimeMaskEdit->Font->Height = FONTSIZE_NORMAL;
		SystemDateTimeAcceptButton->Font->Height = FONTSIZE_NORMAL;

		SystemTempDepthCalcDescrLabel->Font->Height = FONTSIZE_NORMAL;
		SystemTempDepthCalcLabel->Font->Height = FONTSIZE_NORMAL;
		SystemTempADescrLabel->Font->Height = FONTSIZE_NORMAL;
		SystemTempALabel->Font->Height = FONTSIZE_NORMAL;
		SystemTempBDescrLabel->Font->Height = FONTSIZE_NORMAL;
		SystemTempBLabel->Font->Height = FONTSIZE_NORMAL;

		SystemControlGroupBox->Font->Height = FONTSIZE_SMALL;
		SystemShutdownButton->Font->Height = FONTSIZE_NORMAL;
		SystemRestartButton->Font->Height = FONTSIZE_NORMAL;
		SystemProgramTerminationButton->Font->Height = FONTSIZE_NORMAL;

		IdentificationSystemLabel->Font->Height = FONTSIZE_SMALL;
		ComputerNameLabel->Font->Height = FONTSIZE_SMALL;
		CtlUnitSWVersionLabel->Font->Height = FONTSIZE_SMALL;
		DepthCalcSWVersionLabel->Font->Height = FONTSIZE_SMALL;

		// SystemHelpPanel
        ComputerNameLabel->Left = LAYOUT_GUI_Margin;
        ComputerNameLabel->Top = 2 * LAYOUT_GUI_Margin;

		IdentificationSystemLabel->Left = LAYOUT_GUI_Margin;
		IdentificationSystemLabel->Top = ComputerNameLabel->Top + ComputerNameLabel->Height + LAYOUT_GUI_Margin;

		CtlUnitSWVersionLabel->Left = LAYOUT_GUI_Margin;
		CtlUnitSWVersionLabel->Top = IdentificationSystemLabel->Top + IdentificationSystemLabel->Height + LAYOUT_GUI_Margin;

		DepthCalcSWVersionLabel->Left = LAYOUT_GUI_Margin;
		DepthCalcSWVersionLabel->Top = CtlUnitSWVersionLabel->Top + CtlUnitSWVersionLabel->Height + LAYOUT_GUI_Margin;

		SystemCancelButton->Height = BTN_SIZE_H;
		SystemCancelButton->Width = BTN_SIZE_W;
        SystemCancelButton->Left = SystemHelpPanel->ClientWidth - SystemCancelButton->Width - LAYOUT_GUI_Margin;
        SystemCancelButton->Top = SystemHelpPanel->ClientHeight - SystemCancelButton->Height - LAYOUT_GUI_Margin;

		SystemHelpPanel->Height = DepthCalcSWVersionLabel->Top + DepthCalcSWVersionLabel->Height + (2 * LAYOUT_GUI_Margin);

        // SystemDatetimeGroupBox
		SystemDateMaskEdit->Width = SystemDateMaskEdit->Height * 4;
		SystemTimeMaskEdit->Width = SystemTimeMaskEdit->Height * 4;

		HV = abs(SystemDatetimeGroupBox->Font->Height) + LAYOUT_GUI_Margin;
		SystemDateLabel->Top = 2 * HV;
		SystemDateLabel->Left = HV;

		SystemTimeLabel->Top = SystemDateLabel->Top + SystemDateMaskEdit->Height + LAYOUT_GUI_Margin;
		SystemTimeLabel->Left = HV;

		SystemDateMaskEdit->Top = SystemDateLabel->Top;
		SystemTimeMaskEdit->Top = SystemTimeLabel->Top;

		SystemDateTimeAcceptButton->Height = BTN_SIZE_H;
		SystemDateTimeAcceptButton->Width = BTN_SIZE_W;
		SystemDateTimeAcceptButton->Top = SystemTimeMaskEdit->Top + SystemTimeMaskEdit->Height + LAYOUT_GUI_Margin;
		SystemDateTimeAcceptButton->Left = HV;

		HV = SystemDateLabel->Width;
		if (HV < SystemTimeLabel->Width) HV = SystemTimeLabel->Width;
		HV += SystemDateLabel->Left + LAYOUT_GUI_Margin;
		SystemDateMaskEdit->Left = HV;
		SystemTimeMaskEdit->Left = HV;

		HV = SystemDateMaskEdit->Width;
		if (HV < SystemTimeMaskEdit->Width) HV = SystemTimeMaskEdit->Width;
		if (HV < SystemDateTimeAcceptButton->Width) HV = SystemDateTimeAcceptButton->Width;
		SystemDatetimeGroupBox->Width = SystemDateMaskEdit->Left + HV + abs(SystemDatetimeGroupBox->Font->Height);

		// TemperatureGroupBox
//		SystemTemperatureGroupBox->Height = SystemHelpPanel1->ClientHeight - (SystemDatetimeGroupBox->Top + SystemDateTimeAcceptButton->Top + SystemDateTimeAcceptButton->Height + (2 * LAYOUT_GUI_Margin));
		// 1. Spalte
		HV = abs(SystemTemperatureGroupBox->Font->Height) + LAYOUT_GUI_Margin;
		SystemTempDepthCalcDescrLabel->Top = 2 * HV;
		SystemTempDepthCalcDescrLabel->Left = abs(SystemTemperatureGroupBox->Font->Height);
		SystemTempADescrLabel->Top = 3 * HV;
		SystemTempADescrLabel->Left = abs(SystemTemperatureGroupBox->Font->Height);
		SystemTempBDescrLabel->Top = 4 * HV;
		SystemTempBDescrLabel->Left = abs(SystemTemperatureGroupBox->Font->Height);

		// 2. Spalte
		// Die Temperaturwerte erstrecken sich über die gesamte GroupBox-Breite
		// und überdecken ggf. die Beschreibungsbeschriftung
		SystemTempDepthCalcLabel->Left = SystemTemperatureGroupBox->Width - SystemTempDepthCalcLabel->Width - abs(SystemTemperatureGroupBox->Font->Height);
		SystemTempDepthCalcLabel->Top = SystemTempDepthCalcDescrLabel->Top;
		SystemTempALabel->Left = SystemTemperatureGroupBox->Width - SystemTempALabel->Width - abs(SystemTemperatureGroupBox->Font->Height);
		SystemTempALabel->Top = SystemTempADescrLabel->Top;
		SystemTempBLabel->Left = SystemTemperatureGroupBox->Width - SystemTempBLabel->Width - abs(SystemTemperatureGroupBox->Font->Height);
		SystemTempBLabel->Top = SystemTempBDescrLabel->Top;

        // SystemControlGroupBox
		SystemShutdownButton->Height = BTN_SIZE_H;
		SystemRestartButton->Height = BTN_SIZE_H;
		SystemProgramTerminationButton->Height = BTN_SIZE_H;

		HV = abs(SystemControlGroupBox->Font->Height) + LAYOUT_GUI_Margin;
		SystemShutdownButton->Width = SystemControlGroupBox->Width - 2 * HV;
		SystemRestartButton->Width = SystemControlGroupBox->Width - 2 * HV;
		SystemProgramTerminationButton->Width = SystemControlGroupBox->Width - 2 * HV;

		SystemShutdownButton->Top = 2 * HV;
		SystemRestartButton->Top = SystemShutdownButton->Top + SystemShutdownButton->Height + LAYOUT_GUI_Margin;
		SystemProgramTerminationButton->Top = SystemRestartButton->Top + SystemRestartButton->Height + LAYOUT_GUI_Margin;

		SystemShutdownButton->Left = HV;
		SystemRestartButton->Left = HV;
		SystemProgramTerminationButton->Left = HV;

		SystemControlGroupBox->Height = SystemProgramTerminationButton->Top + SystemProgramTerminationButton->Height + HV;

//        SystemControlGroupBox->Width = SystemShutdownButton->Left + SystemShutdownButton->Width + HV;

        // Glyphs
        try
        {
                // Speicher allokieren
                std::auto_ptr<Graphics::TBitmap> DummyImage1(new Graphics::TBitmap());
                std::auto_ptr<Graphics::TBitmap> DummyImage2(new Graphics::TBitmap());

                // ShutDown Bitmap
                DummyImage1->LoadFromResourceName((int)HInstance, "SHUTDOWN");
                DummyImage2->Height = SystemShutdownButton->ClientHeight * 3 / 4;
                DummyImage2->Width = 2 * DummyImage2->Height;
                DummyImage2->Canvas->StretchDraw(Rect(0, 0, DummyImage2->Width, DummyImage2->Height), DummyImage1.get());
                SystemShutdownButton->Glyph->Assign(DummyImage2.get());
                SystemShutdownButton->NumGlyphs = 2;
                SystemShutdownButton->Margin = (SystemShutdownButton->Height - abs(SystemShutdownButton->Font->Height)) / 2;

                // Reboot Bitmap
                DummyImage1->LoadFromResourceName((int)HInstance, "REBOOT");
                DummyImage2->Height = SystemRestartButton->ClientHeight * 3 / 4;
                DummyImage2->Width = 2 * DummyImage2->Height;
                DummyImage2->Canvas->StretchDraw(Rect(0, 0, DummyImage2->Width, DummyImage2->Height), DummyImage1.get());
                SystemRestartButton->Glyph->Assign(DummyImage2.get());
                SystemRestartButton->NumGlyphs = 2;
                SystemRestartButton->Margin = (SystemRestartButton->Height - abs(SystemRestartButton->Font->Height)) / 2;

                // Exit Bitmap
                DummyImage1->LoadFromResourceName((int)HInstance, "EXIT");
                DummyImage2->Height = SystemProgramTerminationButton->ClientHeight * 3 / 4;
                DummyImage2->Width = 2 * DummyImage2->Height;
                DummyImage2->Canvas->StretchDraw(Rect(0, 0, DummyImage2->Width, DummyImage2->Height), DummyImage1.get());
                SystemProgramTerminationButton->Glyph->Assign(DummyImage2.get());
                SystemProgramTerminationButton->NumGlyphs = 2;
                SystemProgramTerminationButton->Margin = (SystemProgramTerminationButton->Height - abs(SystemProgramTerminationButton->Font->Height)) / 2;

        }catch(...)
		{
		};

		SystemPanel->Refresh();
};
//---------------------------------------------------------------------------
// Organisiert die Bildschirmaufteilung der Unter-Objekte von MainMenuPanel
void __fastcall TMainForm::MakeMainMenuPanelLayout() {

		int HV_MenuPosition;
		int HV_TopPos = 4 * LAYOUT_MenuButtonDistance; 							// Oberkante der obersten Taste

		// Tastendimensionen und Positionen
        HV_MenuPosition = 0;

		CalibrateButton->Height = BTN_SIZE_H;
		CalibrateButton->Width = MainMenuPanel->ClientWidth / 2;
		CalibrateButton->Left = (MainMenuPanel->ClientWidth - CalibrateButton->Width) / 2;
		CalibrateButton->Top = HV_MenuPosition * (BTN_SIZE_H + LAYOUT_MenuButtonDistance) + HV_TopPos;

		UnloadCableButton->Height = BTN_SIZE_H;
		UnloadCableButton->Width = MainMenuPanel->ClientWidth / 2;
		UnloadCableButton->Left = (MainMenuPanel->ClientWidth - UnloadCableButton->Width) / 2;
		UnloadCableButton->Top = ++HV_MenuPosition * (BTN_SIZE_H + LAYOUT_MenuButtonDistance) + HV_TopPos;

		SetParameterButton->Height = BTN_SIZE_H;
		SetParameterButton->Width = MainMenuPanel->ClientWidth / 2;
		SetParameterButton->Left = (MainMenuPanel->ClientWidth - SetParameterButton->Width) / 2;
		SetParameterButton->Top = ++HV_MenuPosition * (BTN_SIZE_H + LAYOUT_MenuButtonDistance) + HV_TopPos;

		FileAdminButton->Height = BTN_SIZE_H;
		FileAdminButton->Width = MainMenuPanel->ClientWidth / 2;
		FileAdminButton->Left = (MainMenuPanel->ClientWidth - FileAdminButton->Width) / 2;
		FileAdminButton->Top = ++HV_MenuPosition * (BTN_SIZE_H + LAYOUT_MenuButtonDistance) + HV_TopPos;

		++HV_MenuPosition;	// größeren Abstand einfügen
		MessagePanelButton->Height = BTN_SIZE_H;
		MessagePanelButton->Width = MainMenuPanel->ClientWidth / 2;
		MessagePanelButton->Left = (MainMenuPanel->ClientWidth - MessagePanelButton->Width) / 2;
		MessagePanelButton->Top = ++HV_MenuPosition * (BTN_SIZE_H + LAYOUT_MenuButtonDistance) + HV_TopPos;

		SystemButton->Height = BTN_SIZE_H;
		SystemButton->Width = MainMenuPanel->ClientWidth / 2;
		SystemButton->Left = (MainMenuPanel->ClientWidth - SystemButton->Width) / 2;
		SystemButton->Top = ++HV_MenuPosition * (BTN_SIZE_H + LAYOUT_MenuButtonDistance) + HV_TopPos;

		DevelFormButton->Height = BTN_SIZE_H;
		DevelFormButton->Width = MainMenuPanel->ClientWidth / 2;
		DevelFormButton->Left = (MainMenuPanel->ClientWidth - DevelFormButton->Width) / 2;
		DevelFormButton->Top = ++HV_MenuPosition * (BTN_SIZE_H + LAYOUT_MenuButtonDistance) + HV_TopPos;
}

//---------------------------------------------------------------------------

// ==========================================================================
// SPRACHE ANPASSEN
// ==========================================================================
// Setzt alle Beschriftungen im GUI entsprechend der eingestellten Sprache
void __fastcall TMainForm::SetLabelling()
{
		int HV;
		int Traction_Dimension_Unit = VarControl.GetParam_TractionViewDimensionUnit();
		int Velocity_Dimension_Unit = VarControl.GetParam_VelocityDimensionUnit();

		// Als Container für Unicode-Zeichen
//		wchar_t ExitSymbol[2] ={0x25E2, 0};
		wchar_t ExitSymbol[2] ={0x2192, 0}; 	// Pfeil-Symbol, welches auch auf Win 7 Pro embedded 32bit STRs angezeigt werden kann

		AdditionalFuncTabSheetExitButton->Caption = ExitSymbol;
		AdditionalFuncTabSheet->Caption			= LABEL_ADDITIONAL_FUNCTIONS[GenOrgan.Lng];
		AutomotiveErrorLabel->Caption           = LABEL_AUTOMOTIVE_ERROR[GenOrgan.Lng];
		AutoRunProgramItemListGroupBox->Caption = LABEL_PROGRAMABLEAUTOMATIC_SHORT[GenOrgan.Lng];
        AuxiallyDriveLabel->Caption             = LABEL_HYDRAULIC[GenOrgan.Lng];
        CableTabSheet->Caption                  = LABEL_CABLE[GenOrgan.Lng];
//        CableTabSheetExitButton->Caption      	= LABEL_DISCARD[GenOrgan.Lng];
		CableTabSheetExitButton->Caption      	= ExitSymbol;
//		CalibDepthAbortButton->Caption			= LABEL_READY[GenOrgan.Lng];
    	CalibDepthAbortButton->Caption			= ExitSymbol;
		CalibDepthAcceptButton->Caption         = LABEL_ACCEPT[GenOrgan.Lng];
		CalibDepthCaptionLabel->Caption         = static_cast<UnicodeString>(LABEL_CALIBRATE[GenOrgan.Lng]) + __SEPARATOR01[GenOrgan.Lng] + LABEL_DEPTH[GenOrgan.Lng];
		CalibDepthHint1_SwitchOffMM_Label->Caption	= LABEL_MM_SYNC_SWITCHOFF[GenOrgan.Lng];
		CalibDepthHint2_SetDepth_Label->Caption	= LABEL_SET_NEW_DEPTH[GenOrgan.Lng];
		CalibDepthHint3_MoveLine_Label->Caption	= LABEL_SHUT_DEPTH_DISTANCE_DOWN[GenOrgan.Lng];
		CalibDepthHint4_InsertCorrectDepth_Label->Caption = LABEL_INSERT_REAL_DEPTH[GenOrgan.Lng];
		CalibDepthInfo_ActiveCalFact_Label->Caption = LABEL_CALFACTOR_ACTIVE[GenOrgan.Lng];
		CalibDepthInfo_ActiveCalFact_Label->Caption = CalibDepthInfo_ActiveCalFact_Label->Caption + L": ---" + UNIT_MILLIMETER_PER_IMPULSE[GenOrgan.Lng];
		CalibDepthInfo_EarmarkedCalFact_Label->Caption = LABEL_CALFACTOR_EARMARKED[GenOrgan.Lng];
		CalibDepthInfo_EarmarkedCalFact_Label->Caption = CalibDepthInfo_EarmarkedCalFact_Label->Caption + L": ---" + UNIT_MILLIMETER_PER_IMPULSE[GenOrgan.Lng];
		CalibDepthInfo_ImpulseCount_Label->Caption = LABEL_DEPTHIMPULSESCOUNT[GenOrgan.Lng];
		CalibDepthInfoLabel->Caption            = EXPLAN_CALIBDEPTH[GenOrgan.Lng];
		CalibDepth_DepthEdit->Caption			= LABEL_DEPTH[GenOrgan.Lng];
		CalibDepth_DepthEdit->BackEnd			= UNIT_METER[GenOrgan.Lng];
		CalibrateButton->Caption                = LABEL_CALIBRATE[GenOrgan.Lng];
		CalibTractionAbortButton->Caption       = ExitSymbol;
        CalibTractionCaptionLabel->Caption      = static_cast<UnicodeString>(LABEL_CALIBRATE[GenOrgan.Lng]) + __SEPARATOR01[GenOrgan.Lng] + LABEL_TENSION[GenOrgan.Lng];
        CalibTractionCompleteBox->Caption       = LABEL_COMPLETE[GenOrgan.Lng];
        CalibTractionCompleteInfoLabel->Caption = EXPLAN_CALIBTENSIONCOMPLETE[GenOrgan.Lng];
        CalibTractionCompleteSetButton->Caption = LABEL_ACCEPT[GenOrgan.Lng];
        CalibTractionGetZeroButton->Caption     = LABEL_ACQUIREZEROPOINT[GenOrgan.Lng];
        CalibTractionMessure1Button->Caption    = LABEL_MESSURINGPOINT1[GenOrgan.Lng];
		CalibTension_Tension1Edit->Caption		= L"";
		switch (Traction_Dimension_Unit)
		{
				case 1:         // Kilonewton
								CalibTension_Tension1Edit->BackEnd = UNIT_KILONEWTON[GenOrgan.Lng];
								break;

				case 2:         // Kilopond
								CalibTension_Tension1Edit->BackEnd = UNIT_KILOPOND[GenOrgan.Lng];
								break;

				default:        // Newton
								CalibTension_Tension1Edit->BackEnd = UNIT_NEWTON[GenOrgan.Lng];
								break;
		};
		CalibTension_Tension2Edit->Caption		= L"";
		CalibTractionMessure2Button->Caption    = LABEL_MESSURINGPOINT2[GenOrgan.Lng];
		switch (Traction_Dimension_Unit)
		{
				case 1:         // Kilonewton
								CalibTension_Tension2Edit->BackEnd = UNIT_KILONEWTON[GenOrgan.Lng];
								break;

				case 2:         // Kilopond
								CalibTension_Tension2Edit->BackEnd = UNIT_KILOPOND[GenOrgan.Lng];
								break;

				default:        // Newton
								CalibTension_Tension2Edit->BackEnd = UNIT_NEWTON[GenOrgan.Lng];
								break;
		};
		CalibTractionZeroBox->Caption           = LABEL_ZEROPOINT[GenOrgan.Lng];
		CalibTractionZeroInfoLabel->Caption     = EXPLAN_CALIBTENSIONZERO[GenOrgan.Lng];
		ChangeOilFilterLabel->Caption           = LABEL_CHANGE_OILFILTER[GenOrgan.Lng];
		CommonCtlGroupBox->Caption				= LABEL_COMMON[GenOrgan.Lng];
		CurrDepthBox->Caption                   = LABEL_DEPTH[GenOrgan.Lng];
        ShowCurrTractionViewUnit();             // CurrTractionView
		DepthCalcTabSheet->Caption              = LABEL_DEPTHCALC[GenOrgan.Lng];
//        DepthCalcTabSheetExitButton->Caption  	= LABEL_DISCARD[GenOrgan.Lng];
		DepthCalcTabSheetExitButton->Caption  	= ExitSymbol;
		DepthVisualisation->Unit                = (UnicodeString)(__SEPARATOR01[GenOrgan.Lng]) + UNIT_METER[GenOrgan.Lng];
		ResetButton->Caption      				= LABEL_CLEARING[GenOrgan.Lng];
		UnloadCableButton->Caption           	= LABEL_UNLOADCABLE[GenOrgan.Lng];
//		UnloadCableChancelButton->Caption    	= LABEL_DISCARD[GenOrgan.Lng];
		UnloadCableExitButton->Caption    		= ExitSymbol;
		UnloadCableSetTensionEdit->Caption 		= LABEL_REV_TENSION[GenOrgan.Lng];
		switch (Traction_Dimension_Unit)
        {
                case 1:         // Kilonewton
								UnloadCableSetTensionEdit->BackEnd = UNIT_KILONEWTON[GenOrgan.Lng];
								UnloadCableStretchEndTensionEdit->BackEnd = UNIT_KILONEWTON[GenOrgan.Lng];
								UnloadCableStretchStartTensionEdit->BackEnd = UNIT_KILONEWTON[GenOrgan.Lng];
								break;

				case 2:         // Kilopond
								UnloadCableSetTensionEdit->BackEnd = UNIT_KILOPOND[GenOrgan.Lng];
								UnloadCableStretchEndTensionEdit->BackEnd = UNIT_KILOPOND[GenOrgan.Lng];
								UnloadCableStretchStartTensionEdit->BackEnd = UNIT_KILOPOND[GenOrgan.Lng];
								break;

				default:        // Newton
								UnloadCableSetTensionEdit->BackEnd = UNIT_NEWTON[GenOrgan.Lng];
								UnloadCableStretchEndTensionEdit->BackEnd = UNIT_NEWTON[GenOrgan.Lng];
								UnloadCableStretchStartTensionEdit->BackEnd = UNIT_KILOPOND[GenOrgan.Lng];
								break;
		};
		UnloadCablePanelCaptionLabel->Caption 	= LABEL_UNLOADCABLE[GenOrgan.Lng];
		UnloadCableStretchEndTensionEdit->Caption = LABEL_ENDTENSION[GenOrgan.Lng];
		UnloadCableStretchGroupBox->Caption  	= LABEL_STRETCHING[GenOrgan.Lng];
//		UnloadCableSaveButton->Caption       	= LABEL_ACCEPT[GenOrgan.Lng];
		UnloadCableStretchStartLenghtEdit->Caption = LABEL_BEGINLENGHT_OF_CABLE[GenOrgan.Lng];
		UnloadCableStretchStartLenghtEdit->BackEnd = UNIT_METER[GenOrgan.Lng];
		UnloadCableStretchStartTensionEdit->Caption = LABEL_BEGINTENSION[GenOrgan.Lng];
		UnloadCableStretchEndTensionEdit->Caption = LABEL_ENDTENSION[GenOrgan.Lng];
		UnloadCableStretchEndLenghtEdit->Caption = LABEL_ENDLENGHT_OF_CABLE[GenOrgan.Lng];
		UnloadCableStretchEndLenghtEdit->BackEnd = UNIT_METER[GenOrgan.Lng];
        UnloadCabelStretchCheckBox->Caption  	= LABEL_STRETCHING_ON[GenOrgan.Lng];
        DynamicTensionOnOffLabel->Caption       = LABEL_DYNAMIC_MAX_TENSION[GenOrgan.Lng];
        EmergencySwitchLabel->Caption           = LABEL_EMERGENCYSWITCH[GenOrgan.Lng];
        EngineButton->Caption                   = LABEL_ON_V2[GenOrgan.Lng];
		EngineCoolantTemperatureView->Unit		= UNIT_DEGREE_CELSIUS[GenOrgan.Lng];
		EngineCtlGroupBox->Caption          	= LABEL_ENGINE[GenOrgan.Lng];
		XtraEngineCtlGroupBox->Caption          = LABEL_ENGINE[GenOrgan.Lng];
		EngineStopButton->Caption               = LABEL_OFF[GenOrgan.Lng];
		EngineRevolutionDownButton->Caption     = LABEL_REVOLUTION[GenOrgan.Lng];
		EngineRevolutionUpButton->Caption       = LABEL_REVOLUTION[GenOrgan.Lng];
		EngineSpeedView->UnitLabel				= UNIT_RPM[GenOrgan.Lng];
		DevelFormButton->Caption                = LABEL_DEVELOPER[GenOrgan.Lng];
		Ext_cm_Imp_GroupBox->Caption            = LABEL_EXT_CM_IMPULSE[GenOrgan.Lng];
//        SetFastRun();                           // FastRunButton
		FileAdminButton->Caption                = LABEL_FILEADMIN[GenOrgan.Lng];
		HydraulicCtlGroupBox->Caption 			= LABEL_HYDRAULIC[GenOrgan.Lng];
		HydrLvlView->Caption					= LABEL_TANK[GenOrgan.Lng];
		HydrLvlView->Unit						= UNIT_PERC[GenOrgan.Lng];
		HydrTemperatureView->Unit				= UNIT_DEGREE_CELSIUS[GenOrgan.Lng];
		HydrWrkPressureView->UnitLabel			= UNIT_BAR[GenOrgan.Lng];
		char HVchar[5] = {0x5B,0x4A,0x4D,0x5D,0x00};
#ifdef __DEMO_MODE
		IdentificationSystemLabel->Caption      = (AnsiString)(__PROGNAME) + " " + __VERSION + " " + __DATEOFCOMPIL + " " + __NUMBERPERDATE + " DEMO " + HVchar;
#endif
#ifndef __DEMO_MODE
		IdentificationSystemLabel->Caption      = (AnsiString)(__PROGNAME) + " " + __VERSION + " " + __DATEOFCOMPIL + " " + __NUMBERPERDATE + " " + HVchar;
#endif
		IdentificationSystemLabel->Caption      += (AnsiString)(" ") + HVchar;
		LOGFileAdminCancelButton->Caption       = LABEL_ABORT[GenOrgan.Lng];
		LOGFileAdminCopyButton->Caption         = LABEL_TRANSFUSE[GenOrgan.Lng];
		LOGFileAdminDeleteButton->Caption       = LABEL_DELETE[GenOrgan.Lng];
		LOGFileAdminDestinationLabel->Caption   = LABEL_TRANSFUSEDESTINATION[GenOrgan.Lng];
		LOGFileAdminGermanRadioButton->Caption  = LABEL_GERMAN[GenOrgan.Lng];
		LOGFileAdminEnglishRadioButton->Caption = LABEL_ENGLISH[GenOrgan.Lng];
		LOGFileAdminLanguageGroupBox->Caption   = LABEL_CHARSET_OF_VIEW[GenOrgan.Lng];
		LOGFileAdminRussianRadioButton->Caption = LABEL_RUSSIAN[GenOrgan.Lng];
		LOGFileWellLabel->Caption           	= LABEL_WELL[GenOrgan.Lng];
		LOGFileCancelButton->Caption            = LABEL_DISCARD[GenOrgan.Lng];
		LOGFileCaptionLabel->Caption            = LABEL_LOG_FILE[GenOrgan.Lng];
		LOGFileCommentsGroupBox->Caption        = LABEL_COMMENT[GenOrgan.Lng];
		LOGFileCharacteristicsTabSheet->Caption = LABEL_CHARACTERISTICS[GenOrgan.Lng];
		LOGFileFileAdminTabSheet->Caption       = LABEL_LOG_FILE_ADMINISTRATION[GenOrgan.Lng];
		LOGFileLanguageEnglishRadioButton->Caption = LABEL_ENGLISH[GenOrgan.Lng];
		LOGFileLanguageFormatGroupBox->Caption  = LABEL_FILE_LANGUAGE_FORMAT[GenOrgan.Lng];
		LOGFileLanguageGermanRadioButton->Caption = LABEL_GERMAN[GenOrgan.Lng];
		LOGFileLanguageRussianRadioButton->Caption = LABEL_RUSSIAN[GenOrgan.Lng];
		LOGFileLocationLabel->Caption           = LABEL_LOCATION[GenOrgan.Lng];
		LOGFileNameLabel->Caption               = LABEL_NAME[GenOrgan.Lng];
		LOGFileOkButton->Caption                = LABEL_ACCEPT[GenOrgan.Lng];
		LOGFileTeamLabel->Caption				= LABEL_TEAM[GenOrgan.Lng];
		LOGFileTensionUnitGroupBox->Caption		= LABEL_TENSION_DIMENSIONUNIT[GenOrgan.Lng];
		LOGFileTensionUnit_kN_RadioButton->Caption = UNIT_KILONEWTON[GenOrgan.Lng];
		LOGFileTensionUnit_kp_RadioButton->Caption = UNIT_KILOPOND[GenOrgan.Lng];
		LOGFileTypeGroupBox->Caption            = LABEL_FILETYPE[GenOrgan.Lng];
		LOGFileTypeLAS2RadioButton->Caption     = LABEL_LAS2[GenOrgan.Lng];
		LOGFileTypeGeoSysV1RadioButton->Caption = LABEL_STANDARD_OBSOLETE[GenOrgan.Lng];
		LOGFileTypeGeoSysV2RadioButton->Caption = LABEL_STANDARD[GenOrgan.Lng];
		LOGFileWinchMachineryLabel->Caption		= LABEL_WINCHMACHINERY[GenOrgan.Lng];
		SetLostMotionImpulse();                 // LostMotionImpulseButton
		MagneticMarkOnOffLabel->Caption         = LABEL_MM_SYNC[GenOrgan.Lng];
		MessagePanelButton->Caption				= LABEL_MESSAGES[GenOrgan.Lng];
		MessagePanelOkButton->Caption			= LABEL_READY[GenOrgan.Lng];
		PRelTensionLimitUpperEdit->Caption 		= LABEL_RELTENSIONLIMIT_UPPER[GenOrgan.Lng];
		switch (Traction_Dimension_Unit)
		{
                case 1:         // Kilonewton
								PRelTensionLimitUpperEdit->BackEnd = UNIT_KILONEWTON[GenOrgan.Lng];
                                break;

                case 2:         // Kilopond
								PRelTensionLimitUpperEdit->BackEnd = UNIT_KILOPOND[GenOrgan.Lng];
                                break;

                default:        // Newton
								PRelTensionLimitUpperEdit->BackEnd = UNIT_NEWTON[GenOrgan.Lng];
                                break;
        };
        PCalibTensionGroupBox->Caption         	= LABEL_CALIB_VALUES_TENSION[GenOrgan.Lng];
		PCalibTensionXEdit->Caption           	= LABEL_CALIB_TENSION_X[GenOrgan.Lng];
		PCalibTensionXEdit->BackEnd       		= UNIT_MILLIVOLT[GenOrgan.Lng];
		PCalibTensionYEdit->Caption           	= LABEL_CALIB_TENSION_Y[GenOrgan.Lng];
		PCalibTensionYEdit->BackEnd       		= UNIT_NEWTON[GenOrgan.Lng]; // Eichwerte verbleiben immer in Newton
		PCalibTensionZeroEdit->Caption     		= LABEL_CALIB_TENSION_ZERO[GenOrgan.Lng];
		PCalibTensionZeroEdit->BackEnd   		= UNIT_NEWTON[GenOrgan.Lng];
		PEngineRevolutionChangeableCheckBox->Caption = LABEL_ENGINEREVOLUTIONCHANGEABLE[GenOrgan.Lng];
		PEngineStartStopEnableCheckBox->Caption = LABEL_ENGINESTARTSTOPENABLE[GenOrgan.Lng];
		PLostMotionEnableCheckBox->Caption 		= LABEL_LOSTMOTIONENABLE[GenOrgan.Lng];
		PBrakeOffEnableCheckBox->Caption		= LABEL_BRAKEOFFENABLE[GenOrgan.Lng];
		PHydraulikBrakeEnableCheckBox->Caption  = LABEL_HYDRAULIC_BRAKE[GenOrgan.Lng];
		PTractionViewComboBox->Clear();
        PTractionViewComboBox->Items->Append(UNIT_EXPL_NEWTON[GenOrgan.Lng]);     // Wert 0
        PTractionViewComboBox->Items->Append(UNIT_EXPL_KILONEWTON[GenOrgan.Lng]); // Wert 1
        PTractionViewComboBox->Items->Append(UNIT_EXPL_KILOPOND[GenOrgan.Lng]);   // Wert 2

		PChangeDirectionButton->Caption			= LABEL_DIRECTION[GenOrgan.Lng];
		PIncEncErrorMonitCheckBox->Caption      = LABEL_INCENCERRORMONIT[GenOrgan.Lng];
        PKARATRadioButton->Caption              = LABEL_KARAT[GenOrgan.Lng];
		PKARAT2RadioButton->Caption             = LABEL_KARAT2[GenOrgan.Lng];
        PKARAT2NRadioButton->Caption            = LABEL_KARAT2N[GenOrgan.Lng];
        PKEDRRadioButton->Caption               = LABEL_KEDR[GenOrgan.Lng];
        PKEDRNRadioButton->Caption              = LABEL_KEDRN[GenOrgan.Lng];
		PMaxDepthEdit->Caption                 	= LABEL_MAXDEPTH[GenOrgan.Lng];
		PMaxDepthEdit->BackEnd             		= UNIT_METER[GenOrgan.Lng];
        PMaxTensionEdit->Caption              	= LABEL_MAXTENSION[GenOrgan.Lng];
        switch (Traction_Dimension_Unit)
        {
                case 1:         // Kilonewton
								PMaxTensionEdit->BackEnd = UNIT_KILONEWTON[GenOrgan.Lng];
								break;

				case 2:         // Kilopond
								PMaxTensionEdit->BackEnd = UNIT_KILOPOND[GenOrgan.Lng];
								break;

				default:        // Newton
								PMaxTensionEdit->BackEnd = UNIT_NEWTON[GenOrgan.Lng];
                                break;
        };
        PMinDepthEdit->Caption                 	= LABEL_MINDEPTH[GenOrgan.Lng];
		PMinDepthEdit->BackEnd            		= UNIT_METER[GenOrgan.Lng];
		PMMSyncForceRangeLimEdit->Caption		= LABEL_ENFORCE_MM_SYNC_UNTIL[GenOrgan.Lng];
		PMMSyncForceRangeLimEdit->BackEnd		= UNIT_METER[GenOrgan.Lng];
		PONYXRadioButton->Caption               = LABEL_ONYX[GenOrgan.Lng];
        PowerFailCdLabel->Caption               = LABEL_DURATIONUNTILSHUTDOWN[GenOrgan.Lng];
        PProgramAutomCheckBox->Caption          = LABEL_ON[GenOrgan.Lng];
		PProgramAutomGroupBox->Caption          = LABEL_PROGRAMABLEAUTOMATIC[GenOrgan.Lng];
		PControllerErrorAtSMCheckBox->Caption	= LABEL_CONTROLLERERRORMONIT_ATSM_ENABLE[GenOrgan.Lng];
		PControllerErrorGroupBox->Caption		= LABEL_CONTROLLER[GenOrgan.Lng];
		PControllerErrorMonitCheckBox->Caption  = LABEL_CONTROLLERERRORMONIT[GenOrgan.Lng];
		PControllerErrorLactiveCheckBox->Caption= LABEL_L_ACTIVE[GenOrgan.Lng];
		PReverseDepthCheckBox->Caption			= LABEL_REVERSEDEPTH[GenOrgan.Lng];
		PSetDepthEdit->Caption              	= LABEL_SET_DEPTH[GenOrgan.Lng];
		PSetDepthEdit->BackEnd              	= UNIT_METER[GenOrgan.Lng];
		PDepthCalFactorEdit->Caption     		= LABEL_CALFACTOR[GenOrgan.Lng];
		PDepthCalFactorEdit->BackEnd		    = UNIT_MILLIMETER_PER_IMPULSE[GenOrgan.Lng];
		PRelTensionLimitLowerEdit->Caption      = LABEL_RELTENSIONLIMIT_LOWER[GenOrgan.Lng];
        switch (Traction_Dimension_Unit)
        {
                case 1:         // Kilonewton
								PRelTensionLimitLowerEdit->BackEnd = UNIT_KILONEWTON[GenOrgan.Lng];
                                break;

                case 2:         // Kilopond
								PRelTensionLimitLowerEdit->BackEnd = UNIT_KILOPOND[GenOrgan.Lng];
                                break;

                default:        // Newton
								PRelTensionLimitLowerEdit->BackEnd = UNIT_NEWTON[GenOrgan.Lng];
                                break;
        };
		PTractionViewLabel->Caption             = LABEL_TENSION_DIMENSIONUNIT[GenOrgan.Lng];
		PWinchBrakeInternalAutoCheckBox->Caption= LABEL_WINCHBRAKE_CONTROLED_BY_SW[GenOrgan.Lng];
		PVelocityEdit->Caption                 	= LABEL_REV_VELOCITY[GenOrgan.Lng];
		switch (Velocity_Dimension_Unit) {
			case	1:	// m/min
						PVelocityEdit->BackEnd 	= UNIT_METER_PER_MINUTE[GenOrgan.Lng];
						SpeedView->SpeedUnit   	= UNIT_METER_PER_MINUTE[GenOrgan.Lng];
						break;
			default:	// m/h
						PVelocityEdit->BackEnd 	= UNIT_METER_PER_HOUR[GenOrgan.Lng];
						SpeedView->SpeedUnit	= UNIT_METER_PER_HOUR[GenOrgan.Lng];
		}
		RunTabSheet->Caption                    = LABEL_RUN[GenOrgan.Lng];
//        RunTabSheetExitButton->Caption        	= LABEL_DISCARD[GenOrgan.Lng];
		RunTabSheetExitButton->Caption      	= ExitSymbol;
//        SetDirectionLabel->Caption              = LABEL_DIRECTION[GenOrgan.Lng];
		SetParameterButton->Caption             = LABEL_SET_PARAMETER[GenOrgan.Lng];
//        SetVelocityUnitLabel->Caption           = UNIT_METER_PER_HOUR[GenOrgan.Lng];
//        SignalHornButton->Caption               = LABEL_SIGNAL[GenOrgan.Lng];
		SetStartStop();                         // StartStopButton
		SpecialFuncLabel->Caption				= LABEL_LUBRICATE_LINE[GenOrgan.Lng];
		SpecialSignalModeGroupBox->Caption      = LABEL_SPECIAL_SIGNALS[GenOrgan.Lng];
        PSpecialSignalMode0RadioButton->Caption = LABEL_IGNORE[GenOrgan.Lng];
        PSpecialSignalMode1RadioButton->Caption = LABEL_TWO_SENSOR_GEARSHIFT_CTRL[GenOrgan.Lng];
        PSpecialSignalMode2RadioButton->Caption = LABEL_TWO_EXT_EQUIPMENT_ERRORSIGNALS[GenOrgan.Lng];
		PSpecialSignalMode3RadioButton->Caption = LABEL_ADDITIONAL_OILSTRAINER_MONIT[GenOrgan.Lng];
		PSpecialSignalMode4RadioButton->Caption = LABEL_RIG_LOCK_MONITOR[GenOrgan.Lng];
		PSpecialSignalMode5RadioButton->Caption = LABEL_EXT_BRAKE_MONITORING[GenOrgan.Lng];
		PVelocityViewComboBox->Clear();
		PVelocityViewComboBox->Items->Append(UNIT_EXPL_METER_PER_HOUR[GenOrgan.Lng]);
		PVelocityViewComboBox->Items->Append(UNIT_EXPL_METER_PER_MINUTE[GenOrgan.Lng]);
		PVelocityViewLabel->Caption				= LABEL_VELOCITY_DIMENSIONUNIT[GenOrgan.Lng];
		PLineLubGroupBox->Caption				= LABEL_LINELUBRICATOR[GenOrgan.Lng];
		PLineLubAvailableCheckBox->Caption      = LABEL_AVAILABLE[GenOrgan.Lng];
		PLineLubPermanentOperationCheckBox->Caption = LABEL_PERMANENTOPERATION[GenOrgan.Lng];
		SpeedView->Caption                		= LABEL_VELOCITY[GenOrgan.Lng];
		SuperVisionTabSheet->Caption            = LABEL_SUPERVISION[GenOrgan.Lng];
//        SuperVisionTabSheetExitButton->Caption	= LABEL_DISCARD[GenOrgan.Lng];
		SuperVisionTabSheetExitButton->Caption	= ExitSymbol;
		SystemButton->Caption                   = LABEL_SYSTEM[GenOrgan.Lng];
        SystemCancelButton->Caption             = LABEL_ABORT[GenOrgan.Lng];
        SystemControlGroupBox->Caption          = LABEL_SYSTEM[GenOrgan.Lng];
        SystemDateLabel->Caption                = LABEL_DATE[GenOrgan.Lng];
        SystemDateTimeAcceptButton->Caption     = LABEL_ACCEPT[GenOrgan.Lng];
		SystemDatetimeGroupBox->Caption         = LABEL_DATETIMETUNING[GenOrgan.Lng];
        SystemProgramTerminationButton->Caption = LABEL_EXIT_PROGRAM[GenOrgan.Lng];
        SystemRestartButton->Caption            = LABEL_SYSTEM_RESTART[GenOrgan.Lng];
        SystemShutdownButton->Caption           = LABEL_SYSTEM_TURNOFF[GenOrgan.Lng];
        SystemTempADescrLabel->Caption          = LABEL_SENSOR_A[GenOrgan.Lng];
        SystemTempBDescrLabel->Caption          = LABEL_SENSOR_B[GenOrgan.Lng];
        SystemTempDepthCalcDescrLabel->Caption  = LABEL_SENSOR_DEPTHCALC[GenOrgan.Lng];
        SystemTemperatureGroupBox->Caption      = LABEL_TEMPERATURES[GenOrgan.Lng];
        ShowTemperatures(true);                 // Temperaturwerte
        SystemTimeLabel->Caption                = LABEL_TIMEOFDAY[GenOrgan.Lng];
        ViewTabSheet->Caption                   = LABEL_VIEW[GenOrgan.Lng];
//        ViewTabSheetExitButton->Caption       	= LABEL_DISCARD[GenOrgan.Lng];
		ViewTabSheetExitButton->Caption       	= ExitSymbol;
		WinchDriveCtlGroupBox->Caption			= LABEL_WINCH[GenOrgan.Lng];
		WinchTabSheet->Caption                  = LABEL_WINCH[GenOrgan.Lng];
//        WinchTabSheetExitButton->Caption      	= LABEL_DISCARD[GenOrgan.Lng];
		WinchTabSheetExitButton->Caption      	= ExitSymbol;
		SetWinchBrakeOffButton();               // WinchBrakeOffSpeedButton

    	// ListView
		PWinchCfgListBox->Items->Clear();		// Ggf. alle Einträge entfernen
		try {
			for (int i = 1; i <= _WinchCnt; ++i) {
				PWinchCfgListBox->Items->Add(static_cast<UnicodeString>(LABEL_CONFIGURATION[GenOrgan.Lng]) + __SEPARATOR01[GenOrgan.Lng] + IntToStr(i));
			}
		} catch (...) {
		}
};


// ==========================================================================
// KONVERTIERUNG ZUR DATENEDARSTELLUNG UND ZURÜCK
// ==========================================================================

// --------------------------------------------------------------------------
// erstellen des Strings für die Teufenanzeige
// mit Berücksichtigung der gewählten Einheit
AnsiString __fastcall TMainForm::MakeDepthString(int Value, int Digits, bool UnitOn)
{
		float FValue = (float)(Value) / 1000;
		AnsiString tmpStr = FloatToStrF(FValue, ffFixed, 7, Digits);

		if (UnitOn) tmpStr = tmpStr + __SEPARATOR01[GenOrgan.Lng] + UNIT_METER[GenOrgan.Lng];

		return tmpStr;

};

// --------------------------------------------------------------------------
// ermittelt den Wert aus einem String und gibt diesen im internen Standartformat aus
bool __fastcall TMainForm::MakeDepthValue(AnsiString TheString, int *Value)
{
        int HV;
        float HVfloat;

        TheString = TheString.Trim();

        // evtl. vorhandene Einheit entfernen
        HV = TheString.Pos(__SEPARATOR01[GenOrgan.Lng]);
        if (HV != 0) TheString.SetLength(HV);

        try
        {
                HVfloat = StrToFloat(TheString);
        }catch(...)
        {
				return false;
        };

		// Sinfälligkeits-Test
// 24.05.2012 Negative Teufeneingabe erlauben
//		if (HVfloat < 0) return false;                                          // keine neg. Geschwindigkeit möglich

		// neuen Wert übergeben
// 24.05.2012 Unterscheidung von negativer und positiver Teufeneingabe
		if (HVfloat < 0) HVfloat = (HVfloat * 1000) - 0.5;
		else HVfloat = (HVfloat * 1000) + 0.5;
		*Value = HVfloat;

        return true;
};
// --------------------------------------------------------------------------

// erstellen des Strings für die Zugkraftanzeige mit Berücksichtigung der gewählten Einheit
AnsiString __fastcall TMainForm::MakeTimeString(int Value, bool UnitOn)
{
        AnsiString tmpStr = IntToStr(Value);

        if (UnitOn) tmpStr = tmpStr + __SEPARATOR01[GenOrgan.Lng] + UNIT_SECOUND[GenOrgan.Lng];

        return tmpStr;
};

// --------------------------------------------------------------------------
// ermittelt den Wert aus einem String und gibt diesen im internen Standartformat aus
bool __fastcall TMainForm::MakeTimeValue(AnsiString TheString, int *Value)
{
        int HV;
        float HVfloat;

        TheString = TheString.Trim();

        // evtl. vorhandene Einheit entfernen
        HV = TheString.Pos(__SEPARATOR01[GenOrgan.Lng]);
        if (HV != 0) TheString.SetLength(HV);

        try
        {
                HVfloat = StrToFloat(TheString);
        }catch(...)
        {
                return false;
        };

        // neuen Wert übergeben
        HVfloat += 0.5;

        // Sinfälligkeits-Test

        *Value = HVfloat;

        return true;
};

// --------------------------------------------------------------------------
// erstellen des Strings für die Zugkraftanzeige
// mit Berücksichtigung der gewählten Einheit
// Value        ...Wert in Newton
// UnitOn       ...Maßeinheit an String anhängen
AnsiString __fastcall TMainForm::MakeTensionString(int Value, bool UnitOn)
{
        int Dimension_Unit = VarControl.GetParam_TractionViewDimensionUnit();
        AnsiString tmpStr;

        // Wert ermitteln
		tmpStr = IntToStr((int)(TractionViewApplyDimension(Value) + 0.5));

        // Bei Bedarf Maßeinheit anhängen
        if (UnitOn)
        switch (Dimension_Unit)
        {
                case 1: // Kilonewton
                        tmpStr = tmpStr + __SEPARATOR01[GenOrgan.Lng] + UNIT_KILONEWTON[GenOrgan.Lng];
                        break;

                case 2: // Kilopond
                        tmpStr = tmpStr + __SEPARATOR01[GenOrgan.Lng] + UNIT_KILOPOND[GenOrgan.Lng];
                        break;

                default:// Newton
                        tmpStr = tmpStr + __SEPARATOR01[GenOrgan.Lng] + UNIT_NEWTON[GenOrgan.Lng];
                        break;
        };

        return tmpStr;
};

// --------------------------------------------------------------------------
// ermittelt den Wert aus einem String und gibt diesen im internen Standartformat aus
bool __fastcall TMainForm::MakeTractionValue(AnsiString TheString, int *Value)
{
        int HV;
        float HVfloat;

        TheString = TheString.Trim();

        // evtl. vorhandene Einheit entfernen
        HV = TheString.Pos(__SEPARATOR01[GenOrgan.Lng]);
        if (HV != 0) TheString.SetLength(HV);

        try
        {
                HVfloat = StrToFloat(TheString);
        }catch(...)
        {
                return false;
        };

        HVfloat = TractionViewClearDimension(HVfloat);

        // neuen Wert übergeben
        HVfloat += 0.5;

        // Sinfälligkeits-Test

        *Value = static_cast<int>(HVfloat);

        return true;
};

// --------------------------------------------------------------------------
// erstellen des Strings für die Geschwindigkeitsanzeige
// mit Berücksichtigung der gewählten Einheit
UnicodeString __fastcall TMainForm::MakeVelocityString(int Value, bool UnitOn)
{
		UnicodeString tmpStr, UnitStr;

		switch (VarControl.GetParam_VelocityDimensionUnit()) {
		case	1:	// m/min
			tmpStr = FloatToStrF(static_cast<float>(Value) / 60, ffFixed, 7, 1);
			UnitStr = UNIT_METER_PER_MINUTE[GenOrgan.Lng];
			break;
		default:
			tmpStr = IntToStr(Value);
			UnitStr = UNIT_METER_PER_HOUR[GenOrgan.Lng];
		}

		if (UnitOn) {
			tmpStr = tmpStr + __SEPARATOR01[GenOrgan.Lng] + UnitStr;
		}

		return tmpStr;
};

// --------------------------------------------------------------------------
// ermittelt den Wert aus einem String und gibt diesen im internen Standartformat aus
// Standardformat ist m/h
bool __fastcall TMainForm::MakeVelocityValue(UnicodeString TheString, int *Value)
{
        int HV;
        float HVfloat;

        TheString = TheString.Trim();

        // evtl. vorhandene Einheit entfernen
        HV = TheString.Pos(__SEPARATOR01[GenOrgan.Lng]);
        if (HV != 0) TheString.SetLength(HV);

        try
        {
                HVfloat = StrToFloat(TheString);
        }catch(...)
        {
                return false;
        };

        // Sinfälligkeits-Test
        if (HVfloat <= 0) return false;                                         // keine neg. Geschwindigkeit möglich

        // neuen Wert übergeben
		switch (VarControl.GetParam_VelocityDimensionUnit()) {
		case 1:		// m/min
			*Value = HVfloat * 60;
			break;
		default:	// m/h
			*Value = HVfloat;
			;
		}

        return true;
};

// --------------------------------------------------------------------------
// erstellen der Windenbezeichnung anhand der angegebenen Winden-Nummer
AnsiString __fastcall TMainForm::MakeWinchNameString(unsigned int Value)
{
        switch(VarControl.GetParam_WinchNo())
        {
				case 0:         return LABEL_LEFT_WINCHCHOISE[GenOrgan.Lng];
                case 1:         return LABEL_RIGHT_WINCHCHOISE[GenOrgan.Lng];
                default :       break;
        };

        return "";
};

// --------------------------------------------------------------------------
// Wendet auf einen Standardzugkraftwert die Maßeinheit der Ausgabe an
float __fastcall TMainForm::TractionViewApplyDimension(float OriginalValue)
{
        switch (VarControl.GetParam_TractionViewDimensionUnit())
        {
                case 1: // Kilonewton
                        OriginalValue /= 1000;
                        break;

                case 2: // Kilopond
                        OriginalValue /= __GFORCE;
                        break;

                default:// Newton
                        // Wert in HVfloat ist bereits im gewünschten Format
                        break;
        };

        return OriginalValue;
};

// --------------------------------------------------------------------------
// Bringt einen Zugkraftwert mit der  Maßeinheit der Ausgabe in den programminternen Standardzugkraftwert
float __fastcall TMainForm::TractionViewClearDimension(float OriginalValue)
{
        switch (VarControl.GetParam_TractionViewDimensionUnit())
        {
                case 1: // Kilonewton
                        OriginalValue *= 1000;
                        break;

                case 2: // Kilopond
                        OriginalValue *= __GFORCE;
                        break;

                default:// Newton
                        // Wert in HVfloat ist bereits im gewünschten Format
                        break;
        };

        return OriginalValue;
};

// ==========================================================================
// DATENERFASSUNG IM SYSTEMUMFELD
// ==========================================================================

// --------------------------------------------------------------------------
// Erfasst die Liste der Ablaufprogramme für die Programmautomatik
void __fastcall TMainForm::GetAutoRunProgramList()
{
    TSearchRec sr;
    UnicodeString fpath = L"*";
    fpath = fpath + __AutoRunPrgFileExtention;
    wchar_t Hwstring[WCPrgAutomaticPRGNAMELENGTH];

    if (FindFirst(fpath, faReadOnly | faHidden | faArchive, sr) == 0) {

        do {

            if (GenOrgan.PrgAutomatic.Read(sr.FindData.cFileName)) {
                PProgramAutomFileListBox->Items->Add(sr.FindData.cFileName);
            }
            GenOrgan.PrgAutomatic.Clear();

        } while (FindNext(sr) == 0);

    }

    FindClose(sr);

}


// ==========================================================================
// DATENDARSTELLUNG
// ==========================================================================

// --------------------------------------------------------------------------
// Abhängigkeit vom Zustand
void __fastcall TMainForm::SetClearAfterThreshold()
{
/*        if (VarControl.GetControl_ClearAfterThreshold() == _ON)
        {
                ClearAfterThresholdButton->Down = true;
        }else
        {
                ClearAfterThresholdButton->Down = false;
        };
*/
};

// --------------------------------------------------------------------------
// Abhängigkeit vom Zustand
void __fastcall TMainForm::SetFastRun()
{
        int HV = 0;

        if (VarControl.GetControl_FastRun() == _ON) HV = 1;

        if (Status_FastRunButton == HV) return;
        Status_FastRunButton = HV;

        switch (HV)
        {
                case 1: // Schnell (ACHTUNG, Beschriftung zeigt nicht Zustand sondern Möglichkeit an!)
                        FastRunButton->Down = true;

                        try
                        {
                                // Speicher allokieren
                                std::auto_ptr<Graphics::TBitmap> DummyImage1(new Graphics::TBitmap());
                                std::auto_ptr<Graphics::TBitmap> DummyImage2(new Graphics::TBitmap());

                                DummyImage1->LoadFromResourceName((int)HInstance, "TURTLE");
                                DummyImage2->Height = FastRunButton->Height - 2 * LAYOUT_GUI_Margin;
                                if (DummyImage2->Height > LAYOUT_ControlPanelButtonSymbolSize) DummyImage2->Height = LAYOUT_ControlPanelButtonSymbolSize;
                                DummyImage2->Width = 2 * DummyImage2->Height;
                                DummyImage2->Canvas->StretchDraw(Rect(0, 0, DummyImage2->Width, DummyImage2->Height), DummyImage1.get());
                                FastRunButton->Glyph->Assign(DummyImage2.get());
                                FastRunButton->NumGlyphs = 2;
                        } catch(...){};

//                        FastRunButton->Caption = LABEL_SLOW[GenOrgan.Lng];

                        break;
                default:// Langsam (ACHTUNG, Beschriftung zeigt nicht Zustand sondern Möglichkeit an!)
                        FastRunButton->Down = false;

                        try
                        {
                                // Speicher allokieren
                                std::auto_ptr<Graphics::TBitmap> DummyImage1(new Graphics::TBitmap());
                                std::auto_ptr<Graphics::TBitmap> DummyImage2(new Graphics::TBitmap());

                                DummyImage1->LoadFromResourceName((int)HInstance, "RABBIT");
                                DummyImage2->Height = FastRunButton->Height - 2 * LAYOUT_GUI_Margin;
                                if (DummyImage2->Height > LAYOUT_ControlPanelButtonSymbolSize) DummyImage2->Height = LAYOUT_ControlPanelButtonSymbolSize;
                                DummyImage2->Width = 2 * DummyImage2->Height;
                                DummyImage2->Canvas->StretchDraw(Rect(0, 0, DummyImage2->Width, DummyImage2->Height), DummyImage1.get());
                                FastRunButton->Glyph->Assign(DummyImage2.get());
                                FastRunButton->NumGlyphs = 2;
                        } catch(...){};

//                        FastRunButton->Caption = LABEL_FAST[GenOrgan.Lng];

                        break;
        };
};

// --------------------------------------------------------------------------
// Abhängigkeit vom Zustand
void __fastcall TMainForm::SetLostMotionImpulse()
{
//        if ((VarControl.GetInfo_LostMotion() == _OFF) || (VarControl.GetControl_WinchBrakeOff() == _ON))
		if (VarControl.GetInfo_LostMotion() == _OFF) {
				LostMotionImpulseButton->Down = false;
				LostMotionImpulseButton->Caption= LABEL_LOSTMOTIONON[GenOrgan.Lng];

				// Freilauftaste je nach Einstellung verfügbar oder nicht
				if (VarControl.GetParam_LostMotionEnable()) LostMotionImpulseButton->Visible = true;
				else LostMotionImpulseButton->Visible = false;
        }else {
                LostMotionImpulseButton->Down = true;
                LostMotionImpulseButton->Caption= LABEL_LOSTMOTIONOFF[GenOrgan.Lng];

            	// Freilauftaste unabhängig von Einstellung verfügbar
				LostMotionImpulseButton->Visible = true;
		};
};

// --------------------------------------------------------------------------
// Abhängigkeit vom Zustand
void __fastcall TMainForm::SetStartStop()
{
		// KABEL ENTLADEN
		if (VarControl.GetControl_UnloadCable() == _ON)
        {
                if (VarControl.GetControl_UnloadCableActive() == _OFF)
                {
                        StartStopButton->Caption = LABEL_START_UNLOAD_CABLE[GenOrgan.Lng];
                        StartStopButton->Down = false;
                }else
                {
                        StartStopButton->Caption = LABEL_STOP_UNLOAD_CABLE[GenOrgan.Lng];
                        StartStopButton->Down = true;
                };
        }else
        // AUTOMATIK-BETRIEB
        {
                if (VarControl.GetControl_AutoRunOn() == _OFF)
                {
                        StartStopButton->Caption = LABEL_START_AUTO[GenOrgan.Lng];
                        StartStopButton->Down = false;
                }else
                {
                        StartStopButton->Caption = LABEL_STOP_AUTO[GenOrgan.Lng];
                        StartStopButton->Down = true;
                };
        };
};

// --------------------------------------------------------------------------
// Abhängigkeit vom Zustand
void __fastcall TMainForm::SetWinchBrakeOffButton()
{
        if (VarControl.GetControl_WinchBrakeOff() == _ON)
        {
                WinchBrakeOffSpeedButton->Down = true;
                WinchBrakeOffSpeedButton->Caption = LABEL_AUTOMATIC_BRAKE[GenOrgan.Lng];
        }else
        {
                WinchBrakeOffSpeedButton->Down = false;
                WinchBrakeOffSpeedButton->Caption = LABEL_DEACTIVATE_BRAKE[GenOrgan.Lng];
        };
};

// --------------------------------------------------------------------------
// Blockierung des Keyboards
// Alle in VKBMask gesetzten Bits Blockieren entsprechende Tasten des virtuellen Keyboards
// Die Bit-Maske wird durch die Konstanten _VKB_... definiert (siehe constants.h)
void __fastcall TMainForm::VKeyBoardBlock(unsigned int VKBMask)
{
        // ESC
        if ((VKBMask & _VKB_ESC) != 0) VirtualKeyboard21->KeyBtnActivate(0, 0, false);
        else VirtualKeyboard21->KeyBtnActivate(0, 0, true);

        // TAB
        if ((VKBMask & _VKB_TAB) != 0) VirtualKeyboard21->KeyBtnActivate(0, 2, false);
        else VirtualKeyboard21->KeyBtnActivate(0, 2, true);

        // CAPS SHIFT
        if ((VKBMask & _VKB_CAPS_SHIFT) != 0)
        {
                VirtualKeyboard21->KeyBtnActivate(0, 3, false);
                VirtualKeyboard21->KeyBtnActivate(0, 4, false);
        }else
        {
                VirtualKeyboard21->KeyBtnActivate(0, 3, true);
                VirtualKeyboard21->KeyBtnActivate(0, 4, true);
        };

        // CTRL
        if ((VKBMask & _VKB_CTRL) != 0) VirtualKeyboard21->KeyBtnActivate(0, 5, false);
        else VirtualKeyboard21->KeyBtnActivate(0, 5, true);

        // ALT
        if ((VKBMask & _VKB_ALT) != 0) VirtualKeyboard21->KeyBtnActivate(2, 5, false);
        else VirtualKeyboard21->KeyBtnActivate(2, 5, true);

        // SPACE
        if ((VKBMask & _VKB_SPACE) != 0) VirtualKeyboard21->KeyBtnActivate(3, 5, false);
        else VirtualKeyboard21->KeyBtnActivate(3, 5, true);

        // Layout
        if ((VKBMask & _VKB_LAYOUT) != 0) VirtualKeyboard21->KeyBtnActivate(10, 5, false);
        else VirtualKeyboard21->KeyBtnActivate(10, 5, true);

        // Backspace
        if ((VKBMask & _VKB_BACKSPACE) != 0) VirtualKeyboard21->KeyBtnActivate(13, 1, false);
        else VirtualKeyboard21->KeyBtnActivate(13, 1, true);

        // Return
        if ((VKBMask & _VKB_RETURN) != 0) VirtualKeyboard21->KeyBtnActivate(13, 2, false);
        else VirtualKeyboard21->KeyBtnActivate(13, 2, true);

        // Arrow
        if ((VKBMask & _VKB_ARROW) != 0)
        {
                VirtualKeyboard21->KeyBtnActivate(13, 5, false);
                VirtualKeyboard21->KeyBtnActivate(14, 4, false);
                VirtualKeyboard21->KeyBtnActivate(14, 5, false);
                VirtualKeyboard21->KeyBtnActivate(15, 5, false);
        }else
        {
                VirtualKeyboard21->KeyBtnActivate(13, 5, true);
                VirtualKeyboard21->KeyBtnActivate(14, 4, true);
                VirtualKeyboard21->KeyBtnActivate(14, 5, true);
                VirtualKeyboard21->KeyBtnActivate(15, 5, true);
        };

        // DEL
        if ((VKBMask & _VKB_DEL) != 0) VirtualKeyboard21->KeyBtnActivate(15, 1, false);
        else VirtualKeyboard21->KeyBtnActivate(15, 1, true);

        // INS
        if ((VKBMask & _VKB_INS) != 0) VirtualKeyboard21->KeyBtnActivate(15, 2, false);
        else VirtualKeyboard21->KeyBtnActivate(15, 2, true);

        // HOME
        if ((VKBMask & _VKB_HOME) != 0) VirtualKeyboard21->KeyBtnActivate(15, 3, false);
        else VirtualKeyboard21->KeyBtnActivate(15, 3, true);

        // END
        if ((VKBMask & _VKB_END) != 0) VirtualKeyboard21->KeyBtnActivate(15, 4, false);
        else VirtualKeyboard21->KeyBtnActivate(15, 4, true);

        // Alle sonstige Zeichen-Tasten außer Ziffern und entspr. Zubehör (Komma, ...)
        if ((VKBMask & _VKB_ALPHA) != 0)
        {
                VirtualKeyboard21->KeyBtnActivate(1, 2, false);
                VirtualKeyboard21->KeyBtnActivate(2, 2, false);
                VirtualKeyboard21->KeyBtnActivate(3, 2, false);
                VirtualKeyboard21->KeyBtnActivate(4, 2, false);
                VirtualKeyboard21->KeyBtnActivate(5, 2, false);
                VirtualKeyboard21->KeyBtnActivate(6, 2, false);
                VirtualKeyboard21->KeyBtnActivate(7, 2, false);
                VirtualKeyboard21->KeyBtnActivate(8, 2, false);
                VirtualKeyboard21->KeyBtnActivate(9, 2, false);
                VirtualKeyboard21->KeyBtnActivate(10, 2, false);
                VirtualKeyboard21->KeyBtnActivate(11, 2, false);
                VirtualKeyboard21->KeyBtnActivate(12, 2, false);
                VirtualKeyboard21->KeyBtnActivate(1, 3, false);
                VirtualKeyboard21->KeyBtnActivate(2, 3, false);
                VirtualKeyboard21->KeyBtnActivate(3, 3, false);
                VirtualKeyboard21->KeyBtnActivate(4, 3, false);
                VirtualKeyboard21->KeyBtnActivate(5, 3, false);
                VirtualKeyboard21->KeyBtnActivate(6, 3, false);
                VirtualKeyboard21->KeyBtnActivate(7, 3, false);
                VirtualKeyboard21->KeyBtnActivate(8, 3, false);
                VirtualKeyboard21->KeyBtnActivate(9, 3, false);
                VirtualKeyboard21->KeyBtnActivate(10, 3, false);
                VirtualKeyboard21->KeyBtnActivate(11, 3, false);
                VirtualKeyboard21->KeyBtnActivate(12, 3, false);
                VirtualKeyboard21->KeyBtnActivate(1, 4, false);
                VirtualKeyboard21->KeyBtnActivate(2, 4, false);
                VirtualKeyboard21->KeyBtnActivate(3, 4, false);
                VirtualKeyboard21->KeyBtnActivate(4, 4, false);
                VirtualKeyboard21->KeyBtnActivate(5, 4, false);
                VirtualKeyboard21->KeyBtnActivate(6, 4, false);
                VirtualKeyboard21->KeyBtnActivate(7, 4, false);
                VirtualKeyboard21->KeyBtnActivate(8, 4, false);
        }else
        {
                VirtualKeyboard21->KeyBtnActivate(1, 2, true);
                VirtualKeyboard21->KeyBtnActivate(2, 2, true);
                VirtualKeyboard21->KeyBtnActivate(3, 2, true);
                VirtualKeyboard21->KeyBtnActivate(4, 2, true);
                VirtualKeyboard21->KeyBtnActivate(5, 2, true);
                VirtualKeyboard21->KeyBtnActivate(6, 2, true);
                VirtualKeyboard21->KeyBtnActivate(7, 2, true);
                VirtualKeyboard21->KeyBtnActivate(8, 2, true);
                VirtualKeyboard21->KeyBtnActivate(9, 2, true);
                VirtualKeyboard21->KeyBtnActivate(10, 2, true);
                VirtualKeyboard21->KeyBtnActivate(11, 2, true);
                VirtualKeyboard21->KeyBtnActivate(12, 2, true);
                VirtualKeyboard21->KeyBtnActivate(1, 3, true);
                VirtualKeyboard21->KeyBtnActivate(2, 3, true);
                VirtualKeyboard21->KeyBtnActivate(3, 3, true);
                VirtualKeyboard21->KeyBtnActivate(4, 3, true);
                VirtualKeyboard21->KeyBtnActivate(5, 3, true);
                VirtualKeyboard21->KeyBtnActivate(6, 3, true);
                VirtualKeyboard21->KeyBtnActivate(7, 3, true);
                VirtualKeyboard21->KeyBtnActivate(8, 3, true);
                VirtualKeyboard21->KeyBtnActivate(9, 3, true);
                VirtualKeyboard21->KeyBtnActivate(10, 3, true);
                VirtualKeyboard21->KeyBtnActivate(11, 3, true);
                VirtualKeyboard21->KeyBtnActivate(12, 3, true);
                VirtualKeyboard21->KeyBtnActivate(1, 4, true);
                VirtualKeyboard21->KeyBtnActivate(2, 4, true);
                VirtualKeyboard21->KeyBtnActivate(3, 4, true);
                VirtualKeyboard21->KeyBtnActivate(4, 4, true);
                VirtualKeyboard21->KeyBtnActivate(5, 4, true);
                VirtualKeyboard21->KeyBtnActivate(6, 4, true);
                VirtualKeyboard21->KeyBtnActivate(7, 4, true);
                VirtualKeyboard21->KeyBtnActivate(8, 4, true);
        };
        // Alle Zifferntasten und entspr. Zubehör (Komma, ...)
        if ((VKBMask & _VKB_NUMERIC) != 0)
        {
                VirtualKeyboard21->KeyBtnActivate(1, 1, false);
                VirtualKeyboard21->KeyBtnActivate(2, 1, false);
                VirtualKeyboard21->KeyBtnActivate(3, 1, false);
                VirtualKeyboard21->KeyBtnActivate(4, 1, false);
                VirtualKeyboard21->KeyBtnActivate(5, 1, false);
                VirtualKeyboard21->KeyBtnActivate(6, 1, false);
                VirtualKeyboard21->KeyBtnActivate(7, 1, false);
                VirtualKeyboard21->KeyBtnActivate(8, 1, false);
                VirtualKeyboard21->KeyBtnActivate(9, 1, false);
                VirtualKeyboard21->KeyBtnActivate(10, 1, false);
                VirtualKeyboard21->KeyBtnActivate(9, 4, false);
                VirtualKeyboard21->KeyBtnActivate(10, 4, false);
                VirtualKeyboard21->KeyBtnActivate(11, 4, false);
        }else
        {
                VirtualKeyboard21->KeyBtnActivate(1, 1, true);
                VirtualKeyboard21->KeyBtnActivate(2, 1, true);
                VirtualKeyboard21->KeyBtnActivate(3, 1, true);
                VirtualKeyboard21->KeyBtnActivate(4, 1, true);
                VirtualKeyboard21->KeyBtnActivate(5, 1, true);
                VirtualKeyboard21->KeyBtnActivate(6, 1, true);
                VirtualKeyboard21->KeyBtnActivate(7, 1, true);
                VirtualKeyboard21->KeyBtnActivate(8, 1, true);
                VirtualKeyboard21->KeyBtnActivate(9, 1, true);
                VirtualKeyboard21->KeyBtnActivate(10, 1, true);
                VirtualKeyboard21->KeyBtnActivate(9, 4, true);
                VirtualKeyboard21->KeyBtnActivate(10, 4, true);
                VirtualKeyboard21->KeyBtnActivate(11, 4, true);
        };

};

// --------------------------------------------------------------------------
// Initialisiert die Akustische Alarm-Signalisierung
void __fastcall TMainForm::PlayAlarmSound()
{
	bool HAlarm = false;
	bool HWarning = false;


	if (GenOrgan.Evnt.UnconfirmedEvents(EVNTOBJTYPE_ERROR) > 0) {

		if (!SND_Alarm) {	// Alarm einschalten
			SND_Alarm = true;
			PlaySound("ALARM", HInstance, SND_ASYNC | SND_LOOP | SND_RESOURCE);
		}
		SND_Warning = false;

	} else if (GenOrgan.Evnt.UnconfirmedEvents(EVNTOBJTYPE_WARNING) > 0) {

		if (!SND_Warning) {	// Warnung einschalten
			SND_Warning = true;
			PlaySound("WARNING", HInstance, SND_ASYNC | SND_LOOP | SND_RESOURCE);
		}
		SND_Alarm = false;

	} else {

		PlaySound(NULL, NULL, SND_ASYNC);	// Alle Töne abschalten
		SND_Alarm = false;
		SND_Warning = false;

	}
};

// --------------------------------------------------------------------------
// Zeigt die aktuelle Einstellung für die Programmautomatik an
void __fastcall TMainForm::ShowAutoRunProgramable()
{

    int HV;

    UnicodeString HString = VarControl.GetParam_AutoRunProgramFileName();
    HString = HString + __AutoRunPrgFileExtention;

    HV = PProgramAutomFileListBox->Items->IndexOf(HString);
    if (HV >= 0) PProgramAutomFileListBox->Selected[HV] = true;

    PProgramAutomPrgNameLabel->Caption = GenOrgan.PrgAutomatic.GetPrgName();

    PProgramAutomCheckBox->Checked = VarControl.GetParam_AutoRunProgramableOn();

}

// --------------------------------------------------------------------------
// Zeigt die AblaufProgrammpunktliste mit dem aktuellen Inhalt an, oder verbirgt ihn
void __fastcall TMainForm::ShowAutoRunProgramItemList()
{
    UnicodeString HString;
    int HV;
    float HVfloat;

    // -----------------------
    if (VarControl.GetParam_AutoRunProgramableOn() == _ON) {
        // Programmautomatik aktiv

        AutoRunProgramItemListLabel->Caption = GenOrgan.PrgAutomatic.GetPrgName();

        AutoRunProgramItemListListBox->Clear();
        HV = 0;

        // Programmpunkte in Liste eintragen
        while (HV < GenOrgan.PrgAutomatic.GetIndexCount()) {
            if (GenOrgan.PrgAutomatic.GetVelocity(HV) > 0) {
                // Programmpunkt Fahren
                // Fahrgeschwindigkeit
				switch (VarControl.GetParam_VelocityDimensionUnit()) {
				case	1:
					HString = FloatToStrF(static_cast<float>(GenOrgan.PrgAutomatic.GetVelocity(HV)) / 60, ffFixed, 7, 1) + UNIT_METER_PER_MINUTE[GenOrgan.Lng] + L" ";
                	break;
				default:
					HString = IntToStr(GenOrgan.PrgAutomatic.GetVelocity(HV)) + UNIT_METER_PER_HOUR[GenOrgan.Lng] + L" ";
				}

                // Fahrtrichtung
                if (!GenOrgan.PrgAutomatic.DirectionIgnored(HV)) {
                    if (GenOrgan.PrgAutomatic.GetDirection(HV) == _UPward) HString = HString + LABEL_UPWARD[GenOrgan.Lng] + L" ";
                    else HString = HString + LABEL_DOWNWARD[GenOrgan.Lng] + L" ";
                }
                // Zielteufe
                HVfloat = GenOrgan.PrgAutomatic.GetTargetDepth(HV);
                HString = HString + L"-> " + FloatToStrF(HVfloat / 1000, ffFixed, 7, 3) + UNIT_METER[GenOrgan.Lng];
                AutoRunProgramItemListListBox->Items->Add(HString);
                ++HV;
            } else {
                // Programmpunkt Stopp
                AutoRunProgramItemListListBox->Items->Add(LABEL_STOP[GenOrgan.Lng]);
                ++HV;
            }

        };

        AutoRunProgramItemListGroupBox->Visible = true;

    // -----------------------
    } else {
        // Programmautomatik inaktiv

        AutoRunProgramItemListGroupBox->Visible = false;
    }
}

// --------------------------------------------------------------------------
// Zeigt den aktuellen Ablaufprogrammpunkt an
void __fastcall TMainForm::ShowAutoRunCurrProgramIndex()
{
    if (AutoRunProgramItemListListBox->Visible) {
        if (AutoRunProgramItemList_TmpIndex != GenOrgan.PrgAutoIndex) {
            AutoRunProgramItemList_TmpIndex = GenOrgan.PrgAutoIndex;
            AutoRunProgramItemListListBox->ItemIndex = GenOrgan.PrgAutoIndex;
        }
    } else AutoRunProgramItemList_TmpIndex = -1;
};


// --------------------------------------------------------------------------
// Zeigt an, ob ein Fahrzeug-Fehler aufgetreten ist
void __fastcall TMainForm::ShowAutomotiveError()
{
    int HV = 0;

    if (VarControl.GetInfo_AutomotiveError() == _ON) HV = 1;

    if (HV == Status_AutomotiveErrorSymbolLamp) return;

    Status_AutomotiveErrorSymbolLamp = HV;

    try {
        TSpeedButton *HButton = AutomotiveError_DeadButton;

        // Hilfsbilder vorbereiten und zuschneiden
        std::auto_ptr<Graphics::TBitmap> DummyImageOriginal(new Graphics::TBitmap());
        std::auto_ptr<Graphics::TBitmap> DummyImageComplete(new Graphics::TBitmap());
        DummyImageComplete->Height = (HButton->Height - (2 * LAYOUT_GUI_Margin));
        DummyImageComplete->Width = 2 * DummyImageComplete->Height;

        // Darstellung laden
        switch(HV)
        {
                case 1:
                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "AUTOMOTIVEERROR_ACTIVE");
                        break;
                default:
                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "AUTOMOTIVEERROR_OFF");
                        break;
        };

        // Bilder einfügen
        DummyImageComplete->Canvas->StretchDraw(Rect(0, 0, DummyImageComplete->Width / 2, DummyImageComplete->Height), DummyImageOriginal.get());
        DummyImageComplete->Canvas->StretchDraw(Rect(DummyImageComplete->Width / 2, 0, DummyImageComplete->Width, DummyImageComplete->Height), DummyImageOriginal.get());

        // An Komponente übergeben
        HButton->Glyph->Assign(DummyImageComplete.get());

    } catch (...) {
        // ignorieren
    }

};

// --------------------------------------------------------------------------
// Zeigt an, ob der Nebenantrieb eingeschaltet ist
void __fastcall TMainForm::ShowHydraulicOKState()
{
    int HV = 0;

    if (VarControl.GetInfo_HydraulicOK() == _ON) HV = 1;

    if (HV == Status_AuxiallyDriveSymbolLamp) return;

    Status_AuxiallyDriveSymbolLamp = HV;

    try {
        TSpeedButton *HButton = AuxiallyDrive_DeadButton;

        // Hilfsbilder vorbereiten und zuschneiden
        std::auto_ptr<Graphics::TBitmap> DummyImageOriginal(new Graphics::TBitmap());
        std::auto_ptr<Graphics::TBitmap> DummyImageComplete(new Graphics::TBitmap());
        DummyImageComplete->Height = (HButton->Height - (2 * LAYOUT_GUI_Margin));
        DummyImageComplete->Width = 2 * DummyImageComplete->Height;

        // Darstellung laden
        switch(HV)
        {
                case 1:
// Nebenabtrieb deaktiviert 22.03.2012
//                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "AUXIALLYDRIVE_ACTIVE");
// Durch Hydraulik Ein ersetzt
                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "HYDRAULIC_ON");
                        break;
                default:
// Nebenabtrieb deaktiviert 22.03.2012
//                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "AUXIALLYDRIVE_OFF");
// Durch Hydraulik Ein ersetzt
                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "HYDRAULIC_OFF");
                        break;
        };

        // Bilder einfügen
        DummyImageComplete->Canvas->StretchDraw(Rect(0, 0, DummyImageComplete->Width / 2, DummyImageComplete->Height), DummyImageOriginal.get());
        DummyImageComplete->Canvas->StretchDraw(Rect(DummyImageComplete->Width / 2, 0, DummyImageComplete->Width, DummyImageComplete->Height), DummyImageOriginal.get());

        // An Komponente übergeben
        HButton->Glyph->Assign(DummyImageComplete.get());

    } catch (...) {
        // ignorieren
    }

};

// --------------------------------------------------------------------------
// Aktualisieren der Hydraulik-Niveau-Anzeige
void __fastcall TMainForm::ShowHydrLvl() {

	if (VarControl.GetInfo_HydrLvl() < HYDRLVL_LOW) HydrLvlView->State = 2;
	else if (VarControl.GetInfo_HydrLvl() > HYDRLVL_HIGH) HydrLvlView->State = 1;
	else HydrLvlView->State = 0;
	HydrLvlView->Position = VarControl.GetInfo_HydrLvl();
}

// --------------------------------------------------------------------------
// Aktualisieren der Hydraulikarbeitsdruckanzeige
void __fastcall TMainForm::ShowHydrWrkPressure() {
	HydrWrkPressureView->CurrValue = VarControl.GetInfo_HydrWrkPressure();
}

// --------------------------------------------------------------------------
// Aktualisieren der Hydraulikspeisedruckanzeige
void __fastcall TMainForm::ShowHydrChargePressure() {
	HydrWrkPressureView->LimitValue = VarControl.GetInfo_HydrChargePressure();
}

// --------------------------------------------------------------------------
// Aktualisieren der Skalierung der Hydraulikarbeitsdruckanzeige
void __fastcall TMainForm::ShowHydrWrkPressureView_Scale() {

	// Hydraulikarbeitsdruckanzeige Skalieren
	// ACHTUNG: Festwerte. Änderung nur im Quellcode Möglich!
	HydrWrkPressureView->MinValue = 0;
	HydrWrkPressureView->MaxValue = 450;
    HydrWrkPressureView->ScaleStep = 50;
	HydrWrkPressureView->LimitValue = 20; // Mindestspeisedruckniveau
}

// --------------------------------------------------------------------------
// Zeigt an, ob das manuelle Abschalten der Bremse Verfügbar ist, oder nicht
void __fastcall TMainForm::ShowBrakeOffEnable() {
	PBrakeOffEnableCheckBox->Checked = VarControl.GetParam_BrakeOffEnable();
}

// --------------------------------------------------------------------------
// Zeigt an, ob die Hydraulische Bremse ein- oder ausgeschaltet ist
void __fastcall TMainForm::ShowHydraulicBrake()
{
    PHydraulikBrakeEnableCheckBox->Checked = VarControl.GetParam_HydraulicBrake();
}

// --------------------------------------------------------------------------
// Zeigt die Kraftdifferenz für die dynamische Maximal-Zugkraft-Ermittlung an
void __fastcall TMainForm::ShowCalcCurrTractionDifference()
{
		PRelTensionLimitUpperEdit->Highlighted = false;

		PRelTensionLimitUpperEdit->Text = MakeTensionString(VarControl.GetParam_RelTensionLimitUpper(), false);
};

// --------------------------------------------------------------------------
// Signalisiert, ob die Maximal-Zugkraft zur Zeit dynamisch ermittelt wird
void __fastcall TMainForm::ShowCalcCurrTractionsSignalisation()
{
    int HV = 0;


    // Zustand ermitteln
	if (VarControl.GetControl_CalcCurrTractions() == _ON)
    {
			if (VarControl.GetControl_CalcCurrTractionsActive() == _ON) HV = 1;
            else HV = 2;
	};

    if (HV == Status_DynamicTensionOnOffSymbolLamp) return;

    Status_DynamicTensionOnOffSymbolLamp = HV;

    try {
		TSpeedButton *HButton = DynamicTensionOnOffButton;

        // Hilfsbilder vorbereiten und zuschneiden
        std::auto_ptr<Graphics::TBitmap> DummyImageOriginal(new Graphics::TBitmap());
        std::auto_ptr<Graphics::TBitmap> DummyImageComplete(new Graphics::TBitmap());
        DummyImageComplete->Height = (HButton->Height - (2 * LAYOUT_GUI_Margin));
        DummyImageComplete->Width = 2 * DummyImageComplete->Height;

        // Darstellung laden
        switch(HV)
        {
                case 1:
                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "DYNMAXTENSION_ACTIVE");
                        break;
                case 2:
                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "DYNMAXTENSION_INACTIVE");
                        break;
                default:
                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "DYNMAXTENSION_OFF");
                        break;
        };
        // Bilder einfügen
        DummyImageComplete->Canvas->StretchDraw(Rect(0, 0, DummyImageComplete->Width / 2, DummyImageComplete->Height), DummyImageOriginal.get());
        DummyImageComplete->Canvas->StretchDraw(Rect(DummyImageComplete->Width / 2, 0, DummyImageComplete->Width, DummyImageComplete->Height), DummyImageOriginal.get());

        // An Komponente übergeben
        HButton->Glyph->Assign(DummyImageComplete.get());

    } catch (...) {
        // ignorieren
    }

};

// --------------------------------------------------------------------------
// Zeigt den aktuellen Eichfaktor für den Teufenrechner an
void __fastcall TMainForm::ShowCalFactorDepth()
{
	PDepthCalFactorEdit->Highlighted = false;

	PDepthCalFactorEdit->Text = FloatToStrF(VarControl.GetParam_CalFactDepth() / pow10(DEPTHCALC_CALFACT_DIGITS), ffFixed, 9, DEPTHCALC_CALFACT_DIGITS);
};

// --------------------------------------------------------------------------
// Zeigt den jeweils passenden Hinweis im Teufenkalibrierfenster an
void __fastcall TMainForm::ShowCalibDepthInfo()
{
	// Hinweistexte
	if (VarControl.GetControl_MagneticMarkOn()) {
		CalibDepthHint1_SwitchOffMM_Label->Enabled = true;
		CalibDepthHint2_SetDepth_Label->Enabled = false;
		CalibDepthHint3_MoveLine_Label->Enabled = false;
		CalibDepthHint4_InsertCorrectDepth_Label->Enabled = false;
	} else
		if (!VarControl.GetInfo_CalibDepthAllowed()) {
			CalibDepthHint1_SwitchOffMM_Label->Enabled = false;
			CalibDepthHint2_SetDepth_Label->Enabled = true;
			CalibDepthHint3_MoveLine_Label->Enabled = false;
			CalibDepthHint4_InsertCorrectDepth_Label->Enabled = false;
		} else
			if (!VarControl.GetInfo_CalibDepthCableOk()) {
				CalibDepthHint1_SwitchOffMM_Label->Enabled = false;
				CalibDepthHint2_SetDepth_Label->Enabled = false;
				CalibDepthHint3_MoveLine_Label->Enabled = true;
				CalibDepthHint4_InsertCorrectDepth_Label->Enabled = false;
			} else {
				CalibDepthHint1_SwitchOffMM_Label->Enabled = false;
				CalibDepthHint2_SetDepth_Label->Enabled = false;
				CalibDepthHint3_MoveLine_Label->Enabled = false;
				CalibDepthHint4_InsertCorrectDepth_Label->Enabled = true;
			}

	// Infozeilen
	if (VarControl.GetInfo_CalibDepthAllowed()) {
		CalibDepthInfo_ImpulseCount_Label->Caption = LABEL_DEPTHIMPULSESCOUNT[GenOrgan.Lng];
		if (VarControl.GetParam_CalFactDepth() != 0) {	// Division durch 0 vermeiden
			CalibDepthInfo_ImpulseCount_Label->Caption = CalibDepthInfo_ImpulseCount_Label->Caption + L": " + FloatToStrF(abs(VarControl.GetInfo_CurrDepth() - VarControl.GetParam_StartDepth()) * pow10(DEPTHCALC_CALFACT_DIGITS) / VarControl.GetParam_CalFactDepth(), ffFixed, 15, 0);
		}
	} else {
		CalibDepthInfo_ImpulseCount_Label->Caption = L"";
    }

	CalibDepthInfo_ActiveCalFact_Label->Caption = LABEL_CALFACTOR_ACTIVE[GenOrgan.Lng];
	CalibDepthInfo_ActiveCalFact_Label->Caption = CalibDepthInfo_ActiveCalFact_Label->Caption + L": " + FloatToStrF(VarControl.GetParam_CalFactDepth() / pow10(DEPTHCALC_CALFACT_DIGITS), ffFixed, 9, DEPTHCALC_CALFACT_DIGITS) + UNIT_MILLIMETER_PER_IMPULSE[GenOrgan.Lng];

	if (VarControl.GetParam_CalFactDepth_Earmarked() > 0) {
		CalibDepthInfo_EarmarkedCalFact_Label->Caption = LABEL_CALFACTOR_EARMARKED[GenOrgan.Lng];
		CalibDepthInfo_EarmarkedCalFact_Label->Caption = CalibDepthInfo_EarmarkedCalFact_Label->Caption + L": " + FloatToStrF(VarControl.GetParam_CalFactDepth_Earmarked() / pow10(DEPTHCALC_CALFACT_DIGITS), ffFixed, 9, DEPTHCALC_CALFACT_DIGITS) + UNIT_MILLIMETER_PER_IMPULSE[GenOrgan.Lng];
	} else {
    	CalibDepthInfo_EarmarkedCalFact_Label->Caption = L"";
    }



}


// --------------------------------------------------------------------------
// Zeigt die aktuellen Zugkraftkalibrierungswerte an
void __fastcall TMainForm::ShowCalValTraction()
{
		PCalibTensionXEdit->Highlighted = false;
		PCalibTensionYEdit->Highlighted = false;
		PCalibTensionZeroEdit->Highlighted = false;

		PCalibTensionXEdit->Text = IntToStr(VarControl.GetParam_CalibTractionX());
		PCalibTensionYEdit->Text = IntToStr(VarControl.GetParam_CalibTractionY());
        PCalibTensionZeroEdit->Text = IntToStr(VarControl.GetParam_CalibTractionZero());

		ShowCalValTractionInfo();

};

// --------------------------------------------------------------------------
// Zeigt Informationen zur aktuellen Zugkraftkalibrierung an
void __fastcall TMainForm::ShowCalValTractionInfo()
{
        float HVfloatx, HVfloaty;
		try	{
			HVfloatx = StrToFloat(PCalibTensionXEdit->Text);
			HVfloaty = StrToFloat(PCalibTensionYEdit->Text);
			if (HVfloatx == 0) PCalibTensionCalibInfoLabel->Caption = (UnicodeString)("---") + " " + UNIT_NEWTON_PER_MILLIVOLT[GenOrgan.Lng];
			else PCalibTensionCalibInfoLabel->Caption = FloatToStrF((HVfloaty / HVfloatx), ffFixed, 7, 3) + " " + UNIT_NEWTON_PER_MILLIVOLT[GenOrgan.Lng];
		}catch (...) {
			PCalibTensionCalibInfoLabel->Caption = (UnicodeString)(L"---") + " " + UNIT_NEWTON_PER_MILLIVOLT[GenOrgan.Lng];
        };
};

// --------------------------------------------------------------------------
// Ermittelt den Computernamen und zeigt diesen an
void __fastcall TMainForm::ShowComputerName()
{
        char HChar[MAX_COMPUTERNAME_LENGTH + 1];
        unsigned long HCharSize = sizeof(HChar);
        GetComputerName(HChar, &HCharSize);
		ComputerNameLabel->Caption = HChar;
};

// --------------------------------------------------------------------------
// Zeigt die Software-Version des angeschlossenen Steuergeräts an
void __fastcall TMainForm::ShowCtlUnitSWVersion() {
	CtlUnitSWVersionLabel->Caption = VarControl.GetInfo_CtlUnitSWVersion();
}

// --------------------------------------------------------------------------
// Zeigt momentane Teufe an
void __fastcall TMainForm::ShowCurrDepth()
{
        CurrDepthLabel->Caption = MakeDepthString(VarControl.GetInfo_CurrDepth(), VIEW_DEPTHACCURACY, true);
        DepthVisualisation->CurrValue = (float)(VarControl.GetInfo_CurrDepth()) / 1000;
};

// --------------------------------------------------------------------------
// zeigt die momentane Fahrtrichtung an
void __fastcall TMainForm::ShowCurrDirection()
{
        int HV = 0;

		if (VarControl.GetInfo_CurrVelocity() != 0)
		{
				if (VarControl.GetInfo_CurrDirection() == _UPward) HV = 1;
				else if (VarControl.GetInfo_CurrDirection() == _DOWN) HV = 2;
        };

        if (HV == Status_CurrDirectionView) return;
        Status_CurrDirectionView = HV;

        switch(HV)
        {
                case 1:
						SpeedView->Direction = -1;
						break;
				case 2:
						SpeedView->Direction = 1;
						break;
				default:
						SpeedView->Direction = 0;
						break;
        };
};

// --------------------------------------------------------------------------
// Zeigt momentane Zugkraft an
void __fastcall TMainForm::ShowCurrTraction()
{
        float HVfloat = TractionViewApplyDimension(VarControl.GetInfo_CurrTraction()); // Eingeführt für evtl. __TRACTION_VIEW_NOISE_DAMP

#ifdef __TRACTION_VIEW_NOISE_DAMP
        // "IsTimeElapsed" kann hier nicht verwendet werden
        // (siehe nachfolgende Implementation)
        long CurrTime = GetProcessTime_in_ms();
        if (CurrTime < oldTractionViewDampTime) oldTractionViewDampTime = CurrTime; // Überlauftest
        if ((CurrTime - oldTractionViewDampTime) >= 100)
        {
                oldTractionViewDampTime += 100;
                TractionView_AverageManager << HVfloat;
        };

        // Ist Puffer gleich 0 wird die Spannung direkt weitergegeben, ansonsten hier ausgelesen
        if (TractionView_AverageManager.GetSize() > 0) HVfloat = TractionView_AverageManager.GetAverage();

//        DampNoise(CurrTractionView->CurrValue, &HVfloat, GET_TRACTION_VIEW_NOISE_DAMP_TIME, &oldTractionViewDampTime);
#endif

#ifdef __CURRTRACTIONVIEW_DECIMAL_ACCURACY // auf 10er genau
        if (VarControl.GetParam_TractionViewDimensionUnit() != 1)               // bei Anzeige in N, kp
        {
                CurrTractionView->CurrValue = (((int)(HVfloat) + 5) / 10) * 10;
        } else                                                                  // Anzeige in kN
        {
                CurrTractionView->CurrValue = HVfloat;
        };
#endif
#ifndef __CURRTRACTIONVIEW_DECIMAL_ACCURACY // auf 1er genau
        CurrTractionView->CurrValue = HVfloat;
#endif
};

// --------------------------------------------------------------------------
// Berechnet aus MaxValue die Darstellung der Grenzwerte
void __fastcall TMainForm::ShowCurrTensionLimits()
{
	if (VarControl.GetControl_UnloadCable() == _ON) {
		CurrTractionView->LimitValue = TractionViewApplyDimension(VarControl.GetInfo_CurrTractionReference());
	}else if (VarControl.GetControl_CalcCurrTractionsActive() == _ON){
		if (VarControl.GetInfo_CurrDirection() == _DOWN) {
			CurrTractionView->LimitValue = TractionViewApplyDimension(VarControl.GetInfo_CurrTensionLimit_Lower());
		} else {
			CurrTractionView->LimitValue = TractionViewApplyDimension(VarControl.GetInfo_CurrTensionLimit_Upper());
		}
	}else
		CurrTractionView->LimitValue = TractionViewApplyDimension(VarControl.GetParam_MaxTraction());


};

// --------------------------------------------------------------------------
// Aktualisiert die Einheit in der Zugkraftanzeige an
void __fastcall TMainForm::ShowCurrTractionViewUnit()
{
        AnsiString HString;

        switch (VarControl.GetParam_TractionViewDimensionUnit())
        {
                case 1:         // Kilonewton
                                HString = LABEL_TENSION[GenOrgan.Lng] + (UnicodeString)__SEPARATOR01[GenOrgan.Lng] + CONST__BRACKET01LEFT + UNIT_KILONEWTON[GenOrgan.Lng] + CONST__BRACKET01RIGHT;
                                CurrTractionView->UnitLabel = HString;
                                CurrTractionView->Unit = UNIT_KILONEWTON[GenOrgan.Lng];
                                break;

                case 2:         // Kilopond
                                HString = LABEL_TENSION[GenOrgan.Lng] + (UnicodeString)__SEPARATOR01[GenOrgan.Lng] + CONST__BRACKET01LEFT + UNIT_KILOPOND[GenOrgan.Lng] + CONST__BRACKET01RIGHT;
                                CurrTractionView->UnitLabel = HString;
                                CurrTractionView->Unit = UNIT_KILOPOND[GenOrgan.Lng];
                                break;

                default:        // Newton
                                HString = LABEL_TENSION[GenOrgan.Lng] + (UnicodeString)__SEPARATOR01[GenOrgan.Lng] + CONST__BRACKET01LEFT + UNIT_NEWTON[GenOrgan.Lng] + CONST__BRACKET01RIGHT;
                                CurrTractionView->UnitLabel = HString;
                                CurrTractionView->Unit = UNIT_NEWTON[GenOrgan.Lng];
                                break;
        };
};

// --------------------------------------------------------------------------
// Zeigt momentane Geschwindigkeit an
void __fastcall TMainForm::ShowCurrVelocity()
{
	SpeedView->Speed = VarControl.GetInfo_AnnouncedVelocity();
};

// Zeigt den Status der internen Automatischen Windenbremse an
void __fastcall TMainForm::ShowCurrWinchBrakeInternAutoStatus()
{
	//(0-Automatik inaktiv, 1-angebremst, 2-geloest, 3-Bremsen erwartet)
	switch (VarControl.GetInfo_CurrWinchBrakeInternAutoStatus()) {

	case 3:	SpeedView->Color = COLOR_ATTENTION;
			break;

	default:
			SpeedView->Color = COLOR_NEUTRAL_SIGNALISATION;
	}
};

// --------------------------------------------------------------------------
// Zeigt die Software-Version des angeschlossenen Teufenrechners an
void __fastcall TMainForm::ShowDepthCalcSWVersion()
{
        DepthCalcSWVersionLabel->Caption = (AnsiString)(DEPTHCALC_SW_VERSION_PREFIX) + VarControl.GetInfo_DepthCalcSWVersion();
};

// --------------------------------------------------------------------------
// Zeigt Informationen an, die nur im Entwicklermodus sichtbar sind
void __fastcall TMainForm::ShowDevelInfo()
{
        if (DevelForm != 0) DevelForm->ActualizeDevelViewMemo(&VarControl, &GenOrgan);
};

// --------------------------------------------------------------------------
// Zeigt Soll-Fahrtrichtung
void __fastcall TMainForm::ShowDirection(TObject *Sender)
{
	wchar_t Symbol[2];
	Symbol[1] = 0; // EK

	if (VarControl.GetParam_Direction() == _UPward) {

		Symbol[0] = 0x25B2;
		if (Sender != SpeedView) SpeedView->AutoDirectionDown = false;

	}else if (VarControl.GetParam_Direction() == _DOWN)	{

		Symbol[0] = 0x25BC;
		if (Sender != SpeedView) SpeedView->AutoDirectionDown = true;
	};

	PChangeDirectionButton->Caption = LABEL_DIRECTION[GenOrgan.Lng];
	PChangeDirectionButton->Caption = PChangeDirectionButton->Caption + Symbol;

};

// --------------------------------------------------------------------------
// Zeigt die speziellen Parameter zum entladen des Kabels an
void __fastcall TMainForm::ShowUnloadCableParameters()
{
	UnloadCableSetTensionEdit->Highlighted = false;
	UnloadCableStretchStartLenghtEdit->Highlighted = false;
	UnloadCableStretchEndLenghtEdit->Highlighted = false;
	UnloadCableStretchStartTensionEdit->Highlighted = false;
	UnloadCableStretchEndTensionEdit->Highlighted = false;

	UnloadCableSetTensionEdit->Text = MakeTensionString(VarControl.GetParam_UnloadCableTraction(), false);
	if (VarControl.GetParam_UnloadCableStretch() == _ON) UnloadCabelStretchCheckBox->Checked = true;
	else UnloadCabelStretchCheckBox->Checked = false;
	UnloadCableStretchStartLenghtEdit->Text = MakeDepthString(VarControl.GetParam_UnloadCableBeginLenght(), 3, false);
	UnloadCableStretchEndLenghtEdit->Text = MakeDepthString(VarControl.GetParam_UnloadCableEndLenght(), 3, false);
	UnloadCableStretchStartTensionEdit->Text = MakeTensionString(VarControl.GetParam_UnloadCableStartTraction(), false);
	UnloadCableStretchEndTensionEdit->Text = MakeTensionString(VarControl.GetParam_UnloadCableEndTraction(), false);
};


// --------------------------------------------------------------------------
// Zeigt an, ob der NotausSchalter betätigt wurde
void __fastcall TMainForm::ShowEmergencySwitchState()
{
    int HV = 0;
    if (VarControl.GetInfo_EmergencySwitch() == _ON) HV = 1;

    if (HV == Status_EmergencySwitchSymbolLamp) return;

    Status_EmergencySwitchSymbolLamp = HV;

    try {
        TSpeedButton *HButton = EmergencySwitch_DeadButton;

        // Hilfsbilder vorbereiten und zuschneiden
        std::auto_ptr<Graphics::TBitmap> DummyImageOriginal(new Graphics::TBitmap());
        std::auto_ptr<Graphics::TBitmap> DummyImageComplete(new Graphics::TBitmap());
        DummyImageComplete->Height = (HButton->Height - (2 * LAYOUT_GUI_Margin));
        DummyImageComplete->Width = 2 * DummyImageComplete->Height;

        // Darstellung laden
        switch(HV)
        {
                case 1:
                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "EMERGENCYSWITCH_ACTIVE");
                        break;
                default:
                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "EMERGENCYSWITCH_OFF");
                        break;
        };

        // Bilder einfügen
        DummyImageComplete->Canvas->StretchDraw(Rect(0, 0, DummyImageComplete->Width / 2, DummyImageComplete->Height), DummyImageOriginal.get());
        DummyImageComplete->Canvas->StretchDraw(Rect(DummyImageComplete->Width / 2, 0, DummyImageComplete->Width, DummyImageComplete->Height), DummyImageOriginal.get());

        // An Komponente übergeben
        HButton->Glyph->Assign(DummyImageComplete.get());

    } catch (...) {
        // ignorieren
    }

};

// --------------------------------------------------------------------------
// Zeigt die Antriebsmotorkühlmitteltemperatur an
void __fastcall TMainForm::ShowEngineCoolantTemp() {

	if (VarControl.GetInfo_EngineCoolantTemp() < TEMPERATURELVL_ENGINECOOLANT_COLD) EngineCoolantTemperatureView->State = 1;
	else if (VarControl.GetInfo_EngineCoolantTemp() > TEMPERATURELVL_ENGINECOOLANT_HOT) EngineCoolantTemperatureView->State = 2;
	else EngineCoolantTemperatureView->State = 0;

	EngineCoolantTemperatureView->Position = VarControl.GetInfo_EngineCoolantTemp();
}

// --------------------------------------------------------------------------
// Zeigt den Status an, ob die Motordrehzahl verändert werden kann oder nicht
void __fastcall TMainForm::ShowEngineRevolutionChangeable()
{
		PEngineRevolutionChangeableCheckBox->Checked = VarControl.GetParam_EngineRevolutionChangeable();
};

// --------------------------------------------------------------------------
// Zeigt die Antriebsmotordrehzahl an
void __fastcall TMainForm::ShowEngineSpeed() {
	EngineSpeedView->CurrValue = VarControl.GetInfo_EngineSpeed();
}

// --------------------------------------------------------------------------
// Zeigt den Status an, ob die Motor-Start-Stopp-Funktion aktiv ist oder nicht
void __fastcall TMainForm::ShowEngineStartStopEnable()
{
		PEngineStartStopEnableCheckBox->Checked = VarControl.GetParam_EngineStartStopEnable();
};

// --------------------------------------------------------------------------
// Zeigt die Skalierung der Antriebsdrehzahl an
void __fastcall TMainForm::ShowEngineSpeedViewScale() {

	// Motordrehzahlanzeige Skalieren
	// ACHTUNG: Festwerte. Änderung nur im Quellcode Möglich!
	EngineSpeedView->MinValue = 0;
	EngineSpeedView->MaxValue = 3000;
	EngineSpeedView->ScaleStep = 500;
	EngineSpeedView->LimitValue = 900; // optimale Drehzahl bei GS-W50
};

// --------------------------------------------------------------------------
// Zeigt den aktuellen Fehlerstatus an
void __fastcall TMainForm::ShowErrorState()
{
		bool HAlarm = false;
		bool HWarning = false;

		// Anwendungsrahmen ist Warnleuchte
		// Warnleuchte
		// Bestimmen, ob Fehler oder Warnung vorliegt
		// Alarm dominiert
		if (GenOrgan.Evnt.EnabledEvents(EVNTOBJTYPE_ERROR) > 0) HAlarm = true;
		else if (GenOrgan.Evnt.EnabledEvents(EVNTOBJTYPE_WARNING) > 0) HWarning = true;

		if (HAlarm) {
			MessageLight->Color = static_cast<TColor>(COLOR_ALERT);
		}else if (HWarning) {
			MessageLight->Color = static_cast<TColor>(COLOR_ATTENTION);
		}else {
			MessageLight->Color = static_cast<TColor>(COLOR_OK);
		}

		for (int i = 0; i < ERRORARRAYSIZE; ++i) {

			switch (DetermErrorMessageNotif(i)) {
			case    _HIDE:  DelErrorMessage(i);
							break;
			case    _SHOW:  AddErrorMessage(i);
							break;
			default:;
			}
		}
};

// --------------------------------------------------------------------------
// Zeigt das eingestellte Format für den externen cm-Impuls an
void __fastcall TMainForm::ShowExtCMImpulseFormat()
{
        switch(VarControl.GetParam_FormatCMImpulse())
        {
                case _KARAT:    PKARATRadioButton->Checked = true;
                                ShowExtCMImpulseFormatImage(_KARAT);
                                break;
                case _KARAT2:   PKARAT2RadioButton->Checked = true;
                                ShowExtCMImpulseFormatImage(_KARAT2);
                                break;
                case _KARAT2N:  PKARAT2NRadioButton->Checked = true;
                                ShowExtCMImpulseFormatImage(_KARAT2N);
                                break;
                case _ONYX:     PONYXRadioButton->Checked = true;
                                ShowExtCMImpulseFormatImage(_ONYX);
                                break;
                case _KEDR:     PKEDRRadioButton->Checked = true;
                                ShowExtCMImpulseFormatImage(_KEDR);
                                break;
                case _KEDRN:    PKEDRNRadioButton->Checked = true;
                                ShowExtCMImpulseFormatImage(_KEDRN);
                                break;
                default:        PKARATRadioButton->Checked = true;
                                ShowExtCMImpulseFormatImage(_KARAT);
                                break;
        };
};

// --------------------------------------------------------------------------
// Zeigt eine grafische Darstellung des eingestellten Formats für den
// externen cm-Impuls an
void __fastcall TMainForm::ShowExtCMImpulseFormatImage(int FormatCode)
{
        ShowExtCMImpulseFormatImage_IsRunning = true;

        // Abgesichert, damit Blockierung im Fehlerfall wieder aufgehoben wird
        try
        {
                Ext_cm_Imp_Image->Stretch = true;

                switch(FormatCode)
                {
                        case _KARAT:    Ext_cm_Imp_Image->Picture->Bitmap->LoadFromResourceName((int)HInstance, "TR_IMPULSFORM_KARAT");
                                        break;
                        case _KARAT2:   Ext_cm_Imp_Image->Picture->Bitmap->LoadFromResourceName((int)HInstance, "TR_IMPULSFORM_KARAT2");
                                        break;
                        case _KARAT2N:  Ext_cm_Imp_Image->Picture->Bitmap->LoadFromResourceName((int)HInstance, "TR_IMPULSFORM_KARAT2_NEGIERT");
                                        break;
                        case _ONYX:     Ext_cm_Imp_Image->Picture->Bitmap->LoadFromResourceName((int)HInstance, "TR_IMPULSFORM_ONYX");
                                        break;
                        case _KEDR:     Ext_cm_Imp_Image->Picture->Bitmap->LoadFromResourceName((int)HInstance, "TR_IMPULSFORM_KEDR");
                                        break;
                        case _KEDRN:    Ext_cm_Imp_Image->Picture->Bitmap->LoadFromResourceName((int)HInstance, "TR_IMPULSFORM_KEDR_NEGIERT");
										break;
						default:        Ext_cm_Imp_Image->Picture->Bitmap->LoadFromResourceName((int)HInstance, "TR_IMPULSFORM_KARAT");
										break;
				};
		}catch(...){};  // Fehler ignorieren

		ShowExtCMImpulseFormatImage_IsRunning = false;
};

// --------------------------------------------------------------------------
// Zeigt den Status der IGR-Fehlerüberwachung an (ein-/aus-geschaltet)
void __fastcall TMainForm::ShowIncEncErrorMonitStatus()
{
        if (VarControl.GetParam_IncEncErrorMonit() == _ON)
        {
                PIncEncErrorMonitCheckBox->Checked = true;
        }else
        {
                PIncEncErrorMonitCheckBox->Checked = false;
        };
};

// --------------------------------------------------------------------------
// Zeigt die Parametereinstellung des Kabel-/Seilölers an
void __fastcall	TMainForm::ShowLineLubParam()
{
	PLineLubAvailableCheckBox->Checked = VarControl.GetParam_LineLubAvailable();
	PLineLubPermanentOperationCheckBox->Checked = VarControl.GetParam_LineLubPermanentOperation();
	PLineLubIntensitySlider->Position = VarControl.GetParam_LineLubIntensity();
}

// --------------------------------------------------------------------------
// Zeigt die Kenndaten für die LOG-Datei an
void __fastcall TMainForm::ShowLOGCharacteristics()
{
        LOGFileLocationMaskEdit->Text = GenOrgan.LOG_LOCATION;
        LOGFileWellMaskEdit->Text = GenOrgan.LOG_WELL;
        LOGFileNameMaskEdit->Text = GenOrgan.LOG_NAME;
		LOGFileCommentsMemo->Text = GenOrgan.LOG_COMMENT;
		LOGFileTeamMaskEdit->Text = GenOrgan.LOG_TEAM;
        LOGFileWinchMachineryMaskEdit->Text = GenOrgan.LOG_WINCHMACHINERY;

        switch (GenOrgan.LOG_Lng)
        {
                case __GERMAN:  LOGFileLanguageGermanRadioButton->Checked = true;
                                break;
                case __RUSSIAN: LOGFileLanguageRussianRadioButton->Checked = true;
                                break;
                case __ENGLISH: LOGFileLanguageEnglishRadioButton->Checked = true;
                                break;
		};

		switch (VarControl.GetParam_LOGFileTensionUnit()) {
		case 1: // kp
			LOGFileTensionUnit_kp_RadioButton->Checked = true;
			break;
		default:
			LOGFileTensionUnit_kN_RadioButton->Checked = true;
			;
		}
};

// --------------------------------------------------------------------------
// Zeigt den momentan gewählten Dateityp an
void __fastcall TMainForm::ShowLOGFileAdminFileType()
{
		switch (VarControl.GetParam_LOGFileType()) {

			case LOGFILE_LAS2:
				LOGFileTypeLAS2RadioButton->Checked = true;
				break;

			case LOGFILE_GEOSYSV1:
				LOGFileTypeGeoSysV1RadioButton->Checked = true;
				break;
			case LOGFILE_GEOSYSV2:

			default:
				LOGFileTypeGeoSysV2RadioButton->Checked = true;
		}
};

// --------------------------------------------------------------------------
// Zeigt den Inhalt der LOG-Datei an, die gerade den Fokus besitzt
void __fastcall TMainForm::ShowLOGFileAdminFocusedFile()
{
        AnsiString PathName, FileName;
        int CurrLine = 0;
        const MAXSHOWEDLINES = 100;

        if ((oldLOGFileItem == LOGFileAdminFileListBox->ItemIndex) ||
            (LOGFileAdminFileListBox->Items->Count == 0)) return;

        LOGFileAdminCurrFileMemo->Clear();
        oldLOGFileItem = LOGFileAdminFileListBox->ItemIndex;

        if (LOGFileAdminFileListBox->ItemIndex < LOGFileAdminFileListBox->Items->Count)
        {
                FileName = LOGFileAdminFileListBox->Items->Strings[LOGFileAdminFileListBox->ItemIndex];
                PathName = GenOrgan.LOGPath + FileName;
        };

        ifstream TheFile;
        unsigned int oldfpos = 0, currfpos = 0;
        char LineBuffer[1201];                                                  // Zeilenpuffer auf die maximale Zeilenlänge (1200 Zeichen) für LAS2.0 optimiert 

        // Datei öffnen
        TheFile.open(PathName.c_str());

        // In erster Zeile Steht der Dateiname, danach kommt eine Unterstrichhlinie
        // und dann eine Leerzeile
        LOGFileAdminCurrFileMemo->Lines->Append(FileName);
        LOGFileAdminCurrFileMemo->Lines->Append("------------------------------------------------------");
        LOGFileAdminCurrFileMemo->Lines->Append("");

        while (TheFile.good())
        {
                ++CurrLine;

                TheFile.getline(LineBuffer, 1200);
                LOGFileAdminCurrFileMemo->Lines->Append(LineBuffer);

                currfpos = TheFile.tellg();
                if (TheFile.eof() ||
                    (oldfpos == currfpos) ||
                    (CurrLine >= MAXSHOWEDLINES)) break;
        };

        // Cursor an Anfang positionieren
        LOGFileAdminCurrFileMemo->SelStart = 0;
        LOGFileAdminCurrFileMemo->SelLength = 0;

        TheFile.close();
};

// --------------------------------------------------------------------------
// Aktualisiert die Werte für die LOG-Datei-Verwaltung
void __fastcall TMainForm::ShowLOGFileAdminValues()
{
        TSearchRec FileList;
        int FindErrorCode = 0;
        int HV, HV1, HV2, HV3;
		char Hchar[3];

		oldLOGFileItem = -1;                                                    // Beim nächsten Aufruf von ShowLOGFileAdminFocusedFile wird Dateiinhaltsansicht aktualisiert

		// LOG-Ziele
		LOGFileAdminDestinationComboBox->Clear();
/* Deaktiviert am 21.05.2011 (kann nach erfolgreicher Testphase der
   neuen Routine gelöscht werden
		for (int i = 0; i < LOG_Transfusion_Destinations_Number; ++i)
		{
				LOGFileAdminDestinationComboBox->Items->Append(LOG_Transfusion_Destinations[i]);
		};
*/
		// Alle Verfügbaren Laufwerke durchtesten
		strcpy(Hchar, "C:");
		for (char i = 'A'; i < 'Z'; ++i) {
			if (i == 'C') continue;							// Laufwerk C: ausschließen
			if (i == GenOrgan.CurrDrive[0] ||               // aktuelles Verzeichnis ausschließen unter Beachtung möglicher Groß-/Kleinschreibung
				i == GenOrgan.CurrDrive[0] + 32) continue;
			Hchar[0] = i;
			if (DirectoryExists(Hchar)) {
                LOGFileAdminDestinationComboBox->Items->Append(Hchar);
            }

		}
		LOGFileAdminDestinationComboBox->ItemIndex = 0;

        // Anzeige der vorhandenen LOG-Dateien
        LOGFileAdminFileListBox->Clear();
        FindErrorCode = FindFirst((AnsiString)(GenOrgan.LOGPath) + "*" + ".*", faAnyFile, FileList);

		HV1 = strlen(LOG_FileExt_GEOSYSV1);                                     // Länge LOG GEOSYSV1-Dateierweiterung
		HV2 = strlen(LOG_FileExt_LAS);                                          // Länge LAS-Dateierweiterung
        HV3 = strlen(LOG_FileExt_GEOSYSV2);                                     // Länge LOG GEOSYSV2-Dateierweiterung

        while (FindErrorCode == 0)
        {
// Am 25.05.2009 geändert (Anpassung an C++Builder 2009)
//                HV = strlen(FileList.FindData.cFileName);
				HV = wcslen(FileList.FindData.cFileName);

				// Nur bekannte LOG-Dateien werden angezeigt
                if (HV > HV1)                                                   // Standard-LOG-Datei möglich ?
                {
						if (_wcscmpi(&FileList.FindData.cFileName[HV - HV1], w_LOG_FileExt_GEOSYSV1) == 0)  // GEOSYS V1 LOG-Datei ?
						{
								LOGFileAdminFileListBox->Items->Append(FileList.FindData.cFileName);
								HV = 0;                                         // erledigt, Datei braucht im nächsten Schritt nicht weiter untersucht werden
						};
				};

				if (HV > HV2)                                                   // LAS-Datei möglich ?
				{
						if (_wcscmpi(&FileList.FindData.cFileName[HV - HV2], w_LOG_FileExt_LAS) == 0)      // LAS-Datei ?
						{
								LOGFileAdminFileListBox->Items->Append(FileList.FindData.cFileName);
								HV = 0;                                         // erledigt, Datei braucht im nächsten Schritt nicht weiter untersucht werden
						};
				};

				if (HV > HV3) {
						if (_wcscmpi(&FileList.FindData.cFileName[HV - HV3], w_LOG_FileExt_GEOSYSV2) == 0)      // GEOSYS V2 LOG-Datei ?
						{
								LOGFileAdminFileListBox->Items->Append(FileList.FindData.cFileName);
								HV = 0;                                         // erledigt, Datei braucht im nächsten Schritt nicht weiter untersucht werden
						};
				}

                FindErrorCode = FindNext(FileList);
        };

        FindClose(FileList);

        // Fortschrittsbalken
        ProgressGauge->Progress = 0;

        // Zeichensatz-Anzeige
        switch (GenOrgan.Lng)
        {
                case __GERMAN   :       LOGFileAdminGermanRadioButton->Checked = true;
                                        LOGFileAdminCurrFileMemo->Font->Charset = LNG_CODE[__GERMAN];
                                        break;
                case __RUSSIAN  :       LOGFileAdminRussianRadioButton->Checked = true;
                                        LOGFileAdminCurrFileMemo->Font->Charset = LNG_CODE[__RUSSIAN];
                                        break;
                case __ENGLISH  :       LOGFileAdminEnglishRadioButton->Checked = true;
                                        LOGFileAdminCurrFileMemo->Font->Charset = LNG_CODE[__ENGLISH];
                                        break;
                default         :       LOGFileAdminGermanRadioButton->Checked = true;
                                        LOGFileAdminCurrFileMemo->Font->Charset = LNG_CODE[__GERMAN];
                                        break;
        };

//        // evtl. ausgewählte Datei anzeigen
//        ShowLOGFileAdminFocusedFile();
        // Dateianzeige löschen
        LOGFileAdminCurrFileMemo->Clear();
};

// --------------------------------------------------------------------------
// Zeigt den Status an, ob der Freilauf verfügbar ist
void __fastcall TMainForm::ShowLostMotionEnable()
{
	if (VarControl.GetParam_LostMotionEnable() == _ON) PLostMotionEnableCheckBox->Checked = true;
	else PLostMotionEnableCheckBox->Checked = false;
}

// --------------------------------------------------------------------------
// Signalisiert die Magnetmarke
void __fastcall TMainForm::ShowMagneticMark(bool ONOFF)
{
        if (ONOFF == _ON)
        {
//				MagneticMarkLight->On = true;
				MagneticMarkPanel->Color = static_cast<TColor>(COLOR_MM_OCCOURS);
				MagneticMarkViewTrigger = GetProcessTime_in_ms();
		};

		// wenn Zähler aktiviert
		if (MagneticMarkPanel->Color == static_cast<TColor>(COLOR_MM_OCCOURS))
		{
				// 100 ms ermitteln
				if ((GetProcessTime_in_ms() - MagneticMarkViewTrigger) >= 500)
				{
						MagneticMarkPanel->Color = static_cast<TColor>(COLOR_NO_MM);
//						MagneticMarkLight->On = false;
                };
        }else
        {
                MagneticMarkPanel->Color = static_cast<TColor>(COLOR_NO_MM);
        };
};

// --------------------------------------------------------------------------
// Zeigt den Status der Magnetmarkenüberwachung an (ein-/aus-geschaltet)
void __fastcall TMainForm::ShowMagneticMarkStatus()
{
    int HV = 0;

	if (VarControl.GetControl_MagneticMarkOn() == _ON) HV = 1;

    if (HV == Status_MagneticMarkOnOffSymbolLamp) return;

    Status_MagneticMarkOnOffSymbolLamp = HV;

    try {
        TSpeedButton *HButton = MagneticMarkOnOffButton;

        // Hilfsbilder vorbereiten und zuschneiden
        std::auto_ptr<Graphics::TBitmap> DummyImageOriginal(new Graphics::TBitmap());
        std::auto_ptr<Graphics::TBitmap> DummyImageComplete(new Graphics::TBitmap());
        DummyImageComplete->Height = (HButton->Height - (2 * LAYOUT_GUI_Margin));
        DummyImageComplete->Width = 2 * DummyImageComplete->Height;

        // Darstellung laden
        switch(HV)
        {
                case 1:
                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "MMDETECTION_ACTIVE");
                        break;
                default:
                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "MMDETECTION_OFF");
                        break;
        }
        // Bilder einfügen
        DummyImageComplete->Canvas->StretchDraw(Rect(0, 0, DummyImageComplete->Width / 2, DummyImageComplete->Height), DummyImageOriginal.get());
        DummyImageComplete->Canvas->StretchDraw(Rect(DummyImageComplete->Width / 2, 0, DummyImageComplete->Width, DummyImageComplete->Height), DummyImageOriginal.get());

        // An Komponente übergeben
        HButton->Glyph->Assign(DummyImageComplete.get());

    } catch (...) {
        // ignorieren
    }

};

// --------------------------------------------------------------------------
// Zeigt Mindestteufe an
void __fastcall TMainForm::ShowMaxDepth()
{
        DepthVisualisation->BottomValue = (float)(VarControl.GetParam_MaxDepth()) / 1000;

        PMaxDepthEdit->Highlighted = false;

        PMaxDepthEdit->Text = MakeDepthString(VarControl.GetParam_MaxDepth(), VIEW_DEPTHACCURACY, false);
};
// --------------------------------------------------------------------------
// Zeigt die momentane Maximal-Zugkraft an
void __fastcall TMainForm::ShowMaxTraction()
{
		PMaxTensionEdit->Highlighted = false;

		PMaxTensionEdit->Text = MakeTensionString(VarControl.GetParam_MaxTraction(), false);
};

// --------------------------------------------------------------------------
// Berechnet die Darstellung der Grenzwerte
void __fastcall TMainForm::ShowMaxTractionLimits()
{
        bool ChangeOccurs = false;

		if (TractionViewApplyDimension(VarControl.GetParam_MinTraction()) != CurrTractionView->MinValue)
        {
                CurrTractionView->MinValue = TractionViewApplyDimension(VarControl.GetParam_MinTraction());
                ChangeOccurs = true;
        };

        if ((TractionViewApplyDimension(VarControl.GetParam_MaxTraction()) + 0.5) != CurrTractionView->MaxValue)
        {
                CurrTractionView->MaxValue = TractionViewApplyDimension(VarControl.GetParam_MaxTraction()) + 0.5; // damit letzter Wert dargestellt
                ChangeOccurs = true;                                                                              // Aufrunden, da sonst die doppelte Anzahl an Skalenstrichen entstehen kann (01.09.2005)
        };

        if (ChangeOccurs)
        {
                int HV0, HV1, HV2;
                HV0 = (CurrTractionView->MaxValue - CurrTractionView->MinValue);

                HV1 = pow((double)(10), (int)(log10((double)(HV0))));                     // "Runde" Sprünge
                HV2 = (HV0 * 10) / HV1;                                         // !!! mal 10 um Auflösung zu erhöhen

                CurrTractionView->ScaleStep = HV2 * HV1 / 10 / 10;              // !!! abschließend 1x durch 10, um Auflösungserhöhungsoffset wieder rückgängig zu machen (normieren)
        };
};

// --------------------------------------------------------------------------
// Zeigt Mindestteufe an
void __fastcall TMainForm::ShowMinDepth()
{
		DepthVisualisation->TopValue = (float)(VarControl.GetParam_MinDepth()) / 1000;

		PMinDepthEdit->Highlighted = false;

		PMinDepthEdit->Text = MakeDepthString(VarControl.GetParam_MinDepth(), VIEW_DEPTHACCURACY, false);
};

// --------------------------------------------------------------------------
// Zeigt die Parameter zur MM-Zwangssynchronisation an
void __fastcall TMainForm::ShowMMSyncForceParam()
{
	PMMSyncForceRangeLimEdit->Highlighted = false;

	PMMSyncForceRangeLimEdit->Text = VarControl.GetParam_MMSyncForceRangeLim();
}

// --------------------------------------------------------------------------
// Zeigt an, ob der Ölfilter ausgetauscht werden muss
void __fastcall TMainForm::ShowOilFilterState()
{
    int HV = 0;

	if (VarControl.GetInfo_OilStrainer() == _ON) HV = 1;
	else if (VarControl.GetInfo_OilStrainer2() == _ON) HV = 2;

    if (HV == Status_ChangeOilFilterSymbolLamp) return;

    Status_ChangeOilFilterSymbolLamp = HV;

    try {
        TSpeedButton *HButton = ChangeOilFilter_DeadButton;

        // Hilfsbilder vorbereiten und zuschneiden
        std::auto_ptr<Graphics::TBitmap> DummyImageOriginal(new Graphics::TBitmap());
        std::auto_ptr<Graphics::TBitmap> DummyImageComplete(new Graphics::TBitmap());
        DummyImageComplete->Height = (HButton->Height - (2 * LAYOUT_GUI_Margin));
        DummyImageComplete->Width = 2 * DummyImageComplete->Height;

        // Darstellung laden
        switch(HV)
        {
                case 1:
						DummyImageOriginal->LoadFromResourceName((int)HInstance, "CHANGEOILFILTER_ACTIVE");
						break;
				case 2:
						DummyImageOriginal->LoadFromResourceName((int)HInstance, "CHANGEOILFILTER2_ACTIVE");
						break;
				default:
                        DummyImageOriginal->LoadFromResourceName((int)HInstance, "CHANGEOILFILTER_OFF");
                        break;
        };

        // Bilder einfügen
        DummyImageComplete->Canvas->StretchDraw(Rect(0, 0, DummyImageComplete->Width / 2, DummyImageComplete->Height), DummyImageOriginal.get());
        DummyImageComplete->Canvas->StretchDraw(Rect(DummyImageComplete->Width / 2, 0, DummyImageComplete->Width, DummyImageComplete->Height), DummyImageOriginal.get());

        // An Komponente übergeben
        HButton->Glyph->Assign(DummyImageComplete.get());

    } catch (...) {
        // ignorieren
    }

};

// --------------------------------------------------------------------------
// Ermitteln und Anzeigen des aktuellen Power-Fail-Zustandes
void __fastcall TMainForm::ShowPowerFailStatus()
{
    clock_t Countdown;
    if (VarControl.GetControl_PowerFail() == _ON) {

		GenOrgan.Evnt << 23;						// Power Fail signalisieren

		if (PowerFailStartTime_Reset) {
			IsTimeElapsed(&PowerFailStartTime, &Countdown, POWERFAIL_SHUTDOWN_DELAY * 1000, 0, true);
			PowerFailStartTime_Reset = false;
		} else {
			if (IsTimeElapsed(&PowerFailStartTime, &Countdown, POWERFAIL_SHUTDOWN_DELAY * 1000, 0, false)) {
				SystemShutdownButtonClick(this);
			}
		}
		PowerFailCdResetSpeedButton->Caption = Countdown / 1000;
		PowerFailCdResetSpeedButton->Enabled = true;

	} else {

		GenOrgan.Evnt >> 23;		 				// Power Fail Signalisierung abgeschalten

		PowerFailCdResetSpeedButton->Caption = "---";
		PowerFailCdResetSpeedButton->Enabled = false;

		PowerFailStartTime_Reset = true;
	}

	PowerFailCdResetSpeedButton->Flat = !PowerFailCdResetSpeedButton->Enabled;
};

// --------------------------------------------------------------------------
// Zeigt momentanen Programm-Modus an
void __fastcall TMainForm::ShowProgramMode()
{
        switch (VarControl.GetInfo_ProgramMode())
        {
                case 1:         // Programm-Start
                                ProgramModeLabel->Caption = PROG_MODE_1[GenOrgan.Lng];
                                break;

                case 2:         // Programm-Ende
                                ProgramModeLabel->Caption = PROG_MODE_2[GenOrgan.Lng];
                                break;

                case 3:         // Manueller Betrieb
                                ProgramModeLabel->Caption = PROG_MODE_3[GenOrgan.Lng];
                                break;

                case 4:         // Automatischer Betrieb
                                ProgramModeLabel->Caption = PROG_MODE_4[GenOrgan.Lng];
                                break;

                case 5:         // Kalibrieren
                                ProgramModeLabel->Caption = PROG_MODE_5[GenOrgan.Lng];
                                break;

                case 6:         // Teufenrechner Kalibrieren
                                ProgramModeLabel->Caption = PROG_MODE_6[GenOrgan.Lng];
                                break;

                case 7:         // Zugkraftmesser Kalibrieren
                                ProgramModeLabel->Caption = PROG_MODE_7[GenOrgan.Lng];
                                break;

                case 8:         // Kabel entladen
                                ProgramModeLabel->Caption = PROG_MODE_8[GenOrgan.Lng];
                                break;

                case 9:         // Freilauf ein
                                ProgramModeLabel->Caption = PROG_MODE_9[GenOrgan.Lng];
                                break;

                default:        // Undefiniert
                                ProgramModeLabel->Caption = PROG_MODE_0[GenOrgan.Lng];
                                break;
        };
};

// --------------------------------------------------------------------------
// Zeigt den Status der Steuergerät - Fehlerüberwachung bei langsamer Fahrt an
// (ein-/aus-geschaltet)
void __fastcall TMainForm::ShowControllerErrorMonitAtSM() {
	if (VarControl.GetParam_ControllerErrorMonitAtSM() == _ON) {
		PControllerErrorAtSMCheckBox->Checked = true;
	} else {
		PControllerErrorAtSMCheckBox->Checked = false;
	}
}


// --------------------------------------------------------------------------
// Zeigt den Status der Controller - Fehlerüberwachung an (ein-/aus-geschaltet)
void __fastcall TMainForm::ShowControllerErrorMonitStatus()
{
		if (VarControl.GetParam_ControllerErrorMonit() == _ON)
		{
				PControllerErrorMonitCheckBox->Checked = true;
		}else
		{
				PControllerErrorMonitCheckBox->Checked = false;
		};

};

// --------------------------------------------------------------------------
// Zeigt den Status des Steuergerät Aktiv-Pegels an (L-aktive oder H-aktive Detektion)
void __fastcall TMainForm::ShowControllerLactiveStatus() {
	if (VarControl.GetParam_ControllerErrorLactive() == _ON) {
		PControllerErrorLactiveCheckBox->Checked = true;
	} else {
		PControllerErrorLactiveCheckBox->Checked = false;
	}
}

// --------------------------------------------------------------------------
// Zeigt den Satus der Teufenumkehrung an
void __fastcall TMainForm::ShowReverseDepthStatus()
{
	if (VarControl.GetParam_ReverseDepth() == _ON) {
		PReverseDepthCheckBox->Checked = true;
	} else {
		PReverseDepthCheckBox->Checked = false;
	}
}

// --------------------------------------------------------------------------
// Zeigt die Teufe an, die dem Teufenrechner als Startteufe übergeben werden kann
void __fastcall TMainForm::ShowSetDepth()
{
	PSetDepthEdit->Highlighted = false;

	PSetDepthEdit->Text = MakeDepthString(VarControl.GetInfo_CurrDepth(), VIEW_DEPTHACCURACY, false);
};

// --------------------------------------------------------------------------
// Zeigt den Modus (Version) der Sondersignalverarbeitung an
void __fastcall TMainForm::ShowSpecialSignalMode()
{
        switch (VarControl.GetParam_SpecialSignalMode())
        {
                case 0:
                        PSpecialSignalMode0RadioButton->Checked = true;
                        break;

                case 1:
                        PSpecialSignalMode1RadioButton->Checked = true;
                        break;

                case 2:
                        PSpecialSignalMode2RadioButton->Checked = true;
						break;

				case 3:
						PSpecialSignalMode3RadioButton->Checked = true;
						break;

				case 4:
						PSpecialSignalMode4RadioButton->Checked = true;
						break;

				case 5:
						PSpecialSignalMode5RadioButton->Checked = true;
						break;

				default:
                        break;
        };
};

// --------------------------------------------------------------------------
// Zeigt die aktuelle Uhrzeit an
void __fastcall TMainForm::ShowSystemDateTime()
{
        TDateTime CurrSysTime = Now();
        if (ActualizeSystemDateMaskEdit) SystemDateMaskEdit->Text = FormatDateTime(FORMAT_Date_String, CurrSysTime);
        if (ActualizeSystemTimeMaskEdit) SystemTimeMaskEdit->Text = FormatDateTime(FORMAT_Time_String, CurrSysTime);
};

// --------------------------------------------------------------------------
// Zeigt die Temperaturen der verschiedenen Sensoren an
// ForceActualisation ... true, wenn die Aktualisierung aller Werte gefordert wird
// ForceActualisation ... false, wenn keine Aktualisierung der Werte gefordert wird
//                        (Es wird ansonsten nur bei Änderung der jew. Werte eine
//                         Ansichtsaktualisierung durchgeführt, do so schneller)
void __fastcall TMainForm::ShowTemperatures(bool ForceActualisation)
{
        bool MinOneVisible = false, ActGroupBoxVisibility = false;               // mindestens eine Temperatur sichtbar/ Sichtbarkeitsaktualisierung der GroupBox durchführn
        float HVfloat;

		HVfloat = VarControl.GetInfo_TemperatureSensorDepthCalc();
		// Hat sich was geändert, bzw. Aktualisierung gefordert ?
		if ((oldTemperatureSensorDepthCalc != HVfloat) || (ForceActualisation))
		{
				oldTemperatureSensorDepthCalc = HVfloat;
				ActGroupBoxVisibility = true;                                   // GroupBox-Sichtbarkeits-Aktualisierung jetzt sinnvoll
				// Sensor aktiv oder nicht ?
				if ((HVfloat < TEMPERATURE_SENSOR_DEPTHCALC_OFF_DETECT_H) && (HVfloat > TEMPERATURE_SENSOR_DEPTHCALC_OFF_DETECT_L))
				{
						MinOneVisible = true;
						SystemTempDepthCalcDescrLabel->Visible = true;
						SystemTempDepthCalcLabel->Visible = true;
						SystemTempDepthCalcLabel->Caption = FloatToStrF(HVfloat + 0.5, ffFixed, 7, 0) + UNIT_DEGREE_CELSIUS[GenOrgan.Lng];
				}else
				{
						SystemTempDepthCalcDescrLabel->Visible = false;
						SystemTempDepthCalcLabel->Visible = false;
				};
		};

		HVfloat = VarControl.GetInfo_TemperatureSensorA();
		// Hat sich was geändert, bzw. Aktualisierung gefordert ?
		if ((oldTemperatureSensorA != HVfloat) || (ForceActualisation))
		{
				oldTemperatureSensorA = HVfloat;
				ActGroupBoxVisibility = true;                                   // GroupBox-Sichtbarkeits-Aktualisierung jetzt sinnvoll
				// Sensor aktiv oder nicht ?
				if ((HVfloat < TEMPERATURE_SENSOR_A_OFF_DETECT_H) && (HVfloat > TEMPERATURE_SENSOR_A_OFF_DETECT_L))
				{
						MinOneVisible = true;
						SystemTempADescrLabel->Visible = true;
						SystemTempALabel->Visible = true;
						SystemTempALabel->Caption = FloatToStrF(HVfloat + 0.5, ffFixed, 7, 0) + UNIT_DEGREE_CELSIUS[GenOrgan.Lng];
                }else
                {
                        SystemTempADescrLabel->Visible = false;
                        SystemTempALabel->Visible = false;
                };
        };

        HVfloat = VarControl.GetInfo_TemperatureSensorB();
        // Hat sich was geändert, bzw. Aktualisierung gefordert ?
        if ((oldTemperatureSensorB != HVfloat) || (ForceActualisation))
        {
                oldTemperatureSensorB = HVfloat;
                ActGroupBoxVisibility = true;                                   // GroupBox-Sichtbarkeits-Aktualisierung jetzt sinnvoll
                // Sensor aktiv oder nicht ?
                if ((HVfloat < TEMPERATURE_SENSOR_B_OFF_DETECT_H) && (HVfloat > TEMPERATURE_SENSOR_B_OFF_DETECT_L))
                {
                        MinOneVisible = true;
                        SystemTempBDescrLabel->Visible = true;
                        SystemTempBLabel->Visible = true;
						SystemTempBLabel->Caption = FloatToStrF(HVfloat + 0.5, ffFixed, 7, 0) + UNIT_DEGREE_CELSIUS[GenOrgan.Lng];
                }else
                {
                        SystemTempBDescrLabel->Visible = false;
                        SystemTempBLabel->Visible = false;
                };
        };

        // SystemTemperatureGroupBox nur sichtbar, wenn min. eine Temperatur angezeigt wird
        if (ActGroupBoxVisibility)
        {
                if (MinOneVisible) SystemTemperatureGroupBox->Visible = true;
                else SystemTemperatureGroupBox->Visible = false;
		};

		// Hydrauliköltemperatur
		if (VarControl.GetInfo_TemperatureOilTank() < TEMPERATURELVL_HYDRTANK_COLD) HydrTemperatureView->State = 1;
		else if (VarControl.GetInfo_TemperatureOilTank() > TEMPERATURELVL_HYDRTANK_HOT) HydrTemperatureView->State = 2;
		else HydrTemperatureView->State = 0;
		HydrTemperatureView->Position = VarControl.GetInfo_TemperatureOilTank();
};

// --------------------------------------------------------------------------
// Zeigt die aktuelle Uhrzeit an
void __fastcall TMainForm::ShowTime()
{
        AnsiString tmpTimeStr = TimeToStr(Time());
        if (tmpTimeStr == TimeLabel->Caption) return;
        TimeLabel->Caption = tmpTimeStr;
};

// --------------------------------------------------------------------------
// Zeigt Soll-Zugkraft-Differenz an
void __fastcall TMainForm::ShowTractionDifference()
{
		PRelTensionLimitLowerEdit->Highlighted = false;

		PRelTensionLimitLowerEdit->Text = MakeTensionString(VarControl.GetParam_RelTensionLimitLower(), false);
};

// --------------------------------------------------------------------------
// Zeigt die Maßeinheit der Zugkraft an, mit der die Anzeige und Interaktion mit dem Anwender erfolgt
void __fastcall TMainForm::ShowTractionDimensionUnit()
{
		if ((VarControl.GetParam_TractionViewDimensionUnit() > PTractionViewComboBox->Items->Count) ||
            (VarControl.GetParam_TractionViewDimensionUnit() < 0)) PTractionViewComboBox->ItemIndex = 0;
		else PTractionViewComboBox->ItemIndex = VarControl.GetParam_TractionViewDimensionUnit();
};

// --------------------------------------------------------------------------
// Zeigt Soll-Geschwindigkeit an
void __fastcall TMainForm::ShowVelocity(TObject *Sender)
{
		if (Sender != SpeedView) SpeedView->AutoSpeed = VarControl.GetParam_Velocity();

		PVelocityEdit->Text = MakeVelocityString(VarControl.GetParam_Velocity(), false);
		PVelocityEdit->Highlighted = false;
};

// --------------------------------------------------------------------------
// Zeigt die Maßeinheit der Geschwindigkeit an, mit der die Anzeige und Interaktion mit dem Anwender erfolgt
void __fastcall TMainForm::ShowVelocityDimensionUnit()
{
	// Parametereinstellung
	if ((VarControl.GetParam_VelocityDimensionUnit() > PVelocityViewComboBox->Items->Count) ||
		(VarControl.GetParam_VelocityDimensionUnit() < 0)) PVelocityViewComboBox->ItemIndex = 0;
	else PVelocityViewComboBox->ItemIndex = VarControl.GetParam_VelocityDimensionUnit();

	// Geschwindigkeitsanzeige anpassen
	switch (VarControl.GetParam_VelocityDimensionUnit()) {
	case	1:	// m/min
		SpeedView->SpeedUnit = UNIT_METER_PER_MINUTE[GenOrgan.Lng];
		SpeedView->SpeedFactor = 0.01667; // 1m/min = 1/60*m/h
		SpeedView->SpeedDigits = 1;
		break;
	default:
		SpeedView->SpeedUnit = UNIT_METER_PER_HOUR[GenOrgan.Lng];
		SpeedView->SpeedFactor = 1; // entspricht der genauen Angabe der Zeit
		SpeedView->SpeedDigits = 0;
	}

}

// --------------------------------------------------------------------------
// Zeigt an, ob die Windenbremse durch die Steuerungssoftware gehalten wird, oder nicht
void __fastcall TMainForm::ShowWinchBrakeInternalAuto()
{
	PWinchBrakeInternalAutoCheckBox->Checked = VarControl.GetParam_WinchBrakeInternAuto();
}

// --------------------------------------------------------------------------
// Anzeigen der momentanen Windennummer
void __fastcall TMainForm::ShowWinchNo()
{
//        WinchNoLabel->Caption = LABEL_WINCH[GenOrgan.Lng];
//        WinchNoLabel->Caption = WinchNoLabel->Caption + __SEPARATOR01[GenOrgan.Lng] + MakeWinchNameString(VarControl.GetParam_WinchNo());

		PWinchCfgListBox->ItemIndex = VarControl.GetParam_WinchNo();
};

// --------------------------------------------------------------------------
// Liest alle Kontrollwerte/Parameter/Informationen aus und aktualisiert die Anzeige/Steuerelemente
void __fastcall TMainForm::ActualizeAllInformations()
{
		// Programm-Modus
        ShowProgramMode();

        // PowerFail-Status
        ShowPowerFailStatus();

        // Fehlerstatus
        ShowErrorState();

        // Winde Automatik
        SetStartStop();

		// Freigabe nach Grenzwertüberschreitung

        // Schnelle/Langsame Fahrt
		SetFastRun();

        // Motor ein/aus

        // Motordrehzahl hoch/runter

        // Kabel entladen

        // Kalibrieren

		// Kalibrieren der Teufe
		ShowCalibDepthInfo();

        // Kalibrieren des Zugraftmessermoduls

        // Freilauf aktiv

        // Momentane Teufe
        ShowCurrDepth();

        // momentane Fahrtrichtung
        ShowCurrDirection();

        // Geschwindigkeit
        ShowCurrVelocity();

        // momentane Maximalzugkraft
        ShowMaxTraction();
		ShowMaxTractionLimits();
		ShowCurrTensionLimits();
		// momentane Zugkraft
		ShowCurrTraction();

		// Motordrehzahl
        ShowEngineCoolantTemp();
		ShowEngineSpeedViewScale();
		ShowEngineSpeed();

		// Hydraulikzustand
		ShowHydraulicOKState();
        ShowHydrWrkPressureView_Scale();
		ShowHydrWrkPressure();
		ShowHydrChargePressure();
		ShowHydrLvl();

        // ZugkraftDifferenz
        ShowTractionDifference();

        // Maßeinheit der Zugkraftsanzeige
		ShowTractionDimensionUnit();

        // Maßeinheit der Geschwindigkeit
		ShowVelocityDimensionUnit();

		// Auftreten einer Magnetmarke
        ShowMagneticMark(VarControl.GetInfo_MagneticMark());

        // Inkrementalgeber-Fehler-Überwachung
        ShowIncEncErrorMonitStatus();

        // Steuergerät - Fehlerüberwachung
        ShowControllerErrorMonitStatus();

    	// Steuergerät - aktiv-Pegel
		ShowControllerLactiveStatus();

		// Steuergerät - Fehlerüberwachung im langsamen Gang
		ShowControllerErrorMonitAtSM();

        // Teufenumkehrung
		ShowReverseDepthStatus();

        // Momentane Sollzugkraft (für Kabel entladen)

        // Windennummer
        ShowWinchNo();

        // Mindest-Teufe
        ShowMinDepth();

        // Maximale Teufe
		ShowMaxDepth();

    	// Zeigt die Parameter zur MM-Zwangssynchronisation an
		ShowMMSyncForceParam();

        // Eingestellte Startteufe (StartDepth)
        ShowSetDepth();

        // Eingestellter Modus der Sondersignalüberwachung
        ShowSpecialSignalMode();

        // Eingestellte Geschwindigkeit
        ShowVelocity(this);

        // Eingestellte Fahrtrichtung
        ShowDirection(this);

        // Programmautomatikeistellungen
        ShowAutoRunProgramable();

        // Programmautomatik-Listeninhalt
        ShowAutoRunProgramItemList();

        // Programmautomatik-Ablaufprogrammpunkt
        ShowAutoRunCurrProgramIndex();

        // Motordrehzahl änderbar oder nicht
        ShowEngineRevolutionChangeable();

        // Motor-Start-Stopp-Funktion aktiviert oder nicht
        ShowEngineStartStopEnable();

        // Magnetmarkenüberwachung ein/aus
        ShowMagneticMarkStatus();

        // Kabel entladen: sämtliche Parameter
        ShowUnloadCableParameters();

        // Momentane Mindest-/Maximal-Zugkraft Differenz
        ShowCalcCurrTractionDifference();

        // Kalibrierungsfaktor Teufe
        ShowCalFactorDepth();

        // Ausgabeformat externer cm-Impuls
        ShowExtCMImpulseFormat();

        // Zugkraftkalibrierung Wert X
        // Zugkraftkalibrierung Wert Y
        // Zugkraftkalibrierung  Null
        ShowCalValTraction();
        ShowCalValTractionInfo();
        
        // Inkrementalgeber Fehlerüberwachung ein/aus

        // Zusatzinformationen im Entwicklermodus
        ShowDevelInfo();

        // LOG-Kenndaten
        ShowLOGCharacteristics();
        ShowLOGFileAdminValues();
};

// --------------------------------------------------------------------------
// Aktualisiert die Ansicht der Parameter, welche für das Entladen des Kabels wichtig sind
void __fastcall TMainForm::ActualizeUnloadCableInformation()
{
        ShowUnloadCableParameters();
};

// --------------------------------------------------------------------------
// Aktualisieren aller in FileAdminPanel sichtbaren Angaben
void __fastcall TMainForm::ActualizeFileAdminView()
{
        ShowLOGCharacteristics();
        ShowLOGFileAdminValues();
        ShowLOGFileAdminFileType();
};

// --------------------------------------------------------------------------
// Aktualisieren aller in ParameterPanel sichtbaren Angaben
void __fastcall TMainForm::ActualizeParameterView()
{
	ShowBrakeOffEnable();
	ShowHydraulicBrake();
	ShowCalcCurrTractionDifference();
	ShowCalFactorDepth();
	ShowCalValTraction();
	ShowControllerLactiveStatus();
	ShowControllerErrorMonitAtSM();
	ShowDirection(this);
	ShowEngineRevolutionChangeable();
	ShowEngineStartStopEnable();
	ShowLostMotionEnable();
	ShowLineLubParam();
	ShowExtCMImpulseFormat();
	ShowIncEncErrorMonitStatus();
	ShowMaxDepth();
	ShowMaxTraction();
	ShowMinDepth();
	ShowMMSyncForceParam();
	ShowControllerErrorMonitStatus();
	ShowReverseDepthStatus();
	ShowSetDepth();
	ShowSpecialSignalMode();
	ShowTractionDifference();
	ShowTractionDimensionUnit();
	ShowVelocity(this);
	ShowWinchBrakeInternalAuto();
	ShowWinchNo();
	ShowAutoRunProgramable();
	ShowVelocityDimensionUnit();
};

// --------------------------------------------------------------------------
// Aktualisieren aller in SystemPanel sichtbaren Angaben
void __fastcall TMainForm::ActualizeSystemView()
{
//        ShowSystemDateTime();                                                 // Deaktiviert am 31.01.2007
		ShowComputerName();
    	ShowCtlUnitSWVersion();
		ShowDepthCalcSWVersion();
};

// --------------------------------------------------------------------------

// ==========================================================================
// PROGRAMMODUS-VERWALTUNG
// ==========================================================================
// Führt entsprechende Schritte für die Programmmodi aus
// ProgMode ...         Gibt an auf welchen Programm-Modus sich dieser Methodenaufruf bezieht
// ControlState ...     Gibt den Kontroll-Status an (definiert in "constants.h")
void __fastcall TMainForm::ProgramModeAdministrations(int ProgMode, int ControlState)
{
		// Kalibrieren
		switch(ProgMode)
		{
				case 5: // Kalibrieren
						switch (ControlState)
						{
								case CS_START:          // Kalibrieren starten
														// Auswahlfenster beschriften
														ChoiceAbortButton->Caption = LABEL_ABORT[GenOrgan.Lng];
														ChoiceCaptionLabel->Caption = LABEL_CALIBRATE[GenOrgan.Lng];
														ChoiceLabel1->Caption = LABEL_DEPTH[GenOrgan.Lng];
														ChoiceLabel2->Caption = LABEL_TENSION[GenOrgan.Lng];
														ChoicePanelObjectsVisibility(0x00000007);

														// Steuerflags setzen (Rücksetzen der Infos)
														VarControl.SetControl_CalibrateDepthOff();
														VarControl.SetControl_CalibrateTractionOff();

														// Entsprechendes Auswahlfenster darstellen
														GenOrgan.FavoriteFunctionPanel = INR_ChoicePanel;
														ShowFavoriteFunctionPanel();
														break;

								case CS_STOP:           // Anhalten
								case CS_ABORT:          // Abbrechen, bei Auswahl, was Kalibriert werden soll
														VarControl.SetControl_CalibrateOff();
														VarControl.SetControl_CalibrateDepthOff();
														VarControl.SetControl_CalibrateTractionOff();

														// Normales Panel sichtbar machen
														GenOrgan.FavoriteFunctionPanel = INR_NormalViewPanel;
                                                        ShowFavoriteFunctionPanel();
														break;

								case CS_SPECIAL1:       // Auswahl Kalibrierung Teufe
														GenOrgan.FavoriteFunctionPanel = INR_CalibDepthPanel;
														ShowFavoriteFunctionPanel();

														// Eingabefelder setzen
                                                        CalibDepth_DepthEdit->Text = MakeDepthString(VarControl.GetInfo_CurrDepth(), 3, false);

														// Parameter zurücksetzen
														VarControl.SetInfo_CalibDepthCableOkOff();          // Kabel-Länge nicht ausreichend herausgezogen

														// Programmmodus spezifieren (indirekt)
														VarControl.SetControl_CalibrateDepthOn();

														break;

								case CS_SPECIAL2:       // Auswahl Kalibrierung Zugkraftmesser
														GenOrgan.FavoriteFunctionPanel = INR_CalibTractionPanel;
														ShowFavoriteFunctionPanel();

														// Eingabefelder setzen
														CalibTension_Tension1Edit->Text = MakeTensionString(VarControl.GetInfo_CurrTraction() , 0);
														VarControl.SetParam_CalibTension_Tension(0, VarControl.GetInfo_CurrTraction());
														CalibTension_Tension2Edit->Text = CalibTension_Tension1Edit->Text;
														VarControl.SetParam_CalibTension_Tension(1, VarControl.GetInfo_CurrTraction());

														// Zurücksetzen der Informationen
														VarControl.SetInfo_CalibTractionValue1OkOff();
														VarControl.SetInfo_CalibTractionValue2OkOff();

														// Programmmodus spezifieren (indirekt)
														VarControl.SetControl_CalibrateTractionOn();
														break;

								default:
														break;
						};
						break;

				case 8: switch (ControlState)
						{
								case CS_START:
														VarControl.SetControl_UnloadCableOn();
														ActualizeUnloadCableInformation();
														GenOrgan.FavoriteFunctionPanel = INR_UnloadCablePanel;
														ShowFavoriteFunctionPanel();
														break;
								case CS_STOP:
								case CS_ABORT:
														VarControl.SetControl_UnloadCableOff();
														GenOrgan.FavoriteFunctionPanel = INR_NormalViewPanel;
														ShowFavoriteFunctionPanel();
														break;
								default:
										break;
						};
						break;

				default:
						break;
		};
};

// ==========================================================================
// FUNKTIONSPANELVERWALTUNG
// ==========================================================================

// --------------------------------------------------------------------------
// Sorgt für die sichtbar/unsichtbar-Einstellungen der Objekte auf ChoicePanel
// ObjectBits ... Jedes gesetzte Bit (1) repräsentiert eine Sichtbare Objektgruppe,
//                jedes nicht gesetzte Bit (0) representiert eine unsichtbare Objectgruppe
//                0x1 ... Überschrift
//                0x2 ... Taste und Beschriftung 1. Zeile
//                0x3 ... Taste und Beschriftung 2. Zeile usw.
void __fastcall TMainForm::ChoicePanelObjectsVisibility(unsigned int ObjectBits)
{
        // Überschrift
        if ((ObjectBits & 0x00000001) != 0)
        {
                ChoiceCaptionLabel->Visible = true;
        }else
        {
                ChoiceCaptionLabel->Visible = false;
        };

        // Auswahl-Objekt-Gruppen
        if ((ObjectBits & 0x00000002) != 0)
        {
                ChoiceLabel1->Visible = true;
                ChoiceButton1->Visible = true;
        }else
        {
                ChoiceLabel1->Visible = false;
                ChoiceButton1->Visible = false;
        };

        if ((ObjectBits & 0x00000004) != 0)
        {
                ChoiceLabel2->Visible = true;
                ChoiceButton2->Visible = true;
        }else
        {
                ChoiceLabel2->Visible = false;
                ChoiceButton2->Visible = false;
        };

        if ((ObjectBits & 0x00000008) != 0)
        {
                ChoiceLabel3->Visible = true;
                ChoiceButton3->Visible = true;
        }else
        {
                ChoiceLabel3->Visible = false;
                ChoiceButton3->Visible = false;
        };

        if ((ObjectBits & 0x00000010) != 0)
        {
                ChoiceLabel4->Visible = true;
                ChoiceButton4->Visible = true;
        }else
        {
                ChoiceLabel4->Visible = false;
                ChoiceButton4->Visible = false;
        };

        if ((ObjectBits & 0x00000020) != 0)
        {
                ChoiceLabel5->Visible = true;
                ChoiceButton5->Visible = true;
        }else
        {
                ChoiceLabel5->Visible = false;
                ChoiceButton5->Visible = false;
        };
};

// --------------------------------------------------------------------------
// Macht das aktuelle Funktions-Panel sichtbar, welches in
// GenOrgan.CurrFunctionPanel angegeben ist
void __fastcall TMainForm::ShowCurrFunctionPanel()
{
        // alle unsichtbar
        ChoicePanel->Visible = false;
        CalibDepthPanel->Visible = false;
        CalibTractionPanel->Visible = false;
        DialogPanel->Visible = false;
		UnloadCablePanel->Visible = false;
		FileAdminPanel->Visible = false;
		InfoPanel->Visible = false;
		NormalViewPanel->Visible = false;
		ParameterPanel->Visible = false;
		SystemPanel->Visible = false;
		MainMenuPanel->Visible = false;
		MessagePanel->Visible = false;

// ToDo: Einsatzort prüfen - VirtKeypad1->Visible
		VirtKeypad1->Visible = false; // Bei Änderung der Anzeige Nummerntastatur verschwinden lassen

		// ausgewähltes sichtbar
		switch (GenOrgan.CurrFunctionPanel)
		{
				case INR_ChoicePanel:
								KeyBoardPanel->Visible = false;
								MakeChoicePanelLayout();
								ChoicePanel->Visible = true;
								break;

				case INR_CalibDepthPanel:
// ToDo: wenn Standardtastatur nicht mehr nötig, so lassen
//								KeyBoardPanel->Visible = true;
//								VKeyBoardBlock(_VKB_TAB | _VKB_CTRL | _VKB_ALT | _VKB_RETURN | _VKB_INS);
								KeyBoardPanel->Visible = false;
								MakeCalibDepthPanelLayout();
								CalibDepthPanel->Visible = true;
								break;

				case INR_CalibTractionPanel:
// ToDo: wenn Standardtastatur nicht mehr nötig, so lassen
//								KeyBoardPanel->Visible = true;
//								VKeyBoardBlock(_VKB_TAB | _VKB_CTRL | _VKB_ALT | _VKB_RETURN | _VKB_INS);
								KeyBoardPanel->Visible = false;
								MakeCalibTractionPanelLayout();
								CalibTractionPanel->Visible = true;
								break;

				case INR_DialogPanel:
								KeyBoardPanel->Visible = false;
								MakeDialogPanelLayout();
								DialogPanel->Visible = true;
								break;

				case INR_UnloadCablePanel:
// ToDo: wenn Standardtastatur nicht mehr nötig, so lassen
//								KeyBoardPanel->Visible = true;
//								VKeyBoardBlock(_VKB_TAB | _VKB_CTRL | _VKB_ALT | _VKB_RETURN | _VKB_INS);
								KeyBoardPanel->Visible = false;
//                                MakeUnloadCablePanelLayout();
								UnloadCablePanel->Visible = true;
								break;

				case INR_FileAdminPanel:
								KeyBoardPanel->Visible = true;
								VKeyBoardBlock(_VKB_TAB);
								MakeFileAdminPanelLayout();
								FileAdminPanel->Visible = true;
								break;

				case INR_InfoPanel:
								KeyBoardPanel->Visible = false;
								MakeInfoPanelLayout();
								InfoPanel->Visible = true;
								break;

				case INR_NormalViewPanel:
								KeyBoardPanel->Visible = false;
								MakeNormalViewPanelLayout();
								NormalViewPanel->Visible = true;
								break;

				case INR_ParameterPanel:
// ToDo: wenn Standardtastatur nicht mehr nötig, so lassen
//								KeyBoardPanel->Visible = true;
//								VKeyBoardBlock(_VKB_TAB | _VKB_CTRL | _VKB_ALT | _VKB_RETURN | _VKB_INS);
								KeyBoardPanel->Visible = false;
								MakeParameterPanelLayout();
								ParameterPanel->Visible = true;
								break;

				case INR_SystemPanel:
// ToDo: wenn Standardtastatur nicht mehr nötig, so lassen
//								KeyBoardPanel->Visible = true;
//								VKeyBoardBlock(_VKB_TAB | _VKB_CAPS_SHIFT | _VKB_CTRL | _VKB_ALT | _VKB_RETURN | _VKB_BACKSPACE | _VKB_DEL | _VKB_INS);
								KeyBoardPanel->Visible = false;
								MakeSystemPanelLayout();
								SystemPanel->Visible = true;
								break;

				case INR_MainMenuPanel:
								KeyBoardPanel->Visible = false;
								MakeMainMenuPanelLayout();
								MainMenuPanel->Visible = true;
								break;

				case INR_MessagePanel:
								KeyBoardPanel->Visible = false;
								MessagePanel->Visible = true;
								MakeMessagePanelLayout();
								break;

				default:        KeyBoardPanel->Visible = false;
								NormalViewPanel->Visible = true;
								break;
		};
};

// --------------------------------------------------------------------------
// Macht das favorisierte Funktions-Panel, welches in GenOrgan.FavoriteFunctionPanel angegeben ist, zun aktuellen und sichtbar
void __fastcall TMainForm::ShowFavoriteFunctionPanel() {
	GenOrgan.CurrFunctionPanel = GenOrgan.FavoriteFunctionPanel;
    ShowCurrFunctionPanel();
}
// --------------------------------------------------------------------------
// ==========================================================================
// TASTENDRUCK
// ==========================================================================
// MENÜ
void __fastcall TMainForm::SetParameterButtonClick(TObject *Sender)
{
		// ParameterPanel aktivieren und anzeigen
		if (GenOrgan.CurrFunctionPanel != INR_ParameterPanel) {
			GenOrgan.CurrFunctionPanel = INR_ParameterPanel;
			ActualizeParameterView();
			ShowCurrFunctionPanel();
			return;
		}

		// Favoriten wieder anzeigen
		ShowFavoriteFunctionPanel();

}

//---------------------------------------------------------------------------

void __fastcall TMainForm::FileAdminButtonClick(TObject *Sender)
{
		// FileAdminPanel aktivieren und anzeigen
		if (GenOrgan.FavoriteFunctionPanel != INR_FileAdminPanel) {
			ActualizeFileAdminView();
			GenOrgan.CurrFunctionPanel = INR_FileAdminPanel;
			ShowCurrFunctionPanel();
			return;
		}

		// Favoriten wieder anzeigen
		ShowFavoriteFunctionPanel();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CalibrateButtonClick(TObject *Sender)
{
		if (VarControl.GetControl_CalibrateOn() == _ON) // Modus Kalibrieren aus
		{
				ProgramModeAdministrations(5, CS_STOP);
		}else                                           // Modus Kalibrieren ein
		{
				VarControl.SetControl_CalibrateOn();
				ProgramModeAdministrations(5, CS_START);
		};
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UnloadCableButtonClick(TObject *Sender)
{
		// UnloadCablePanel aktivieren und anzeigen
		if (GenOrgan.FavoriteFunctionPanel != INR_UnloadCablePanel) {
			ProgramModeAdministrations(8, CS_START);
			return;
		}

		// Zur Normalansicht wechseln
		if (GenOrgan.CurrFunctionPanel == INR_UnloadCablePanel) {
			ProgramModeAdministrations(8, CS_STOP);
			return;
		}

		// Favoriten wieder anzeigen
		ShowFavoriteFunctionPanel();

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SystemButtonClick(TObject *Sender)
{
		// SystemPanel Aktivieren und Anzeigen
		if (GenOrgan.FavoriteFunctionPanel != INR_SystemPanel) {
			ActualizeSystemView();
			ActualizeSystemDateMaskEdit = true;
			ActualizeSystemTimeMaskEdit = true;
			GenOrgan.CurrFunctionPanel = INR_SystemPanel;
			ShowCurrFunctionPanel();
			return;
		}

		// Favoriten wieder anzeigen
		ShowFavoriteFunctionPanel();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::MainMenuSpeedButtonClick(TObject *Sender)
{

		// Bei Bedarf Modus "Kalibrieren" ausschalten
		if (VarControl.GetControl_CalibrateOn() == _ON) ProgramModeAdministrations(5, CS_STOP);

		if (GenOrgan.CurrFunctionPanel == INR_MainMenuPanel)
		{
				ShowFavoriteFunctionPanel();
		}else
		{
				GenOrgan.CurrFunctionPanel = INR_MainMenuPanel;
				ShowCurrFunctionPanel();
		};


}
//---------------------------------------------------------------------------

void __fastcall TMainForm::MessagePanelButtonClick(TObject *Sender)
{
	// MessagePanel aktivieren und anzeigen
	if (GenOrgan.FavoriteFunctionPanel != INR_MessagePanel) {
		GenOrgan.CurrFunctionPanel = INR_MessagePanel;
		ShowCurrFunctionPanel();
		return;
	}

	// Favoriten wieder anzeigen
	ShowFavoriteFunctionPanel();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// EINGABETASTEN FÜR DATENÜBERNAHME/-GABE (Parameter)
//---------------------------------------------------------------------------
void __fastcall TMainForm::ParameterExitClick(TObject *Sender)
{
		// Wenn Kabel entladen abbrechen gedrückt wird,
		// muss "Kabel entladen"-Modus beendet werden
		if (Sender == UnloadCableExitButton)
		{
				ProgramModeAdministrations(8, CS_STOP);
		};

		GenOrgan.FavoriteFunctionPanel = INR_NormalViewPanel;
		ShowFavoriteFunctionPanel();
}

//---------------------------------------------------------------------------
// Setzen neuer Parameter aus dem ParameterPanel und FileAdminPanel
// Sind ungültige Werte enthalten, bleiben die alten erhalten
void __fastcall TMainForm::ParameterOkClick(TObject *Sender)
{
		int HV;

		GenOrgan.FavoriteFunctionPanel = INR_NormalViewPanel;

		// ----------------
		// LOG-Datei

		AnsiString HAnsi;
		if (Sender == LOGFileOkButton)
		{
//                LOGFileLocationMaskEdit->Text.SetLength(LOG_LOCATION_LENGHT - 1);
//                strcpy(GenOrgan.LOG_LOCATION, LOGFileLocationMaskEdit->Text.c_str());

				LOGFileLocationMaskEdit->Text.SetLength(LOG_LOCATION_LENGHT - 1);
				HAnsi = LOGFileLocationMaskEdit->Text;
				strcpy(GenOrgan.LOG_LOCATION, HAnsi.c_str());

				LOGFileWellMaskEdit->Text.SetLength(LOG_LOCATION_LENGHT - 1);
				HAnsi = LOGFileWellMaskEdit->Text;
				strcpy(GenOrgan.LOG_WELL, HAnsi.c_str());

				LOGFileNameMaskEdit->Text.SetLength(LOG_LOCATION_LENGHT - 1);
				HAnsi = LOGFileNameMaskEdit->Text;
                strcpy(GenOrgan.LOG_NAME, HAnsi.c_str());

				LOGFileTeamMaskEdit->Text.SetLength(LOG_TEAM_LENGTH - 1);
				HAnsi = LOGFileTeamMaskEdit->Text;
				strcpy(GenOrgan.LOG_TEAM, HAnsi.c_str());

				LOGFileWinchMachineryMaskEdit->Text.SetLength(LOG_TEAM_LENGTH - 1);
				HAnsi = LOGFileWinchMachineryMaskEdit->Text;
				strcpy(GenOrgan.LOG_WINCHMACHINERY, HAnsi.c_str());

				LOGFileCommentsMemo->Text.SetLength(LOG_LOCATION_LENGHT - 1);
				HAnsi = LOGFileCommentsMemo->Text;
				strcpy(GenOrgan.LOG_COMMENT, HAnsi.c_str());

				if (LOGFileLanguageGermanRadioButton->Checked)
                {
                        GenOrgan.LOG_Lng = __GERMAN;
                }else
                if(LOGFileLanguageRussianRadioButton->Checked)
                {
                        GenOrgan.LOG_Lng = __RUSSIAN;
                }else
                if(LOGFileLanguageEnglishRadioButton->Checked)
                {
                        GenOrgan.LOG_Lng = __ENGLISH;
                };

                // Datei-Typ
				if (LOGFileTypeLAS2RadioButton->Checked) VarControl.SetParam_LOGFileType(LOGFILE_LAS2);
				else if (LOGFileTypeGeoSysV1RadioButton->Checked) VarControl.SetParam_LOGFileType(LOGFILE_GEOSYSV1);
				else VarControl.SetParam_LOGFileType(LOGFILE_GEOSYSV2);

				if (LOGFileTensionUnit_kp_RadioButton->Checked) VarControl.SetParam_LOGFileTensionUnit(LOGFILE_TENSIONUNIT_kp);
				else VarControl.SetParam_LOGFileTensionUnit(LOGFILE_TENSIONUNIT_kN);

                // Neue Protokolldatei bei Druck der "Übernehmen"-Taste im LOG-File-Menü 
                GenOrgan.NewLOG = true;
        };

		// ----------------
		// Daten in die Parameter-Datei übernehmen
		if (VarControl.WriteData() == _BAD)
		{
				GenOrgan.Evnt << 12;											// Fehler 12 ein

		}else
		{
				GenOrgan.Evnt >> 11;											// Fehler 11 aus
				GenOrgan.Evnt >> 12;											// Fehler 12 aus
		};

        // ----------------
        // Datum / Uhrzeit setzen
        if (Sender == SystemDateTimeAcceptButton)
        {
                time_t HTime;
                tm HTimeStruct;
                char HString[5];

                // Sommerzeitstatus ermitteln
//                time(&HTime);

                // Datum Tag
                HString[0] = SystemDateMaskEdit->Text.c_str()[0];
                HString[1] = SystemDateMaskEdit->Text.c_str()[1];
                HString[2] = 0;
                HV = atoi(HString);
                HTimeStruct.tm_mday = HV;
                // Datum Monat
                HString[0] = SystemDateMaskEdit->Text.c_str()[3];
                HString[1] = SystemDateMaskEdit->Text.c_str()[4];
                HString[2] = 0;
                HV = StrToInt(HString);
                HTimeStruct.tm_mon = HV - 1;
                // Datum Jahr
                HString[0] = SystemDateMaskEdit->Text.c_str()[6];
                HString[1] = SystemDateMaskEdit->Text.c_str()[7];
                HString[2] = SystemDateMaskEdit->Text.c_str()[8];
                HString[3] = SystemDateMaskEdit->Text.c_str()[9];
                HString[4] = 0;
                HV = StrToInt(HString);
                HTimeStruct.tm_year = HV - 1900;

                // Uhrzeit Stunde
                HString[0] = SystemTimeMaskEdit->Text.c_str()[0];
                HString[1] = SystemTimeMaskEdit->Text.c_str()[1];
                HString[2] = 0;
                HV = StrToInt(HString);
                HTimeStruct.tm_hour = HV;
                // Uhrzeit Minute
                HString[0] = SystemTimeMaskEdit->Text.c_str()[3];
                HString[1] = SystemTimeMaskEdit->Text.c_str()[4];
                HString[2] = 0;
                HV = StrToInt(HString);
                HTimeStruct.tm_min = HV;
                // Uhrzeit Sekunde
                HString[0] = SystemTimeMaskEdit->Text.c_str()[6];
                HString[1] = SystemTimeMaskEdit->Text.c_str()[7];
                HString[2] = 0;
                HV = StrToInt(HString);
                HTimeStruct.tm_sec = HV;
                HTimeStruct.tm_isdst = -1;

                HTime = mktime(&HTimeStruct);
				stime(&HTime);
		};

		ActualizeParameterView();
		ActualizeFileAdminView();
		ShowFavoriteFunctionPanel();
        ShowSystemDateTime();
}

//---------------------------------------------------------------------------
// Soll-Fahrtrichtung umschalten
void __fastcall TMainForm::PChangeDirectionButtonClick(TObject *Sender)
{
	if (VarControl.GetParam_Direction() == _UPward) VarControl.SetParam_DirectionDown();
	else VarControl.SetParam_DirectionUp();
	VarControl.SavableParamChanged = true;

    ShowDirection(this);
}

//---------------------------------------------------------------------------
// CHOICEPANEL
//---------------------------------------------------------------------------
// Reaktionen auf Tastendrücke vom ChoicePanel
void __fastcall TMainForm::ChoicePanelButtonsClick(TObject *Sender)
{
        // Auswahl 1
        if (Sender == ChoiceButton1)
        {
                // Wurde beim Kalibrieren verwendet
                if (VarControl.GetControl_CalibrateOn() == _ON)
                {
                        ProgramModeAdministrations(5, CS_SPECIAL1);
                };
        };

        // Auswahl 2
        if (Sender == ChoiceButton2)
        {
                // Wurde beim Kalibrieren verwendet
                if (VarControl.GetControl_CalibrateOn() == _ON)
                {
                        ProgramModeAdministrations(5, CS_SPECIAL2);
                };
        }else

        // Auswahl 3
        if (Sender == ChoiceButton3)
        {
        }else

        // Auswahl 4
        if (Sender == ChoiceButton4)
        {
        }else

        // Auswahl 5
        if (Sender == ChoiceButton5)
        {
        };

        // Abbrechen gedrückt
        if (Sender == ChoiceAbortButton)
        {
                // Wurde beim Kalibrieren verwendet
                if (VarControl.GetControl_CalibrateOn() == _ON)
                {
                        ProgramModeAdministrations(5, CS_ABORT);
                };
        };
}
//---------------------------------------------------------------------------

// BEDIENTASTEN KALIBRIERUNG
//---------------------------------------------------------------------------

// Teufenrechnerkalibrierung Tastendruck
void __fastcall TMainForm::CalibDepthButtonsClick(TObject *Sender)
{

		if (Sender == CalibDepthAcceptButton)
			if (VarControl.GetParam_CalFactDepth_Earmarked() > 0) {

				// Wert übernehmen
				VarControl.SetParam_CalFactDepth(VarControl.GetParam_CalFactDepth_Earmarked());
				VarControl.SetParam_CalFactDepth_Earmarked(0);	// Kalibrierfaktor übergeben, vorgemerkter Wert verworfen

				// Setzen aktivieren
				VarControl.SetControl_SetDepthCalFactor_On();
				VarControl.SetControl_SetDepthCalFactor_DC_On();

				// Wert abspeichern
				VarControl.SavableParamChanged = true;
				ParameterInputExit(this);

				GenOrgan.NewLOG = true;									// Neue Protokolldatei beginnen, damit Parameter im Header übernommen werden

			}

		if (Sender == CalibDepthAbortButton) ProgramModeAdministrations(5, CS_STOP);
}

//---------------------------------------------------------------------------
// Zugkraftkalibrierung Tastendruck
void __fastcall TMainForm::CalibTractionButtonsClick(TObject *Sender)
{
        int HV;

        // Messung 1 durchführen
        if (Sender == CalibTractionMessure1Button)
        {
				VarControl.SetControl_CalibTension_GetVal1On();
        }else

        // Messung 2 durchführen
        if (Sender == CalibTractionMessure2Button)
        {
                VarControl.SetControl_CalibTension_GetVal2On();
        }else

        // Daten übernehmen
        if (Sender == CalibTractionCompleteSetButton)
        {
				VarControl.SetControl_CalibTractionSetParamOn();
		}else

		// Nullpunkt setzen
		if (Sender == CalibTractionGetZeroButton)
		{
				VarControl.SetControl_CalibTractionSetZeroOn();
		}else

		// Fertig
		if (Sender == CalibTractionAbortButton) ProgramModeAdministrations(5, CS_STOP);
}
//---------------------------------------------------------------------------

// STEUERTASTEN GEMÄß PROGRAMMZWECK
//---------------------------------------------------------------------------

// Akustische Alarm-/Warnungs-Signalisierung ein/aus
void __fastcall TMainForm::AlarmSoundOnOffButtonClick(TObject *Sender)
{
// ToDo: Nach bewähren der neuen Methode "AcousticSignalOn"-Flag vollständig entfernen
//		GenOrgan.AcousticSignalOn = false;
    GenOrgan.Evnt.ConfirmAll();
}
//---------------------------------------------------------------------------

// Freigabe nach Grenzwertüberschreitung
void __fastcall TMainForm::ResetButtonClick(TObject *Sender)
{
        VarControl.SetControl_ClearAfterThresholdDepthOn();
        VarControl.SetControl_ClearAfterThresholdTractionOn();
        VarControl.SetControl_ResetSpecialSignalError1FlagOn();
        VarControl.SetControl_ResetSpecialSignalError2FlagOn();
}
//---------------------------------------------------------------------------

// Motor "Ein" Taste gedrückt
void __fastcall TMainForm::EngineButtonDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        VarControl.SetControl_EngineStartOn();
}
//---------------------------------------------------------------------------

// Motor "Ein" Taste loslassen
void __fastcall TMainForm::EngineButtonUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        VarControl.SetControl_EngineStartOff();
}
//---------------------------------------------------------------------------
// Motor-Drehzahl veringern
void __fastcall TMainForm::EngineRevolutionDownButtonDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        VarControl.SetControl_EngineRevolutionDown();
}

//---------------------------------------------------------------------------
// Motor-Drehzahl erhöhen
void __fastcall TMainForm::EngineRevolutionUpMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        VarControl.SetControl_EngineRevolutionUp();
}

//---------------------------------------------------------------------------
// Motor-Drehzahl beibehalten
void __fastcall TMainForm::EngineRevolutionHoldButton(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        VarControl.SetControl_EngineRevolutionHold();
}

//---------------------------------------------------------------------------
// Schnelle Fahrt ein/aus
void __fastcall TMainForm::FastRunButtonClick(TObject *Sender)
{
        if (VarControl.GetControl_FastRun() == _OFF) VarControl.SetControl_FastRunOn();
        else VarControl.SetControl_FastRunOff();
}
//---------------------------------------------------------------------------

// LOG-Datei kopieren
// vorhandene Dateien werden kommentarlos überschrieben
void __fastcall TMainForm::LOGFileAdminCopyButtonClick(TObject *Sender)
{
	const CPYLNSIZE = 512;	// erlaubte Zeichenanzahl einer Protokolldateizeile
		ifstream SourceFile;
		ofstream DestinationFile;
		AnsiString SourceName, DestinationName;
		AnsiString SourceNameValidation, DestinationNameValidation, HAnsi;
		char CopyLine[CPYLNSIZE];
        char CopyValidationArray[1024];
        int HV;

        unsigned long oldSourcePos = 0, currSourcePos = 0, SourceFileSize, HVlong;

        // Alle ausgewählten Dateien kopieren
        for (int i = 0; i < LOGFileAdminFileListBox->Items->Count; ++i)
        {
                // Item ausgewählt
                if (!LOGFileAdminFileListBox->Selected[i]) continue;

                // Dateinamen erstellen
                // LOG-Datei
				SourceName = GenOrgan.LOGPath + LOGFileAdminFileListBox->Items->Strings[i];
				DestinationName = LOGFileAdminDestinationComboBox->Items->Strings[LOGFileAdminDestinationComboBox->ItemIndex] + LOGFileAdminFileListBox->Items->Strings[i];
                // Validierungsdatei
                HAnsi = LOGFileAdminFileListBox->Items->Strings[i];
                HV = HAnsi.LastDelimiter(".");                                  // Trenn-Punkt Dateierweiterung/Dateiname suchen
                if (HV > 0) HAnsi.SetLength(--HV);                              // Wenn vorhanden, Dateierweiterung abschneiden
                HAnsi = HAnsi + LOG_VALIDATION_FileExt;                         // Dateierweiterung der Validierungsdatei anhängen
                SourceNameValidation = GenOrgan.LOGPath + HAnsi;
                DestinationNameValidation = LOGFileAdminDestinationComboBox->Items->Strings[LOGFileAdminDestinationComboBox->ItemIndex] + HAnsi;

                // Protokolldatei kopieren
                // ------------------------------
                SourceFile.open(SourceName.c_str());
                DestinationFile.open(DestinationName.c_str());

                if (SourceFile && DestinationFile)
                {
						GenOrgan.Evnt >> 14;									// Fehler 14 aus

                        // Dateigröße ermitteln
                        SourceFile.seekg(0, ios_base::end);
                        SourceFileSize = SourceFile.tellg();
                        SourceFile.seekg(0);

                        // Kopiervorgang
                        while (true)
                        {
                                // Zeilenweise kopieren
                                SourceFile.getline(CopyLine, CPYLNSIZE);

                                // Dateiende erreicht ? ->fertig
                                if (SourceFile.eof()) break;

                                DestinationFile << CopyLine << endl;

                                // Lese/Schreib Fehler
                                if (SourceFile.bad() || DestinationFile.bad())
                                {
										GenOrgan.Evnt << 14;					// Fehler 14 ein
										break;
								};

								// aktuelle Dateiposition ermitteln
								currSourcePos = SourceFile.tellg();

								ProgressGauge->Progress = currSourcePos * 100 / SourceFileSize;

								// Dateizeiger bewegt sich nicht mehr
								if (currSourcePos == oldSourcePos)
								{
										GenOrgan.Evnt << 14;					// Fehler 14 ein
										break;
								};

								oldSourcePos = currSourcePos;
						};
				}else GenOrgan.Evnt << 14;										// Fehler 14 ein

                SourceFile.close();
                DestinationFile.close();

                // Bei Kopierfehler (14) abbrechen
				if (GenOrgan.Evnt[14] == _ON) break;
                // ------------------------------

                // Validierungsdatei Kopieren
                // ------------------------------
                // Bei einem Fehler wird hier kommentarlos der aktuelle Kopier-
                // vorgang abgebrochen und mit der nächsten Datei weitergemacht
                SourceFile.open(SourceNameValidation.c_str(), ios_base::binary);
                DestinationFile.open(DestinationNameValidation.c_str(), ios_base::binary);

                if (SourceFile.is_open() && DestinationFile.is_open())
                {
                        SourceFile.seekg(0, ios_base::end);
                        SourceFileSize = SourceFile.tellg();
                        SourceFile.seekg(0, ios_base::beg);
                        while (SourceFile.good() && !SourceFile.eof() && DestinationFile.good())
                        {
                                if (((unsigned long)(SourceFile.tellg()) + sizeof(CopyValidationArray)) < SourceFileSize)
								{
                                        SourceFile.read(CopyValidationArray, sizeof(CopyValidationArray));
                                        DestinationFile.write(CopyValidationArray, sizeof(CopyValidationArray));
                                }else
                                {
                                        HV = SourceFileSize - (unsigned long)(SourceFile.tellg());
                                        if (HV < 1) break;                      // Datei komplett ausgelesen
                                        SourceFile.read(CopyValidationArray, HV);
                                        DestinationFile.write(CopyValidationArray, HV);
                                };
                        }
                };

                SourceFile.close();
                DestinationFile.close();
                // ------------------------------


                // Auswahl des aktuellen Items aufheben, da erledigt
                LOGFileAdminFileListBox->Selected[i] = false;
        };
}

//---------------------------------------------------------------------------
// LOG-Dateien löschen
// alle markierten LOG-Dateien werden gelöscht
void __fastcall TMainForm::LOGFileAdminDeleteButtonClick(TObject *Sender)
{
        int HV = 0;
        AnsiString LOGFileName, VALFileName, HAnsi;
        bool OK = true;
        // Alle ausgewählten Dateien kopieren
        for (int i = 0; i < LOGFileAdminFileListBox->Items->Count; ++i)
		{
                // Item ausgewählt
                if (!LOGFileAdminFileListBox->Selected[i]) continue;

                // Protokolldateipfad ermitteln
                LOGFileName = GenOrgan.LOGPath + LOGFileAdminFileListBox->Items->Strings[i];
                // Validierungsdatei-Pfad ermitteln
                HAnsi = LOGFileAdminFileListBox->Items->Strings[i];
                HV = HAnsi.LastDelimiter(".");                                  // Trenn-Punkt Dateierweiterung/Dateiname suchen
                if (HV > 0) HAnsi.SetLength(--HV);                              // Wenn vorhanden, Dateierweiterung abschneiden
                HAnsi = HAnsi + LOG_VALIDATION_FileExt;                         // Dateierweiterung der Validierungsdatei anhängen
                VALFileName = GenOrgan.LOGPath + HAnsi;

                if (DeleteFile(LOGFileName))
                {       // Löschen erfolgreich
                        LOGFileAdminFileListBox->Items->Delete(i);
                        --i;                                                    // LOGFileAdminFileListBox->Items->Count verringert sich nach dem Entfernen des aktuellen Wertes aus der Liste um 1, daher ist Anpassung notwendig 
                        DeleteFile(VALFileName);                                // Löschen der Validierungsdatei ohne Erfolgsauswertung
                }else   // Löschen erfolglos
                {
                        OK = false;
                };
        };

        // Fehlerauswertung
		if (OK) GenOrgan.Evnt >> 19;											// Fehler 19 AUS
		else GenOrgan.Evnt << 19;												// Fehler 19 EIN
}

//---------------------------------------------------------------------------
// Doppelklick auf die ausgewählte Datei
void __fastcall TMainForm::LOGFileAdminFileListBoxDoubleClick(
      TObject *Sender)
{
        ShowLOGFileAdminFocusedFile();
}

//---------------------------------------------------------------------------
// Sprachauswahl für LOG-Datei-Ansicht betätigt
void __fastcall TMainForm::LOGFileAdminLanguageCheck(TObject *Sender)
{
        if (LOGFileAdminGermanRadioButton->Checked)             LOGFileAdminCurrFileMemo->Font->Charset = LNG_CODE[__GERMAN];
        else if (LOGFileAdminRussianRadioButton->Checked)       LOGFileAdminCurrFileMemo->Font->Charset = LNG_CODE[__RUSSIAN];
        else if (LOGFileAdminEnglishRadioButton->Checked)       LOGFileAdminCurrFileMemo->Font->Charset = LNG_CODE[__ENGLISH];
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Freilauf ein/aus
void __fastcall TMainForm::LostMotionImpulseButtonClick(TObject *Sender)
{
        if (VarControl.GetControl_WinchBrakeOff() == _ON)
        {
                // Freilauf-Signal ist hier schon an

                // Folgende Zeile ermöglicht einen "Not-Ausstieg" aus gelöster Bremse, wenn "LostMotion-Signal ausbleibt" 
                VarControl.SetControl_WinchBrakeOff_Off();                      // In "WCAnalysis::ActUnloadCable()" wird dadurch das Umwickelsignal entfernt
        }else
        {
                VarControl.SetControl_LostMotionImpulseOn();
        };
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::UnloadCableMinTractionUpDownClick(
      TObject *Sender, TUDBtnType Button)
{
        int HV;
        bool DataChanged = false;                                               // wenn Daten geändert wurden, dann wird dieses Flag true

        if (Button == btNext)
        {
                HV = VarControl.GetParam_UnloadCableTraction() + STEP_UnloadCableTraction;
                if (HV > VarControl.GetParam_MaxTraction()) HV = VarControl.GetParam_MaxTraction();
                VarControl.SetParam_UnloadCableTraction(HV);

                ShowUnloadCableParameters();
                DataChanged = true;
        }else if (Button == btPrev)
        {
                HV = VarControl.GetParam_UnloadCableTraction() - STEP_UnloadCableTraction;
                if (HV < 100) HV = 100;
                VarControl.SetParam_UnloadCableTraction(HV);

                ShowUnloadCableParameters();
                DataChanged = true;
        };

        if (!DataChanged) return;

        // Daten in die Parameter-Datei übernehmen
        if (VarControl.WriteData() == _BAD)
        {
				GenOrgan.Evnt << 12;											// Fehler 12 ein
		}else
		{
				GenOrgan.Evnt >> 11;											// Fehler 11 aus
				GenOrgan.Evnt >> 12;											// Fehler 12 aus
        };

}
//---------------------------------------------------------------------------

// Signal-Horn ein
void __fastcall TMainForm::SignalHornButtonMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        VarControl.SetControl_SignalHornOn();
}
//---------------------------------------------------------------------------

// Signal-Horn aus
void __fastcall TMainForm::SignalHornButtonMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        VarControl.SetControl_SignalHornOff();
}
//---------------------------------------------------------------------------

// Automatik starten/anhalten
void __fastcall TMainForm::StartStopButtonClick(TObject *Sender)
{
        // Kabel entladen
		if (VarControl.GetControl_UnloadCable() == _ON)
        {
                if (VarControl.GetControl_UnloadCableActive() == _OFF)
                // Einschalten
                {
						VarControl.SetControl_UnloadCableActiveOn();
                }else
                // Ausschalten
                {
						VarControl.SetControl_UnloadCableActiveOff();
                };
        }else
        // Automatische Teufen-Fahrt
        {
                if (VarControl.GetControl_AutoRunOn() == _OFF)
				// Einschalten
                {
                        // bei Schwellwertüberschreitung gar nicht erst aktivierbar
                        if ((VarControl.GetInfo_DepthThresholdExceeded() == _OFF) &&
                            (VarControl.GetInfo_TractionThresholdExceeded() == _OFF) &&
                            (VarControl.GetInfo_IncEncError() == _OFF))
                        {
								VarControl.SetControl_AutoRunOn();
						};
				}else
				// Ausschalten
				{
						VarControl.SetControl_AutoRunOff();
                };
        };
}
//---------------------------------------------------------------------------

// System neu Booten
void __fastcall TMainForm::SystemRestartButtonClick(TObject *Sender)
{
        int HV;

        HANDLE hToken;
		TOKEN_PRIVILEGES tkp;
        OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken); // Token für diesen Prozess besorgen
        LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);    // LUID für shutsown ermitteln
        tkp.PrivilegeCount=1;
        tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken,false,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);   // Privileg für Shutdown setzen
//        ExitWindowsEx(EWX_REBOOT, 0);
        InitiateSystemShutdown(
                                NULL, // computer name
                                (char*)(SHUTDOWN_SYSTEM_REBOOT_MESSAGE), // message to display
                                SHUTDOWN_DELAY, // length of time to display
                                true, // force closed option
                                true // reboot option
                               );
        ExitButtonClick(this);
}
//---------------------------------------------------------------------------

// PC Herunterfahren
void __fastcall TMainForm::SystemShutdownButtonClick(TObject *Sender)
{
        int HV;
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;
        OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken); // Token für diesen Prozess besorgen
        LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);    // LUID für shutsown ermitteln
        tkp.PrivilegeCount=1;
        tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken,false,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);   // Privileg für Shutdown setzen
//        ExitWindowsEx(EWX_POWEROFF | EWX_SHUTDOWN, 0);
        InitiateSystemShutdown(
                                NULL, // computer name
                                (char*)(SHUTDOWN_SYSTEM_HALT_MESSAGE), // message to display
								SHUTDOWN_DELAY, // length of time to display
                                true, // force closed option
                                false // reboot option
                               );
         ExitButtonClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SystemProgramTerminationButtonClick(TObject *Sender)
{
        ExitButtonClick(Sender);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// SICHERHEITSABFRAGE
// Alle abgesicherten Aktionen werden erst durch einen "Ja-Nein-Dialog" umgeleitet
//---------------------------------------------------------------------------
// Umleiten von Tastendrücken zu Sicherheitsabfrage
void __fastcall TMainForm::SecCheckRedirectionStart(TObject *Sender)
{
        RedirectionInitiator = Sender;
//        if (GenOrgan.CurrFunctionPanel != INR_DialogPanel) RedirectionStartFunctionPanel = GenOrgan.CurrFunctionPanel;

		// Löschen der LOG-Dateien
		if (Sender == LOGFileAdminDeleteButton)
		{
				DialogCaptionLabel->Caption = LABEL_DELETE_LOG_FILES[GenOrgan.Lng];
				DialogInfoLabel->Caption = EXPLAN_DELETELOGFILES[GenOrgan.Lng];
				DialogCancelSpeedButton->Caption = LABEL_NO[GenOrgan.Lng];
				DialogOkSpeedButton->Caption = LABEL_YES[GenOrgan.Lng];

				GenOrgan.CurrFunctionPanel = INR_DialogPanel;
				ShowCurrFunctionPanel();
		}else

		// Freilaufabfrage
		if (Sender == LostMotionImpulseButton)
		{
				// Dialog-Text
//                if ((VarControl.GetInfo_LostMotion() == _OFF) || (VarControl.GetControl_WinchBrakeOff() == _ON)) // Freilauf ist aus
				if (VarControl.GetInfo_LostMotion() == _OFF)
				{
						DialogCaptionLabel->Caption = LABEL_LOSTMOTIONON[GenOrgan.Lng];
						DialogInfoLabel->Caption = EXPLAN_LOSTMOTIONONQUEST[GenOrgan.Lng];
				}else                                                           // Freilauf ist ein
				{
						DialogCaptionLabel->Caption = LABEL_LOSTMOTIONOFF[GenOrgan.Lng];
						DialogInfoLabel->Caption = EXPLAN_LOSTMOTIONOFFQUEST[GenOrgan.Lng];
				}

				DialogCancelSpeedButton->Caption = LABEL_NO[GenOrgan.Lng];
				DialogOkSpeedButton->Caption = LABEL_YES[GenOrgan.Lng];

				GenOrgan.CurrFunctionPanel = INR_DialogPanel;
				ShowCurrFunctionPanel();
		}else

		// System herunterfahren
		if (Sender == SystemShutdownButton)
		{
				VarControl.SetControl_EngineRevolutionAutoDown();               // Bei möglichem Steuerprogrammende Motor herunterfahren
				DialogCaptionLabel->Caption = LABEL_SYSTEM_TURNOFF[GenOrgan.Lng];
				DialogInfoLabel->Caption = EXPLAN_SYSTEM_TURNOFF[GenOrgan.Lng];
				DialogCancelSpeedButton->Caption = LABEL_NO[GenOrgan.Lng];
				DialogOkSpeedButton->Caption = LABEL_YES[GenOrgan.Lng];

				GenOrgan.CurrFunctionPanel = INR_DialogPanel;
				ShowCurrFunctionPanel();
		}else

		// System neu starten
		if (Sender == SystemRestartButton)
		{
				VarControl.SetControl_EngineRevolutionAutoDown();               // Bei möglichem Steuerprogrammende Motor herunterfahren
				DialogCaptionLabel->Caption = LABEL_SYSTEM_RESTART[GenOrgan.Lng];
				DialogInfoLabel->Caption = EXPLAN_SYSTEM_RESTART[GenOrgan.Lng];
				DialogCancelSpeedButton->Caption = LABEL_NO[GenOrgan.Lng];
				DialogOkSpeedButton->Caption = LABEL_YES[GenOrgan.Lng];

				GenOrgan.CurrFunctionPanel = INR_DialogPanel;
				ShowCurrFunctionPanel();
		}else

		// Programm beenden
		if (Sender == SystemProgramTerminationButton)
		{
				VarControl.SetControl_EngineRevolutionAutoDown();               // Bei möglichem Steuerprogrammende Motor herunterfahren
				DialogCaptionLabel->Caption = LABEL_EXIT_PROGRAM[GenOrgan.Lng];
				DialogInfoLabel->Caption = EXPLAN_SYSTEM_TERMINATE[GenOrgan.Lng];
				DialogCancelSpeedButton->Caption = LABEL_NO[GenOrgan.Lng];
				DialogOkSpeedButton->Caption = LABEL_YES[GenOrgan.Lng];

				GenOrgan.CurrFunctionPanel = INR_DialogPanel;
				ShowCurrFunctionPanel();
		};
};
//---------------------------------------------------------------------------

// Umleitungsende
void __fastcall TMainForm::SecCheckRedirectionEnd(TObject *Sender)
{
		if (Sender == DialogOkSpeedButton)
		{
				// Löschen der ausgewählten LOG-Dateien erwünscht
                if (RedirectionInitiator == LOGFileAdminDeleteButton)
                {
                        LOGFileAdminDeleteButtonClick(RedirectionInitiator);

						// wieder vorheriges Fenster anzeigen
//						GenOrgan.CurrFunctionPanel = RedirectionStartFunctionPanel;
//						ShowCurrFunctionPanel();
						// Favorisiertes Fenster wieder anzeigen
						ShowFavoriteFunctionPanel();
						return;
				};

                // Freilaufimpuls erwünscht
                if (RedirectionInitiator == LostMotionImpulseButton)
                {
                        LostMotionImpulseButtonClick(RedirectionInitiator);

                        // wieder vorheriges Fenster anzeigen
//                        GenOrgan.CurrFunctionPanel = RedirectionStartFunctionPanel;
//						ShowCurrFunctionPanel();
						// Favorisiertes Fenster wieder anzeigen
						ShowFavoriteFunctionPanel();
                        return;
                };

                // System herunterfahren erwünscht
                if (RedirectionInitiator == SystemShutdownButton)
                {
                        SystemShutdownButtonClick(RedirectionInitiator);

                        // wieder vorheriges Fenster anzeigen
// nicht nötig, da Programm sowieso beendet wird
//                        GenOrgan.CurrFunctionPanel = RedirectionStartFunctionPanel;
//                        ShowCurrFunctionPanel();
                        return;
                };

                // System-Neustart erwünscht
                if (RedirectionInitiator == SystemRestartButton)
                {
                        SystemRestartButtonClick(RedirectionInitiator);

                        // wieder vorheriges Fenster anzeigen
// nicht nötig, da Programm sowieso beendet wird
//                        GenOrgan.CurrFunctionPanel = RedirectionStartFunctionPanel;
//                        ShowCurrFunctionPanel();
                        return;
                };

                // Programmende erwünscht
                if (RedirectionInitiator == SystemProgramTerminationButton)
                {
                        SystemProgramTerminationButtonClick(RedirectionInitiator);

                        // wieder vorheriges Fenster anzeigen
// nicht nötig, da Programm sowieso beendet wird
//                        GenOrgan.CurrFunctionPanel = RedirectionStartFunctionPanel;
//                        ShowCurrFunctionPanel();
                        return;
                };

        };

        if (Sender == DialogCancelSpeedButton)
		{
                // Löschen der ausgewählten LOG-Dateien abgebrochen
                if (RedirectionInitiator == LOGFileAdminDeleteButton)
                {
                        // wieder vorheriges Fenster anzeigen
//                        GenOrgan.CurrFunctionPanel = RedirectionStartFunctionPanel;
//                        ShowCurrFunctionPanel();
						// Favorisiertes Fenster wieder anzeigen
						ShowFavoriteFunctionPanel();
                        return;
                };

                // Freilaufimpuls verworfen
                if (RedirectionInitiator == LostMotionImpulseButton)
                {
                        // wieder vorheriges Fenster anzeigen
//                        GenOrgan.CurrFunctionPanel = RedirectionStartFunctionPanel;
//                        ShowCurrFunctionPanel();
						// Favorisiertes Fenster wieder anzeigen
						ShowFavoriteFunctionPanel();
						return;
				};

                // System herunterfahren verworfen
                if (RedirectionInitiator == SystemShutdownButton)
                {
                        // wieder vorheriges Fenster anzeigen
//                        GenOrgan.CurrFunctionPanel = RedirectionStartFunctionPanel;
//                        ShowCurrFunctionPanel();
						// Favorisiertes Fenster wieder anzeigen
						ShowFavoriteFunctionPanel();
						return;
                };

                // System-Neustart verworfen
                if (RedirectionInitiator == SystemRestartButton)
                {
                        // wieder vorheriges Fenster anzeigen
//                        GenOrgan.CurrFunctionPanel = RedirectionStartFunctionPanel;
//						ShowCurrFunctionPanel();
						// Favorisiertes Fenster wieder anzeigen
						ShowFavoriteFunctionPanel();
						return;
				};

				// Programmende verworfen
				if (RedirectionInitiator == SystemProgramTerminationButton)
				{
						// wieder vorheriges Fenster anzeigen
//                        GenOrgan.CurrFunctionPanel = RedirectionStartFunctionPanel;
//                        ShowCurrFunctionPanel();
						// Favorisiertes Fenster wieder anzeigen
						ShowFavoriteFunctionPanel();
                        return;
                };
        };
};
//---------------------------------------------------------------------------
// Windenbremse Ein/-Ausschalten
void __fastcall TMainForm::WinchBrakeOffSpeedButtonClick(TObject *Sender)
{
		// diese Sperren allein sind noch nicht 100% wirksam, komplettiert wird das durch die Blockade in WCAnalysis::SetLockStates()
		if ((VarControl.GetControl_WinchBrakeOffImpulse() == _ON) ||
			(VarControl.GetControl_LostMotionImpulse() == _ON)) return;

        VarControl.SetControl_WinchBrakeOffImpulse_On();                        // Gibt den Impuls zum Lösen der Bremse
}
//---------------------------------------------------------------------------
// Wird von verschiedenen Komponenten ausgelöst, sobald sie den Fokus erhalten
void __fastcall TMainForm::ComponentEntered(TObject *Sender)
{
        if (Sender == SystemDateMaskEdit) ActualizeSystemDateMaskEdit = false;
		if (Sender == SystemTimeMaskEdit) ActualizeSystemTimeMaskEdit = false;
		VirtKeypad1->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::DevelFormButtonClick(TObject *Sender)
{
        if (DevelForm == 0) return;

        if (DevelForm->Visible) DevelForm->Hide();
        else DevelForm->Show();
}
//---------------------------------------------------------------------------

// Motor "Aus" Taste gedrückt
void __fastcall TMainForm::EngineStopButtonDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        VarControl.SetControl_EngineStopOn();
}
//---------------------------------------------------------------------------

// Motor "Aus" Taste losgelassen
void __fastcall TMainForm::EngineStopButtonUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        VarControl.SetControl_EngineStopOff();
}

//---------------------------------------------------------------------------
// Dynamische Maximalzugkraftberechnung über Tastendruck auf Hauptbildschirm Ein/Aus
void __fastcall TMainForm::DynamicTensionOnOffButtonClick(TObject *Sender)
{
		if (VarControl.GetControl_CalcCurrTractions() == _ON) VarControl.SetControl_CalcCurrTractionsOff();
        else VarControl.SetControl_CalcCurrTractionsOn();
}

//---------------------------------------------------------------------------
// Auswahl ext. cm-Impuls
void __fastcall TMainForm::Ext_cm_Imp_RadioButtonClick(TObject *Sender)
{
	int HV = VarControl.GetParam_FormatCMImpulse();

	if (PKARAT2RadioButton->Checked) {
		VarControl.SetParam_FormatCMImpulse(_KARAT2);
		ShowExtCMImpulseFormatImage(_KARAT2);
	} else if (PKARAT2NRadioButton->Checked) {
		VarControl.SetParam_FormatCMImpulse(_KARAT2N);
		ShowExtCMImpulseFormatImage(_KARAT2N);
	} else if (PONYXRadioButton->Checked) {
		VarControl.SetParam_FormatCMImpulse(_ONYX);
		ShowExtCMImpulseFormatImage(_ONYX);
	} else if (PKEDRRadioButton->Checked) {
		VarControl.SetParam_FormatCMImpulse(_KEDR);
		ShowExtCMImpulseFormatImage(_KEDR);
	} else if (PKEDRNRadioButton->Checked) {
		VarControl.SetParam_FormatCMImpulse(_KEDRN);
		ShowExtCMImpulseFormatImage(_KEDRN);
	} else { // wenn nichts anderes gesetzt ist, dann wird so KARAT gesetzt
		VarControl.SetParam_FormatCMImpulse(_KARAT);
		ShowExtCMImpulseFormatImage(_KARAT);
	}

	// Nur wenn Änderung vorhanden
	if (HV != VarControl.GetParam_FormatCMImpulse()) {

		VarControl.SavableParamChanged = true;
		VarControl.SetControl_SetFormatCMImpulseOn(); // cm-Inpulse müssen gesetzt werden

		// Anweisung an ControlThread, die Daten an Teufenrechner zu senden
		VarControl.SetControl_SetDepthCalcParamOn();
	}
}
//---------------------------------------------------------------------------
// Rücksetzen des Countdowns bei PowerFail
void __fastcall TMainForm::PowerFailCdResetSpeedButtonClick(TObject *Sender)
{
	PowerFailStartTime_Reset = true;

}
//---------------------------------------------------------------------------
// MM Ein- Aus
void __fastcall TMainForm::MagneticMarkOnOffButtonClick(TObject *Sender)
{
    if (VarControl.GetControl_MagneticMarkOn() == _ON) VarControl.SetControl_MagneticMarkOff();
    else VarControl.SetControl_MagneticMarkOn();

	VarControl.SetControl_SetMagneticMarkOn();
	VarControl.SetControl_SetDepthCalcParamOn();
//    ShowMagneticMarkStatus(); muss nicht aufgerufen werden, da durch WCDisplayThread::ActualizeNormalView() ständig aktualisiert
}
//---------------------------------------------------------------------------
// Zeigt den Namen des ausgewählten Ablaufprogramms für die Programmautomatik an
void __fastcall TMainForm::PProgramAutomChoiseClick(TObject *Sender)
{
	int HV = PProgramAutomFileListBox->ItemIndex;
	wchar_t Hwstring[WCPrgAutomaticPRGNAMELENGTH];
	UnicodeString Hunicodestring;

	GenOrgan.PrgAutoIndex = 0;  // Ablaufprogrammindex auf zurücksetzen

	// Beschreibung / Namen des Ausgewählten Programms anzeigen
	// --------------------------------------------------------
	if (HV < 0) { // nichts ausgewählt
		PProgramAutomPrgNameLabel->Caption = L"";
		return;
	}

	if (GenOrgan.PrgAutomatic.GetFilePrgName(PProgramAutomFileListBox->Items->Strings[HV].w_str(), Hwstring))
		PProgramAutomPrgNameLabel->Caption = Hwstring;
	else PProgramAutomPrgNameLabel->Caption = L"";

	// Programm setzen
	// --------------------------------------------------------
	HV = 0; // wird >0 gesetzt, wenn alles i.O.
	if (PProgramAutomFileListBox->ItemIndex > -1) {
		Hunicodestring = PProgramAutomFileListBox->Items->Strings[PProgramAutomFileListBox->ItemIndex];
		Hunicodestring.SetLength(Hunicodestring.LastDelimiter(L".") - 1); // ab Punkt alles abschneiden (Dateierweiterung)
		VarControl.SetParam_AutoRunProgramFileName(Hunicodestring.w_str());
		Hunicodestring = VarControl.GetParam_AutoRunProgramFileName();
		Hunicodestring = Hunicodestring + __AutoRunPrgFileExtention;
		if (GenOrgan.PrgAutomatic.Read(Hunicodestring.w_str()))
			HV = 1; // Programm gültig
		}

	// Programm aktivieren
	// --------------------------------------------------------
	if (PProgramAutomCheckBox->Checked && (HV == 1)) VarControl.SetParam_AutoRunProgramableOn();
	else VarControl.SetParam_AutoRunProgramableOff();

	ShowAutoRunProgramItemList();   // Damit Anzeige je nach Bedarf aktiviert oder deaktiviert wird
}
//---------------------------------------------------------------------------

// Setzen des neuen Programmindex
void __fastcall TMainForm::AutoRunProgramItemListListBoxClick(TObject *Sender)
{
	GenOrgan.PrgAutoIndex = AutoRunProgramItemListListBox->ItemIndex;
}
//---------------------------------------------------------------------------

// Richtung für Automatikfahrt wurde über SpeedView geändert
void __fastcall TMainForm::SpeedViewAutoDirChanged(TObject *Sender)
{
	if (SpeedView->AutoDirectionDown) VarControl.SetParam_DirectionDown();
	else VarControl.SetParam_DirectionUp();

	ShowDirection(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SpeedViewAutoSpeedChanged(TObject *Sender)
{
	VarControl.SetParam_Velocity(SpeedView->AutoSpeed);
	ShowVelocity(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::SpecialFuncButtonMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	if (VarControl.GetParam_LineLubPermanentOperation()) {
		if (VarControl.GetControl_SpecialFunc1()) VarControl.SetControl_SpecialFunc1Off();
		else VarControl.SetControl_SpecialFunc1On();
	} else
		VarControl.SetControl_SpecialFunc1On();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::SpecialFuncButtonMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	if (!VarControl.GetParam_LineLubPermanentOperation()) VarControl.SetControl_SpecialFunc1Off();
}
//---------------------------------------------------------------------------
// Fokus auf Eingabefeld setzen
void __fastcall TMainForm::NumEditEntered(TObject *Sender)
{
	VirtKeypad1->Show();
}
//---------------------------------------------------------------------------
// Ohne Werteübernahme verlassen
void __fastcall TMainForm::NumEditCanceled(TObject *Sender)
{
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// Mindestteufe
// Inhalt Prüfen und übernehmen
void __fastcall TMainForm::PMinDepthEditAssumedInput(TObject *Sender)
{
	int HV;

	// Mindestteufe OK ?
	if (MakeDepthValue(PMinDepthEdit->Text, &HV) &&
		VarControl.CheckParam_MinDepth(HV, 0)) {
		VarControl.SetParam_MinDepth(HV);// OK -> Wert übernehmen
		VarControl.SavableParamChanged = true;

	} else {
		ShowMinDepth(); // Alten Wert wieder anzeigen, Wert nicht übernommen
		PMinDepthEdit->Highlighted = true;
		return;
	};

	// Ggf. wird Maximalteufe angepasst
	if (HV > VarControl.GetParam_MaxDepth()) {
		VarControl.SetParam_MaxDepth(HV);
		ShowMaxDepth();
		PMaxDepthEdit->Highlighted = true;
	}

	// erfolgreich übernommen
	PMinDepthEdit->Highlighted = false;
	VirtKeypad1->Hide();
}

//---------------------------------------------------------------------------
// Maximalteufe
// Inhalt Prüfen und übernehmen
void __fastcall TMainForm::PMaxDepthEditAssumedInput(TObject *Sender)
{
	int HV;

	// Mindestteufe OK ?
	if (MakeDepthValue(PMaxDepthEdit->Text, &HV) &&
		VarControl.CheckParam_MaxDepth(HV, 0)) {
		VarControl.SetParam_MaxDepth(HV);// OK -> Wert übernehmen
		VarControl.SavableParamChanged = true;
	} else {
		ShowMaxDepth(); // Alten Wert wieder anzeigen, Wert nicht übernommen
		PMaxDepthEdit->Highlighted = true;
		return;
	};

	// Ggf. wird Mindestteufe angepasst
	if (HV < VarControl.GetParam_MinDepth()) {
		VarControl.SetParam_MinDepth(HV);
		ShowMinDepth();
		PMinDepthEdit->Highlighted = true;
	}

	// erfolgreich übernommen
	PMaxDepthEdit->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// Soll-Geschwindigkeit Automatikfahrt
// Inhalt Prüfen und übernehmen
void __fastcall TMainForm::PVelocityEditAssumedInput(TObject *Sender)
{
	int HV;
	if(MakeVelocityValue(PVelocityEdit->Text, &HV) &&
	   VarControl.CheckParam_Velocity(HV, 0)) {
		VarControl.SetParam_Velocity(HV); // OK -> Wert übernehmen
		VarControl.SavableParamChanged = true;
	} else {
		ShowVelocity(this); // Alten Wert wieder anzeigen, Wert nicht übernommen
		PVelocityEdit->Highlighted = true;
		return;
	}

	PVelocityEdit->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// Durch WinchCtrl gesteuerte Bremse
// Inhalt übernehmen
void __fastcall TMainForm::PWinchBrakeInternalAutoCheckBoxClick(TObject *Sender)
{
	if (PWinchBrakeInternalAutoCheckBox->Checked) VarControl.SetParam_WinchBrakeInternAutoOn();
	else VarControl.SetParam_WinchBrakeInternAutoOff();
	VarControl.SavableParamChanged = true;
}
//---------------------------------------------------------------------------
// Antriebsmotor-Drehzahl
// Inhalt übernehmen und Anzeige aktualisieren
void __fastcall TMainForm::PEngineRevolutionChangeableCheckBoxClick(TObject *Sender)
{
	bool HV = VarControl.GetParam_EngineRevolutionChangeable();

	if (PEngineRevolutionChangeableCheckBox->Checked) VarControl.SetParam_EngineRevolutionChangeableOn();
	else VarControl.SetParam_EngineRevolutionChangeableOff();

	// Nur wenn Änderhungen vorhanden
	if (HV != VarControl.GetParam_EngineRevolutionChangeable()) VarControl.SavableParamChanged = true;

	MakeMainCtlPanelLayout(); // Wegen der Aktivierung und Deaktivierung der Drehzahl-Tasten erforderlich

}
//---------------------------------------------------------------------------
// Antriebsmotor-Start/Stop
// Inhalt übernehmen und Anzeige aktualisieren
void __fastcall TMainForm::PEngineStartStopEnableCheckBoxClick(TObject *Sender)
{
	bool HV = VarControl.GetParam_EngineStartStopEnable();

	if (PEngineStartStopEnableCheckBox->Checked) VarControl.SetParam_EngineStartStopEnable();
	else VarControl.SetParam_EngineStartStopDisable();

	// Nur wenn Änderhungen vorhanden
	if (HV != VarControl.GetParam_EngineStartStopEnable()) VarControl.SavableParamChanged = true;

	MakeMainCtlPanelLayout(); // Wegen der Aktivierung und Deaktivierung der Start/Stop-Tasten erforderlich
}
//---------------------------------------------------------------------------
// Freilauf verfügbar
// Inhalt übernehmen und Anzeige aktualisieren
void __fastcall TMainForm::PLostMotionEnableCheckBoxClick(TObject *Sender)
{
	bool HV = VarControl.GetParam_LostMotionEnable();

	if (PLostMotionEnableCheckBox->Checked) VarControl.SetParam_LostMotionEnable();
	else VarControl.SetParam_LostMotionDisable();

	// Nur wenn Änderhungen vorhanden
	if (HV != VarControl.GetParam_LostMotionEnable()) VarControl.SavableParamChanged = true;

	MakeMainCtlPanelLayout(); // Wegen der Aktivierung und Deaktivierung der Freilauf-Taste erforderlich
}
//---------------------------------------------------------------------------
// Manuelle Bremse verfügbar
// Inhalt übernehmen und Anzeige aktualisieren
void __fastcall TMainForm::PBrakeOffEnableCheckBoxClick(TObject *Sender)
{
	bool HV = VarControl.GetParam_BrakeOffEnable();

	if (PBrakeOffEnableCheckBox->Checked) VarControl.SetParam_BrakeOffEnable();
	else VarControl.SetParam_BrakeOffDisable();

	// Nur wenn Änderhungen vorhanden
	if (HV != VarControl.GetParam_BrakeOffEnable()) VarControl.SavableParamChanged = true;

	MakeMainCtlPanelLayout(); // Wegen der Aktivierung und Deaktivierung der Bremsen-Taste erforderlich
}
//---------------------------------------------------------------------------
// Maximalzugkraft, absolut
// Inhalt Prüfen und übernehmen
void __fastcall TMainForm::PMaxTensionEditAssumedInput(TObject *Sender)
{
	int HV;

	if(MakeTractionValue(PMaxTensionEdit->Text, &HV) &&
	VarControl.CheckParam_MaxTraction(HV, 0)) {
		VarControl.SetParam_MaxTraction(HV); // OK -> Wert übernehmen
		VarControl.SavableParamChanged = true;
	} else {
		ShowMaxTraction(); // Alten Wert wieder anzeigen, Wert nicht übernommen
		PMaxTensionEdit->Highlighted = true;
		return;
	};

	PMaxTensionEdit->Highlighted = false;
    VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// Zugkraftdifferenz (bei Abwärtsfahrt)
// Inhalt Prüfen und übernehmen
void __fastcall TMainForm::PRelTensionLimitLowerEditAssumedInput(TObject *Sender)
{
	int HV;

	if(MakeTractionValue(PRelTensionLimitLowerEdit->Text, &HV) &&
		VarControl.CheckParam_RelTensionLimitLower(HV, 0)) {
		VarControl.SetParam_RelTensionLimitLower(HV); // OK -> Wert übernehmen
		VarControl.SavableParamChanged = true;
	} else
	{
		ShowTractionDifference(); // Alten Wert wieder anzeigen, Wert nicht übernommen
		PRelTensionLimitLowerEdit->Highlighted = true;
		return;
	};

	PRelTensionLimitLowerEdit->Highlighted = false;
	VirtKeypad1->Hide();

}

//---------------------------------------------------------------------------
// Dynamische Maximal-Zugkraft  (bei Aufwärtsfahrt)
// Inhalt Prüfen und übernehmen
void __fastcall TMainForm::PRelTensionLimitUpperEditAssumedInput(TObject *Sender)
{
	int HV;

	// Maximal-Zugkraft dynamisch ermitteln
	if(MakeTractionValue(PRelTensionLimitUpperEdit->Text, &HV) &&
	   VarControl.CheckParam_RelTensionLimitUpper(HV, 0)) {
		VarControl.SetParam_RelTensionLimitUpper(HV);  // OK -> Wert übernehmen
		VarControl.SavableParamChanged = true;
	} else {
		ShowCalcCurrTractionDifference(); // Alten Wert wieder anzeigen, Wert nicht übernommen
		PRelTensionLimitUpperEdit->Highlighted = true;
		return;
	}

	PRelTensionLimitUpperEdit->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// Kalibrierwert Zugkraft - Anstieg X-Anteil
// Inhalt übernehmen
void __fastcall TMainForm::PCalibTensionXEditAssumedInput(TObject *Sender)
{
	int HV;
	HV = StrToInt(PCalibTensionXEdit->Text);
	if (HV != 0) {
		VarControl.SetParam_CalibTractionX(HV);
    	VarControl.SavableParamChanged = true;
		ShowCalValTractionInfo();
	} else {
		ShowCalValTraction();
		PCalibTensionXEdit->Highlighted = true;
		return;
	}

	PCalibTensionXEdit->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// Kalibrierwert Zugkraft - Anstieg Y-Anteil
// Inhalt übernehmen
void __fastcall TMainForm::PCalibTensionYEditAssumedInput(TObject *Sender)
{
	int HV;
	HV = StrToInt(PCalibTensionYEdit->Text);
	if (HV != 0) {
		VarControl.SetParam_CalibTractionY(HV);
		VarControl.SavableParamChanged = true;
		ShowCalValTractionInfo();
	} else {
    	ShowCalValTraction();
		PCalibTensionYEdit->Highlighted = true;
		return;
    }

	PCalibTensionYEdit->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// Kalibrierwert Zugkraft - Offset
// Inhalt übernehmen
void __fastcall TMainForm::PCalibTensionZeroEditAssumedInput(TObject *Sender)
{
	VarControl.SetParam_CalibTractionZero(StrToInt(PCalibTensionZeroEdit->Text));
	VarControl.SavableParamChanged = true;

	PCalibTensionZeroEdit->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// Windenauswahl
void __fastcall TMainForm::PWinchCfgListBoxClick(TObject *Sender)
{
	int HV = VarControl.GetParam_WinchNo(); // Windennummer

	if (HV != VarControl.SetParam_WinchNo(PWinchCfgListBox->ItemIndex))
	{
		// Einige Windenabhängige Parameter bei Änderung der Winde
		// erfordern spezielle Hardwarezugriffe
		// Teufenrechner
		VarControl.SetControl_SetDepthCalFactor_On();
		VarControl.SetControl_SetDepthCalFactor_DC_On();
		VarControl.SetControl_SetDepthCalcParamOn();

		VarControl.SavableParamChanged = true;

        VarControl.SetInfo_CalibDepthAllowedOff();
	};

	GenOrgan.NewLOG = true;
	ActualizeParameterView();
}

//---------------------------------------------------------------------------
// Teufe setzen
void __fastcall TMainForm::PSetDepthEditAssumedInput(TObject *Sender)
{
	int HV;
	TAdvLabeledEdit *SenderPtr;

	// Verantwortliches Steuerelement auswählen
	if (Sender == CalibDepth_DepthEdit) SenderPtr = CalibDepth_DepthEdit;
	else if (Sender == PSetDepthEdit) SenderPtr = PSetDepthEdit;
	else {  // Keiner der erwarteten Sender
			VirtKeypad1->Hide();
			return;
	}

	if(MakeDepthValue(SenderPtr->Text, &HV)) {
		VarControl.CheckParam_StartDepth(HV, &HV);		// ggf. Teufenwert in erlaubten Bereich einpassen
		VarControl.SetParam_StartDepth(HV);				// OK -> Wert übernehmen
		VarControl.SetControl_SetDepth_DC_On();
		VarControl.SetControl_SetDepth_On();

		// Bei Bedarf Teufenzwangssynchronisation aktivieren/deaktivieren
		HV /= 1000;
		if ((VarControl.GetParam_MMSyncForceOn() == _ON) &&
			(HV < VarControl.GetParam_MMSyncForceRangeLim())) {
			HV /= 10; // Auf 10m genau abrunden
			HV *= 10;
			VarControl.SetControl_MMSyncForceCounter(HV);
			VarControl.SetControl_MMSyncForceActiveOn();
		} else VarControl.SetControl_MMSyncForceActiveOff();

	}else {
		ShowSetDepth();                                 // Aktuelle Teufe anzeigen, Wert nicht übernommen
		VarControl.SetControl_SetDepth_DC_Off();
		VarControl.SetControl_SetDepth_Off();


		SenderPtr->Highlighted = true;
		return;
	}

	// Anweisung an ControlThread, die Daten an Teufenrechner zu senden
	VarControl.SetControl_SetDepthCalcParamOn();

	SenderPtr->Highlighted = false;
	VirtKeypad1->Hide();

}
//---------------------------------------------------------------------------
// Kalibrierfaktor
void __fastcall TMainForm::PDepthCalFactorEditAssumedInput(TObject *Sender)
{
	// ermittelter Kalibrierfaktor
	int HV = (int)(StrToFloat(PDepthCalFactorEdit->Text) * pow10(DEPTHCALC_CALFACT_DIGITS) + 0.5);

	// Prüfen und Korrigieren
	if ((HV < DEPTHCALC_CALFACT_MIN) ||
		(HV > DEPTHCALC_CALFACT_MAX)) {
		ShowCalFactorDepth();                                   // Alten Wert wieder anzeigen, Wert nicht übernommen
		VarControl.SetControl_SetDepthCalFactor_DC_Off();
		VarControl.SetControl_SetDepthCalFactor_Off();
		PDepthCalFactorEdit->Highlighted = true;
		return;

	} else {

		// Setzen aktivieren
		VarControl.SetParam_CalFactDepth(HV);					// OK -> Wert übernehmen
		VarControl.SetControl_SetDepthCalFactor_On();
		VarControl.SetControl_SetDepthCalFactor_DC_On();
		VarControl.SavableParamChanged = true;

		GenOrgan.NewLOG = true;									// Neue Protokolldatei beginnen, damit Parameter im Header übernommen werden
	}

	// Anweisung an ControlThread, die Daten an Teufenrechner zu senden
	VarControl.SetControl_SetDepthCalcParamOn();

	PDepthCalFactorEdit->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// MM-Zwangssynchronisation
void __fastcall TMainForm::PMMSyncForceRangeLimEditAssumedInput(TObject *Sender)
{
	int HV = static_cast<int>(StrToFloat(PMMSyncForceRangeLimEdit->Text));
	if (HV > 0) { // Bei Wert > 0 ist die Zwangssynchronisierung aktiv
		VarControl.CheckParam_MMSyncForceRangeLim(HV, &HV);		// In jedem Fall wird ein gültiger Wert übernommen
		VarControl.SetParam_MMSyncForceRangeLim(HV);            // Fehleingabe praktisch nicht möglich
		VarControl.SetParam_MMSyncForceOn();
		VarControl.SavableParamChanged = true;

	} else {
		VarControl.SetParam_MMSyncForceRangeLim(0);				// 0 setzen
        ShowMMSyncForceParam();
		VarControl.SetParam_MMSyncForceOff();
		VarControl.SavableParamChanged = true;
	}

	PMMSyncForceRangeLimEdit->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// IGR-Fehlerüberwachung ein/aus
void __fastcall TMainForm::PIncEncErrorMonitCheckBoxClick(TObject *Sender)
{
	if (PIncEncErrorMonitCheckBox->Checked) VarControl.SetParam_IncEncErrorMonitOn();
	else VarControl.SetParam_IncEncErrorMonitOff();
}
//---------------------------------------------------------------------------
// Steuergerät - Fehlerüberwachung ein/aus
void __fastcall TMainForm::PControllerErrorMonitCheckBoxClick(TObject *Sender)
{
	if (PControllerErrorMonitCheckBox->Checked) VarControl.SetParam_ControllerErrorMonitOn();
	else VarControl.SetParam_ControllerErrorMonitOff();
}
//---------------------------------------------------------------------------
// Steuergerät - Fehler Low-Aktiv
void __fastcall TMainForm::PControllerErrorLactiveCheckBoxClick(TObject *Sender)
{
	bool HV = VarControl.GetParam_ControllerErrorLactive();

	if (PControllerErrorLactiveCheckBox->Checked) VarControl.SetParam_ControllerErrorLactiveOn();
	else VarControl.SetParam_ControllerErrorLactiveOff();

	// Nur wenn Änderhungen vorhanden
	if (HV != VarControl.GetParam_ControllerErrorLactive()) VarControl.SavableParamChanged = true;
}
//---------------------------------------------------------------------------
// Steuergerät - Fehler in langsamer Fahrstufe Low-Aktiv
void __fastcall TMainForm::PControllerErrorAtSMCheckBoxClick(TObject *Sender)
{
	bool HV = VarControl.GetParam_ControllerErrorMonitAtSM();

	if (PControllerErrorAtSMCheckBox->Checked) VarControl.SetParam_ControllerErrorMonitAtSMOn();
	else VarControl.SetParam_ControllerErrorMonitAtSMOff();

	// Nur wenn Änderhungen vorhanden
	if (HV != VarControl.GetParam_ControllerErrorMonitAtSM()) VarControl.SavableParamChanged = true;
}
//---------------------------------------------------------------------------
// Sondersignal- Art der Bewertung
void __fastcall TMainForm::PSpecialSignalModeRadioButtonClick(TObject *Sender)
{
	int HV = VarControl.GetParam_SpecialSignalMode();

	if (VarControl.devel()) {
		if (PSpecialSignalMode0RadioButton->Checked) VarControl.SetParam_SpecialSignalMode(0);  // keine Überwachung
		else if (PSpecialSignalMode1RadioButton->Checked) VarControl.SetParam_SpecialSignalMode(1);  // 2-Sensor-Schalthebelüberwachung
		else if (PSpecialSignalMode2RadioButton->Checked) VarControl.SetParam_SpecialSignalMode(2); // Überwachung zweier Fehlersignale
		else if (PSpecialSignalMode3RadioButton->Checked) VarControl.SetParam_SpecialSignalMode(3); // Überwachung eines zusätzlichen Ölfilters
		else if (PSpecialSignalMode4RadioButton->Checked) VarControl.SetParam_SpecialSignalMode(4); // Überwachung der Mastverrieglung (2 Pkt.)
		else if (PSpecialSignalMode5RadioButton->Checked) VarControl.SetParam_SpecialSignalMode(5); // Überwachung einer externen Bremse

		// Nur wenn Änderhungen vorhanden
		if (HV != VarControl.GetParam_SpecialSignalMode()) VarControl.SavableParamChanged = true;
	};
}
//---------------------------------------------------------------------------
// Maßeinheit Zugkraftanzeige
void __fastcall TMainForm::PTractionViewComboBoxChange(TObject *Sender)
{
	VarControl.SetParam_TractionViewDimensionUnit(PTractionViewComboBox->ItemIndex);
	VarControl.SavableParamChanged = true;
	SetLabelling();
	ActualizeAllInformations();
	MakeLayout();

}
//---------------------------------------------------------------------------
// Maßeinheit der Geschwindigkeit
void __fastcall TMainForm::PVelocityViewComboBoxChange(TObject *Sender)
{
	VarControl.SetParam_VelocityDimensionUnit(PVelocityViewComboBox->ItemIndex);
	VarControl.SavableParamChanged = true;
	SetLabelling();
	ActualizeAllInformations();
	MakeLayout();
}
//---------------------------------------------------------------------------
// Teufe umkehren
void __fastcall TMainForm::PReverseDepthCheckBoxClick(TObject *Sender)
{
	if (PReverseDepthCheckBox->Checked) VarControl.SetParam_ReverseDepth(_ON);
	else VarControl.SetParam_ReverseDepth(_OFF);
	VarControl.SavableParamChanged = true;
}

//---------------------------------------------------------------------------
// Sollzugkraft bei aktivem Kabel Entladen
void __fastcall TMainForm::UnloadCableSetTensionEditAssumedInput(TObject *Sender)
{
	int HV;
	if (MakeTractionValue(UnloadCableSetTensionEdit->Text, &HV) &&
		VarControl.CheckParam_UnloadCableTraction(HV, 0)){
		VarControl.SetParam_UnloadCableTraction(HV); // OK -> Wert übernehmen
		VarControl.SavableParamChanged = true;
	} else {
		ShowUnloadCableParameters();					// Alle zuvor eingestellten Werte wieder anzeigen
		UnloadCableSetTensionEdit->Highlighted = true;
		return;
	};

	UnloadCableSetTensionEdit->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// Stretching aktiv, bei aktivem Kabel Entladen
void __fastcall TMainForm::UnloadCabelStretchCheckBoxClick(TObject *Sender)
{
	bool HV = VarControl.GetParam_UnloadCableStretch();

	if (UnloadCabelStretchCheckBox->Checked) VarControl.SetParam_UnloadCableStretchOn();
	else VarControl.SetParam_UnloadCableStretchOff();

	// Nur wenn Änderungen vorhanden
	if (HV != VarControl.GetParam_UnloadCableStretch()) VarControl.SavableParamChanged = true;
}
//---------------------------------------------------------------------------
// Startlänge für Stretching bei aktivem Kabel Entladen
void __fastcall TMainForm::UnloadCableStretchStartLenghtEditAssumedInput(TObject *Sender)
{
	int HV;
	if (MakeDepthValue(UnloadCableStretchStartLenghtEdit->Text, &HV) &&
		VarControl.CheckParam_UnloadCableBeginLenght(HV, 0)) {
		VarControl.SetParam_UnloadCableBeginLenght(HV); // OK -> Wert übernehmen
		VarControl.SavableParamChanged = true;
	} else {
		ShowUnloadCableParameters();					// Alle zuvor eingestellten Werte wieder anzeigen
		UnloadCableStretchStartLenghtEdit->Highlighted = true;
		return;
	};

	UnloadCableStretchStartLenghtEdit->Highlighted = false;
	VirtKeypad1->Hide();
}

//---------------------------------------------------------------------------
// Endlänge für Stretching bei aktivem Kabel Entladen
void __fastcall TMainForm::UnloadCableStretchEndLenghtEditAssumedInput(TObject *Sender)
{
	int HV;
	if (MakeDepthValue(UnloadCableStretchEndLenghtEdit->Text, &HV) &&
		VarControl.CheckParam_UnloadCableEndLenght(HV, 0)) {
		VarControl.SetParam_UnloadCableEndLenght(HV); // OK -> Wert übernehmen
		VarControl.SavableParamChanged = true;
	} else {
		ShowUnloadCableParameters();					// Alle zuvor eingestellten Werte wieder anzeigen
		UnloadCableStretchEndLenghtEdit->Highlighted = true;
		return;
	};

	UnloadCableStretchEndLenghtEdit->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// Startzugkraft für Stretching bei aktivem Kabel Entladen
void __fastcall TMainForm::UnloadCableStretchStartTensionEditAssumedInput(TObject *Sender)
{
	int HV;
	if (MakeTractionValue(UnloadCableStretchStartTensionEdit->Text, &HV) &&
		VarControl.CheckParam_UnloadCableStartTraction(HV, 0)) {
		VarControl.SetParam_UnloadCableStartTraction(HV); // OK -> Wert übernehmen
		VarControl.SavableParamChanged = true;
	} else {
		ShowUnloadCableParameters();					// Alle zuvor eingestellten Werte wieder anzeigen
		UnloadCableStretchStartTensionEdit->Highlighted = true;
		return;
	};

	UnloadCableStretchStartTensionEdit->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// Endzugkraft für Stretching bei aktivem Kabel Entladen
void __fastcall TMainForm::UnloadCableStretchEndTensionEditAssumedInput(TObject *Sender)
{
	int HV;
	if (MakeTractionValue(UnloadCableStretchEndTensionEdit->Text, &HV) &&
		VarControl.CheckParam_UnloadCableEndTraction(HV, 0)) {
		VarControl.SetParam_UnloadCableEndTraction(HV); // OK -> Wert übernehmen
		VarControl.SavableParamChanged = true;
	} else {
		ShowUnloadCableParameters();					// Alle zuvor eingestellten Werte wieder anzeigen
		UnloadCableStretchEndTensionEdit->Highlighted = true;
		return;
	};

	UnloadCableStretchEndTensionEdit->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------
// Daten in die Parameter-Datei übernehmen
void __fastcall TMainForm::ParameterInputExit(TObject *Sender)
{
	if (!VarControl.SavableParamChanged) return;

	// ----------------
	// Daten in die Parameter-Datei übernehmen
	if (VarControl.WriteData() == _BAD) {
		GenOrgan.Evnt << 12;											// Fehler 12 ein
	}else {
		GenOrgan.Evnt >> 11;											// Fehler 11 aus
		GenOrgan.Evnt >> 12;											// Fehler 12 aus
	};

}
//---------------------------------------------------------------------------
// Kalibrierung: Teufe eingegeben
// Gleichzeitig wird neue Teufe sofort gesetzt
void __fastcall TMainForm::CalibDepth_DepthEditAssumedInput(TObject *Sender)
{
	int HV_EnteredDepth;
	float HVf_RangeOld, HVf_RangeNew, HVf_EarmarkedCalFac;

	// Kalibrierwert ermitteln ?
	if (	VarControl.GetInfo_CalibDepthAllowed()
		 &&	VarControl.GetInfo_CalibDepthCableOk()) {

		if(MakeDepthValue(CalibDepth_DepthEdit->Text, &HV_EnteredDepth))

			// Teufenwert prüfen
			if (VarControl.CheckParam_StartDepth(HV_EnteredDepth, 0)) {

				// Berechnen des neuen Kalibrierwerts
				HVf_RangeOld = abs(VarControl.GetInfo_CurrDepth() - VarControl.GetParam_StartDepth()); // Seit letztem Setzen der Teufe zurückgelegte Strecke mit bisherigem CalFac
				HVf_RangeNew = abs(HV_EnteredDepth - VarControl.GetParam_StartDepth());                   // Seit letztem Setzen der Teufe zurückgelegte Strecke lt. Angabe
				HVf_EarmarkedCalFac = HVf_RangeNew / HVf_RangeOld * VarControl.GetParam_CalFactDepth(); // neuer Kalibrierwert

				if ((HVf_EarmarkedCalFac < DEPTHCALC_CALFACT_MIN) ||
					(HVf_EarmarkedCalFac > DEPTHCALC_CALFACT_MAX)) {

					// Ungültige Eingabe
					VarControl.SetParam_CalFactDepth_Earmarked(0);
					CalibDepth_DepthEdit->Highlighted = true;
					return;
				}

				// Neuen Kalibrierwert vormerken
				VarControl.SetParam_CalFactDepth_Earmarked(static_cast<int>(HVf_EarmarkedCalFac));

			} else {
				// Ungültige Eingabe
				CalibDepth_DepthEdit->Highlighted = true;
				return;
			}
	}

	// Teufe setzen
	// Hervorhebung und Keypad werden durch "PSetDepthEditAssumedInput" gesteuert
	PSetDepthEditAssumedInput(Sender);
}
//---------------------------------------------------------------------------
// Teufenfeld im Kalibrierbildschirm betreten
void __fastcall TMainForm::CalibDepth_RealDepthEditEnterd(TObject *Sender)
{
	// Beim "Betreten" des Editfelds zum Eingeben der Teufe muss neben den
	// üblichen Maßnahmen (Keypad usw.) ebenfalls der Vorgemerkte Wert gelöscht werden.
	// Andernfalls gibt es für den Bediener unübersichtlichkeiten.
	// Die "Übernehmen"-Taste wäre andernfalls noch Bedienbar, so dass
	// irrtümlich diese gedrückt werden könnte.
	VarControl.SetParam_CalFactDepth_Earmarked(0);
	NumEditEntered(Sender);
}
//---------------------------------------------------------------------------
// Kalibrierung: Zugkraft eingegeben
void __fastcall TMainForm::CalibTension_TensionEditAssumedInput(TObject *Sender)
{
	TAdvLabeledEdit *EdtField;
	int Idx;
	int TmpTension;

	// Sender-Objekt erfassen
	if (Sender == CalibTension_Tension1Edit) {
		EdtField = CalibTension_Tension1Edit;
		Idx = 0;
	} else if (Sender == CalibTension_Tension2Edit) {
		EdtField = CalibTension_Tension2Edit;
		Idx = 1;
	} else return;

	// Zugkraftwert ermitteln und Wert prüfen
	if (	!MakeTractionValue(EdtField->Text, &TmpTension)
			|| (VarControl.SetParam_CalibTension_Tension(Idx, TmpTension) != TmpTension)) {
		// bei ungültigem Wert
		EdtField->Highlighted = true;
		return;
	}

	// i.O. -> abschließen
	EdtField->Highlighted = false;
	VirtKeypad1->Hide();
}
//---------------------------------------------------------------------------

// Hydraulische Bremse aktiv
// Inhalt übernehmen und Anzeige aktualisieren
void __fastcall TMainForm::PHydraulikBrakeEnableCheckBoxClick(TObject *Sender)
{
	bool HV = VarControl.GetParam_HydraulicBrake();

	if (PHydraulikBrakeEnableCheckBox->Checked) VarControl.SetParam_HydraulicBrakeOn();
	else VarControl.SetParam_HydraulicBrakeOff();

	// Nur wenn Änderhungen vorhanden
//	ToDo: Nachfolgendes aktivieren, wenn Abspeichern implementiert
//	if (HV != VarControl.GetParam_HydraulicBrake()) VarControl.SavableParamChanged = true;

	MakeMainCtlPanelLayout(); // Wegen der Aktivierung und Deaktivierung der Bremsen-Taste erforderlich

}

//---------------------------------------------------------------------------
// Verlassen des Eingabefelds für das Datum
void __fastcall TMainForm::SystemDateMaskEditExit(TObject *Sender)
{
	VirtKeypad1->Hide();
}

//---------------------------------------------------------------------------
// Verlassen des Eingabefelds für die Zeit
void __fastcall TMainForm::SystemTimeMaskEditExit(TObject *Sender)
{
	VirtKeypad1->Hide();
}

//---------------------------------------------------------------------------
// Seil-/Kabelöler verfügbar
void __fastcall TMainForm::PLineLubAvailableCheckBoxClick(TObject *Sender)
{
	if (PLineLubAvailableCheckBox->Checked) VarControl.SetParam_LineLubAvailable(_ON);
	else VarControl.SetParam_LineLubAvailable(_OFF);
	VarControl.SavableParamChanged = true;

	MakeControlLampPanelLayout();
}
//---------------------------------------------------------------------------
// Seil-/Kabelöler Dauerbetrieb
void __fastcall TMainForm::PLineLubPermanentOperationCheckBoxClick(TObject *Sender)
{
	if (PLineLubPermanentOperationCheckBox->Checked) VarControl.SetParam_LineLubPermanentOperation(_ON);
	else VarControl.SetParam_LineLubPermanentOperation(_OFF);
	VarControl.SavableParamChanged = true;
}
//---------------------------------------------------------------------------
// Seil-/Kabelöler Intensität
void __fastcall TMainForm::PLineLubIntensitySliderChange(TObject *Sender)
{
	int HV;
	VarControl.CheckParam_LineLubIntensity(PLineLubIntensitySlider->Position, &HV);
	if (HV != VarControl.GetParam_LineLubIntensity()) {
		VarControl.SetParam_LineLubIntensity(HV);
        VarControl.SavableParamChanged = true;
	}
}
//---------------------------------------------------------------------------



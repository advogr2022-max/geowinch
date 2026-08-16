/*
WinchControl    Jörg Müller    constants.h

Enthält Programmkonstanten

Portierbarkeit: teilweise Systemspezifische Anpassungen notwendig
*/

// Systemspezifische Angaben (sind vor Portierung zu kontrollieren) sind mit
// "SYS" gekennzeichnet

#ifndef __CONSTANTS_H
#define __CONSTANTS_H

// -----------------------------------------------------------------------------
// Rechnerspezifische Einstellung forcieren
//#define __PC1            // Compilierung für "Messrechner2" verwenden
// Entwicklungsspezifische Einstellungen
#define __DEF_IGNORE_EXT_CMIMPULSE_FORMAT_SET_RESPONSE  // Ignoriert auftretende Fehler beim Senden des externen cm-Impuls-Formates an den Teufenrechner
#define __USE_THREAD_CLOCK                              // Nicht durch TTimer den internen Takt ermitteln, sondern durch den ControlThread (siehe WCAnalysis)
//#define __IGNORE_INCENCERROR                            // Inkrementalfehler-Signalisierung nicht abfragen
//#define __CURRTRACTIONVIEW_DECIMAL_ACCURACY             // Anzeige der Momentanen Zugkraft auf 10er Stelle (vor dem Komma genau) anzeigen (1er sind dann immer 0)
#define __TRACTION_NOISE_DAMP                           // mit Dämpfung der ZugkraftERFASSUNG, so dass Rauschen im Zugkraftsignal gedämpft wird (Nachteil: schnelle reguläre Zugkraftänderung werden nicht erkannt)
//#define __TRACTION_VIEW_NOISE_DAMP                      // mit Dämpfung der Zugkraftanzeige, so dass Rauschen in der Anzeige gedämpft wird
//#define __NO_ENGINE_REVOLUTION_BUTTONS                  // keine Motordrehzahl-Einstelltasten anbieten
//#define __USE_LAS2_LOG                                  // Protokolldateien im LAS 2.0 - Format
//#define __BLOCK_WINCH_CHOISE                            // Blockieren der Windenauswahl (linke/rechte Winde)
#define __USE_GET_DEPTH_EXTENDED                        // Erweiterte Teufenerfassung verwenden (mit MM-Zustand und Auslesen der Zeit zwischen Teufenabfragen)
#define __WRITE_CRC_VALIDATION                          // Schreibt die Validitätsinformationen

// sonstige Einstellungen
#define __SHOW_SPLASHSCREEN                             // Splash-Screen anzeigen
//#define __DEMO_MODE										// Demo-Modus aktivieren
#define __ENABLE_PROGRAMAUTOMATIC						// Programmauomatik verfügbar
// Hardware-Einstellungen
//#define __ENABLE_ADV_PCI1762							// Aktiviert die Verwendung der LP Advantech PCI-1762
// -----------------------------------------------------------------------------
// Programminfos
const char *__PROGNAME = "WinchControl";                // Programmbezeichnung

const char *__VERSION 		= "5.0.0ru";               	// Programmversion
const char __VERSION_MAIN 	= 5;					    // Hauptversion
const char __VERSION_MID 	= 0;						// Nebenversion
const char __VERSION_REV 	= 0;						// Revisionsnummer

const char *__DATEOFCOMPIL = "2018-04-09";       		// Datum der Compilierung
const char *__NUMBERPERDATE = "0";                      // fortlaufende Nummer der Compilierung am jew. Datum

// -----------------------------------------------------------------------------
// Grunddefinitionen
// folgende Zustands-Konstanten nie ändern !!!
const bool _UPward = true;                              // Definition hoch (true)
const bool _DOWN = false;                               // Definition runter (false)
const bool _YES = true;                                 // Definition ja (true)
const bool _NO = false;                                 // Definition nein (false)
const bool _ON = true;                                  // Definition an (true)
const bool _OFF = false;                                // Definition aus (false)
const bool _GOOD = true;                                // Definition gut (true)
const bool _BAD = false;                                // Definition schlecht (false)
const bool _HIGH = true;                                // Definition für HIGH (true)
const bool _LOW = false;                                // Definition für Low (false)
const short _IGNORE = 0;                                // Definition für Ignorieren
const short _SHOW = 1;                                  // Definition für Anzeigen
const short _HIDE = -1;                                 // Definition für Ausblenden

const _KARAT    = 1;                                    // Definition für cm-Impuls "KARAT"
const _ONYX     = 2;                                    // Definition für cm-Impuls "ONYX"
const _KEDR     = 3;                                    // Definition für cm-Impuls "KEDR"
const _KARAT2   = 4;                                    // Definition für cm-Impuls "KARAT-2"
const _KARAT2N  = 5;                                    // Definition für cm-Impuls "KARAT-2 negiert"
const _KEDRN    = 6;                                    // Definition für cm-Impuls "KEDR negiert"

const char *CONST__SEPARATOR01 = " ";                   // Trennzeichen: Leerzeichen
const char *CONST__BRACKET01LEFT = "[";                 // Klammer links "eckig"
const char *CONST__BRACKET01RIGHT = "]";                // Klammer rechts "eckig"

// SteuerStati (ControlState...CS)
const   CS_START        = 1;                            // Starten eines Zustandes
const   CS_STOP         = 2;                            // Anhalten eines Zustandes
const   CS_RESTART      = 3;                            // Entspricht nacheinander Anhalten und Starten
const   CS_ABORT        = 4;                            // Abbrechen eines Zustandes
const   CS_SPECIAL0     = 5;                            // Spezial-Signalisierungen
const   CS_SPECIAL1     = 6;                            // Spezial-Signalisierungen
const   CS_SPECIAL2     = 7;                            // Spezial-Signalisierungen
const   CS_SPECIAL3     = 8;                            // Spezial-Signalisierungen
const   CS_SPECIAL4     = 9;                            // Spezial-Signalisierungen
const   CS_SPECIAL5     = 10;                           // Spezial-Signalisierungen
const   CS_SPECIAL6     = 11;                           // Spezial-Signalisierungen
const   CS_SPECIAL7     = 12;                           // Spezial-Signalisierungen
const   CS_SPECIAL8     = 13;                           // Spezial-Signalisierungen
const   CS_SPECIAL9     = 14;                           // Spezial-Signalisierungen

// -----------------------------------------------------------------------------
// SplashScreen
const SplashScreenDelay = 2;                            // Anzeigedauer des Splash-Screens in Sekunden
// -----------------------------------------------------------------------------
// Datums-/Zeit-Konvertierungen
const char      *FORMAT_Date_String     = "dd.mm.yyyy";         // Formatierung des Datums zur Ausgabe
const char      *FORMAT_Date_Mask       = "!90\.90\.0000;1; ";  // Formatierung des Datums als Eingabemaske
const char      *FORMAT_Time_String     = "hh:nn:ss";           // Formatierung der Zeit zur Ausgabe
const char      *FORMAT_Time_Mask       = "!90\:90\:00;1; ";    // Formatierung des Datums als Eingabemaske

// Natur-/mathematische Konstanten
const float     __GFORCE                = 9.80665;              // Gravitationskraft der Erde in m/s²

// -----------------------------------------------------------------------------
// Interne Taktung
const CLOCK_Base = 100;                                                         // kleinste Takt-Dauer in ms (MUSS größer als die Dauer einer Wiederholung des ermittelnden Threads sein)

// -----------------------------------------------------------------------------
// Panel-Nummern
// Jedes FunktionsPanel bekommt eine eindeutige Nummer zugewiesen, anhand der
// festgestellt wird, bzw. mit deren Hilfe angegeben wird, welches Panel zur
// Zeit angezeigt werden soll
const char INR_ChoicePanel              = 0x01;
const char INR_CalibDepthPanel          = 0x02;
const char INR_CalibTractionPanel       = 0x03;
const char INR_DialogPanel              = 0x04;
const char INR_UnloadCablePanel      	= 0x05;
const char INR_FileAdminPanel           = 0x06;
const char INR_InfoPanel                = 0x07;
const char INR_NormalViewPanel          = 0x08;
const char INR_ParameterPanel           = 0x09;
const char INR_SystemPanel              = 0x0A;
const char INR_MainMenuPanel			= 0x0B;
const char INR_MessagePanel				= 0x0C;
// -----------------------------------------------------------------------------
// Programmzweck-gebundene generelle Einstellungen
const _WinchCnt = 10;                                    		// Anzahl der vom Programm steuerbaren Winden

// -----------------------------------------------------------------------------
// Dateiarbeit
const char *__ParameterFile             = "WinchCtrl.par";          // Datei für die Parameterdaten
const char *__InternalParameterFile     = "WinchCtrl.int";          // Datei für die speziellen internen Parameterdaten (Entwicklung)
//const char *__ParameterBackupFileExt    = ".pab";                   // Dateierweiterung der Parameter-Backupdatei
const char *__ParameterBackupFile       = "WinchCtrl.pab";          // Parameter-Backupdatei
const char *__ParameterTmpFile          = "WinchCtrl.pa~";          // Temporäre Parameterdatei, bei Schreib-Fehler kann diese zurückgeschrieben werden, bzw. wird sichergestellt, dass Datenträgeroberfläche geschont wird
const char *__AutoRunPrgFileExtention   = ".wpr";                   // Dateierweiterung der Ablaufprogrammdateien für die Programmautomatik
const       StdDriveLength              = 4;                        // Zeichenanzahl für den Laufwerksbuchstaben inkl. Backslash
const char __StdDrive[StdDriveLength]  = "C:\\";                    // Standardlaufwerk für Datenspeicherung

// Kommentare in der Parameterdatei
const char *COMMENT_PF_ENTRY001                 = "CAUTION!!!_THIS_FILE_IS_GENERATED_AUTOMATICALLY.";
const char *COMMENT_PF_ENTRY002                 = "___________PLEASE_DO_NOT_CHANGE_THIS_FILE_MANUALLY.";
const char *COMMENT_PF_ENTRY003                 = "Last_Used_Winch_Number....................   ";
//const char *COMMENT_PF_ENTRY004                 = "Hardware-Control_On/Off................... ";
const char *COMMENT_PF_ENTRY005                 = "Discharge_Cable_Stretching_On/Off.........   ";
const char *COMMENT_PF_ENTRY006                 = "Minimal_Depth.............................   ";
const char *COMMENT_PF_ENTRY007                 = "Maximal_Depth.............................   ";
const char *COMMENT_PF_ENTRY008                 = "Setted_Depth..............................   ";
const char *COMMENT_PF_ENTRY009                 = "Setted_Velocity...........................   ";
const char *COMMENT_PF_ENTRY010                 = "Setted_Direction..........................   ";
//const char *COMMENT_PF_ENTRY011                 = "Calculate_Traction_Dynamically_On/Off.....   ";
const char *COMMENT_PF_ENTRY012                 = "Minimal_Traction(not_used)................ ";
const char *COMMENT_PF_ENTRY013                 = "Maximal_Traction.......................... ";
const char *COMMENT_PF_ENTRY014                 = "Relative_Tension_Difference_Lower......... ";
const char *COMMENT_PF_ENTRY015                 = "Minimal_Traction(Discharging_Cable)....... ";
const char *COMMENT_PF_ENTRY016                 = "Begin_Traction(Discharging_Cable)......... ";
const char *COMMENT_PF_ENTRY017                 = "End_Traction(Discharging_Cable)........... ";
//const char *COMMENT_PF_ENTRY018                 = "Maximal_Time_Difference................... ";
//const char *COMMENT_PF_ENTRY019                 = "Magnetic_Mark_On/Off...................... ";
const char *COMMENT_PF_ENTRY020                 = "Calibration_Factor_For_Depth_Calculator... ";
const char *COMMENT_PF_ENTRY021                 = "Calibration_Factor_X_For_Dynamometer...... ";
const char *COMMENT_PF_ENTRY022                 = "Calibration_Factor_Y_For_Dynamometer...... ";
const char *COMMENT_PF_ENTRY023                 = "Calibration_Zero_For_dynamometer.......... ";
const char *COMMENT_PF_ENTRY024                 = "Step_For_Dynam_Max_Traction_Calculation...   ";
const char *COMMENT_PF_ENTRY025                 = "Begin_Lenght(Discharging_Cable)...........   ";
const char *COMMENT_PF_ENTRY026                 = "End_Lenght(Discharging_Cable).............   ";
const char *COMMENT_PF_ENTRY027                 = "Format_of_external_cm-impulse.............   ";
const char *COMMENT_PF_ENTRY028                 = "Relative_Tension_Difference_Upper......... ";
const char *COMMENT_PF_ENTRY029                 = "Traction_View_Dimension_Unit..............   ";
const char *COMMENT_PF_ENTRY030                 = "LOG_File_Type.............................   ";
// const char *COMMENT_PF_ENTRY031                 = "P_Velocity_Control_Step_normal_[mV].......   ";
// const char *COMMENT_PF_ENTRY032                 = "P_Velocity_Control_Step_small_[mV]........   ";
// const char *COMMENT_PF_ENTRY033                 = "Count_of_velocity_steps_for_stop..........   ";
// const char *COMMENT_PF_ENTRY034                 = "Auto_Velocity_Adjustment_Clock[ms]........   ";
//const char *COMMENT_PF_ENTRY035                 = "Stopped_Winch_Reference_Voltage_[mV]......   ";
//const char *COMMENT_PF_ENTRY036                 = "Stop_Adjustment_Clock(Auto_Stop)_[ms].....   ";
// const char *COMMENT_PF_ENTRY037                 = "PID_Discharge_Cable_TD_slow_[ms]..........   ";
// const char *COMMENT_PF_ENTRY038                 = "PID_Discharge_Cable_TI_slow_[ms]..........   ";
// const char *COMMENT_PF_ENTRY039                 = "PID_Discharge_Cable_KR_slow_[ms]..........   ";
// const char *COMMENT_PF_ENTRY040                 = "PID_Discharge_Cable_TD_fast_[ms]..........   ";
// const char *COMMENT_PF_ENTRY041                 = "PID_Discharge_Cable_TI_fast_[ms]..........   ";
// const char *COMMENT_PF_ENTRY042                 = "PID_Discharge_cable_KR_fast_[ms]..........   ";
const char *COMMENT_PF_ENTRY043                 = "Temperat_Caract_Slope_S_DepthCalc_[°C/V]..   ";
const char *COMMENT_PF_ENTRY044                 = "Temperat_Caract_Zero_S_DepthCalc_[°C].....   ";
const char *COMMENT_PF_ENTRY045                 = "Temperat_Caract_Slope_S_A_[°C/V]..........   ";
const char *COMMENT_PF_ENTRY046                 = "Temperat_Caract_Zero_S_A_[°C].............   ";
const char *COMMENT_PF_ENTRY047                 = "Temperat_Caract_Slope_S_B_[°C/V]..........   ";
const char *COMMENT_PF_ENTRY048                 = "Temperat_Caract_Zero_S_B_[°C].............   ";
const char *COMMENT_PF_ENTRY049                 = "Number_of_managed_winches.................   ";
//const char *COMMENT_PF_ENTRY050                 = "PID_AutoRunVelocity_Kp....................   ";
//const char *COMMENT_PF_ENTRY051                 = "PID_AutoRunVelocity_Ki....................   ";
//const char *COMMENT_PF_ENTRY052                 = "PID_AutoRunVelocity_Kd....................   ";
const char *COMMENT_PF_ENTRY053                 = "Special_Signal_Mode.......................   ";
const char *COMMENT_PF_ENTRY054                 = "EngineRevolutionChangeable................   ";
//const char *COMMENT_PF_ENTRY055                 = "PID_AutoRunVelocity_Kp_Fast...............   ";
//const char *COMMENT_PF_ENTRY056                 = "PID_AutoRunVelocity_Ki_Fast...............   ";
//const char *COMMENT_PF_ENTRY057                 = "PID_AutoRunVelocity_Kd_Fast...............   ";
const char *COMMENT_PF_ENTRY058                 = "AutoRun_Programable.......................   ";
const char *COMMENT_PF_ENTRY059                 = "AutoRun_PrgFileName.......................   ";
const char *COMMENT_PF_ENTRY060                 = "EngineStartStop_Available.................   ";
const char *COMMENT_PF_ENTRY061                 = "Reverse_Depth.............................   ";
const char *COMMENT_PF_ENTRY062					= "LOG_File_Tension_Unit.....................   ";
const char *COMMENT_PF_ENTRY063					= "MM_Sync_Force_On/Off......................   ";
const char *COMMENT_PF_ENTRY064					= "MM_Sync_Force_Range_Limit.................   ";
const char *COMMENT_PF_ENTRY065					= "LostMotion_Available......................   ";
const char *COMMENT_PF_ENTRY066					= "Wich_brake_controlled_by_SW...............   ";
const char *COMMENT_PF_ENTRY067					= "Velocity_Dimension_Unit...................   ";
const char *COMMENT_PF_ENTRY068					= "Controller_Error_L-active.................	";
const char *COMMENT_PF_ENTRY069					= "Brake_Off_Enabled.........................	";
const char *COMMENT_PF_ENTRY070					= "Controller_Error_At_Slow_Motion...........   ";
const char *COMMENT_PF_ENTRY071					= "Line_Lub_Available........................   ";
const char *COMMENT_PF_ENTRY072					= "Line_Lub_Permanent_Operation..............   ";
const char *COMMENT_PF_ENTRY073					= "Line_Lub_Intensity........................   ";

// Komentare in der internen Parameter-Datei
const char *COMMENT_IPF_ENTRY001                = "CAUTION!!!_THIS_FILE_IS_GENERATED_AUTOMATICALLY.";
const char *COMMENT_IPF_ENTRY002                = "___________PLEASE_DO_NOT_CHANGE_THIS_FILE_MANUALLY.";
const char *COMMENT_IPF_ENTRY003                = "ONLY FOR TEST-PARAMETRISATION";
const char *COMMENT_IPF_ENTRY004                = "Voltage_step_for_velocity_control_normal_[mV]........   ";
const char *COMMENT_IPF_ENTRY005                = "Voltage_step_for_velocity_control_small_[mV].........   ";
const char *COMMENT_IPF_ENTRY006                = "Count_of_velocity_steps_for_stop.....................   ";
const char *COMMENT_IPF_ENTRY007                = "Clock_for_automatical_velocity_adjustment_[ms].......   ";
//const char *COMMENT_IPF_ENTRY008                = "Reference-voltage_for_stopped_winch_[mV].............   ";
const char *COMMENT_IPF_ENTRY009                = "Clock_for_stop_adjustment_[ms]_(from_automatic)......   ";
//const char *COMMENT_IPF_ENTRY010                = "Discharge_cable_PID_Derivative_time_(TD)_slow........   ";
//const char *COMMENT_IPF_ENTRY011                = "Discharge_cable_PID_Reset_time_(TI)_slow.............   ";
//const char *COMMENT_IPF_ENTRY012                = "Discharge_cable_PID_Amplification_factor_(KR)_slow...   ";
//const char *COMMENT_IPF_ENTRY013                = "Discharge_cable_PID_Derivative_time_(TD)_fast........   ";
//const char *COMMENT_IPF_ENTRY014                = "Discharge_cable_PID_Reset_time_(TI)_fast.............   ";
//const char *COMMENT_IPF_ENTRY015                = "Discharge_cable_PID_Amplification_factor_(KR)_fast...   ";


// -----------------------------------------------------------------------------
// Hardware-Grundeinstellungen
const char      *__HW_DepthCalc_Port            = "COM1";       // Standard-Port für die Kommunikation mit dem Teufenrechner
const char      __HW_DepthCalc_DataBit          = 8;
const char      __HW_DepthCalc_StopBit          = 0;            // 0,1,2 = 1,1.5,2
const char      __HW_DepthCalc_Parity           = 0;
const           __HW_DepthCalc_Baud             = 38400;
const           __HW_DepthCalc_ReadTOInt        = 0;            // Read-Timeout-Interval
const           __HW_DepthCalc_ReadTOConst      = 25;           // Read-Timeout-Konstante
const           __HW_DepthCalc_WriteTOConst     = 75;           // Write-Timeout-Konstante
const           __HW_DepthCalc_Long_ReadTOConst = 150;          // Längerer Timeout für Teufenrechner für Anweisungen, auf dessen Antwort länger gewartet werden muss

const char      *__HW_InfoOut_Port              = "COM2";       // Standard-Port für die zusätzliche Informationsausgabe
const char      __HW_InfoOut_DataBit            = 8;
const char      __HW_InfoOut_StopBit            = 2;            // 0,1,2 = 1,1.5,2
const char      __HW_InfoOut_Parity             = 0;
const           __HW_InfoOut_Baud               = 19200;
const           __HW_InfoOut_ReadTOInt          = 0;            // Read-Timeout-Interval
const           __HW_InfoOut_ReadTOConst        = 2;            // Read-Timeout-Konstante
const           __HW_InfoOut_WriteTOConst       = 5;            // Write-Timeout-Interval

// -----------------------------------------------------------------------------
// Teufenrechner
const           DEPTHCALC_CALFACT_DIGITS        = 5;            // Nachkommastellen des Eichfaktors beim Teufenrechner (10^x gibt Faktor an, mit dem Eichfaktor zum Teufenrechner gesendet werden muss)
const float     DEPTHCALC_CALFACT_MAX           = 19900000;     // Maximal möglicher Eichfaktor im hier verwendeten Datenformat
const float     DEPTHCALC_CALFACT_MIN           = 1;            // Minimal möglicher Eichfaktor im hier verwendeten Datenformat
const           DEPTHCALC_SW_VERSION_NAMELENGTH = 20;           // Puffergröße in Byte für den Versionsstring des Teufenrechners
const char      *DEPTHCALC_SW_VERSION_PREFIX    = "TR-SW: ";    // Präfix vor der Teufenrechner-Versionszeichenkette

// -----------------------------------------------------------------------------
// Informations-Ausgabe-Schnittstelle
const char      INFOOUT_IN_1                    = 0x80;         // Empfangs-Kommandobyte 1
const char      INFOOUT_IN_2                    = 0x81;         // Empfangs-Kommandobyte 2
const char      INFOOUT_OUT_1                   = 0x8E;         // Sende-Headerbyte bei Antwort auf Empfangs-Kommandobyte 1
const char      INFOOUT_OUT_2                   = 0x81;         // Sende-Headerbyte bei Antwort auf Empfangs-Kommandobyte 2

// -----------------------------------------------------------------------------
// DAC/DIO-Card
const           MAXNUMBEROFADVANTECHCARDS       = 10;           // Höchstanzahl von DAC/DIO-Karten, die durch das System erkannt werden sollen (getrost bis auf Maximum siehe Driver-Referenz erhöhbar)
const char      *DIORelayCardName               = "PCI-1762 ";  // Device-Bezeichner beginnt mit ...
const char      *DIODACCardName                 = "PCI-1716 ";  // Device-Bezeichner beginnt mit ...

// Nummernbelegung der Digitalein-/ausgänge
// Relais
const short     DIOAutomaticNUM                 = 1;            // Automatik ein
const short     DIOUnloadCableNUM            	= 3;            // Kabel "aktiv" entladen
const short     DIOEngineRevolutionDownNUM      = 9;            // Drehzahl senken
const short     DIOEngineRevolutionUpNUM        = 8;            // Drehzahl erhöhen
const short     DIOEngineStartNUM               = 6;            // Motor starten
const short     DIOEngineStopNUM                = 7;            // Motor ausschalten
const short     DIOFastRunNUM                   = 2;            // Schnell/Langsam (Spreizung)
const short     DIOHWControlNUM                 = 5;            // Hardware-Regler
const short     DIOLostMotionImpulseNUM         = 0;            // Freilauf
const short     DIOPowerLockNUM                 = 11;           // Selbsthaltung Spannungsversorgung STR
const short     DIOControllerNUM                = 4;            // Steuergerät
const short		DIOReleaseBrakeNUM				= 12;			// Bremse lösen
const short     DIOSignalHornNUM                = 10;           // Signal(Hupe)

// Opto-Eingänge
const short     DIOAutomotiveErrorNUM           = 2;            // Fahrzeug-Fehler Indikator
//const short     DIOAuxiliaryDriveNUM            = 5;            // Nebenantrieb Indikator
const short     DIOEmergencySwitchNUM           = 10;           // Not-Schalter Indikator
const short     DIOEngineOnNUM                  = 9;            // Motor-an Indikator
const short     DIOHydraulicOKNUM            	= 5;            // Hydraulik OK Indikator
const short     DIOIncEncErrorNUM               = 8;            // IGR-Fehler Indikator
const short     DIOControllerErrorNUM           = 8;            // Steuergerät Fehler Indikator
const short     DIOLeverStateNUM                = 1;            // Steuerhebelbetätigungs Indikator
const short     DIOLostMotionNUM                = 11;           // Freilauf Indikator
const short     DIOOilStrainerNUM               = 0;            // Ölfilter wechseln Indikator
const short     DIOPowerFailNUM                 = 14;           // Power Fail Indikator
const short     DIOSpecialSignal01NUM           = 12;           // Sonder-Signal 01
const short     DIOSpecialSignal02NUM           = 13;           // Sonder-Signal 02

// Parameter für die Analog-Ausgänge
// Winden-Drehzahl-Steuerung
const unsigned short    AO_WinchRevChannel      = 0;            // Analoger Ausgangs-Kanal
const unsigned short    AO_WinchRevReference    = 0;            // 0...interene Referenzquelle, 1...externe Referenzquelle
const float             AO_WinchRevMinVoltage   = 0;            // Mindestausgangsspannung in mV
const float             AO_WinchRevMaxVoltage   = 4999;         // Maximalausgangsspannung in mV
const float             AO_WinchRevZeroBy       = 2.5;          // Ruhestellung in V
const float				AO_WinchRevCfgMinVoltage = 0;			// Mindestausgangsspannung für Schnittstellenkonfig. in mV (Achtung, Hardware-Fähigkeiten beachten!)
const float				AO_WinchRevCfgMaxVoltage = 5000;		// Maximalausgangsspannung für Schnittstellenkonfig. in mV (Achtung, Hardware-Fähigkeiten beachten!)
const					AO_DAQNavi_WinchRevCfgMaxVoltage = 0;	// DAQNavi: Maximalausgangsspannung für Schnittstellenkofig. 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10

// Zugkraft-Ausgabe an externe Apparatur
const unsigned short    AO_TensionOutChannel    = 1;            // Analoger Ausgangs-Kanal
const unsigned short    AO_TensionOutReference  = 0;            // ADSAPI: 0...interene Referenzquelle, 1...externe Referenzquelle
const float             AO_TensionOutMinVoltage = 0;            // Mindestausgangsspannung in mV (anpassen mit Zugkraftausgabe einzustellen)
const float             AO_TensionOutMaxVoltage = 9999;         // Maximalausgangsspannung in mV (anpassen mit Zugkraftausgabe einzustellen)
const float				AO_TensionOutCfgMinVoltage = 0;			// ADSAPI: Mindestausgangsspannung für Schnittstellenkofig. in mV (Achtung, Hardware-Fähigkeiten beachten!)
const float				AO_TensionOutCfgMaxVoltage = 10000;		// ADSAPI: Maximalausgangsspannung für Schnittstellenkofig. in mV (Achtung, Hardware-Fähigkeiten beachten!)
const					AO_DAQNavi_TensionOutRange = 1;		 	// DAQNavi: Maximalausgangsspannung für Schnittstellenkofig. 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10

// Zugkraft-Steuersignal für externe Regelung
const unsigned short    AO_TensionCtrl_ExtReg_Channel    = 1;   // Analoger Ausgangs-Kanal
const unsigned short    AO_TensionCtrl_ExtReg_Reference  = 0;   // ADSAPI: 0...interene Referenzquelle, 1...externe Referenzquelle
const float             AO_TensionCtrl_ExtReg_MinVoltage = 0;   // Mindestausgangsspannung in mV (anpassen mit Zugkraftausgabe einzustellen)
const float             AO_TensionCtrl_ExtReg_MaxVoltage = 9999;// Maximalausgangsspannung in mV (anpassen mit Zugkraftausgabe einzustellen)
const float				AO_TensionCtrl_ExtReg_CfgMinVolt = 0;	// ADSAPI: Mindestausgangsspannung für Schnittstellenkonfig. in mV (Achtung, Hardware-Fähigkeiten beachten!)
const float				AO_TensionCtrl_ExtReg_CfgMaxVolt = 10000;// ADSAPI: Maximalausgangsspannung für Schnittstellenkonfig. in mV (Achtung, Hardware-Fähigkeiten beachten!)
const					AO_DAQNavi_TensionCtrl_ExtReg_Range = 1;// DAQNavi: Maximalausgangsspannung für Schnittstellenkonfig. 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10

// Parameter für Analog-Eingänge
// Zugkraftmesser
const unsigned short    AI_TractionChannel      = 6;            // Eingangskanal
const unsigned short    AI_TractionGain         = 0x04;         // ADSAPI: bipolar, Verstärkung 0.5 bei +-10V
const unsigned short    AI_TractionTrigger      = 0;            // ADSAPI: Trigger 0...intern, 1...extern
const					AI_DAQNavi_TensionRange	= 3;       		// DAQNavi: Eingangsspannunggsbereich, 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10, 4..V_0To2pt5, 5..V_Neg2pt5To2pt5

// Messen der Steuerspannung für Geschwindigkeit
const unsigned short    AI_WinchRevMessChannel  = 7;            // Eingangskanal
const unsigned short    AI_WinchRevMessGain     = 0x04;         // ADSAPI: bipolar, Verstärkung 0.5 bei +-10V
const unsigned short    AI_WinchRevMessTrigger  = 0;            // ADSAPI: Trigger 0...intern, 1...extern
const					AI_DAQNavi_WinchRevMessRange = 3;       // DAQNavi: Eingangsspannunggsbereich, 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10, 4..V_0To2pt5, 5..V_Neg2pt5To2pt5

// Temperatursensoren
const unsigned short    AI_TemperatureDepthCalcChannel  = 5;    // Eingangskanal
const unsigned short    AI_TemperatureDepthCalcGain     = 0x11; // ADSAPI: unipolar, Verstärkung 2 bei 0-5V
const unsigned short    AI_TemperatureDepthCalcTrigger  = 0;    // Trigger 0...intern, 1...extern
const					AI_DAQNavi_TemperatureDepthCalcRange= 0;// DAQNavi: Eingangsspannunggsbereich, 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10, 4..V_0To2pt5, 5..V_Neg2pt5To2pt5

const unsigned short    AI_TemperatureAChannel          = 3;     // Eingangskanal
const unsigned short    AI_TemperatureAGain             = 0x11;  // ADSAPI: unipolar, Verstärkung 2 bei 0-5V
const unsigned short    AI_TemperatureATrigger          = 0;     // ADSAPI: Trigger 0...intern, 1...extern
const					AI_DAQNavi_TemperatureARange	= 0;     // DAQNavi: Eingangsspannunggsbereich, 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10, 4..V_0To2pt5, 5..V_Neg2pt5To2pt5

const unsigned short    AI_TemperatureBChannel          = 4;     // Eingangskanal
const unsigned short    AI_TemperatureBGain             = 0x11;  // ADSAPI: unipolar, Verstärkung 2 bei 0-5V
const unsigned short    AI_TemperatureBTrigger          = 0;     // ADSAPI: Trigger 0...intern, 1...extern
const					AI_DAQNavi_TemperatureBRange	= 0;     // DAQNavi: Eingangsspannunggsbereich, 0..V_0To5, 1..V_0To10, 2..V_Neg5To5, 3..V_Neg10To10, 4..V_0To2pt5, 5..V_Neg2pt5To2pt5

// -----------------------------------------------------------------------------
// Timer (Takt 1000 ms)

const   TIMEOUT_Termination             = 5000;          // maximale Wartezeit, für die Beendigung des Programmes, bis der Programmabbruch erzwungen wird (in ms)

const   IMPULSE_LENGHT_LostMotion       = 5;            // x * 100ms  Impulslänge für Freilauf-Schrittrelais

// -----------------------------------------------------------------------------
// Kommandozeilen-Parameter
const char CMDP_Entry                   = '-';          // Einführungszeichen für Kommandozeilenparameter
const char *CMDP_DEVEL                  = "devel";      // Parameter für Entwicklermodus
const char *CMDP_LANGUAGE_GERMAN        = "lang=de";    // Parameter für Sprache deutsch
const char *CMDP_LANGUAGE_RUSSIAN       = "lang=ru";    // Parameter für Sprache russisch
const char *CMDP_LANGUAGE_ENGLISH       = "lang=en";    // Parameter für Sprache englisch
const char *CMDP_PROGRAM_VERSION        = "version";    // Parameter für Ausgabe der Programmversion

// -----------------------------------------------------------------------------
// Schrittwerte für diverse aufwärts/abwärts-Zählungen
const STEP_TractionDifference           = 200;          // Zugkraftdifferenz wird um ... erhöht/vermindert bei Dip-Regelung
const STEP_TimeDifference               = 1;            // Zeitdifferenz wird um ... erhöht/vermindert bei Dip-Regelung
const STEP_UnloadCableTraction       	= 100;          // Zugkraft wird um ... N erhöht/vermindert bei Dip-Regelung
// -----------------------------------------------------------------------------
// feste Fahr-Parameter
const MIN_DEPTH_FOR_AUTO_UP             = 20000;        // Grenzwert für Automatik-Fahrt, wo nicht mehr nach oben gefahren werden kann

// -----------------------------------------------------------------------------
// Standardeinstellung der Werte
// für Kontrollvariablen zur Programmsteuerung
const STD_AutoRunOn = _OFF;                             // Automatische Teufenfahrt (true) oder manuelle Teufenfahrt (false)
const STD_WinchWork = _OFF;                             // Winde ein (true), angehalten (false)
const STD_CalcCurrTractions = _OFF;                     // momentane Zugkraft in Abhängigkeit der Teufe berechnen
const STD_CalcCurrTractionsActive = _OFF;               // momentane Zugkraft in Abhängigkeit der Teufe berechnen tatsächlich aktiv
const STD_ThresholdExceedenceStopDelayOn = _OFF;        // Verzögerung des Ausschaltens vom Steuergerät bei Zugkraft-/ Teufenüberschreitung
const STD_ClearAfterThresholdDepth = _OFF;              // Freigabe nach Grenzwertüberschreitung aktiv (true) oder nicht (false)
const STD_ClearAfterThresholdTraction = _OFF;           // Freigabe nach Grenzwertüberschreitung aktiv (true) oder nicht (false)
const STD_ResetSpecialSignalErrorFlags = _OFF;          // Bestätigen von ausgewählten durch Sondersignale ausgelösten Fehlern
const STD_FastRun = _ON;                                // Schnelle (true) oder langsame (false) Fahrt
const STD_EngineRevolutionUpDown = 0;                   // Motordrehzahl erhöhen (+1), verringern (-1), belassen (0)
const STD_EngineRevolutionAutoUpDown = 0;               // Motordrehzahl erhöhen (+1), verringern (-1), belassen (0); Standardwert für Variable für programminterne Automatik
const STD_Hello_WinchClear = _ON; 						// Hallo über das Signal WinchClear zur externen Steuerelektronik (forciertes kurzes Auslösen des Signals)
const STD_HydraulicBrakeOn = _OFF;						// Hydraulische Bremse aktiv (true) / inaktiv (false)
const STD_MagneticMarkOn = _OFF;                        // Magnetmarken ein (true) oder aus (false)
const STD_MMSyncForceActive = _OFF;						// MM-Zwangssynchronisation aktiv (true) oder inaktiv (false)
const STD_MMSyncForceCounter = 0;				   		// MM Teufenzähler für MM-Zwangssynchronisation
const STD_devel = _OFF;                                 // Entwicklermodus ein (true), aus (false)
const STD_UnloadCable = _OFF;                        	// Kabel entladen ein (true) oder aus (false)
const STD_UnloadCableActive = _OFF;                  	// Aktiv Kabel entladen ein (true) oder aus (false)
const STD_UnloadCablePassive = _OFF;                 	// Passiv Kabel entladen ein (true) oder aus (false)
const STD_CalibrateOn = _OFF;                           // Kalibrieren ein (true) oder aus (false)
const STD_CalibrateDepth = _OFF;                        // Kalibrieren des Teufenrechners (true) - Eichwertbestimmung
const STD_CalibrateTraction = _OFF;                     // Kalibrieren des Zugraftmesser-Moduls (true) - Anstieg- und Nullwertbestimmung
const STD_GetCtlUnitSWVersion = _ON;					// Gibt an, ob die Software-Version des Steuergeräts ausgelesen werden soll
const STD_GetDepthCalcSWVersion = _OFF;                 // Gibt an, ob die Software-Version des Teufenrechners ausgelesen werden soll
const STD_LostMotionImpulse = _OFF;                     // Gibt an, ob der Freilaufimpuls noch auftreten muss oder nicht
const STD_SetDepth = _NO;                               // Gibt an, ob die Start-Teufe neu an den Teufenrechner übergeben werden soll (true)
const STD_SetDepthCalcParam = _OFF;                     // Gibt an, ob die Parameter an den Teufenrechner übergeben werden sollen (true)
const STD_SetDepthCalFactor = _OFF;                     // Gibt an, ob der Eichfaktor neu an den Teufenrechner übergeben werden soll (true)
const STD_SetFormatCMImpulse = _OFF;                    // Gibt an, ob der Wert in FormatCMImpulse an den Teufenrechner übergeben werden soll (true) oder nicht (false)
const STD_SetMagneticMark = _OFF;                       // Gibt an, ob der Wert in MagneticMarkOn an den Teufenrechner übergeben werden soll (true) oder nicht (false)
const STD_SpecialFunc1 = _OFF;							// Sonderfunktion 1
const STD_WinchBrakeOff = _OFF;                         // Gibt an, ob die Bremse der Winde gelöst werden soll (NICHT GLEICH FREILAUF)
const STD_WinchBrakeOffImpulse = _OFF;                  // Gibt an, ob ein Impuls zur Steuerung der Bremslösung bzw. Anzug kommen soll

const STD_CalibTension_GetVal1 = _OFF;                	// Gibt an, ob der erste Spannungswert für die Zugkraftkalibrierung ermittelt werden soll
const STD_CalibTension_GetVal2 = _OFF;                	// Gibt an, ob der zweite Spannungswert für die Zugkraftkalibrierung ermittelt werden soll
const STD_CalibTractionCalculateParam = _OFF;           // Gibt an, ob die Parameter zur Zugkraftermittlung berechnet werden sollen
const STD_CalibTractionSetParam = _OFF;                 // Gibt an, ob die Parameter zur Zugkraftermittlung übernommen werden sollen
const STD_CalibTractionSetZero = _OFF;                  // Gibt an, ob ein Nullableich für den Zugkraftmesser durchegführt werden soll
//const STD_CalibDepthCountImpulsesOn = _OFF;             // Gibt an, ob die Impulse des Teufenrechners zur Teufenrechnerkalibrierung gezählt werden sollen
//const STD_CalibDepthCalculateParam = _OFF;              // Gibt an, ob der Eichfaktor für die Teufenermittlung berechnet werden kann
//const STD_CalibDepthSetParm = _OFF;                     // Gibt an, ob der Eichfaktor für die Teufenermittlung übernommen werden soll

const STD_SignalHornOn = _OFF;                          // Gibt an, ob das Signal-Horn betätigt ist (true) oder nicht (false)
const STD_EngineStart = _OFF;                           // Gibt an, ob der Motor gestartet werden soll (true)
const STD_EngineStop = _OFF;                            // Gibt an, ob der Motor ausgeschaltet werden soll (true)

const STD_PowerFail = _OFF;                             // Gibt an, ob die Stromversorgung des STR durch den SST-Schlüsselschalte oder die Kfz-Zündung abgeschaltet wurde

// für Statusinformationen
const STD_CurrDepth = 0;                                // momentane Teufe
const STD_CurrDirection = _DOWN;                        // momentane Fahrtrichtung; abwärts (false) / aufwärts (true)
const STD_CurrVelocity = 0;                             // momentane Sondengeschwindigkeit
const STD_AnnouncedVelocity = 0;                        // angezeigte Sondengeschwindigkeit
const STD_CurrWinchRevMessVoltage = 0;                  // Gemessene momentane Windengeschwindigkeitssteuer-Spannung in mV
const STD_CurrTensionLimit_Lower = 0;                   // momentane Mindestzugkraft
const STD_CurrTensionLimit_Upper = 50000;               // momentane Maximalzugkraft
const STD_CurrTraction = 0;                             // momentane Zugkraft
const STD_CurrTractionDifference = 20000;               // momentane Zugkraftdifferenz
const STD_CurrTractionReference = 0;                    // momentane Soll-Zugkraft
const STD_CurrTimeDifference = 5;                       // momentane Zeitdifferenz
const STD_CurrTractionVoltage = 0;                      // momentane Spannung vom Zugkraftmesser
const STD_MagneticMark = _OFF;                          // Magnetmarke tritt auf (true)
//const STD_IncEncError = _OFF;                           // Inkrementalgeber scheint fehlerhaft (true)
const STD_ControllerError = _OFF;                       // Steuergerät signalisiert Fehler (true)
const STD_LostMotion = _NO;                             // Leerlauf aktiv (true), inaktiv (false)
const STD_EngineCoolantTemp = 0;						// Antriebsmotorkühlmitteltemperatur
const STD_EngineOn = _OFF;                              // Motor ein(true)/aus(false)
const STD_EngineSpeed = 0;								// Antriebsmotordrehzahl
const STD_OilStrainer = _OFF;                           // Ölfilter ein/aus
//const STD_AuxiliaryDrive = _OFF;                        // Nebenantrieb ein/aus
const STD_HydraulicOK = _OFF;                        	// Hydraulik OK
const STD_HydrWrkPressure = 0;							// Aktueller Arbeitsdruck der Hydraulik
const STD_HydrChargePressure = 0;						// Speisedruck der Hydraulik
const STD_HydrLvl = 0;									// Aktueller Hydraulikfüllstand
const STD_EmergencySwitch = _OFF;                       // Notschalter betätigt ein/aus
const STD_DepthThresholdExceeded = _OFF;                // Teufenbereich verlassen
const STD_TractionThresholdExceeded = _OFF;             // Zugkraftschwelle überschritten
const STD_LeverOn = _OFF;                               // Fahrhebel aktiv (Manuelle Fahrtrichtungs-/Geschwindigkeitseinstellung)
const STD_HWCtrlSignalisation = _OFF;                   // Signalisiert den tatsächlichen Hardware-Kontroll-Status
const STD_AutomotiveError = _OFF;                       // Signalisiert einen Fahrzeugfehler (Warnung für z.B. Motor-Öldruck, Motor-Temperatur)
const STD_SpecialSignal01 = _OFF;                       // Konfigurierbares Sondersignal zur Signalisierung verschiedener Ereignisse
const STD_SpecialSignal02 = _OFF;                       // Konfigurierbares Sondersignal zur Signalisierung verschiedener Ereignisse
const STD_SpecialSignalExtBrake = _OFF;					// Durch ein Sondersignal überwachte externe Bremse
const STD_CalibTractionValue1Ok = _OFF;                 // Gibt an, ob der erste Spannungswert erfolgreich ermittelt wurde
const STD_CalibTractionValue2Ok = _OFF;                 // Gibt an, ob der zweite Spannungswert erfolgreich ermittelt wurde
const STD_CalibDepthAllowed = _OFF;                     // Gibt an, ob das Kalibrieren der Teufe im Aktuellen Zustand generell möglich ist
const STD_CalibDepthCableOk = _OFF;                     // Gibt an, ob das Kabel eine bestimmte Länge herausgezogen wurde und nun die Teufe angegeben werden kann
const char *STD_CtlUnitSWVersion = "---";               // Gibt Versionsstring des Steuergeräts an
const char *STD_DepthCalcSWVersion = "---";             // Gibt Versionsstring des TR an
const float STD_TemperatureSensorDepthCalc = 0;         // Enthält die Temperatur des T.-Sensors auf dem Teufenrechner in °C
const float STD_TemperatureSensorA = 0;                 // Enthält die Temperatur des T.-Sensors A in °C
const float STD_TemperatureSensorB = 0;                 // Enthält die Temperatur des T.-Sensors B in °C
const STD_TemperatureOilTank = 0;                 		// Öltanktemperatur in °C

// für Programmeinstellungen
const STD_WinchNo = 0;                                  // Nummer der aktuellen Winde
//const STD_WinchCnt = 1;                                 // Anzahl der zu verwaltenden Winden
const STD_UnloadCableStretch = _OFF;                 	// Stretching beim entladen des Kabels ein (true) oder aus (false)
const STD_MinDepth = 0;                                 // einstellbare Mindestteufe
const STD_MaxDepth = 0;                                 // einstellbare Maximalteufe
const STD_StartDepth = 0;                               // einstellbare Startteufe
const STD_ReverseDepth = _OFF;							// Einstellung, ob die Teufe umgekehrt angezeigt werden soll (aus - wird + und umgekehrt)
const STD_DepthThresholdForAuto = 10000;                // Teufe, ab wann Automatikbetrieb möglich ist
const STD_DepthDistanceForBreak = 10000;                // Abstand vorm Endewert, ab welchem die Winde abgebremst werden soll
const STD_MMSyncForceOn = _OFF;							// MM-Zwangssynchr. Ein/Aus
const STD_MMSyncForceRangeLim = 50;   					// MM-Zwangssynchr. Teufenbereichsgrenze in Meter
const STD_Velocity = 0;                                 // einstellbare Geschwindigkeit
const STD_Velocity_PrgAutom = 0;                        // einstellbare Geschwindigkeit für Programmautomatik (wird je nach Programmpunkt gesetzt)
const STD_Direction = _DOWN;                            // einstellbare Fahrtrichtung (false = abwärts, true = aufwärts)
const STD_Direction_PrgAutom = _DOWN;                   // einstellbare Fahrtrichtung (false = abwärts, true = aufwärts) für Programmautomatik (wird je nach Programmpunkt gesetzt)
const STD_EngineRevolutionChangeable = _NO;             // Drehzahl Fahrzeugmotor veränderbar
const STD_EngineStartStopEnable = _YES;                 // Antriebsmotor Starten / Stoppen möglich
const STD_LostMotionEnable = _ON;                 		// Freilaufbedienung möglich
const STD_BrakeOffEnable = _ON;                         // Bremse abschalten möglich
const STD_ExternalBrakeAvailable = _OFF;				// Externe Bremse verfügbar

const STD_FormatCMImpulse = _KARAT;                     // eingestelltes Format für den cm-Impuls (0=nichts | 1=KARAT | 2=ONYX | 3=KEDR)
//const STD_DriveVelocityStepNormal = 50;                 // Geschwindigkeitsabstufung während der Fahrt im Automatikbetrieb groß
//const STD_DriveVelocityStepSmall = 1;                   // Geschwindigkeitsabstufung während der Fahrt im Automatikbetrieb klein
//const STD_StopVelocityStep = 2;                         // Geschwindigkeitsabstufung beim Anhalten der Winde im Automatikbetrieb
const STD_MinTraction = 0;		                        // einstellbare Mindestzugkraft
const STD_MaxTraction = 50000;	                        // einstellbare Maximalzugkraft
//const STD_MaxTractionDifference = 20000;	            // einstellbare Maximale Zugrkraftdifferenz
const STD_RelTensionLimitLower = 2000;	            	// einstellbare untere Zugkraftgerenze, abhängig von der aktuell höchsten Zugkraft (wichtig bei Abwärtsfahrt)
const STD_RelTensionLimitUpper = 2000;             		// einstellbare obere Zugkraftgerenze, abhängig von der aktuell niedrigsten Zugkraft (wichtig bei Abwärtsfahrt)
const STD_UnloadCableBeginLenght = 0;                	// Anfangslänge beim Entladen des Kabels
const STD_UnloadCableEndLenght = 0;                  	// Endlänge beim Entladen des Kabels
const STD_UnloadCableTraction = 0;		             	// einstellbare Zugkraft zum Entladen des Kabels für aktiven Zustand (Regime 1)
const STD_UnloadCableMinTraction = 1000;             	// Mindestzugkraft zum Entladen des Kabels für aktiven Zustand (Regime 1)
const STD_UnloadCableStartTraction = 0;		        	// einstellbare Anfangszugkraft beim Entladen des Kabels
const STD_UnloadCableEndTraction = 0;		          	// einstellbare Endzugkraft beim Entladen des Kabels
//const STD_CalcMaxTractionsDepthStep = 100000;           // Intervall für dynamische Zugkraftberechnung
//const float STD_UnloadCable_PID_TD = 1;              	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit
//const float STD_UnloadCable_PID_TI = 100;            	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit
//const float STD_UnloadCable_PID_KR = 0.05;           	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor
//const float STD_UnloadCable_PID_TD_2 = 1;            	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Vorhaltezeit
//const float STD_UnloadCable_PID_TI_2 = 100;          	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Nachstellzeit
//const float STD_UnloadCable_PID_KR_2 = 0.05;         	// PID-Regler für Zugkraftkontrolle beim Kabel entladen: Verstärkungsfaktor
// ToDo: Unbenötigte Konstanten und deren Zielvariablen zu STD_UnloadCable_ExtReg... entfernen
//const STD_UnloadCable_ExtReg_MaxTension = 30000;		// Kabel Entladen mit externem Regler Kennliniendef. Maximalzugkraft in N
//const STD_UnloadCable_ExtReg_MaxVoltage = 10000;		// Kabel Entladen mit externem Regler Kennliniendef. Maximalspannung in mV
//const STD_UnloadCable_ExtReg_MinTension = 0; 			// Kabel Entladen mit externem Regler Kennliniendef. Mindestzugkraft in N
//const STD_UnloadCable_ExtReg_MinVoltage = 0; 			// Kabel Entladen mit externem Regler Kennliniendef. Mindestspannung in mV
//const STD_UnloadCable_ExtReg_MaxPosOffset = 2000;     	// Kabel Entladen mit externem Regler max. positiver Offset der Kennlinie in mV
//const STD_UnloadCable_ExtReg_MaxNegOffset = -2000;    	// Kabel Entladen mit externem Regler max. negativer Offset der Kennlinie in mV
//const STD_UnloadCable_ExtReg_Delay = 10000;             // Kabel Entladen mit externem Regler Verzögerung beim Abfahren (Rampe) in ms
//const STD_UnloadCable_ExtReg_P = 20;                 	// Kabel Entladen mit externem Regler P-Anteil *1000
//const STD_UnloadCable_ExtReg_I = 10000;                	// Kabel Entladen mit externem Regler I-Anteil (Nachstellzeit in ms)
//const STD_UnloadCable_ExtReg_ReturnRotVoltage = 1900;	// Kabel Entladen mit externem Regler Windenansteuerspannung zum Kabel straffen in mV
const STD_TractionViewDimensionUnit = 0;                // Darstellung der Zugkraft (0=1N|1=1kN|2=1kp)
//const STD_MaxTimeDifference = 5;	                    // einstellbare Maximale Zeitdifferenz
const STD_CalFactDepth = 60550;	                        // Kalibrierfaktor für den Teufenrechner
const STD_CalFactDepth_Earmarked = 0;					// Teufenkalibrierfaktor, ermittelt bei der Kalibrierung und vorgemerkt für die Übernahme
const STD_CalibTractionX = 10;                     		// Zugkraftkalibrierung X-Achse für Anstiegsberechnung
const STD_CalibTractionY = 590;		                    // Zugkraftkalibrierung Y-Achse für Anstiegsberechnung
const STD_CalibTractionZero = 0;	                    // Nullwert für Zugkraftkalibrierung
const STD_IncEncErrorMonitOn = _ON;                     // Inkrementalgeber-Fehler-Überwachung
const STD_ControllerErrorMonitOn = _ON;                 // Steuergerät - Fehler-Überwachung
const STD_ControllerErrorMonitOnAtSM = _ON;				// Steuergerät - Fehler-Überwachung in der langsamen Fahrstufe aktiv

const STD_ControllerErrorLactive = _OFF;				// Steuergerät - Fehler L-aktiv

//const STD_DriveVelocityStepClock = 100;                 // Takt für Automatik-Betrieb: normale Geschwindigkeitsabstufung
const bool STD_AutoRunProgramableOn = false;            // Programmautomatik Ein/Aus
const wchar_t *STD_AutoRunProgramFileName = L"";        // Dateiname für eingestelltes Ablaufprogramm für die Programmautomatik
const STD_VelocityDimensionUnit = 0;					// Darstellung der Geschwindigkeit (0=1m/h|1=1m/min)
const STD_WinchBrakeInternAuto = _OFF;					// Windenbremse intern automatisch
const STD_WinchBrakeInternAutoReleasePerc = 3;			// Lösen der Windenbremse bei interner Automatikbremse in Prozent
const STD_WinchBrakeInternAutoReleaseHysteresisPerc = 1;// Jitterkompensation Lösen/Feststellen der Windenbremse bei interner Automatikbremse in Prozent


const STD_CalibTractionValue1 = 0;                      // Die mit dem, bei der Zugkraftkalibrierung ermittelte, ersten Spannungswert korespondierende Zugkraft
const STD_CalibTractionValue2 = 0;                      // Die mit dem, bei der Zugkraftkalibrierung ermittelte, zweiten Spannungswert korespondierende Zugkraft
const STD_CalibTension_MeasuringVal = 0;				// Der jeweilig mit der Zugkraft korrespondierende Messwert
const STD_CalibTension_Tension = 0;						// Die jeweilig mit dem Messwert korrespondierende Zugkraft

//const STD_CalibDepthLenght = 0;                         // Puffer für die herausgezogene Kabellänge (vom Benutzer einzugeben)
//const STD_CalibDepthCalcCalFactor = 0;                  // Puffer für berechneten oder eingegebenen Eichfaktor für den Teufenrechner (wird nach Bestätigung in CalFactDepth übernommen und damit aktiv)
//const STD_CalibDepthMinImpulses = 1000;                 // Anzahl der Impulse, die mindestens gezählt werden müssen, damit CalibDepthCableOk = true werden kann
const STD_CalibDepthMinLenght = 50000;                  // Strecke in mm, die mindestens abgewickelt werden müssen, damit CalibDepthCableOk = true werden kann

//const STD_CalibDepthImpulses = 0;                       // Puffer für die Anzahl der bei der Impuls-Zählung ermittelten IGR-Impulse
const STD_CalibDepthCountCalFactor = 100000;            // Der Eichfaktor, der während des Zählens der Teufenimpulse bei der Teufenkalibrierung gesetzt sein soll
const STD_CalibDepthCountDepth = 0;                     // Die Teufe, mit der während des Zählens der Teufenimpulse bei der Teufenkalibrierung gestartet werden soll
const STD_CalibDepthCountMagneticMark = false;          // Die Magnetmarkeneinstellung, die während des Zählens der Teufenimpulse bei der Teufenkalibrierung gesetzt sein soll

const float STD_CalibTSensorDepthCalc_CaractSlope = -83.2853; // Anstieg der Kennlinie des T.-Sensors des Teufenrechners in °C/V
const float STD_CalibTSensorDepthCalc_CaractZero  = 156.177;  // Nulldurchgang der Kennlinie des T.-Sensors des Teufenrechners in °C
const float STD_CalibTSensorA_CaractSlope         = -83.2853; // Anstieg der Kennlinie des T.-Sensors A in °C/V
const float STD_CalibTSensorA_CaractZero          = 156.177;  // Nulldurchgang der Kennlinie des T.-Sensors A in °C
const float STD_CalibTSensorB_CaractSlope         = -83.2853; // Anstieg der Kennlinie des T.-Sensors B in °C/V
const float STD_CalibTSensorB_CaractZero          = 156.177;  // Nulldurchgang der Kennlinie des T.-Sensors B in °C


const STD_DynMaxTractionStepClock = 200;                // Takt für Zugkraftproben-Aufnahme für Maximal-Zugkraftermittlung

const STD_LOGFileTensionUnit = 0;						// Zugkrafteinheit für die Protokollierung (0...kN, 1...kg)
const STD_LOGFileType = 2;                              // Typ der LOG-Datei (0...Geo Sys V1; 1...LAS 2.0, 2...Geo Sys V2)

const STD_SpecialSignalMode = 1;                        // Modus, wie die Sondersignale ausgewertet werden sollen (welche Version der Abarbeitung) (0=Aus, 1=Positionssensoren Schaltgetriebe, ...)

const bool STD_LineLubAvailable = false;				// Kabel-/Seilöler verfügbar
const bool STD_LineLubPermanentOperation = false;		// Kabel-/Seilöler Dauerbetrieb
const STD_LineLubIntensity = 255;						// Kabel-/Seilöler Intensität


// Limits für Programmeinstellungen
// Diese Limits müssen auch dann festgelegt werden, wenn der Benutzer keine direkte Eingabe vornehmen darf
// Die Limits dienen auch für das Einlesen aus der Parameterdatei
// Boolsche Parameter benötigen keine Limits
const LIM_PAR_WinchNo_MIN                               = 0;                    // Limit für Nummer Windenauswahl Minimum
const LIM_PAR_WinchNo_MAX                               = _WinchCnt - 1;        // Limit für Nummer Windenauswahl Maximum
const LIM_PAR_MinDepth_MIN                              = 0;                    // Limit für Mindestteufe in m Minimum
const LIM_PAR_MinDepth_MAX                              = 9999999;              // Limit für Mindestteufe in m Maximum
const LIM_PAR_MaxDepth_MIN                              = 0;                    // Limit für Maximalteufe in m Minimum
const LIM_PAR_MaxDepth_MAX                              = 9999999;              // Limit für Maximalteufe in m Maximum
const LIM_PAR_StartDepth_MIN                            = -9999999;             // Limit für Start-/Setzen-Teufe in m Minimum
const LIM_PAR_StartDepth_MAX                            = 9999999;              // Limit für Start-/Setzen-Teufe in m Maximum
const LIM_PAR_MMSyncForceRangeLim_MIN					= 0;					// Limit für Bereich MM-Zwangssynchr. Minimum
const LIM_PAR_MMSyncForceRangeLim_MAX				 	= 100;					// Limit für Bereich MM-Zwangssynchr. Maximum
const LIM_PAR_Velocity_MIN                              = 0;                    // Limit für Automatische Fahrgeschwindigkeit in m/h Minimum
const LIM_PAR_Velocity_MAX                              = 20000;                // Limit für Automatische Fahrgeschwindigkeit in m/h Maximum
const LIM_PAR_FormatCMImpulse_MIN                       = 1;                    // Limit für Format externer Zentimeter-Impuls Minimum
const LIM_PAR_FormatCMImpulse_MAX                       = 5;                    // Limit für Format externer Zentimeter-Impuls Maximum
const LIM_PAR_StopVelocityStep_MIN                      = 1;                    // Limit für Anhaltestufen aus Automatikfahrt Minimum
const LIM_PAR_StopVelocityStep_MAX                      = 10;                   // Limit für Anhaltestufen aus Automatikfahrt Maximum
const LIM_PAR_MinTraction_MIN                           = 0;                    // Limit für Mindestzugkraft Minimum
const LIM_PAR_MinTraction_MAX                           = 0;                    // Limit für Mindestzugkraft Maximum
const LIM_PAR_MaxTraction_MIN                           = 0;                    // Limit für Maximalzugkraft Minimum
const LIM_PAR_MaxTraction_MAX                           = 70000;                // Limit für Maximalzugkraft Maximum
//const LIM_PAR_MaxTractionDifference_MIN                 = 0;                    // Limit für Maximale Zugkraftdifferenz bei Zugkraftabfall Minimum
//const LIM_PAR_MaxTractionDifference_MAX                 = 20000;                // Limit für Maximale Zugkraftdifferenz bei Zugkraftabfall Maximum
const LIM_PAR_MaxTimeDifference_MIN                     = 0;                    // Limit für Maximale Zeitdifferenz bei Zugkraftabfall Minimum
const LIM_PAR_MaxTimeDifference_MAX                     = 10;                   // Limit für Maximale Zeitdifferenz bei Zugkraftabfall Maximum
const LIM_PAR_RelTensionLimitLower_MAX                  = 20000;                // Limit für einstellbare untere Zugkraftgerenze, abhängig von der aktuell höchsten Zugkraft (wichtig bei Abwärtsfahrt) - Maximun
const LIM_PAR_RelTensionLimitLower_MIN                  = 0;                    // Limit für einstellbare untere Zugkraftgerenze, abhängig von der aktuell höchsten Zugkraft (wichtig bei Abwärtsfahrt) - Minimum
const LIM_PAR_RelTensionLimitUpper_MAX                  = 20000;                // Limit für einstellbare obere Zugkraftgerenze, abhängig von der aktuell niedrigsten Zugkraft (wichtig bei Abwärtsfahrt) - Maximun
const LIM_PAR_RelTensionLimitUpper_MIN                  = 0;                    // Limit für einstellbare obere Zugkraftgerenze, abhängig von der aktuell niedrigsten Zugkraft (wichtig bei Abwärtsfahrt) - Minimum
const LIM_PAR_UnloadCableBeginLenght_MIN             	= -9999999;             // Limit für Anfangslänge bei aktivem Abwickeln des Kabels Minimum
const LIM_PAR_UnloadCableBeginLenght_MAX             	= 9999999;              // Limit für Anfangslänge bei aktivem Abwickeln des Kabels Maximum
const LIM_PAR_UnloadCableEndLenght_MIN               	= -9999999;             // Limit für Maximale Zugkraftdifferenz bei dynamischer Maximalzugkraftberechung Minimum
const LIM_PAR_UnloadCableEndLenght_MAX               	= 9999999;              // Limit für Maximale Zugkraftdifferenz bei dynamischer Maximalzugkraftberechung Maximum
const LIM_PAR_UnloadCableTraction_MIN                	= -1000;                // Limit für Sollzugkraft beim aktiven Entladen des Kabels Minimum
const LIM_PAR_UnloadCableTraction_MAX                	= 70000;                // Limit für Sollzugkraft beim aktiven Entladen des Kabels Maximum
const LIM_PAR_UnloadCableStartTraction_MIN           	= 0;                    // Limit für Startzugkraft beim aktiven Entladen des Kabels und längenabhängige Sollzugkraftanpassung Minimum
const LIM_PAR_UnloadCableStartTraction_MAX           	= 70000;                // Limit für Startzugkraft beim aktiven Entladen des Kabels und längenabhängige Sollzugkraftanpassung Maximum
const LIM_PAR_UnloadCableEndTraction_MIN             	= 0;                    // Limit für Endzugkraft beim aktiven Entladen des Kabels und längenabhängige Sollzugkraftanpassung Minimum
const LIM_PAR_UnloadCableEndTraction_MAX             	= 70000;                // Limit für Endzugkraft beim aktiven Entladen des Kabels und längenabhängige Sollzugkraftanpassung Maximum
//const LIM_PAR_CalcMaxTractionDifference_MIN             = 0;                    // Limit für Maximale Zugkraftdifferenz bei dynamischer Maximalzugkraftberechung Minimum
//const LIM_PAR_CalcMaxTractionDifference_MAX             = 20000;                // Limit für Maximale Zugkraftdifferenz bei dynamischer Maximalzugkraftberechung Maximum
const float LIM_PAR_UnloadCable_PID_TD_MIN           	= 1;                    // Limit D-Anteil des Reglers für aktives entladen des Kabel Minimum
const float LIM_PAR_UnloadCable_PID_TD_MAX           	= 10;                   // Limit D-Anteil des Reglers für aktives entladen des Kabel Maximum
const float LIM_PAR_UnloadCable_PID_TI_MIN           	= 1;                    // Limit I-Anteil des Reglers für aktives entladen des Kabel Minimum
const float LIM_PAR_UnloadCable_PID_TI_MAX           	= 150;                  // Limit I-Anteil des Reglers für aktives entladen des Kabel Maximum
const float LIM_PAR_UnloadCable_PID_KR_MIN           	= 0.0005;               // Limit P-Anteil des Reglers für aktives entladen des Kabel Minimum
const float LIM_PAR_UnloadCable_PID_KR_MAX           	= 1;                    // Limit P-Anteil des Reglers für aktives entladen des Kabel Maximum
const LIM_PAR_TractionViewDimensionUnit_MIN             = 0;                    // Limit für Einheitenauswahl der Zugkraftanzeige Minimum
const LIM_PAR_TractionViewDimensionUnit_MAX             = 2;                    // Limit für Einheitenauswahl der Zugkraftanzeige Maximum
const LIM_PAR_CalFactDepth_MIN                          = DEPTHCALC_CALFACT_MIN;// Limit für Eichwert Teufe Minimum
const LIM_PAR_CalFactDepth_MAX                          = DEPTHCALC_CALFACT_MAX;// Limit für Eichwert Teufe Maximum
//const LIM_PAR_CalibTractionX_MIN      // Limits für Zugkraftkalibrierung gibt es keine (nur Integer-Grenzen)
//const LIM_PAR_CalibTractionX_MAX
//const LIM_PAR_CalibTractionY_MIN
//const LIM_PAR_CalibTractionY_MAX
//const LIM_PAR_CalibTractionZero_MIN
//const LIM_PAR_CalibTractionZero_MAX
const LIM_PAR_DriveVelocityStepClock_MIN                = 50;                   // Limit für Takt Geschwindigkeitssteuerung Automatikfahrt in ms Minimum
const LIM_PAR_DriveVelocityStepClock_MAX                = 1000;                 // Limit für Takt Geschwindigkeitssteuerung Automatikfahrt in ms Maximum
//const LIM_PAR_DriveVelocity0mph_MIN                     = 2300;                 // Limit für Mittenspannung (Stoppspannung Winde) in mV Minimum
//const LIM_PAR_DriveVelocity0mph_MAX                     = 2550;                 // Limit für Mittenspannung (Stoppspannung Winde) in mV Maximum
const LIM_PAR_AutoRunPrgFileNameLength                  = 100;                  // Maximal erlaubte Länge des Dateinamens für die Programmautomatik
const LIM_PAR_VelocityDimensionUnit_MIN					= 0;					// Limit für Darstellung der Geschwindigkeit Minimum
const LIM_PAR_VelocityDimensionUnit_MAX					= 1;					// Limit für Darstellung der Geschwindigkeit Maximum
const LIM_PAR_CalibTractionValue1_MIN                   = 0;                    // Limit für Zugkraftangabe1 für Zugkraftkalibrierung Minimum
const LIM_PAR_CalibTractionValue1_MAX                   = 70000;                // Limit für Zugkraftangabe1 für Zugkraftkalibrierung Maximum
const LIM_PAR_CalibTractionValue2_MIN                   = 0;                    // Limit für Zugkraftangabe2 für Zugkraftkalibrierung Minimum
const LIM_PAR_CalibTractionValue2_MAX                   = 70000;                // Limit für Zugkraftangabe2 für Zugkraftkalibrierung Maximum
const LIM_PAR_CalibDepthLenght_MIN                      = 1;                    // Limit für herausgezogenes Kabel für Teufenkalibrierung Minimum
const LIM_PAR_CalibDepthLenght_MAX                      = 9999999;              // Limit für herausgezogenes Kabel für Teufenkalibrierung Maximum
const LIM_PAR_LOGFileTensionUnit_MIN					= 0;                    // Limit für Auswahl der Zugkrafteinheit für die Protokollierung Minimum
const LIM_PAR_LOGFileTensionUnit_MAX					= 1;                    // Limit für Auswahl der Zugkrafteinheit für die Protokollierung Maximum
const LIM_PAR_LOGFileType_MIN                           = 0;                    // Limit für Auswahl des Protokollierungsdateityps Minimum
const LIM_PAR_LOGFileType_MAX                           = 1;                    // Limit für Auswahl des Protokollierungsdateityps Maximum
const LIM_PAR_SpecialSignalMode_MIN                     = 0;                    // Limit für Auswahl der Sondersignalauswertung Minimum
const LIM_PAR_SpecialSignalMode_MAX                     = 4;                    // Limit für Auswahl der Sondersignalauswertung Maximum
const LIM_PAR_LineLubIntensity_MIN						= 0;                    // Limit für Kabel-/Seilölerintensität Minimum
const LIM_PAR_LineLubIntensity_MAX						= 255;                  // Limit für Kabel-/Seilölerintensität Maximum

// -----------------------------------------------------------------------------
// Fenster-Layout / Objekteinteilung

// Allgemein
const LAYOUT_STD_FONT_HEIGHT_PERC						= 3;	// Standardschriftgröße in % der Fenster-Client-Höhe
const LAYOUT_STD_SMALLFONT_HEIGHT_PERC					= 2;	// Kleine Standardschriftgröße in % der Fenster-Client-Höhe
const LAYOUT_STD_BIGFONT_HEIGHT_PERC					= 4;	// Große Standardschriftgröße in % der Fenster-Client-Höhe
const LAYOUT_STD_BTN_HEIGHT_PERC						= LAYOUT_STD_FONT_HEIGHT_PERC * 2;	// Standardhöhe von Buttons in % der Fenster-Client-Höhe

// Hauptfenster
const LAYOUT_GUI_MinWidth                               = 800;  // Mindestbreite des GUI in px
const LAYOUT_GUI_MinHeight                              = 600;  // Mindesthöhe des GUI in px
const LAYOUT_GUI_Margin                                 = 5;    // algemeiner Abstand von Objekten untereinander und Abstand des Client-Bereichs vom Rand des GUI-Formulars

// Menü-Panel
//const LAYOUT_MenuButton_HEIGHT_PERC                     = 8;    // in % der Fensterhöhe
//const LAYOUT_MenuButtonProp_WIDTH                       = 4;    // Verhältnis der Breite zur Höhe (=Breite/Höhe)
//const LAYOUT_MenuFontProp_Height                        = 3;    // Verhältnis der Höhe der Taste zur Höhe der Tastenschrift(=Tastenhöhe/Schrifthöhe)
const LAYOUT_MenuButtonDistance                         = 10;   // Abstand zwischen den Menütasten

// Kontroll-Panel
const LAYOUT_ControlPanel_WIDTH_PERC                    = 35;   // in % der Fensterbreite
const LAYOUT_ControlPanelButtonSymbolSize               = 32;   // Die Abmessungen der Symbole auf den Tasten in Pixel, welche in ControlPanel angestrebt werden sollen
const LAYOUT_SubCtlPanel_PROP_HEIGHT					= 4;	// Verhältnis der Unter-Steuerungs-Panele zur Fensterhöhe (=Fensterhöhe/Panelhöhe)

//const LAYOUT_MessagePanel_HEIGHT_PERC                   = 8;    // in % der Fenster-Höhe
//const LAYOUT_MessageListBox_HEIGHT_PERC                 = 70;   // in % der MessagePanel-Höhe
//const LAYOUT_MessageHelpPanel1_WIDTH_PERC               = 15;   // in % der MessagePanel-Breite
//const LAYOUT_MessageListFontProp_Height                 = 3;    // Verhältnis der Höhe der MessageListBox zur Höhe der Schrift(=ListBox-höhe/Schrifthöhe)
//const LAYOUT_MessageListFont_Height_PERC                = 3;    // Schriftgröße in % der Höhe der MessageListBox
//const LAYOUT_MessagePanelButton_HEIGHT_PERC				= 5;    // Tastenhöhe in % der Höhe von MessagePanel
//const LAYOUT_MessagePanelButton_PROP_Width				= 4;    // Tastenbreite im Verhältnis zur Tastenhöhe
//const LAYOUT_MessagePanelButton_FontProp_Height         = 2;    // Verhältnis der Höhe der Taste zur Höhe der Tastenschrift(=Tastenhöhe/Schrifthöhe)
//const LAYOUT_MessagePanelButtonSymbolSize               = 32;   // Die Abmessungen der Symbole auf den Tasten in Pixel, welche in MessagePanel angestrebt werden sollen
//const LAYOUT_WarningLightFontProp_Height                = 4;    // Verhältnis der Höhe des Panels zur Höhe der Schrift(=Panelhöhe/Schrifthöhe)

const LAYOUT_MainViewPanel_HEIGHT_PERC                  = 65;   // in % der ControlPanel-Höhe
const LAYOUT_TimeLabel_HEIGHT_PERC                      = 4;    // in % der MainViewHelpPanel1-Höhe
const LAYOUT_ProgramModeLabel_HEIGHT_PERC               = 4;    // in % der MainViewHelpPanel1-Höhe
const LAYOUT_CurrDepthBox_HEIGHT_PERC                   = 13;   // in % der MainViewHelpPanel1-Höhe
const LAYOUT_CurrVelocityBox_HEIGHT_PERC                = 20;   // in % der MainViewHelpPanel1-Höhe

const LAYOUT_StartStopButton_HEIGHT_PERC                = 6;    // in % der ControlPanel-Höhe
const LAYOUT_StartStopFontProp_Height                   = 2;    // Verhältnis der Höhe der Taste zur Höhe der Tastenschrift(=Tastenhöhe/Schrifthöhe)

const LAYOUT_MainCtlPageControl_FontProp_Height			= 10;   // in % der MainCtlPageControl-Höhe
const LAYOUT_SignalHornButton_HEIGHT_PERC               = 25;   // in % der ControlPanel-Höhe
const LAYOUT_SignalHornFontProp_Height                  = 3;    // Verhältnis der Höhe der Taste zur Höhe der Tastenschrift(=Tastenhöhe/Schrifthöhe)
const float LAYOUT_SignalHornButtonProp_WIDTH			= 2;  	// Verhältnis der Breite der Taste zur Höhe der Taste
//const LAYOUT_SignalHornButton_WIDTH_PERC                = 46;   // in % der ControlPanel-Breite
const LAYOUT_ResetButton_HEIGHT_PERC      				= 25;    // in % der WinchDrivePanel-Höhe
const LAYOUT_ResetButtonFontProp_Height         		= 3;    // Verhältnis der Höhe der Taste zur Höhe der Tastenschrift(=Tastenhöhe/Schrifthöhe)
//const LAYOUT_ClearAfterThresholdButton_WIDTH_PERC       = 46;   // in % der ControlPanel-Breite
const float LAYOUT_ResetButtonProp_WIDTH  				= 2;  // Verhältnis der Breite der Taste zur Höhe der Taste

const LAYOUT_CurrTractionView_TranspCol                 = 0x111111; // Transparentfarbe der Zugkraftanzeige
const LAYOUT_CurrTractionView_MeterBackCol              = 0xFFFFFF; // Instrumentenhintergrundfarbe der Zugkraftanzeige

const LAYOUT_MeterBgColor                               = 0xFFFFFF; // Instrumentenhintergrundfarbe allgemein

const LAYOUT_EngineSpeedView_TranspCol              	= 0x111111; // Transparentfarbe der Antriebsmotordrehzahlanzeige

const LAYOUT_HydrWrkPressureView_TranspCol              = 0x111111; // Transparentfarbe der Hydraulikarbeitsdruckanzeige

// const LAYOUT_ButtonPanel_HEIGHT_PERC                    = 10;   // in % der FunctionPanel-Klient-Höhe
//const LAYOUT_EngineButtonProp_HEIGHT_PERC               = 20;    // in % der Höhe des übergeordneten Containers
//const float LAYOUT_EngineButtonProp_WIDTH               = 1.4;  // Verhältnis der Breite der Taste zur Höhe der Taste
//const float LAYOUT_EngineButtonFontProp_Height          = 3.5;  // Verhältnis der Höhe der Taste zur Höhe der Tastenschrift(=Tastenhöhe/Schrifthöhe)
//const LAYOUT_FastRunButtonProp_HEIGHT_PERC              = 25;    // in % der EngineCtlPanel-Höhe
//const float LAYOUT_FastRunButtonProp_WIDTH              = 2;    // Verhältnis der Breite der Taste zur Höhe der Tastenschrift
//const float LAYOUT_FastRunButtonFontProp_Height         = 3;    // Verhältnis der Breite der Taste zur Höhe der Tastenschrift
//const LAYOUT_LostMotionImpulseButton_HEIGHT_PERC        = 25;    // in % der ControlPanel-Höhe
//const float LAYOUT_LostMotionImpulseButtonProp_WIDTH    = 2;    // Verhältnis der Breite der Taste zur Höhe der Tasten
//const float LAYOUT_LostMotionImpulseButtonFontProp_Height = 3.5;// Verhältnis der Breite der Taste zur Höhe der Tastenschrift
//const LAYOUT_WinchBrakeOffSpeedButton_HEIGHT_PERC       = 25;    // in % der ControlPanel-Höhe
//const float LAYOUT_WinchBrakeOffSpeedButton_WIDTH       = 2;  // Verhältnis der Breite der Taste zur Höhe der Taste
//const float LAYOUT_WinchBrakeOffButtonFontProp_Height   = 3.5;  // Verhältnis der Breite der Taste zur Höhe der Taste

// Kontroll-Lampen-Feld
const LAYOUT_ControlLampPanelProp_WIDTH_PERC            = 10;   // in % der Fensterbreite
const LAYOUT_ControlLampPanelSymbolSize                 = 32;   // Die Abmessungen der Symbole (ControlLamps) in Pixel, welche in ControlLampPanel angestrebt werden sollen

// Functions-Panel
const float LAYOUT_KeyBoardPanelProp_HEIGHT             = 3;    // Verhältnis des Keyboard-Panels Länge / Breite

// ChoicePanel
//const LAYOUT_ChoiceButtons_HEIGHT_PERC                  = 7;   	// in % der ChoicePanel-Klient-Höhe
//const LAYOUT_ChoiceLabelFontProp_Height                 = 2;    // Verhältnis der Höhe der Taste zur Höhe der Tastenschrift(=Tastenhöhe/Schrifthöhe)
//const LAYOUT_ChoiceAbortButton_HEIGHT_PERC              = 7;   // in % der ChoicePanel-Klient-Höhe
//const LAYOUT_ChoiceAbortButtonProp_WIDTH                = 4;    // Verhältnis der Breite zur Höhe (=Breite/Höhe)
//const LAYOUT_ChoiceAbortFontProp_Height                 = 3;    // Verhältnis der Höhe der Taste zur Höhe der Tastenschrift(=Tastenhöhe/Schrifthöhe)

// DialogPanel
//const LAYOUT_DialogPanelFont_HEIGHT_PERC                = 3;    // in % der DialogPanel-Klient-Höhe
//const LAYOUT_DialogPanelCaptionFont_HEIGHT_PERC         = 5;    // in % der DialogPanel-Klient-Höhe
//const LAYOUT_DialogPanelButtonProp_WIDTH                = 4;    // Verhältnis der Breite zur Höhe (=Breite/Höhe)

// UnloadCablePanel
//const LAYOUT_UnloadCablePanelButtonProp_WIDTH        	= 4;    // Verhältnis Breite/Höhe
//const LAYOUT_UnloadCablePanelControlFont_HEIGHT_PERC 	= 4;    // in % von ParamPageControl-Klient-Höhe
//const LAYOUT_UnloadCableButtonFontProp_Height        	= 2;    // Verhältnis der Höhe der Taste zur Höhe der Tastenschrift(=Tastenhöhe/Schrifthöhe)
//const LAYOUT_UnloadCablePanelMaskEditProp_WIDTH      	= 6;    // Verhältnis Breite/Höhe

// FileAdminPanel
//const LAYOUT_FileAdminPanelFont_HEIGHT_PERC             = 4;    // in % von FileAdminPanel-Klient-Höhe
//const LAYOUT_FileAdminButton_HEIGHT_PERC                = 7;    // in % der FileAdminPanel-Klient-Höhe
//const LAYOUT_FileAdminButtonProp_WIDTH                  = 4;    // Verhältnis der Breite zur Höhe (=Breite/Höhe)

const LAYOUT_FileAdminComboBoxProp_WIDTH                = 6;    // Verhältnis Breite/Höhe
const char *LAYOUT_LOGFileAdminCurrFileMemo_Font        = "Curier"; // Schriftart für die Dateiansicht
//const LAYOUT_LOGFileAdminCurrFileMemo_Font_HEIGHT_PERC  = 4;    // in % von FileAdminPanel-Klient-Höhe
//const LAYOUT_ProgressGauge_HEIGHT                       = 2;    // in % von FileAdminPanel-Klient-Höhe

// InfoPanel
//const LAYOUT_InfoCaptionLabelFont_Height                = 5;    // in % der InfoPanel-Klient-Höhe
//const LAYOUT_InfoMemoFont_Height                        = 4;    // in % der InfoPanel-Klient-Höhe
//const LAYOUT_InfoOkButton_HEIGHT_PERC                   = 7;   	// in % der InfoPanel-Klient-Höhe
//const LAYOUT_InfoOkButtonFontProp_Height                = 2;    // Verhältnis der Höhe der Taste zur Höhe der Tastenschrift(=Tastenhöhe/Schrifthöhe)
//const LAYOUT_InfoOkButtonButtonProp_WIDTH               = 1;    // Verhältnis der Breite zur Höhe (=Breite/Höhe)

// NormalViewPanel
const LAYOUT_DepthVisualisation_WIDTH_PERC              = 45;   // in % der NormalViewHelpPanel1-Klient-Höhe
const LAYOUT_DepthVisualisation_FrontColor              = clYellow; // Farbe der Röhren-Visualisierung
const LAYOUT_DepthVisualisation_BorderColor             = 0x111111;  // Schatten-Farbe der Röhren-Visualisierung

const LAYOUT_AutoRunProgramListPanel_HEIGHT_PERC        = 22;   // Höhe der Ablaufprogrammpuktliste in % der NormalViewHelpPanel2-Klient-Höhe

const LAYOUT_LED_SIZE                                   = 7;    // in % der NormalViewHelpPanel1-Klient-Höhe
const LAYOUT_LED_LABEL_HEIGHT_PERC                      = 55;   // in % der LED-Größe

const LAYOUT_Entry_HEIGHT_PERC                          = 3;    // in % der FunctionPanel-Klient-Höhe
const LAYOUT_Label_HEIGHT_PERC                          = 90;   // in % von LAYOUT_Entry_HEIGHT_PERC

const LAYOUT_MaskEditProp_WIDTH                         = 4;    // Verhältnis der Breite zur Höhe (=Breite/Höhe)
const LAYOUT_UpDownProp_WIDTH                           = 2;    // Verhältnis der Breite zur Höhe (=Höhe/Breite)
const LAYOUT_UpDownProp_HEIGHT_PERC                     = 95;   // in % der DifferencesBox-Höhe
const LAYOUT_TractionBoxDifferenceBox_HEIGHT_PERC       = 20;   // in % der FunctionPanel-Klient-Höhe
const LAYOUT_DifferencesBoxMaskEdit_HEIGHT_PERC         = 30;   // in % der GroupBox-Höhe
const LAYOUT_TractionBoxDifferenceBoxFont_HEIGHT_PERC   = 20;   // in % der GroupBox-Höhe
const LAYOUT_MaxTractionBox_WIDTH_PERC                  = 30;   // in % der NormalViewHelpPanel1-Klient-Breite
const LAYOUT_DifferencesBox_WIDTH_PERC                  = 50;   // in % der NormalViewHelpPanel1-Klient-Breite
const LAYOUT_SetVelocityUPDownButton_HEIGHT_PERC        = 13;   // in % der NormalViewHelpPanel1-Klient-Höhe

// ParameterPanel
const LAYOUT_ParamPageControlFont_HEIGHT_PERC           = 4;    // in % von ParamPageControl-Klient-Höhe
//const LAYOUT_ButtonProp_WIDTH                           = 4;    // Verhältnis Breite/Höhe
//const LAYOUT_MaskEditProp_WIDTH                         = 6;    // Verhältnis Breite/Höhe

//const LAYOUT_PTractionViewComboBoxProp_WIDTH            = 6;    // Verhältnis Breite/Höhe
//const LAYOUT_PWinchComboBoxProp_WIDTH                   = 6;    // Verhältnis Breite/Höhe

// CalibTractionPanel
//const LAYOUT_CalibTractionCaptionFont_Height            = 5;    // in % von CalibTractionPanel
//const LAYOUT_CalibTractionButton_HEIGHT_PERC            = 7;    // in % von CalibTractionPanel
//const LAYOUT_CalibTractionButtonFont_Prop               = 2;    // Verhältnis zur Tastenhöhe (Schrifthöhe = Tastenhöhe / x)
//const LAYOUT_CalibTractionButton_Prop_WIDTH             = 4;    // Verhältnis zur Tastenhöhe (Breite = Höhe * x)
//const LAYOUT_CalibTractionMaskEdit_Prop_WIDTH           = 4;    // Verhältnis zur MaskEdit-Höhe (Breite = Höhe * x)

// CalibDepthPanel
//const LAYOUT_CalibDepthCaptionFont_Height               = 5;    // in % von CalibDepthFramePanel
//const LAYOUT_CalibDepthButton_HEIGHT_PERC               = 7;    // in % von CalibDepthFramePanel
//const LAYOUT_CalibDepthButtonFont_Prop                  = 2;    // Verhältnis zur Tastenhöhe (Schrifthöhe = Tastenhöhe / x)
//const LAYOUT_CalibDepthButton_Prop_WIDTH                = 4;    // Verhältnis zur Tastenhöhe (Breite = Höhe * x)

// SystemHelpPanel
//const LAYOUT_SystemPanelFont_HEIGHT_PERC                = 4;    // in % von SystemPanel->ClientHeight
//const LAYOUT_SystemPanelButton_HEIGHT_PERC              = 7;   	// in % der SystemPanel->ClientHeight
//const LAYOUT_SystemPanelButtonProp_WIDTH                = 8;    // Verhältnis Breite/Höhe

// -----------------------------------------------------------------------------
// Farbfestlegungen
const COLOR_OK                          = clGreen;      // Farbe, wenn alles OK
const COLOR_ATTENTION                   = clYellow;     // Farbe, wenn Aufmerksamkeit erforderlich
const COLOR_ALERT                       = clRed;        // Farbe, wenn Fehler/Alarm ausgelöst

const COLOR_NORMAL_SIGNAL_OCCOURS       = clGreen;      // Farbe, für die Signalisierung eines normalen(erlaubten) Ereignisses
const COLOR_NEUTRAL_SIGNALISATION       = clBtnFace;    // Farbe, für die Signalisierung eines normalen(erlaubten) Ereignisses

const COLOR_MM_OCCOURS					= 0x00FF80;		// Farbe, zur Signalisierung einer Magnetmarke auf einem Flächenelement
const COLOR_NO_MM						= clBtnFace;    // Farbe, zur Signalisierung auf einem Flächenelemen, wenn keine Magnetmarke auftritt

// Temperatursignalfarben
const COLOR_TEMPERATURE_COLD			= 0xFF8000;		// Farbe, die kalt darstellt
const COLOR_TEMPERATURE_NORMAL         	= 0x40FF40;		// Farbe, die normale Temperatur darstellt
const COLOR_TEMPERATURE_HOT				= 0x0080FF;		// Farbe, die heiß darstellt

// LED-Farben
const COLOR_LED_FRAME_SHADOW            = 0x555555;     // Schattenfarbe des LED-Rahmens (links)
const COLOR_LED_FRAME_Lighted           = 0xFFFFFF;     // Farbe Angeleuchtete Seite des LED-Rahmens (rechts)

const COLOR_LED_ON_GREEN_CENTER         = 0x00FF80;
const COLOR_LED_ON_GREEN_BORDER         = 0x003000;
const COLOR_LED_OFF_GREEN_CENTER        = 0xFFFFFF;
const COLOR_LED_OFF_GREEN_BORDER        = 0x777777;

const COLOR_LED_ON_RED_CENTER           = 0x5555FF;
const COLOR_LED_ON_RED_BORDER           = 0x000055;
const COLOR_LED_OFF_RED_CENTER          = 0xFFFFFF;
const COLOR_LED_OFF_RED_BORDER          = 0x777777;

const COLOR_LED_ON_YELLOW_CENTER        = 0x00FFFF;
const COLOR_LED_ON_YELLOW_BORDER        = 0x005555;
const COLOR_LED_OFF_YELLOW_CENTER       = 0xFFFFFF;
const COLOR_LED_OFF_YELLOW_BORDER       = 0x777777;

// Eingabefelder-Farben
const COLOR_EDITFIELD_BACKKGR_NORMAL    = clWindow;     // Farbe der Standard-Eingabefelder im Normalfall
const COLOR_EDITFIELD_BACKKGR_INVALID   = 0x0000FF;     // Farbe der Standard-Eingabefelder bei ungültigen Werten

// -----------------------------------------------------------------------------
// Akustische Signalisierungen / Alarmschemen
const ACOUSTICALARMORWARNINGOFF         = 0;            // Alarm und Warnung aus
const ACOUSTICWARNING                   = 1;            // Warnung UNKRITISCH
const ACOUSTICALARM                     = 3;            // Alarm KRITISCH

// -----------------------------------------------------------------------------
// Virtuelles Keyboard
// Tastengruppen-Konstanten
const unsigned int _VKB_ESC              = 0x00000001;                          // ESC
const unsigned int _VKB_TAB              = 0x00000002;                          // TAB
const unsigned int _VKB_CAPS_SHIFT       = 0x00000004;                          // CAPS-LOCK und Shift
const unsigned int _VKB_CTRL             = 0x00000008;                          // Control
const unsigned int _VKB_ALT              = 0x00000010;                          // ALT
const unsigned int _VKB_SPACE            = 0x00000020;                          // Space
const unsigned int _VKB_LAYOUT           = 0x00000040;                          // Keyboard-Layout
const unsigned int _VKB_BACKSPACE        = 0x00000080;                          // Backspace
const unsigned int _VKB_RETURN           = 0x00000100;                          // Return
const unsigned int _VKB_ARROW            = 0x00000200;                          // Pfeiltasten
const unsigned int _VKB_DEL              = 0x00000400;                          // DEL
const unsigned int _VKB_INS              = 0x00000800;                          // INS
const unsigned int _VKB_HOME             = 0x00001000;                          // HOME
const unsigned int _VKB_END              = 0x00002000;                          // END
const unsigned int _VKB_ALPHA            = 0x00004000;                          // Alle sonstigen Tasten, außer Ziffern und Komma usw.
const unsigned int _VKB_NUMERIC          = 0x00008000;                          // Alle Zifferntasten (+ Komma usw.)

// -----------------------------------------------------------------------------
// Starteinstellungen
const PROGRAM_START_DELAY               = 0;                                    // Zeitverzögerung in ms, die beim Programmstart auftritt, bevor die Anwendung mit exzessiver Systemauslastung beginnt

// Startup-Zyklen der Threads
const   ControlThread_STARTUP_DELAY     = 1000;                                 // Wartezeit seit dem Anwendungsstart in ms, bis der ControlThread mit der Arbeit beginnen soll
const   DisplayThread_STARTUP_DELAY     = 2000;                                 // Wartezeit seit dem Anwendungsstart in ms, bis der DisplayThread mit der Arbeit beginnen soll

// zuerst sichtbares Panel im Funktions-Bereich
const char STARTUP_FunctionPanel        = INR_NormalViewPanel;
const STARTUP_ProgramMode               = 1;            // 1...Modus "Programm-Start"

// -----------------------------------------------------------------------------
// Darstellung
const VIEW_DEPTHACCURACY                = 2;            // Nachkommastellen bei der Teufenanzeige
const VIEW_VELOCITYACCURACY             = 0;            // Nachkommastellen bei der Geschwindigkeitsanzeige
// -----------------------------------------------------------------------------
// Fehler-Meldungs-Einstellungen
const ERRORNUM							= 47;			// Gibt die Anzahl der möglichen Fehler an
const ERRORARRAYSIZE					= ERRORNUM + 1;
const bool ERRORONLYWARNING[ERRORARRAYSIZE] =
										{				// Hier gesetzte Flags signalisieren, welcher "Fehler" nur als Warnung zu verstehen ist
											_OFF,		// Dummy für keinen Fehler
											_OFF,		// Schnittstelle zum Teufenrechner konnte nicht geöffnet werden.
											_OFF,		// Kommunikationsfehler mit Teufenrechner (dieser antwortet nicht, oder nicht korrekt)
											_ON,   		// Schnittstelle zur Informationsausgabe konnte nicht geöffnet werden
											_OFF,  		// DIO-Relay-Karte nicht gefunden
											_OFF,  		// DAC-Karte nicht gefunden
											_OFF,  		// Für DIO-Relay-Karte konnte kein Speicher reserviert werden
											_OFF,  		// Für DAC-Karte konnte kein Speicher reserviert werden
											_OFF,  		// Inkrementalgeber ausgefallen
											_ON,   		// Außerhalb des angegebenen Teufenbereichs
											_ON,   		// Max. Zugkraft überschritten
											_ON,   		// Parameterdatei konnte nicht eingelesen werden
											_ON,   		// Parameterdatei konnte nicht geschrieben werden
											_ON,   		// Fehler beim Schreiben der LOG-Daten
											_ON,   		// Kopieren der LOG-Datei(en) fehlgeschlagen
											_OFF,  		// WinchControl scheint bereits aktiv zu sein
											_ON,   		// Spezielle Parameterdatei konnte nicht geschrieben werden
											_OFF,  		// Fehler Steuergerät
											_OFF,  		// Fahrzeug-Fehler
											_ON,   		// Löschen der LOG-Datei(en) fehlgeschlagen
											_ON,   		// Zu wenig Speicherplatz für Protokollierung
											_ON,   		// Zu wenig Speicherplatz für interne Protokollierung
											_OFF,  		// Schaltgetriebe nicht korrekt umgeschaltet
											_OFF,  		// Power Fail
											_OFF,  		// Fehler 1 einer externen Apparatur
											_OFF,  		// Fehler 2 einer externen Apparatur
											_OFF,  		// Mast ungesichert
											_OFF,       // Steuergeräte-CAN: Bibliothek nicht gefunden
											_OFF,       // Steuergeräte-CAN: Schnittstelle nicht initialisiert
											_OFF,        // Steuergeräte-CAN: Kommunikationsfehler
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF
										};
const bool ERRORHIDE[ERRORARRAYSIZE] =	{				// Hier gesetzte Flags definieren Fehler/Warnungen, die nur im Entwicklermodus "-devel" angezeigt werden
											_OFF,		// Dummy für keinen Fehler
											_OFF,		// Schnittstelle zum Teufenrechner konnte nicht geöffnet werden.
											_OFF,		// Kommunikationsfehler mit Teufenrechner (dieser antwortet nicht, oder nicht korrekt)
											_OFF,   	// Schnittstelle zur Informationsausgabe konnte nicht geöffnet werden
											_OFF,  		// DIO-Relay-Karte nicht gefunden
											_ON,  		// DAC-Karte nicht gefunden
											_OFF,  		// Für DIO-Relay-Karte konnte kein Speicher reserviert werden
											_OFF,  		// Für DAC-Karte konnte kein Speicher reserviert werden
											_OFF,  		// Inkrementalgeber ausgefallen
											_OFF,   	// Außerhalb des angegebenen Teufenbereichs
											_OFF,   	// Max. Zugkraft überschritten
											_OFF,   	// Parameterdatei konnte nicht eingelesen werden
											_OFF,   	// Parameterdatei konnte nicht geschrieben werden
											_OFF,   	// Fehler beim Schreiben der LOG-Daten
											_OFF,   	// Kopieren der LOG-Datei(en) fehlgeschlagen
											_OFF,  		// WinchControl scheint bereits aktiv zu sein
											_OFF,   	// Spezielle Parameterdatei konnte nicht geschrieben werden
											_OFF,  		// Fehler Steuergerät
											_OFF,  		// Fahrzeug-Fehler
											_OFF,   	// Löschen der LOG-Datei(en) fehlgeschlagen
											_OFF,   	// Zu wenig Speicherplatz für Protokollierung
											_ON, 	  	// Zu wenig Speicherplatz für interne Protokollierung
											_OFF,  		// Schaltgetriebe nicht korrekt umgeschaltet
											_OFF,  		// Power Fail
											_OFF,  		// Fehler 1 einer externen Apparatur
											_OFF,  		// Fehler 2 einer externen Apparatur
											_OFF,  		// Mast ungesichert
											_OFF,       // Steuergeräte-CAN: Bibliothek nicht gefunden
											_OFF,       // Steuergeräte-CAN: Schnittstelle nicht initialisiert
											_OFF,       // Steuergeräte-CAN: Kommunikationsfehler
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
											_OFF,
										};

// -----------------------------------------------------------------------------
// Verfahrenskonstanten
// Dynamische Maximal-Zugkraft-Berechnung
const           PROOF_MAX_TRACTION_SAMPLES      = 20;           // Anzahl der Messproben für dynamische Maximal-Zugkraftberechnung
const float     PROOF_MAX_TRACTION_GRUB         = 2.557;        // nach Tabelle bei 20 Stichproben
//const float     PROOF_MAX_TRACTION_LOS          = 0.05;         // Signifikanzniveau (LOS...Level Of Significance)

// Schleifenfänger
// Für Reaktion auf Abfall der Zugkraft bei Abwärtsfahrt
//const   TENSION_DECREASE_CALC_BUFF_SIZE = 60;                   // Größe entspricht der Maximalen Zeit in Sekunden, die für den Schleifenfänger eingestellt werden kann

// Schwellen
const   MAX_IGNORED_VELOCITY                    = 150;          // Geschwindigkeit, die noch als 0 m/h angesehen wird (in m/h)
const   MAX_IGNORED_VELOCITY_OFF_THRESHOLD      = 100;          // Geschwindigkeit, die noch als 0 m/h angesehen wird (in m/h)
const	WINCHBRAKEINTERNAUTO_VELOCITY_THRES		= 5;			// Geschwindigkeitsschwelle, bei deren Unterschreitung die intern gesteuerte Automatische Windenbremse anhalten kann.
const	WINCHBRAKEINTERNAUTO_VELO_HLDLNGTH		= 2000;			// Haltedauer des Geschwindigkeitsbereich, ehe die Windenbremse angezogen wird
const   INCENCERROR_AUTORUN_VOLTAGEDIFF         = 300;          // Spannungsdifferenz in mV für das Auslösen des Fehlers bei Teufenimpulsausfalls für Automatikfahrt
const   CONTROLLERERROR_AUTORUN_VOLTAGEDIFF     = 300;          // Spannungsdifferenz in mV für das Auslösen des Steuergerät-Fehlers für Automatikfahrt
//const   MIN_DISCHARGE_CABLE_VELOCITY            = 70;           // Geschwindigkeit, die für das Entladnen des Kabels mindestens erreicht sein muss (in mm/s)
//const float MIN_DISCHARGE_CABLE_VOLTAGE         = AO_WinchRevZeroBy; // Mindestspannung, bei der Zugkraftregelung beim aktiven Entladen des Kabels anliegen muss. Damit soll zurückdrehen der Winde Minimiert/Gestoppt werden. Sollte nicht über AO_WinchRevZeroBy liegen, sonst dreht sich die Winde an dieser Stelle immer vorwärts!!!
const   GET_DEPTH_STEP                          = 100;          // Mindest-Zeitabstand in ms für Teufenrechnerabfrage
const   GET_VELOCITY_CALC_STEP                  = 300;          // Mindest-Zeitabstand in ms, für Berechnung der Geschwindigkeit (notwendig für langsame Geschwindigkeiten)
const   GET_TRACTION_NOISE_DAMP_TIME            = 1000;         // Zeitdauer, für die Berechnung der Annäherungsstufen der momentan im Programm bekannten Zugkraft zur tatsächlich vorhandenen Zugkraft (für imag. Filterung schneller Zugkraft-Schwankungen) je größer Zeit, desto gedämpfter die Zugkaft
const   GET_TRACTION_VIEW_NOISE_DAMP_TIME       = 400;          // Zeitdauer, für die Berechnung der Annäherungsstufen des momentan im Programm dargestellten Zugkraft zur tatsächlich vorhandenen Zugkraft (für imag. Filterung schneller Zugkraft-Schwankungen) je größer Zeit, desto gedämpfter die Zugkaftanzeige
const   GET_VELOCITY_NOISE_DAMP_TIME            = 200;          // Zeitdauer, für die Berechnung der Annäherungsstufen der momentan im Programm bekannten Geschwindigkeit zur tatsächlich vorhandenen Geschwindigkeit (für imag. Filterung schneller Geschwindigkeits-Schwankungen) je größer Zeit, desto gedämpfter die Geschwindigkeit
const   GET_ANNOUNCED_VELOCITY_NOISE_DAMP_TIME  = 1000;         // Zeitdauer, für die Berechnung der Annäherungsstufen der angezigten Geschwindigkeit zur tatsächlich vorhandenen Geschwindigkeit (für imag. Filterung schneller Geschwindigkeits-Schwankungen) je größer Zeit, desto gedämpfter die Geschwindigkeitsanzeige
const   GET_VELOCITY_NOISE_DAMP_JUMPTOZERO_VELOCITY = 10;       // m/h; Wird aus großer Geschwindigkeit (> GET_VELOCITY_NOISE_DAMP_JUMPTOZERO_VELOCITY) auf v = 0 abgebremst, dann wird die interne Geschwindigkeit auch auf 0 gesetzt. (Bei langsamen Geschw. darf das nicht getan werden, da Aussetzer bei der Berechnung auftreten können

// Geschwindigkeitsermittlung
const VELOCITY_CALC_TIMEDIFF_FOR_1MPH           = 3600;         // notwendige Zeitdifferenz zur Geschwindigkeitsberechnung, um eine Genauigkeit von 1m/h zu erreichen
const VELOCITY_CALC_BUF_SIZE                    = (VELOCITY_CALC_TIMEDIFF_FOR_1MPH / GET_DEPTH_STEP) + 1; // Puffergröße für Geschwindigkeitsermittlung (ist Abhängig vom Teufenabfrageintervall) / es muss mindestens 3,6s Zeitdifferenz zwischen den Messwerten haben, um Geschwindigkeit auf 1m/h genau ermitteln zu können, da Teufenauflösung 1mm ist

// Zugkraftkalibrierung
const CalibTension_AverageBuffer_SIZE           = 20;           // Größe des Puffers für Mittelwertbildung für Zugkraftkalibrierung
const CalibTension_AverageBuffer_CLOCK			= 100;			// Takt in ms für das Füllen des Puffers für Mittelwertbildung für Zugkraftkalibrierung
const CalibTension_NumOfCalMeasuringPoints		= 2;			// Anzahl der Messpunkte für die Zugkraftkalibrierung

// Zugkraftregelung (entladen des Kabels)
const UNLOAD_CABLE_PID_REGULATOR_MIN_CLOCK   	= 30;           // Diese Zeit muss mindestens vergehen, bis neue Regelungsberechnung stattfindet (Thread-Wiederholzeit ist dynamisch - OS und Hardware bestimmen das)

// Verzögerung des Winden-Anhaltevorgangs bei Schwellwertüberschreitung Teufe/Zugkraft
const THRESHOLD_EXCEEDENCE_STOP_DELAY           = 1000;         // Die Verzögerung in ms bis zum Ausschalten des Steuergerät bei Schwellwertüberschreitung (gleichzeitig Haltedauer der Mittenspannung durch den STR)

// Temperaturerfassung
const TEMPERATURE_ACCEPTED_MINIMUM              = -100;         // Kleinstmögliche akzeptierte Messtemperatur überhaupt in °C
const TEMPERATURE_ACCEPTED_MAXIMUM              = 200;          // Größtmögliche akzeptierte Messtemperatur überhaupt in °C
const TEMPERATURE_AQUISITION_CLOCK              = 1000;        	// Temperaturerfassungsinterfall in ms
const TEMPERATURE_SENSOR_DEPTHCALC_OFF_DETECT_H = 140;          // höchste Temperatur, ab der der Temperatur-Sensor des Teufenrechners als außer Betrieb angenommen wird
const TEMPERATURE_SENSOR_DEPTHCALC_OFF_DETECT_L = -40;          // kleinste Temperatur, ab der der Temperatur-Sensor des Teufenrechners als außer Betrieb angenommen wird
const TEMPERATURE_SENSOR_A_OFF_DETECT_H         = 140;          // höchste Temperatur, ab der der Temperatur-Sensor A als außer Betrieb angenommen wird
const TEMPERATURE_SENSOR_A_OFF_DETECT_L         = -60;          // kleinste Temperatur, ab der der Temperatur-Sensor A als außer Betrieb angenommen wird
const TEMPERATURE_SENSOR_B_OFF_DETECT_H         = 140;          // höchste Temperatur, ab der der Temperatur-Sensor B als außer Betrieb angenommen wird
const TEMPERATURE_SENSOR_B_OFF_DETECT_L         = -60;          // kleinste Temperatur, ab der der Temperatur-Sensor B als außer Betrieb angenommen wird

const TEMPERATURELVL_HYDRTANK_COLD				= 0;			// Hydrauliktanktemperatur in °C, bei deren Unterschreitung "Kalt" signalisiert wird
const TEMPERATURELVL_HYDRTANK_HOT				= 60;			// Hydrauliktanktemperatur in °C, bei deren Überschreitung "Heiß" signalisiert wird
const TEMPERATURE_HYDRTANK_MIN					= -40;			// Mindesttemperatur Hydrauliktankanzeige
const TEMPERATURE_HYDRTANK_MAX					= 90;			// Maximaltemperatur Hydrauliktankanzeige
const TEMPERATURE_HYDRTANK_SCALFREQ				= 10;			// Skalenstrichfrequenz für die Hydrauliktankanzeige

const TEMPERATURELVL_ENGINECOOLANT_COLD			= 60;           // Motorkühlflüssigkeitstemperatur in °C, bei deren Unterschreitung "Kalt" signalisiert wird
const TEMPERATURELVL_ENGINECOOLANT_HOT			= 95;           // Motorkühlflüssigkeitstemperatur in °C, bei deren Unterschreitung "Heiß" signalisiert wird
const TEMPERATURE_ENGINECOOLANT_MIN				= -20;          // Mindesttemperatur Motorkühlflüssigkeitstemperaturanzeige
const TEMPERATURE_ENGINECOOLANT_MAX				= 120;          // Maximaltemperatur Motorkühlflüssigkeitstemperaturanzeige
const TEMPERATURE_ENGINECOOLANT_SCALEFREQ		= 10;          	// Skalenstrichfrequenz für die Motorkühlflüssigkeitstemperaturanzeige

const TEMPERATURE_SENSOR_DEPTHCALC_CRITICAL     = 40;           // kritische Temperatur auf dem Teufenrechner, ab der reagiert werden soll

// Signal: Ölfilter wechseln
const OILSTRAINER_TRIGGER_TIME                  = 1000;         // Triggerzeit in ms, zum Entprellen des Ein-Zustandes des Ölfilters (auch ein "flackerndes" Filtersignal stört so nicht)

// Mittenspannungslimits (0mph-Ansteuerungsspannungsreferenz)
//const WINCHREVMESSVOLTAGE_MINVOLT_0MPHREFERENCE = LIM_PAR_DriveVelocity0mph_MIN;         // Min. Spannung in mV, die für die Einstellung der Mittenspannung des Handsteuergebers akzeptiert wird
//const WINCHREVMESSVOLTAGE_MAXVOLT_0MPHREFERENCE = LIM_PAR_DriveVelocity0mph_MAX;         // Max. Spannung in mV, die für die Einstellung der Mittenspannung des Handsteuergebers akzeptiert wird

// Steuergerät Steuerspannungsmessung
const WINCHREVMESSVOLTAGE_AVERAGEBUFFER_SIZE    = 40;           // Puffergröße für Gemessene Steuerspannung Steuergerät für Störungsfilter
const WINCHREVMESSVOLTAGE_AQUISITION_CLOCK      = 40;           // Takt der Abfrage in ms

// Hydraulikarbeitsdruckmessung
const HYDRWRKPRESSURE_AVERAGEBUFFER_SIZE		= 20;			// Puffergröße für erfassten Hydraulikarbeitsdruck, als Störfilter
const HYDRWRKPRESSURE_AQUISITION_CLOCK      	= 40;           // Takt der Abfrage in ms

// Hydraulikfüllstand
const HYDRLVL_LOW								= 30;			// Füllstandsniveau Hydrauliktank Untergrenze in %
const HYDRLVL_HIGH								= 80;			// Füllstandsniveau Hydrauliktank Obergrenze in %

// Ausgabe für Fremdapparaturen
const TENSIONOUT_KEDR_MIN_Voltage               = (float)(AO_TensionOutMinVoltage);// Mindest-Spannung in mV, welche ausgegeben werden darf
const TENSIONOUT_KEDR_MAX_Voltage               = (float)(AO_TensionOutMaxVoltage);// Maximal-Spannung in mV, welche ausgegeben werden darf
const TENSIONOUT_KEDR_MIN_Tension               = 0;            // Referenzzugkraft in N, zur Mindestausgabesannung
const TENSIONOUT_KEDR_MAX_Tension               = 98067;        // Referenzzugkraft in N, zur Maxmalausgabesannung

// Automatikfahrt Geschwindigkeitssteuerung
const AUTORUNVOLTAGE_PID_LIMIT_e                = 500;          // PID-Regler e-Limit (Regelabweichungslimit)
const AUTORUNVOLTAGE_PID_I_LIMIT_esum           = 100000;       // PID-Regler I-Anteil esum-Limit

// Sondersignale
const SPECIALSIGNAL_V01_DELAY                   = 1000;         // Verzögerung der Aktivierung der Meldung für die Variante 1 der Sondersignalisierung ("Schaltgetriebe nicht korrekt umgeschaltet")

// -----------------------------------------------------------------------------
// Kommunikation mit externem Steuergerät
// Allgemeine Meldungen zum Steuergerät
const HELLO_WINCHCLEAR_DURATION					= 2000;			// Zeitdauer des "Hallo's" über das WinchClear-Signal
const CTLUNIT_SW_VERSION_NAMELENGTH 			= 13;           // Puffergröße in Byte für den Versionsstring des Steuergeräts

// -----------------------------------------------------------------------------
// LOG-Datei
const           LOG_Path_SIZE                   = 255;                          // Die maximale Zeichenanzahl in LOG_Path + EK
const char      LOG_Path[LOG_Path_SIZE]         = "C:\\WinchCtrlLOG\\";         // Pfad für die LOG-Datei
//const char      *LOG_tmpFile                    = "tmp.LOG";                    // temporäre LOG-Datei
//const char      *LOG_FileExt_Standard           = ".LOG";                       // Dateierweiterung für die LOG-Datei Typ Standard
//const wchar_t   *w_LOG_FileExt_Standard         = L".LOG";                      // Dateierweiterung für die LOG-Datei Typ Standard
const char      *LOG_FileExt_GEOSYSV1           = ".LOG";                       // Dateierweiterung für die LOG-Datei Typ Geo Sys V1
const wchar_t   *w_LOG_FileExt_GEOSYSV1         = L".LOG";                      // Dateierweiterung für die LOG-Datei Typ Geo Sys V1
const char      *LOG_FileExt_GEOSYSV2           = ".LG2";                       // Dateierweiterung für die LOG-Datei Typ Geo Sys V2
const wchar_t   *w_LOG_FileExt_GEOSYSV2         = L".LG2";                      // Dateierweiterung für die LOG-Datei Typ Geo Sys V2
const char      *LOG_FileExt_LAS                = ".LAS";                       // Dateierweiterung für die LOG-Datei Typ LAS
const wchar_t   *w_LOG_FileExt_LAS              = L".LAS";                      // Dateierweiterung für die LOG-Datei Typ LAS
const char      *LOG_Identification_Prefix      = "IDENT: ";                    // Ein Präfix für die Identifikationszeile
const char      *LOG_Identification_Suffix      = " --- LOG";                   // Ein Suffix für die Identifikationszeile

// Auskommentiert am 21.05.2011
// Kann nach erfolgreicher Testphase deaktiviert werden
// const           LOG_Transfusion_Destinations_Number = 1;                        // Anzahl der Ziele für die LOG-Datei-Datenübertragung
// const char      *LOG_Transfusion_Destinations[LOG_Transfusion_Destinations_Number] =
//                                                {"E:"};                         // Mögliche Ziele für die LOG-Datei-Übertragung

const           LOG_MIN_ENTRYS                  = 2;                            // Mindestanzahl der Einträge in der LOG-Datei, das diese gesichert werden soll
const           LOG_DEPTH_INTERVAL              = 10;                           // Teufenintervall je geschriebenen LOG-Datensatz (Angabe im Standardteufenformat)
const           LOG_TIME_INTERVAL               = 1;                            // Zeitintervall je geschriebenen LOG-Datensatz (Angabe in Sekunden)

const           LOG_LOCATION_LENGHT             = 80;                           // max. Länge der Ortsangabe
const           LOG_WELL_LENGHT             	= 80;                           // max. Länge der Bohrloch
const           LOG_NAME_LENGHT                 = 80;                           // max. Länge der Namensangabe
const           LOG_COMMENT_LENGHT              = 500;                          // max. Länge des Kommentartexts
const           LOG_TEAM_LENGTH					= 80;							// max. Länge der Truppangabe
const           LOG_WINCHMACHINERY_LENGTH       = 80;                           // max. Länge der Windenangabe
const           LOG_Col_Width_General           = 10;                           // Generelle Spaltenbreite der Wertetabelle

const           LOGFILE_GEOSYSV1             	= 0;                            // LOG-Dateityp Geo Sys-Format (obsolet)
const           LOGFILE_LAS2                    = 1;                            // LOG-Dateityp LAS 2.0
const           LOGFILE_GEOSYSV2             	= 2;                            // LOG-Dateityp Geo Sys-Format V2

const			LOGFILE_TENSIONUNIT_kN			= 0;							// Zugkrafteinheit beim Protokollieren ist kN
const			LOGFILE_TENSIONUNIT_kp			= 1;							// Zugkrafteinheit beim Protokollieren ist kp (kg)
//const           LOGFILE_STANDARD                = LOGFILE_GEOSYSV2;             // LOG-Dateityp Standard
// -----------------------------------------------------------------------------
// Interne LOG-Datei
const           LOG_Path_internal_SIZE          = 255;                          // Die maximale Zeichenanzahl in LOG_Path_internal + EK
const char      LOG_Path_internal[LOG_Path_internal_SIZE] = "C:\\mt.res";       // Pfad für die interne LOG-Datei
//const char      *LOG_Path_MaxThreshold          = "D:\\mt.res";                 // Pfad für die interne Maximal-Zugkraft-Überschreitungs-LOG-Datei
// -----------------------------------------------------------------------------
// Speicherplatz
const           LOG_DiskSpace_Test_Clock        = 5000;                         // Takt in ms, in welchem der Datenträgerspeicherplatz überprüft werden soll 
const           LOG_DiskSpace_sleep             = 536870912;                    // Grenze in Byte, ab wann das Protokollieren aufgrund Speicherplatz-Mangels eingestellt werden soll
const           LOG_DiskSpace_wakeup            = 641728512;                    // Grenze in Byte, ab wann das Protokollieren, nachdem es einmal eingestellt wurde, wieder aufgenommen werden soll
const           LOGinternal_DiskSpace_sleep     = 268435456;                    // Grenze in Byte, ab wann das interne Protokollieren aufgrund Speicherplatz-Mangels eingestellt werden soll
const           LOGinternal_DiskSpace_wakeup    = 373293056;                    // Grenze in Byte, ab wann das interne Protokollieren, nachdem es einmal eingestellt wurde, wieder aufgenommen werden soll
// -----------------------------------------------------------------------------
// LOG-Datei Validierung
#define __CRC16_1_POLYNOM                       0x11021                         // Generatorpolynom CRC-16 nach CCITT: x^16+x^12+x^5+1
const char      *LOG_VALIDATION_FileExt         = ".VAL";                       // Dateierweiterung für die Validierungsdateien der LOG-Dateien
const char      LOG_VALIDATION_TYPCODE_1[2]     = {0x00, 0x01};                 // Typifizierungscode für Validierung TYP 1
const char      *LOG_VALIDATION_TYPSTRING_1     = "TYP 1";                      // Typifizierungszeichenkette für Validierung TYP 1
const char      *LOG_VALIDATION_IDENTSTRING_PRE_1 = "Validation file generated by "; // Präampel für Dateiidentifikationszeichenkette für Validierung TYP 1
const char      LOG_VALIDATION_SEPARATOR_1      = 0x00;                         // Definition des Trennbytes innerhalb der Validierungsdatei Validierung TYP 1 

// -----------------------------------------------------------------------------
// Motorsteuerung (Fahrzeug)
const           ENGINE_STARTSTOP_DELAY          = 2000;                         // Verzögerung nach Tastendruck bis zum Start oder Stopp des Motors
const           ENGINE_STOPSIGNALOFF_DELAY      = 2000;                         // Mindestdauer, die das Motor-Stop-Relais anziehen soll, um sicheres Motorausschalten zu gewährleisten
const           ENGINE_REVOLUTION_AUTO_UPDOWN_DELAY = 2000;                     // Die Dauer, wie lange die Motordrehzahl-Automatik aktiv sein soll, wenn sie einmal aktiviert wurde

// -----------------------------------------------------------------------------
// Programende
const           SHUTDOWN_DELAY                  = 3;                            // Dauer, bis zum Systemshutdown in Sekunden
const char      *SHUTDOWN_SYSTEM_REBOOT_MESSAGE = "System Reboot";              // Meldungen beim Herunterfahren des Rechners
const char      *SHUTDOWN_SYSTEM_HALT_MESSAGE   = "System Halt";

// Power Fail
const           POWERFAIL_SHUTDOWN_DELAY        = 30;                           // Die Verzögerungszeit in Sekunden, die nach Abschalten der Hauptstromversorgung vom STR noch gewartet werden soll, bis das System automatisch herunterfahren soll
//const wchar_t   *EVENT_NAME_POWERFAIL           = L"GEOSYSPOWERFAIL";           // Der Ereignisname des durch eine Zweitsoftware bereitgestellten PowerFail-Ereignisses
const           POWERFAIL_TRIGGER_TIME          = 1000;                         // Triggerzeit in ms bis zum Auslösen des Power-Fail-Signals (zum Entprellen des Signals)

// -----------------------------------------------------------------------------
// Absolute Grenzwerte, die wegen der Produkt-Garantie vergeben werden
const           ABSOLUTE_MAX_TRACTION           = 49033;                        // Maximal zulässige Zugkraft in Newton, die speziell geloggt wird, sobald überschritten (INTERN)

#endif  // __CONSTANTS_H

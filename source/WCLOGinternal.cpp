//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WCLOGinternal.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)


//---------------------------------------------------------------------------
// Konstruktor
WCLOGinternal::WCLOGinternal()
{
};

//---------------------------------------------------------------------------
// Destruktor
WCLOGinternal::~WCLOGinternal()
{
        // Dateien Schließen
        CloseInternalLOGFile();
};

// ==========================================================================
// DATEI ÖFFNEN UND SCHLIEßEN
// ==========================================================================
//---------------------------------------------------------------------------
// Öffnet eine neue LOG-Datei
bool WCLOGinternal::OpenInternalLOGFile(char *LOGFilePathName)
{
        CloseInternalLOGFile();

        if (LOGFilePathName == 0)return false;

        // Standardinitialisierung Hilfsvariable
        HV_LastMaxThreshold = 0;
        HV_OilStrainerIsOn = false;
        HV_AutomotiveErrorIsOn = false;
        HV_HighTemperature_DepthCalcIsOn = false;
        HV_LastHighTemperature_DepthCalc = 0;
        HV_LOGSleepIsOn = false;
        HV_LOGinternalSleepIsOn = false;
        HV_DepthCalcCommErrorOn = false;
        HV_ControllerErrorMonitIsOff = false;
        HV_IncEncErrorMonitIsOff = false;
        HV_GearShiftErrorIsOn = false;
        HV_LostMotionIsOn = false;
        HV_UnloadCableIsOn = false;

        InternalLOGFile.open(LOGFilePathName, ios_base::app);

        if (InternalLOGFile.bad()) return false;

        WriteOpenFile();

        return true;
};

// Schließt eine geöffnete LOG-Datei
bool WCLOGinternal::CloseInternalLOGFile()
{
        WriteCloseFile();

        if (InternalLOGFile.is_open()) InternalLOGFile.close();

        return true;
};

// ==========================================================================
// DATEN SCHREIBEN
// ==========================================================================

//---------------------------------------------------------------------------
// Schreiben in die Maximal-Zugkraft-LOG-Datei
bool WCLOGinternal::WriteMaxThresholdInFile(int Value)
{
        if (HV_LastMaxThreshold == Value) return true;                          // Eintrag dieses Wertes bereits erfolgt

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);

        if (oldTime == timer) return true;                                      // in dieser Sekunde wurde schon was geschrieben

        tm      *timeblock;
        timeblock = localtime(&timer);

        oldTime = timer;

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV001" << " " << Value << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_LastMaxThreshold = Value;                                            // Eintrag erfolgt

        return true;
};

//---------------------------------------------------------------------------
// Schreiben des Kommentars zum Schließen der Datei
bool WCLOGinternal::WriteCloseFile()
{
        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);

        // Eintragung erfolgt nicht im Sekundentakt, sondern sofort, wenn das Problem auftritt
//        if (oldTime == timer) return true;                                      // in dieser Sekunde wurde schon was geschrieben

        tm      *timeblock;
        timeblock = localtime(&timer);

//        oldTime = timer;

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "--- Close File ---" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        return true;
};

//---------------------------------------------------------------------------
// Schreiben des Kommentars zum Öffnen der Datei
bool WCLOGinternal::WriteOpenFile()
{
        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);

        // Eintragung erfolgt nicht im Sekundentakt, sondern sofort, wenn das Problem auftritt
//        if (oldTime == timer) return true;                                      // in dieser Sekunde wurde schon was geschrieben

        tm      *timeblock;
        timeblock = localtime(&timer);

//        oldTime = timer;

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "+++ Open File +++" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        return true;
};

//---------------------------------------------------------------------------
// Öl-Filter Lampe ist angegangen
bool WCLOGinternal::WriteOilStrainerLampON()
{
        if (HV_OilStrainerIsOn) return true;                                    // Eintrag bereits erfolgt

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);

        // Eintragung erfolgt nicht im Sekundentakt, sondern sofort, wenn das Problem auftritt
//        if (oldTime == timer) return true;                                      // in dieser Sekunde wurde schon was geschrieben

        tm      *timeblock;
        timeblock = localtime(&timer);

//        oldTime = timer;

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV002on" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_OilStrainerIsOn = true;                                              // Eintrag erfolgt

        return true;
};

//---------------------------------------------------------------------------
// Öl-Filter Lampe ist erloschen
bool WCLOGinternal::WriteOilStrainerLampOFF()
{
        if (!HV_OilStrainerIsOn) return true;                                   // Eintrag bereits erfolgt, bzw. nicht nötig

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);

        // Eintragung erfolgt nicht im Sekundentakt, sondern sofort, wenn das Problem auftritt
//        if (oldTime == timer) return true;                                      // in dieser Sekunde wurde schon was geschrieben

        tm      *timeblock;
        timeblock = localtime(&timer);

//        oldTime = timer;

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV002off" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_OilStrainerIsOn = false;                                             // Eintrag erfolgt

        return true;
};

//---------------------------------------------------------------------------
// Fahrzeug-Fehler ausgelöst
bool WCLOGinternal::WriteAutomotiveErrorON()
{
        if (HV_AutomotiveErrorIsOn) return true;                                // Eintrag bereits erfolgt, bzw. nicht nötig

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);

        // Eintragung erfolgt nicht im Sekundentakt, sondern sofort, wenn das Problem auftritt
//        if (oldTime == timer) return true;                                      // in dieser Sekunde wurde schon was geschrieben

        tm      *timeblock;
        timeblock = localtime(&timer);

//        oldTime = timer;

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV003on" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_AutomotiveErrorIsOn = true;                                         // Eintrag erfolgt

        return true;
};

//---------------------------------------------------------------------------
// Fahrzeug-Fehler aus
bool WCLOGinternal::WriteAutomotiveErrorOFF()
{
        if (!HV_AutomotiveErrorIsOn) return true;                               // Eintrag bereits erfolgt, bzw. nicht nötig

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);

        // Eintragung erfolgt nicht im Sekundentakt, sondern sofort, wenn das Problem auftritt
//        if (oldTime == timer) return true;                                      // in dieser Sekunde wurde schon was geschrieben

        tm      *timeblock;
        timeblock = localtime(&timer);

//        oldTime = timer;

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV003off" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_AutomotiveErrorIsOn = false;                                         // Eintrag erfolgt

        return true;
};

//---------------------------------------------------------------------------
// Protokollieren, wenn Temperatur zu hoch
bool WCLOGinternal::WriteHighTemperature_DepthCalcOn(int Value)
{
        if (HV_HighTemperature_DepthCalcIsOn && (HV_LastHighTemperature_DepthCalc == Value)) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);

        // Eintragung erfolgt nicht im Sekundentakt, sondern sofort, wenn das Problem auftritt
//        if (oldTime == timer) return true;                                      // in dieser Sekunde wurde schon was geschrieben

        tm      *timeblock;
        timeblock = localtime(&timer);

//        oldTime = timer;

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV006on" << " " << Value << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_LastHighTemperature_DepthCalc = Value;                               // Gerade geschriebene Temperatur merken
        HV_HighTemperature_DepthCalcIsOn = true;                                // Eintrag erfolgt

        return true;
};

//---------------------------------------------------------------------------
// Protokollieren, wenn Temperatur zu hoch
bool WCLOGinternal::WriteHighTemperature_DepthCalcOff()
{
        if (!HV_HighTemperature_DepthCalcIsOn) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);

        // Eintragung erfolgt nicht im Sekundentakt, sondern sofort, wenn das Problem auftritt
//        if (oldTime == timer) return true;                                      // in dieser Sekunde wurde schon was geschrieben

        tm      *timeblock;
        timeblock = localtime(&timer);

//        oldTime = timer;

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV006off" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_HighTemperature_DepthCalcIsOn = false;

        return true;
};

//---------------------------------------------------------------------------
// Protokollierung bei Value eingestellt
bool WCLOGinternal::WriteLOGSleepOn(__int64 Value)
{
        if (HV_LOGSleepIsOn) return true;                                       // Eintrag bereits erfolgt

        if (!InternalLOGFile.is_open()) return false;

        // Eintragung erfolgt nicht im Sekundentakt, sondern sofort, wenn das Problem auftritt
        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV004on" <<  " " << Value << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_LOGSleepIsOn = true;

        return true;
};
//---------------------------------------------------------------------------
// Protokollierung bei Value wieder aufgenommen
bool WCLOGinternal::WriteLOGSleepOff(__int64 Value)
{
        if (!HV_LOGSleepIsOn) return true;                                      // Eintrag bereits erfolgt

        if (!InternalLOGFile.is_open()) return false;

        // Eintragung erfolgt nicht im Sekundentakt, sondern sofort, wenn das Problem auftritt
        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV004off" <<  " " << Value << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_LOGSleepIsOn = false;

        return true;
};
//---------------------------------------------------------------------------
// interne Protokollierung bei Value eingestellt
bool WCLOGinternal::WriteLOGinternalSleepOn(__int64 Value)
{
        if (HV_LOGinternalSleepIsOn) return true;                                       // Eintrag bereits erfolgt

        if (!InternalLOGFile.is_open()) return false;

        // Eintragung erfolgt nicht im Sekundentakt, sondern sofort, wenn das Problem auftritt
        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV005on" <<  " " << Value << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_LOGinternalSleepIsOn = true;

        return true;
};
//---------------------------------------------------------------------------
// interne Protokollierung bei Value wieder aufgenommen
bool WCLOGinternal::WriteLOGinternalSleepOff(__int64 Value)
{
        if (!HV_LOGinternalSleepIsOn) return true;                              // Eintrag bereits erfolgt

        if (!InternalLOGFile.is_open()) return false;

        // Eintragung erfolgt nicht im Sekundentakt, sondern sofort, wenn das Problem auftritt
        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV005off" << " " << Value << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_LOGinternalSleepIsOn = false;

        return true;
};
//---------------------------------------------------------------------------
// Fehler bei der Kommunikation mit dem Teufenrechner aufgetreten
// Eintragung erfolgt sofort, wenn das Problem auftritt
bool WCLOGinternal::WriteDepthCalcCommErrorON()
{
        if (HV_DepthCalcCommErrorOn) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV007on" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_DepthCalcCommErrorOn = true;

        return true;
};

//---------------------------------------------------------------------------
// Fehler bei der Kommunikation mit dem Teufenrechner wieder verschwunden
// Eintragung erfolgt sofort, wenn das Problem verschwindet
bool WCLOGinternal::WriteDepthCalcCommErrorOFF()
{
        if (!HV_DepthCalcCommErrorOn) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV007off" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_DepthCalcCommErrorOn = false;

        return true;
};

//---------------------------------------------------------------------------
// Protokollierung, wenn Steuergerät Fehlerüberwachung abgeschalten wurde
// ACHTUNG HIER IST DAS EREIGNIS "ON", wenn MONITORING "OFF"!!!
bool WCLOGinternal::WriteControllerErrorMonitON()
{
        if (HV_ControllerErrorMonitIsOff) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV008on" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_ControllerErrorMonitIsOff = true;

        return true;
};

//---------------------------------------------------------------------------
// Protokollierung, wenn Steuergerät Fehlerüberwachung wieder eingeschalten wurde
// ACHTUNG HIER IST DAS EREIGNIS "OFF", wenn MONITORING "ON"!!!
bool WCLOGinternal::WriteControllerErrorMonitOFF()
{
        if (!HV_ControllerErrorMonitIsOff) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV008off" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_ControllerErrorMonitIsOff = false;

        return true;
};

//---------------------------------------------------------------------------
// Protokollierung, wenn Teufen-Encoder Fehlerüberwachung abgeschalten wurde
// ACHTUNG HIER IST DAS EREIGNIS "ON", wenn MONITORING "OFF"!!!
bool WCLOGinternal::WriteIncEncErrorMonitON()
{
        if (HV_IncEncErrorMonitIsOff) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV009on" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_IncEncErrorMonitIsOff = true;

        return true;
};

//---------------------------------------------------------------------------
// Protokollierung, wenn Teufen-Encoder Fehlerüberwachung wieder eingeschalten wurde
// ACHTUNG HIER IST DAS EREIGNIS "OFF", wenn MONITORING "ON"!!!
bool WCLOGinternal::WriteIncEncErrorMonitOFF()
{
        if (!HV_IncEncErrorMonitIsOff) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV009off" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_IncEncErrorMonitIsOff = false;

        return true;
};

//---------------------------------------------------------------------------
// Protokollierung, wenn die Schaltstellung des Schaltgetriebes fehlerhaft ist
bool WCLOGinternal::WriteGearShiftErrorON()
{
        if (HV_GearShiftErrorIsOn) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV010on" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_GearShiftErrorIsOn = true;

        return true;

};
//---------------------------------------------------------------------------
// Protokollierung, wenn die Schaltstellung des Schaltgetriebes wieder i.O. ist
bool WCLOGinternal::WriteGearShiftErrorOFF()
{
        if (!HV_GearShiftErrorIsOn) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV010off" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_GearShiftErrorIsOn = false;

        return true;
};

//---------------------------------------------------------------------------
// Protokollierung, wenn Freilauf eingeschaltet wird
bool WCLOGinternal::WriteLostMotionON()
{
        if (HV_LostMotionIsOn) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV011on" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_LostMotionIsOn = true;

        return true;
};

//---------------------------------------------------------------------------
// Protokollierung, wenn Freilauf ausgeschaltet wird
bool WCLOGinternal::WriteLostMotionOFF()
{
        if (!HV_LostMotionIsOn) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV011off" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_LostMotionIsOn = false;

        return true;
};

//---------------------------------------------------------------------------
// Protokollierung, wenn "Kabel entladen" eingeschaltet wird
bool WCLOGinternal::WriteUnloadCableON()
{
        if (HV_UnloadCableIsOn) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV012on" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_UnloadCableIsOn = true;

        return true;
};

//---------------------------------------------------------------------------
// Protokollierung, wenn "Kabel entladen" ausgeschaltet wird
bool WCLOGinternal::WriteUnloadCableOFF()
{
        if (!HV_UnloadCableIsOn) return true;

        if (!InternalLOGFile.is_open()) return false;

        time_t  timer = time(NULL);
        tm      *timeblock;
        timeblock = localtime(&timer);

        InternalLOGFile << timeblock->tm_mday << "." << (timeblock->tm_mon + 1) << "." << (timeblock->tm_year + 1900) << " "; // Datum
        InternalLOGFile << timeblock->tm_hour << ":" << timeblock->tm_min << ":" << timeblock->tm_sec << " ";                 // Zeit
        InternalLOGFile << "EV012off" << endl;
        InternalLOGFile.flush();

        if (InternalLOGFile.bad()) return false;

        HV_UnloadCableIsOn = false;

        return true;
};


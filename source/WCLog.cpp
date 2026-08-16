/*
WinchControl    Jörg Müller     WCLog.cpp

Klassenimplementation   WCLog
                        >> Log-Datei schreiben

Portierbarkeit: nach Anpassung von OpenFile 
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "WCLog.h"
#include <string>
#include <stdio.h>
#include <time.h>
#include <dir.h>
#include <iomanip>
#include <sstream>

#include "Labelling.h"
using namespace std;

//---------------------------------------------------------------------------
#pragma package(smart_init)

const int _WC_CREATE = 0x1;
const int _WC_DELETE = 0x2;

// Konstruktor
WCLog::WCLog(WCGenOrganisationS *GenOrg, WCVarControl *VarControl)
{
        // Grundeinstellungen
        LANGUAGE = 0;
        DatasetWritten = 0;
        GOrg = GenOrg;
        VarCtrl = VarControl;
        FileCreationTime[0] = 0;
        LAS_CurrTime = 0;
};

//---------------------------------------------------------------------------
// Destruktor
WCLog::~WCLog()
{
        // Aufräumen
        CloseFile();
};
//---------------------------------------------------------------------------

// =========================================================================
// Hilfsmethoden
// =========================================================================

// Verwaltet den LOG-Datei-Namen
// Parameter:
// Action ... gibt an, ob der Speicher freigegeben werden soll oder nicht
// Action = _WC_CREATE:
//      -oldLogFileName wird ignoriert,
//      -Rückgabewert ist Zeiger auf den String mit dem Namen der LOG-Datei oder
//       NULL, wenn Speicherreservierung fehlgeschlagen ist
//      ACHTUNG: Es ist die Sache des Programmierers, den reservierten Speicher
//               wieder freizugeben. z.B. mit Action = _DELETE
// Action = _WC_DELETE
//      -der Speicher, auf den *oldLOGFileName zeigt, wird freigegeben
//      -Rückgabewert ist 0, wenn Speicherfreigabe erfolgreich war
//      -Rückgabewert ist der Zeiger auf den immer noch belegten Speicher,
//       wenn die Speicherfreigabe fehlgeschlagen ist
// Hat Action keinen definierten Wert, wird keine Aktion ausgeführt und immer 0
// zurückgegeben.
char *WCLog::ManageLOGFileName(char *oldLOGFileName, int Action)
{
        char    *tmpFileName = 0;
        time_t  timer;
        tm      *timeblock;
        int     HV;
        char HString[5];

        switch (Action)
        {
                case _WC_CREATE :       // Speicherreservierung und Namensvergabe

                                        // Ziel-Dateinamen erstellen
                                        time(&timer);
                                        timeblock = localtime(&timer);

                                        HV = 1900 + timeblock->tm_year;         // Jahr schreiben
                                        itoa(HV, FileCreationTime, 10);
                                        FileCreationTime[4] = 0;                // zur Sicherheit nach i Buchstaben die Endekennung setzen
                                        itoa(timeblock->tm_mon + 1, HString, 10);// Monat schreiben
                                        if (strlen(HString) == 1)
                                        {
                                                HString[2] = 0;
                                                HString[1] = HString[0];
                                                HString[0] = '0';
                                        };
                                        strcat(FileCreationTime, HString);
                                        FileCreationTime[6] = 0;                // zur Sicherheit nach i Buchstaben die Endekennung setzen
                                        itoa(timeblock->tm_mday, HString, 10);  // Tag schreiben
                                        if (strlen(HString) == 1)
                                        {
                                                HString[2] = 0;
                                                HString[1] = HString[0];
                                                HString[0] = '0';
                                        };
                                        strcat(FileCreationTime, HString);
                                        FileCreationTime[8] = 0;                // zur Sicherheit nach i Buchstaben die Endekennung setzen
                                        itoa(timeblock->tm_hour, HString, 10);  // Stunde schreiben
                                        if (strlen(HString) == 1)
                                        {
                                                HString[2] = 0;
                                                HString[1] = HString[0];
                                                HString[0] = '0';
                                        };
                                        strcat(FileCreationTime, HString);
                                        FileCreationTime[10] = 0;               // zur Sicherheit nach i Buchstaben die Endekennung setzen
                                        itoa(timeblock->tm_min, HString, 10);   // Minute schreiben
                                        if (strlen(HString) == 1)
                                        {
                                                HString[2] = 0;
                                                HString[1] = HString[0];
                                                HString[0] = '0';
                                        };
                                        strcat(FileCreationTime, HString);
                                        FileCreationTime[12] = 0;               // zur Sicherheit nach i Buchstaben die Endekennung setzen
                                        itoa(timeblock->tm_sec, HString, 10);   // Sekunde schreiben
                                        if (strlen(HString) == 1)
                                        {
                                                HString[2] = 0;
                                                HString[1] = HString[0];
                                                HString[0] = '0';
                                        };
										strcat(FileCreationTime, HString);

										// Speicherzuweisung für Dateinamen
										// und Zusammensetzen des Dateinamens
                                        try
                                        {
												switch (VarCtrl->GetParam_LOGFileType()) {

													case LOGFILE_LAS2:
														tmpFileName = new char[strlen(FileCreationTime) + strlen(LOG_FileExt_LAS) + 1];
														strcpy(tmpFileName, FileCreationTime);
														strcat(tmpFileName, LOG_FileExt_LAS);
														break;

													case LOGFILE_GEOSYSV1:
														tmpFileName = new char[strlen(FileCreationTime) + strlen(LOG_FileExt_GEOSYSV1) + 1];
														strcpy(tmpFileName, FileCreationTime);
														strcat(tmpFileName, LOG_FileExt_GEOSYSV1);
														break;

													case LOGFILE_GEOSYSV2:

												default:
														tmpFileName = new char[strlen(FileCreationTime) + strlen(LOG_FileExt_GEOSYSV2) + 1];
														strcpy(tmpFileName, FileCreationTime);
														strcat(tmpFileName, LOG_FileExt_GEOSYSV2);
												}
										}catch(...)
										{
												if(tmpFileName != 0) delete tmpFileName;
												return 0;                       // Speicherreservierung fehlgeschlagen
										};

                                        return tmpFileName;                     // Zeiger auf Speicher mit Dateinamen zurückgeben

                case _WC_DELETE :       // Speicherfreigabe
                                        if (oldLOGFileName == 0) return 0;      // nichts zu tun-> alles i.O.
                                        try
                                        {
                                                delete oldLOGFileName;
                                        }catch(...)                             // wenn Speicherfreigabe fehlgeschlagen
                                        {
                                                return oldLOGFileName;          // dann Speicheradresse zurückgeben
                                        };

                                        return 0;                               // Speicher freigegeben-> alles i.O.

                default         :       // Nichts zu tun
                                        return 0;
        };

        return 0; // nur da, damit keine Compiler-Warnung erfolgt
};

// Verwaltet den Namen des vollständigen Pfads der LOG-Datei
// Parameter:
// Action ... gibt an, ob der Speicher freigegeben werden soll oder nicht
// Action = _WC_CREATE:
//      -oldLOGFilePath wird ignoriert,
//      -Rückgabewert ist Zeiger auf den String mit dem Namen der LOG-Datei oder
//       NULL, wenn Speicherreservierung fehlgeschlagen ist
//      ACHTUNG: Es ist die Sache des Programmierers, den reservierten Speicher
//               wieder freizugeben. z.B. mit Action = _DELETE
// Action = _WC_DELETE
//      -LOGFileName wird ignoriert
//      -der Speicher, auf den *oldLOGFilePath zeigt, wird freigegeben
//      -Rückgabewert ist 0, wenn Speicherfreigabe erfolgreich war
//      -Rückgabewert ist der Zeiger auf den immer noch belegten Speicher,
//       wenn die Speicherfreigabe fehlgeschlagen ist
// Hat Action keinen definierten Wert, wird keine Aktion ausgeführt und immer 0
// zurückgegeben.
// LOGFileName ... gibt den Namen der LOG-Datei an, die Bestandteil des Pfades
//                 ist
char *WCLog::ManageLOGFilePath(char *LOGFileName, char *oldLOGFilePath, int Action)
{
        char *tmpPath = 0;
        
        switch (Action)
        {
                case _WC_CREATE :       // Speicherreservierung und Namensvergabe
                                        if (LOGFileName == 0) return 0;         // keine Zieldatei angegeben

                                        // Speicherzuweisung für Dateinamen
                                        try
                                        {
                                                tmpPath = new char[strlen(GOrg->LOGPath) + strlen(LOGFileName) + 1];
                                        }catch(...)
                                        {
                                                return 0;                       // Speicherreservierung fehlgeschlagen
                                        };

                                        // Pfad zusammensetzen
                                        strcpy(tmpPath, GOrg->LOGPath);
                                        strcat(tmpPath, LOGFileName);

                                        return tmpPath;                         // Zeiger auf Speicher mit Dateinamen zurückgeben

                case _WC_DELETE :       // Speicherfreigabe
                                        if (oldLOGFilePath == 0) return 0;      // nichts zu tun-> alles i.O.
                                        try
                                        {
                                                delete oldLOGFilePath;
                                        }catch(...)                              // wenn Speicherfreigabe fehlgeschlagen
                                        {
                                                return oldLOGFilePath;          // dann Speicheradresse zurückgeben
                                        };

                                        return 0;                               // Speicher freigegeben-> alles i.O.

                default         :       // Nichts zu tun
                                        return 0;
        };

        return 0; // nur da, damit keine Compiler-Warnung erfolgt
};

// Verwaltet den Namen des vollständigen Pfads der zur LOG-Datei gehörigen Validierungsdatei
// Parameter:
// Action ... gibt an, ob der Speicher freigegeben werden soll oder nicht
// Action = _WC_CREATE:
//      -oldValidationFilePath wird ignoriert,
//      -Rückgabewert ist Zeiger auf den String mit dem Namen der LOG-Datei oder
//       NULL, wenn Speicherreservierung fehlgeschlagen ist
//      ACHTUNG: Es ist die Sache des Programmierers, den reservierten Speicher
//               wieder freizugeben. z.B. mit Action = _DELETE
// Action = _WC_DELETE
//      -der Speicher, auf den *oldValidationFilePath zeigt, wird freigegeben
//      -Rückgabewert ist 0, wenn Speicherfreigabe erfolgreich war
//      -Rückgabewert ist der Zeiger auf den immer noch belegten Speicher,
//       wenn die Speicherfreigabe fehlgeschlagen ist
// Hat Action keinen definierten Wert, wird keine Aktion ausgeführt und immer 0
// zurückgegeben.
// LOGFileName ... gibt den Namen der LOG-Datei an, deren Name ausschlaggebend
//                 für die Bezeichnung der zugehörigen Validierungsdatei ist
//                 Der Bezeichner der LOG-Datei und der Validierungsdatei sollen
//                 identisch sein. Nur die Dateierweiterungen unterscheiden sich
char *WCLog::ManageValidationFilePath(char *LOGFileName, char *oldValidationFilePath, int Action)
{
        char *tmpPath = 0;
        stringstream tmpStr;
        int HV;

        switch (Action)
        {
                case _WC_CREATE :       // Speicherreservierung und Namensvergabe
                                        if (LOGFileName == 0) return 0;         // keine Zieldatei angegeben

                                        // Namen für die Validierungsdatei erstellen
                                        HV = strlen(LOGFileName);
                                        for (int i = 0; i < HV; ++i)
                                        {
                                                // Bis zum Punkt "." suchen (Ende des Dateibezeichners)
                                                if (LOGFileName[i] == '.') break;
                                                tmpStr << LOGFileName[i];
                                                if (tmpStr.fail()) return 0;    // es ist ein Fehler aufgetreten, sicherheitshalber wird keine Validierungsdatei erstellt (Dateiname könnte falsch werden)
                                        };
                                        if (tmpStr.str().length() < 1) return 0;// kein Bezeichner vorhanden, kann keinen Namen für Validierungsdatei finden

                                        // Validierungsdateinamen vollst. zusammenbasteln
                                        tmpStr << LOG_VALIDATION_FileExt;
                                        if (tmpStr.fail()) return 0;            // es ist ein Fehler aufgetreten, sicherheitshalber wird keine Validierungsdatei erstellt (Dateiname könnte falsch werden)

                                        // Speicherzuweisung für Dateinamen
                                        try
                                        {
                                                tmpPath = new char[strlen(GOrg->LOGPath) + tmpStr.str().length() + 1];
                                        }catch(...)
                                        {
                                                return 0;                       // Speicherreservierung fehlgeschlagen
                                        };

                                        // Pfad zusammensetzen
                                        strcpy(tmpPath, GOrg->LOGPath);
                                        strcat(tmpPath, tmpStr.str().c_str());

                                        return tmpPath;                         // Zeiger auf Speicher mit Dateinamen zurückgeben

                case _WC_DELETE :       // Speicherfreigabe
                                        if (oldValidationFilePath == 0) return 0;// nichts zu tun-> alles i.O.
                                        try
                                        {
                                                delete oldValidationFilePath;
                                        }catch(...)                             // wenn Speicherfreigabe fehlgeschlagen
                                        {
                                                return oldValidationFilePath;   // dann Speicheradresse zurückgeben
                                        };

                                        return 0;                               // Speicher freigegeben-> alles i.O.

                default         :       // Nichts zu tun
                                        return 0;
        };

        return 0; // nur da, damit keine Compiler-Warnung erfolgt
 };

// =========================================================================
// Dateiverwaltung
// =========================================================================

//---------------------------------------------------------------------------
// Ermittelt eine 16-Bit CRC-Sequenz mit Vorgehensweise "1" und
// schreibt diese in die Validierungsdatei
// konnte keine CRC-Sequenz ermittelt werden, wird auch nichts geschrieben
// Rückgabewert:        true, wenn CRC-Sequ. ordnungsgemäß ermittelt und in Datei geschrieben wurde
//                      false, wenn am Ende keine CRC-Sequenz in die Datei geschrieben werden konnte
bool WCLog::MakeCRC16_1(const char *Data, int Length)
{
        if (Data == 0) return false;
        if (Length == 0) return false;

        unsigned long crc16 = 0;                                                // CRC-Puffer
        char HChar, ResultChar[2];                                              // Zwischenspeicher

        for (int i = 0; i < (Length + 2); ++i)                                  // ------ Schiebe-Algorithmus
        {                                                                       // Length + 2, da noch 2Byte mit Inhalt 0x0 für CRC-Prüfung angehängt werden muss  
                if (i < Length)
                {                                                               // Daten NEGIERT in Zwischespeicher schieben
                        try
                        {                                                       // wenn Fehler auftritt (z.B. Speicherzugriffsfehler wegen falscher Längenangabe)
                                HChar = ~Data[i];
                        }catch(...)
                        {                                                       // so reagieren, als ob Stringende erreicht->damit kann ordnungsgemäße CRC-Ermittlung stattfinden
                                Length = i;
                                HChar = 0x0;
                        };
                }else HChar = 0x0;                                              // Anhängen der 0-Bits

                for (int j = 0; j < 8; ++j)
                {
                        crc16 <<= 1;                                            // CRC-Pufferinhalt um ein Bit verschieben
                        if ((HChar & 0x80) == 0x80) crc16 |= 0x1;               // Wenn aktuelles Daten-Bit gesetzt, dann auch in CRC-Puffer schreiben
                        HChar <<= 1;                                            
                                                                                // -------
                        if ((crc16 & 0x10000) == 0x10000) crc16 ^= __CRC16_1_POLYNOM; // wenn Puffer voll, dann CRC-Operation ausführen
                };
        };

        // Schreiben der Sequenz in eine Datendatei
        ResultChar[1] = crc16 & 0xFF;
        crc16 >>= 8;
        ResultChar[0] = crc16 & 0xFF;

        if (ValidationFile.is_open())
        {
                ValidationFile.write(ResultChar, 2);
                ValidationFile.flush();
                if (ValidationFile.fail()) return false;
        } else return false;

        return true;
};

//---------------------------------------------------------------------------
// Öffnet eine neue Datendatei für die Gültigkeitsüberprüfung
// Parameter:
//              FilePath        ... Gibt Dateinamen der Validierungs-Datei an
//              ReferenceFile   ... Gibt Dateinamen der Datei an, die durch die Validierungsdatei überprüft werden soll
bool WCLog::OpenValidationFile(char *FilePath, char *ReferenceFile)
{
        // Eingangsprüfung
        if (FilePath == 0) return false;

        // Datei öffnen
        ValidationFile.open(FilePath, ios_base::binary);
        if (!ValidationFile.is_open()) return false;

        // Header schreiben
        ValidationFile.write(LOG_VALIDATION_TYPCODE_1, 2);                      // Typifizierungscode
        ValidationFile.write(LOG_VALIDATION_TYPSTRING_1, strlen(LOG_VALIDATION_TYPSTRING_1)); // Typifizierungsstring
        ValidationFile.write(&LOG_VALIDATION_SEPARATOR_1, 1);                   // Trennbyte
        ValidationFile.write(LOG_VALIDATION_IDENTSTRING_PRE_1, strlen(LOG_VALIDATION_IDENTSTRING_PRE_1)); // Präampel Identifikationsstring
        ValidationFile.write(__PROGNAME, strlen(__PROGNAME));                   // Programmname für Identifikationsstring
        ValidationFile.write(" ", 1);
        ValidationFile.write(__VERSION, strlen(__VERSION));                     // Programm-Versionsnummer für Identifikationsstring
        ValidationFile.write(" ", 1);
        ValidationFile.write(__DATEOFCOMPIL, strlen(__DATEOFCOMPIL));           // Datum der Compilierung für Identifikationsstring
        ValidationFile.write(" ", 1);
        ValidationFile.write(__NUMBERPERDATE, strlen(__NUMBERPERDATE));         // Datum der Compilierung für Identifikationsstring
        ValidationFile.write(&LOG_VALIDATION_SEPARATOR_1, 1);                   // Trennbyte
        ValidationFile.write(ReferenceFile, strlen(ReferenceFile));             // Referenzdatei eintragen
        ValidationFile.write(&LOG_VALIDATION_SEPARATOR_1, 1);                   // Trennbyte
        // Hier folgt nun der Datenbereich
        
        return true;
};

//---------------------------------------------------------------------------
// Schließt die Datendatei für die Gültigkeitsüberprüfung
bool WCLog::CloseValidationFile()
{
        if (ValidationFile.is_open()) ValidationFile.close();
        return true;
};

//---------------------------------------------------------------------------
// Löscht Validierungs-Dateien
bool WCLog::CleanUpValidationFile(char *FilePath)
{
        if (FilePath != 0) return ((remove(FilePath) == 0) ? true : false);

        return false;
};

//---------------------------------------------------------------------------
// Schreibt den DateiHeader
bool WCLog::WriteHeader()
{
        if (GOrg == 0) return false;

		switch (VarCtrl->GetParam_LOGFileType()) {

		case LOGFILE_LAS2:
			return WriteHeaderLAS2_0();

		case LOGFILE_GEOSYSV1:
			return WriteHeaderGEOSYS();

		case LOGFILE_GEOSYSV2:

		default:
        	;
		}

		return WriteHeaderGEOSYSV2();
};

//---------------------------------------------------------------------------
// // Öffnet eine neue LOG-Datei
// Rückgabewert: true   ...     alles i.O.
//               fasle  ...     irgendein Fehler ist aufgetreten
bool WCLog::OpenFile(char *FilePath)
{
		// Verzeichnis erstellen, sofern noch nicht vorhanden
		mkdir(GOrg->LOGPath);

		// Datei öffnen / erstellen
		LOGFile.open(FilePath);

		if (!LOGFile.is_open()) return false;

		return true;

};

//---------------------------------------------------------------------------
// Schließt eine geöffnete LOG-Datei
// Der Inhalt der temporären LOG-Datei wird in eine entgültige Datei geschrieben.
// Besteht der bereits eine datei mit dem Namen der Zieldatei, so werden die
// neuen Daten am dateiende angehängt.
// Rückgabewert: true   ...     alles i.O.
//               fasle  ...     irgendein Fehler ist aufgetreten
bool WCLog::CloseFile()
{
		if (LOGFile.is_open()) LOGFile.close();
		return true;
};

//---------------------------------------------------------------------------
// Löscht LOG-Dateien, die keinen LOG-Daten enthalten (wäre sonst Platzverschwendung und Unordnung)
bool WCLog::CleanUpEmptyFiles(char *FilePath)
{
        // Datei löschen, wenn nicht die Mindestanzahl von Einträgen vorhanden ist
        if ((DatasetWritten < LOG_MIN_ENTRYS) && (FilePath != 0)) return ((remove(FilePath) == 0) ? true : false);

        return false;
};
//---------------------------------------------------------------------------
// Mit neuer LOG-Datei weiterarbeiten
bool WCLog::StartLOG()
{
        bool OK = true;
        char *LOGFileName = 0, *LOGFilePath = 0, *ValidationFilePath = 0;

        // evtl. geöffneten LOG abschließen
        StopLOG();

        // Verschiedene Resets
        // Standard-Tabellenbreiten einstellen
        Col1Size = Col2Size = Col3Size = Col4Size = Col5Size = LOG_Col_Width_General;
        LANGUAGE = GOrg->LOG_Lng;
        DatasetWritten = 0;

        // Namenserstellung der Zieldateien
        LOGFileName = ManageLOGFileName(0, _WC_CREATE);
        LOGFilePath = ManageLOGFilePath(LOGFileName, 0, _WC_CREATE);
        ValidationFilePath = ManageValidationFilePath(LOGFileName, 0, _WC_CREATE);

        // Dateien öffnen
        OK = OpenFile(LOGFilePath);
#ifdef __WRITE_CRC_VALIDATION
        if (OK) OpenValidationFile(ValidationFilePath, LOGFileName);
#endif

        if (OK) WriteHeader();                                                  // Header in die LOG-datei schreiben

        // Speicherfreigabe der char-Felder (Dateinamen/ -pfade)
        LOGFileName = ManageLOGFileName(LOGFileName, _WC_CREATE);
        LOGFilePath = ManageLOGFilePath(0, LOGFilePath, _WC_CREATE);
        ValidationFilePath = ManageValidationFilePath(0, ValidationFilePath, _WC_CREATE);

        return OK;
};

//---------------------------------------------------------------------------
// Logging unterbrechen und Datei schließen
bool WCLog::StopLOG()
{
        stringstream Str_LOGFileName;
        char *LOGFilePath = 0, *ValidationFilePath = 0;

        if (!LOGFile.is_open()) return true;                                    // Ist LOG-Datei nicht geöffnet abbrechen

        // LOG-Datei abschließen, falls es das Format verlangt
        ConcludeFileLAS2_0();                                                   // Prüfung, ob Aktion notwendig, wird innerhalb der Methode überprüft

        // Dateien schließen
        CloseFile();
#ifdef __CRC16_1_POLYNOM
        CloseValidationFile();
#endif

        // Dateipfade zusammenstellen aufräumen leerer Dateien
        // Name nicht durch ManageLOGFileName-Methode erstellen, da sonst der Name nicht stimmt
		switch(VarCtrl->GetParam_LOGFileType()) {

			case LOGFILE_LAS2:
            	Str_LOGFileName << FileCreationTime << LOG_FileExt_LAS;
				break;

			case LOGFILE_GEOSYSV1:
            	Str_LOGFileName << FileCreationTime << LOG_FileExt_GEOSYSV1;
				break;

			case LOGFILE_GEOSYSV2:

			default:
				Str_LOGFileName << FileCreationTime << LOG_FileExt_GEOSYSV2;
		}

        if (Str_LOGFileName.fail()) return false;                               // abbrechen (Löschen leerer Dateien entfällt)

        LOGFilePath = ManageLOGFilePath((char*)(Str_LOGFileName.str().c_str()), 0, _WC_CREATE);
        ValidationFilePath = ManageValidationFilePath((char*)(Str_LOGFileName.str().c_str()), 0, _WC_CREATE);

        // Leere LOG-Dateien können gelöscht werden
#ifndef __CRC16_1_POLYNOM
        CleanUpEmptyFiles(LOGFilePath);
#endif
#ifdef __CRC16_1_POLYNOM
        // Wenn LOG-Datei gelöscht wurde, kann auch Validierungs Datei gelöscht werden
        if (CleanUpEmptyFiles(LOGFilePath)) CleanUpValidationFile(ValidationFilePath);
#endif

        // Speicherfreigabe durchführen
        // Um Str_LOGFileName braucht sich nicht gekümmert werden, das erledigt der Destruktor von Str_LOGFileName
        LOGFilePath = ManageLOGFilePath(0, LOGFilePath, _WC_DELETE);
        ValidationFilePath = ManageLOGFilePath(0, ValidationFilePath, _WC_DELETE);

        // Abschließendes aufräumen
        DatasetWritten = 0;
        FileCreationTime[0] = 0;  // EK-setzen

        return true;
};

//---------------------------------------------------------------------------
// Schreibt einen Datensatz
bool WCLog::WriteDataSet()
{
        if (!LOGFile.is_open()) return false;

		switch (VarCtrl->GetParam_LOGFileType()) {

			case LOGFILE_LAS2:
				return WriteDataSetLAS2_0();

			case LOGFILE_GEOSYSV1:
				return WriteDataSetGEOSYS();

			case LOGFILE_GEOSYSV2:

		default:
			;
		}
		return WriteDataSetGEOSYSV2();
};

// =========================================================================
// LOG-File-Format-spezifische Routinen
// =========================================================================
// GEOSYS-Standard
// ------------------------------------
// Datei-Header schreiben
bool WCLog::WriteHeaderGEOSYS()
{
		char HString1[11];
		char HString2[9];
		int HV;
		stringstream StringBuffer;

		LAS_StopPosition = 0;   // Muss gesetzt werden, da beim Aufräumen sonst mit einer LAS-Datei gerechnet werden könnte

		// Identifikationszeile
		StringBuffer << LOG_Identification_Prefix << __PROGNAME << __SEPARATOR01[LANGUAGE] << __VERSION << __SEPARATOR01[LANGUAGE] << __DATEOFCOMPIL << __SEPARATOR01[LANGUAGE] << __NUMBERPERDATE << __SEPARATOR01[LANGUAGE] << LOG_Identification_Suffix;
		LOGFile << StringBuffer.str() << endl << endl;
#ifdef __WRITE_CRC_VALIDATION
		MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
		StringBuffer.str("");

        // Datum
        HString1[0]  = FileCreationTime[6];
        HString1[1]  = FileCreationTime[7];
        HString1[2]  = __SEPARATOR04[LANGUAGE][0];
        HString1[3]  = FileCreationTime[4];
        HString1[4]  = FileCreationTime[5];
        HString1[5]  = __SEPARATOR04[LANGUAGE][0];
        HString1[6]  = FileCreationTime[0];
        HString1[7]  = FileCreationTime[1];
        HString1[8]  = FileCreationTime[2];
        HString1[9]  = FileCreationTime[3];
        HString1[10] = 0;

        // Uhrzeit
        HString2[0]  = FileCreationTime[8];
        HString2[1]  = FileCreationTime[9];
        HString2[2]  = __SEPARATOR03[LANGUAGE][0];
        HString2[3]  = FileCreationTime[10];
        HString2[4]  = FileCreationTime[11];
        HString2[5]  = __SEPARATOR03[LANGUAGE][0];
        HString2[6]  = FileCreationTime[12];
        HString2[7]  = FileCreationTime[13];
        HString2[8]  = 0;

        // Ermitteln des Breitesten Wortes für eine saubere Darstellung
        HV = strlen(LABEL_LOCATION[LANGUAGE]);
        if (HV < strlen(LABEL_WELL[LANGUAGE])) HV = strlen(LABEL_WELL[LANGUAGE]);
        if (HV < strlen(LABEL_DATE[LANGUAGE])) HV = strlen(LABEL_DATE[LANGUAGE]);
        if (HV < strlen(LABEL_TIMEOFDAY[LANGUAGE])) HV = strlen(LABEL_TIMEOFDAY[LANGUAGE]);
		if (HV < strlen(LABEL_NAME[LANGUAGE])) HV = strlen(LABEL_NAME[LANGUAGE]);
		if (HV < strlen(LABEL_TEAM[LANGUAGE])) HV = strlen(LABEL_TEAM[LANGUAGE]);
		if (HV < strlen(LABEL_WINCHMACHINERY[LANGUAGE])) HV = strlen(LABEL_WINCHMACHINERY[LANGUAGE]);
		if (HV < strlen(LABEL_COMMENT[LANGUAGE])) HV = strlen(LABEL_COMMENT[LANGUAGE]);
        if (HV < strlen(LABEL_WINCH[LANGUAGE])) HV = strlen(LABEL_WINCH[LANGUAGE]);
        if (HV < strlen(LABEL_DEPTH[LANGUAGE])) HV = strlen(LABEL_DEPTH[LANGUAGE]);
        if (HV < strlen(LABEL_VELOCITY[LANGUAGE])) HV = strlen(LABEL_VELOCITY[LANGUAGE]);
		if (HV < strlen(LABEL_TENSION[LANGUAGE])) HV = strlen(LABEL_TENSION[LANGUAGE]);

        StringBuffer.setf(ios_base::left);

        // Ort
        StringBuffer << setw(HV) << LABEL_LOCATION[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << GOrg->LOG_LOCATION;
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        // Bohrlochbezeichnung
        StringBuffer << setw(HV) << LABEL_WELL[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << GOrg->LOG_WELL;
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        // Datum und Uhrzeit schreiben
        StringBuffer << setw(HV) << LABEL_DATE[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << HString1;
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        StringBuffer << setw(HV) << LABEL_TIMEOFDAY[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << HString2;
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        // Name
        StringBuffer << setw(HV) << LABEL_NAME[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << GOrg->LOG_NAME;
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
		StringBuffer.str("");

		// Trupp
		StringBuffer << setw(HV) << LABEL_TEAM[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << GOrg->LOG_TEAM;
		LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
		MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
		StringBuffer.str("");

		// Windenanlage
		StringBuffer << setw(HV) << LABEL_WINCHMACHINERY[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << GOrg->LOG_WINCHMACHINERY;
		LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
		MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
		StringBuffer.str("");

		// Kommentar
		StringBuffer << setw(HV) << LABEL_COMMENT[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << GOrg->LOG_COMMENT;
		LOGFile << StringBuffer.str() << endl << endl;
#ifdef __WRITE_CRC_VALIDATION
		MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
		StringBuffer.str("");

		// Windennummer
        StringBuffer << setw(HV) << LABEL_WINCH[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << VarCtrl->GetParam_WinchNo();
        LOGFile << StringBuffer.str() << endl << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        // Erläuterungen der Maßeinheiten
        StringBuffer << "-----------------------------------------------------------";
        LOGFile << StringBuffer << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << setw(HV) << LABEL_DEPTH[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << UNIT_METER[LANGUAGE];
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << setw(HV) << LABEL_VELOCITY[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << UNIT_METER_PER_HOUR[LANGUAGE];
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
		StringBuffer.str("");
		switch (VarCtrl->GetParam_LOGFileTensionUnit()) {
		case 1:
			StringBuffer << setw(HV) << LABEL_TENSION[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << UNIT_KILOPOND[LANGUAGE];
			break;
		case 0:
		default:
			StringBuffer << setw(HV) << LABEL_TENSION[LANGUAGE] << __SEPARATOR03[LANGUAGE] << __SEPARATOR01[LANGUAGE] << UNIT_KILONEWTON[LANGUAGE];
		}
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "-----------------------------------------------------------";
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        StringBuffer.unsetf(ios_base::left);

        // Tabellenkopf

        // Spaltenbreiten festlegen
        HV = strlen(LABEL_DATE[LANGUAGE]);
        if (HV >= LOG_Col_Width_General) Col1Size = HV + 1;
        HV = strlen(LABEL_TIMEOFDAY[LANGUAGE]);
        if (HV >= LOG_Col_Width_General) Col2Size = HV + 1;
        HV = strlen(LABEL_DEPTH[LANGUAGE]);
        if (HV >= LOG_Col_Width_General) Col3Size = HV + 1;
        HV = strlen(LABEL_MAGNETICMARK[LANGUAGE]);
        if (HV >= LOG_Col_Width_General) Col4Size = HV + 1;
        HV = strlen(LABEL_VELOCITY[LANGUAGE]);
        if (HV >= LOG_Col_Width_General) Col5Size = HV + 1;

        StringBuffer << setw(Col1Size) << LABEL_DATE[LANGUAGE];
        StringBuffer << setw(Col2Size) << LABEL_TIMEOFDAY[LANGUAGE];
        StringBuffer << setw(Col3Size) << LABEL_DEPTH[LANGUAGE];
        StringBuffer << setw(Col4Size) << LABEL_MAGNETICMARK[LANGUAGE];
        StringBuffer << setw(Col5Size) << LABEL_VELOCITY[LANGUAGE];
        StringBuffer << setw(LOG_Col_Width_General) << LABEL_TENSION[LANGUAGE];
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        if (LOGFile.fail()) return false;

        return true;
};

// ------------------------------------
// Schreibt einen Datensatz
bool WCLog::WriteDataSetGEOSYS()
{
        time_t  timer;
        tm      *timeblock;
		char HString[10];
        char HBuffer[20];
		stringstream StringBuffer;
		float Hfloat;

        if (DatasetWritten < LOG_MIN_ENTRYS) ++DatasetWritten;

        time(&timer);
        timeblock = localtime(&timer);

        // Datum
        StringBuffer << setw(Col1Size - 8) << timeblock->tm_mday;                    // Tag
        StringBuffer << __SEPARATOR04[LANGUAGE];
        StringBuffer << setw(2) << timeblock->tm_mon + 1;                            // Monat
        StringBuffer << __SEPARATOR04[LANGUAGE];
        StringBuffer << setw(4) << timeblock->tm_year + 1900;                        // Jahr

        // Uhrzeit
        StringBuffer << setw(Col2Size - 6) << timeblock->tm_hour;                    // Stunde
        StringBuffer << __SEPARATOR03[LANGUAGE];
        StringBuffer << setw(2) << timeblock->tm_min;                                // Minute
        StringBuffer << __SEPARATOR03[LANGUAGE];
        StringBuffer << setw(2) << setw(2) << timeblock->tm_sec;                     // Sekunde

        StringBuffer << std::fixed;

        // Teufe in m (Genauigkeit: 2 Stellen nach Komma)
        StringBuffer << setw(Col3Size) << setprecision(2) << float(VarCtrl->GetInfo_CurrDepth()) / 1000;

        // Magnetmarke
        StringBuffer << setw(Col4Size) << VarCtrl->GetInfo_MagneticMarkStaticLOG();

        // Geschwindigkeit in m / h (Genauigkeit: 1 Stelle nach Komma)
        StringBuffer << setw(Col5Size) << setprecision(1) << VarCtrl->GetInfo_AnnouncedVelocity();

		// Zugkraft
		switch (VarCtrl->GetParam_LOGFileTensionUnit()) {
		case 1:   // in kp (kg)
			Hfloat = static_cast<float>(VarCtrl->GetInfo_CurrTraction()) / __GFORCE;
			StringBuffer << setw(LOG_Col_Width_General) << static_cast<int>(Hfloat);
			break;
		case 0:
		default:  // in kN
			StringBuffer << setw(LOG_Col_Width_General) << ((VarCtrl->GetInfo_CurrTraction() + 500) / 1000); // + 500 um korrekt aufzurunden
		}

        LOGFile << StringBuffer.str() << endl;
        LOGFile.flush();
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        if (LOGFile.bad()) return false;
        else return true;
};

// ------------------------------------
// LAS 2.0
// ------------------------------------
// Datei-Header schreiben
bool WCLog::WriteHeaderLAS2_0()
{
		char HString1[20];
        stringstream StringBuffer;

        // Versions-Info
        StringBuffer << "~VERSION INFORMATION";
		LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
		MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        StringBuffer << "VERS." << setw(32) << "2.0" << " : CWS LOG ASCII STANDARD -VERSION 2.0";    // Version
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "WRAP." << setw(32) << "NO" << " : ONE LINE PER TIME STEP";                  // Wrapping
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "#";
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        // Bohrloch-Info
        StringBuffer << "~WELL INFORMATION";
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "STRT .S " << setw(30) << LAS_CurrTime << " :START TIME";// Startzeit
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        LAS_StopPosition = LOGFile.tellp();                                     // Position merken;
        StringBuffer << "STOP .S ";                                             // Stoppzeit
        StringBuffer << setw(30) << " " << " :STOP TIME";
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        ValidationFile_LAS_StopPosition = ValidationFile.tellp();               // Position merken
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "STEP .S " << setw(30) << LOG_TIME_INTERVAL << " :STEP";// Zeitschritt
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "NULL .  " << setw(30) << -999.225 << " :NULL VALUE";   // Nullwert (welcher Wert einem nicht vorhandenen Datum entspricht)
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "COMP .  " << setw(30) << "NOT SPECIFIED" << " :COMPANY";// Auftragsfirma
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
		StringBuffer.str("");
		if (GOrg->LOG_WELL[0] == 0)
                StringBuffer << "WELL .  " << setw(30) << "NOT SPECIFIED" << " :WELL"; // Bohrloch
        else
                StringBuffer << "WELL .  " << setw(30) << GOrg->LOG_WELL << " :WELL";
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        if (GOrg->LOG_LOCATION[0] == 0)
                StringBuffer << "FLD  .  " << setw(30) << "NOT SPECIFIED" << " :FIELD"; // Bohrfeld
        else
                StringBuffer << "FLD  .  " << setw(30) << GOrg->LOG_LOCATION << " :FIELD";
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "LOC  .  " << setw(30) << "NOT SPECIFIED" << " :LOCATION"; // Ort
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "PROV .  " << setw(30) << "NOT SPECIFIED" << " :PROVENCE"; // Provinz
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "SRVC .  " << setw(30) << "NOT SPECIFIED" << " :SERVICE COMPANY"; // Service-Firma
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        HString1[0]  = FileCreationTime[6];        // Tag                       // Aufzeichnungsdatum und -uhrzeit
        HString1[1]  = FileCreationTime[7];
        HString1[2]  = '.';
        HString1[3]  = FileCreationTime[4];        // Monat
        HString1[4]  = FileCreationTime[5];
        HString1[5]  = '.';
        HString1[6]  = FileCreationTime[0];        // Jahr
        HString1[7]  = FileCreationTime[1];
        HString1[8]  = FileCreationTime[2];
        HString1[9]  = FileCreationTime[3];
        HString1[10] = ' ';
        HString1[11]  = FileCreationTime[8];      // Stunde
		HString1[12]  = FileCreationTime[9];
        HString1[13]  = '-';
        HString1[14]  = FileCreationTime[10];     // Minute
        HString1[15]  = FileCreationTime[11];
        HString1[16]  = '-';
        HString1[17]  = FileCreationTime[12];     // Sekunde
        HString1[18]  = FileCreationTime[13];
        HString1[19]  = 0;

        StringBuffer << "DATE .  " << setw(30) << HString1 << " :LOG DATE";
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "UWI  .  " << setw(30) << "NOT SPECIFIED" << " :UNIQUE WELL ID";             // Eindeutige Bohrloch ID
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "#";
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        // Kurven-Info
        StringBuffer << "~CURVE INFORMATION";
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "ETIM .S " << setw(30) << " " << " : 1 ELAPSED TIME";   // Vergangene Zeit
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "DEPT .M " << setw(30) << " " << " : 2 DEPTH";          // Teufe
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "VELO .M/H" << setw(29) << " " << " : 3 VELOCITY";      // Geschwindigkeit
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
		MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
		switch (VarCtrl->GetParam_LOGFileTensionUnit()) {
		case 1: // in kg (kp)
			StringBuffer << "TENS .KP" << setw(30) << " " << " : 4 TENSION";    // Zugkraft
        	break;
		case 0:
		default: // in kN
			StringBuffer << "TENS .KN" << setw(30) << " " << " : 4 TENSION";    // Zugkraft
			;
		}
		LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "MM ." << setw(34) << " " << " : 5 MAGNETIC MARK";      // Magnetmarke
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
		switch (VarCtrl->GetParam_LOGFileTensionUnit()) {
		case 1: // in kg (kp)
			StringBuffer << "MAXTE .KP" << setw(29) << " " << " : 6 MAX TENSION";    // Maximale Zugkraft
			break;
		case 0:
		default: // in kN
			StringBuffer << "MAXTE .KN" << setw(29) << " " << " : 6 MAX TENSION";    // Maximale Zugkraft
			;
		}
		LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
		switch (VarCtrl->GetParam_LOGFileTensionUnit()) {
		case 1: // in kg (kp)
			StringBuffer << "DIFTE .KP" << setw(29) << " " << " : 7 MIN TENSION"; // Zugkraftdifferenz für Schleifenfänger
			break;
		case 0:
		default: // in kN
			StringBuffer << "DIFTE .KN" << setw(29) << " " << " : 7 MIN TENSION"; // Zugkraftdifferenz für Schleifenfänger
			;
		}
		LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
		StringBuffer.str("");
		StringBuffer << "DIFTI .S" << setw(30) << " " << " : 8 PLACEHOLDER"; 	// Zeitdifferenz für Schleifenfänger
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "MAXDE .M" << setw(30) << " " << " : 9 MAX DEPTH";      // Maximale Teufe (Teufenbereich)
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "MINDE .M" << setw(30) << " " << " : 10 MIN DEPTH";     // Minimale Teufe (Teufenbereich)
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "OILF ." << setw(32) << " " << " : 11 CHANGE OILFILTER";// Signal Ölfilter
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
		StringBuffer.str("");
        StringBuffer << "RC2E ." << setw(32) << " " << " : 12 ERROR RC2_2";     // RC2-2 Fehler
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "HYDR ." << setw(31) << " " << " : 13 HYDRAULIC ON";    // Hydraulik
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "JOYST ." << setw(31) << " " << " : 14 JOYSTICK ACTIVE";// Steuerhebel aktiv
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "AUTOM ." << setw(31) << " " << " : 15 AUTOMATIC ON";      // Automatische Fahrt
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "RC2MON ." << setw(30) << " " << " : 16 RC2-2 MONITORING OFF";     // Minimale Teufe (Teufenbereich)
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");
        StringBuffer << "#";
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        // Datenbereich
        StringBuffer << "~A";
        LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
		StringBuffer.str("");

        if (LOGFile.fail()) return false;

        return true;
};

// ------------------------------------
// Schreibt einen Datensatz
bool WCLog::WriteDataSetLAS2_0()
{
		float Hfloat;

		if (DatasetWritten < LOG_MIN_ENTRYS) ++DatasetWritten;

		stringstream StringBuffer;

		StringBuffer << std::fixed;

		// Vergangene Zeit in Sekunden
		StringBuffer << setw(12) << LAS_CurrTime++;

		// Teufe in m (Genauigkeit: 2 Stellen nach Komma)
		StringBuffer << setw(9) << setprecision(2) << float(VarCtrl->GetInfo_CurrDepth()) / 1000;

		// Geschwindigkeit in m / h (Genauigkeit: 1 Stelle nach Komma)
		StringBuffer << setw(9) << setprecision(1) << VarCtrl->GetInfo_AnnouncedVelocity();

		// Zugkraft
		switch (VarCtrl->GetParam_LOGFileTensionUnit()) {
		case 1: // in kg (kp)
			Hfloat = static_cast<float>(VarCtrl->GetInfo_CurrTraction()) / __GFORCE;
			StringBuffer << setw(6) << static_cast<int>(Hfloat);
			break;
		case 0:
		default: // in kN
			StringBuffer << setw(6) << ((VarCtrl->GetInfo_CurrTraction() + 500) / 1000); // + 500 um korrekt aufzurunden
			;
		}

		// Magnetmarke
        StringBuffer << setw(2) << VarCtrl->GetInfo_MagneticMarkStaticLOG();

		// momentan maximale Zugkraft (hier beachten, dass die automatische Maximalzugkraftberechnung Einfluss nehmen kann)
		switch (VarCtrl->GetParam_LOGFileTensionUnit()) {
		case 1: // in kg (kp)
			Hfloat = static_cast<float>(VarCtrl->GetInfo_CurrTensionLimit_Upper()) / __GFORCE;
			StringBuffer << setw(6) << static_cast<int>(Hfloat);
			break;
		case 0:
		default: // in kN
			StringBuffer << setw(6) << ((VarCtrl->GetInfo_CurrTensionLimit_Upper() + 500) / 1000); // + 500 um korrekt aufzurunden
			;
		}

		// Maximale Zugkraftdifferenz für Schleifenfänger
		switch (VarCtrl->GetParam_LOGFileTensionUnit()) {
		case 1: // in kg (kp)
			Hfloat = static_cast<float>(VarCtrl->GetParam_RelTensionLimitLower()) / __GFORCE;
			StringBuffer << setw(6) << static_cast<int>(Hfloat);
			break;
		case 0:
		default: // in kN
			StringBuffer << setw(6) << ((VarCtrl->GetParam_RelTensionLimitLower() + 500) / 1000); // + 500 um korrekt aufzurunden
			;
		}

		// Maximale Zeitdifferenz in Sekunden für Schleifenfänger (obsolet, nur noch Füllwert, bis Datenformat verändert wird)
//		StringBuffer << setw(3) << VarCtrl->GetParam_MaxTimeDifference();
		StringBuffer << setw(3) << 0;

		// Maximale Teufe im Fahrbereich in m (Genauigkeit: 2 Stellen nach Komma)
		StringBuffer << setw(9) << setprecision(2) << float(VarCtrl->GetParam_MaxDepth()) / 1000;

        // Minimale Teufe im Fahrbereich in m (Genauigkeit: 2 Stellen nach Komma)
        StringBuffer << setw(9) << setprecision(2) << float(VarCtrl->GetParam_MinDepth()) / 1000;

        // Ölfilter wechseln
        StringBuffer << setw(2) << (VarCtrl->GetInfo_OilStrainer() || VarCtrl->GetInfo_OilStrainer2());

		// RC2-2 Fehler tritt auf
		StringBuffer << setw(2) << VarCtrl->GetInfo_ControllerError();

        // Nebenabtrieb ein
        StringBuffer << setw(2) << VarCtrl->GetInfo_HydraulicOK();

		// Steuerhebel ein (Indiz für ausgelenkten Steuerhebel)
        StringBuffer << setw(2) << VarCtrl->GetInfo_LeverOn();

		// Automatische Fahrt ein
        StringBuffer << setw(2) << VarCtrl->GetControl_AutoRunOn();

        // Steuergerät Überwachung aus
        StringBuffer << setw(2) << !VarCtrl->GetParam_ControllerErrorMonit();

		LOGFile << StringBuffer.str() << endl;

#ifdef __WRITE_CRC_VALIDATION
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif
        StringBuffer.str("");

        if (LOGFile.bad()) return false;
        else return true;
};

// ------------------------------------
// Abschließen der LAS-Datei
bool WCLog::ConcludeFileLAS2_0()
{
        unsigned int HVuint;
        if ((LAS_StopPosition == 0) ||                                          // Prüfung, ob überhaupt LAS-Nachbehandlung notwendig (wenn 0, dann war's keine LAS-Datei)
            (!LOGFile.is_open()))                                               // Datei überhaupt geöffnet ?
                return true;

        stringstream StringBuffer;

        // Die Endzeit nachtragen
        StringBuffer << "STOP .S " << setw(30) << (LAS_CurrTime - 1) << " :STOP TIME";
        LOGFile.seekp(LAS_StopPosition);                                        // An entsprechende zuvor gemerkte Position springen
        LOGFile << StringBuffer.str() << endl;     // Eintrag vornehmen
#ifdef __WRITE_CRC_VALIDATION
        HVuint = ValidationFile.tellp();                                        // Position des Dateizeigers in der Validierungsdatei merken
        ValidationFile.seekp(ValidationFile_LAS_StopPosition);                  // auf zu aktualisierende Position setzen
        MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
        ValidationFile.seekp(HVuint);                                           // Position des Dateizeigers in der Validierungsdatei zurücksetzen
#endif
        StringBuffer.str("");

        if (LOGFile.bad()) return false;
		else return true;
};

// ------------------------------------
// GEOSYS-Standard V2
// ------------------------------------
// Daten-Header schreiben
bool WCLog::WriteHeaderGEOSYSV2()
{
	stringstream StringBuffer;
	float Hfloat;

	LAS_StopPosition = 0;   // Muss gesetzt werden, da beim Aufräumen sonst mit einer LAS-Datei gerechnet wird

	// Zeile 1: Identifikationszeile
	// -----------------------------
	StringBuffer.str("");
	StringBuffer << LOG_Identification_Prefix << __PROGNAME << __SEPARATOR01[LANGUAGE] << __VERSION << __SEPARATOR01[LANGUAGE] << __DATEOFCOMPIL << __SEPARATOR01[LANGUAGE] << __NUMBERPERDATE << __SEPARATOR01[LANGUAGE] << LOG_Identification_Suffix;
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 2: Leerzeile
	// ------------------
	LOGFile << endl;

	// Zeile 3: Start Of Header - Marke
	// --------------------------------
	StringBuffer.str("");
	StringBuffer << "[SOH]";
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 4: Messort
	// --------------------------------
	StringBuffer.str("");
	StringBuffer << "Location = ";
	StringBuffer << GOrg->LOG_LOCATION;
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 5: Bohrloch
	// --------------------------------
	StringBuffer.str("");
	StringBuffer << "Well = ";
	StringBuffer << GOrg->LOG_WELL;
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 6: Datum und Uhrzeit der Dateierstellung
	// ----------------------------------------------
	StringBuffer.str("");
	StringBuffer << "Date = ";
	StringBuffer << FileCreationTime[6];        // Tag
	StringBuffer << FileCreationTime[7];
	StringBuffer << ".";
	StringBuffer << FileCreationTime[4];        // Monat
	StringBuffer << FileCreationTime[5];
	StringBuffer << ".";
	StringBuffer << FileCreationTime[0];        // Tag
	StringBuffer << FileCreationTime[1];
	StringBuffer << FileCreationTime[2];
	StringBuffer << FileCreationTime[3];
	StringBuffer << " ";
	StringBuffer << FileCreationTime[8];        // Stunde
	StringBuffer << FileCreationTime[9];
	StringBuffer << "-";
	StringBuffer << FileCreationTime[10];       // Minute
	StringBuffer << FileCreationTime[11];
	StringBuffer << "-";
	StringBuffer << FileCreationTime[12];        // Sekunde
	StringBuffer << FileCreationTime[13];
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 7: Operator
	// --------------------------------
	StringBuffer.str("");
	StringBuffer << "Operator = ";
	StringBuffer << GOrg->LOG_NAME;
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 8: Operator
	// --------------------------------
	StringBuffer.str("");
	StringBuffer << "Team = ";
	StringBuffer << GOrg->LOG_TEAM;
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 9: Anlage / Maschine
	// --------------------------------
	StringBuffer.str("");
	StringBuffer << "Machinery = ";
	StringBuffer << GOrg->LOG_WINCHMACHINERY;
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 10: Comments
	// --------------------------------
	StringBuffer.str("");
	StringBuffer << "Comments = ";
	StringBuffer << GOrg->LOG_COMMENT;
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 11: End Of Header - Marke
	// --------------------------------
	StringBuffer.str("");
	StringBuffer << "[EOH]";
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 12: Leerzeile
	// --------------------------------
	LOGFile << endl;

	// Zeile 13: Start Of Parameter - Marke
	// ------------------------------------
	StringBuffer.str("");
	StringBuffer << "[SOP]";
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 14: Windennummer
	// --------------------------------
	StringBuffer.str("");
	StringBuffer << "WinchNumber = ";
	StringBuffer << VarCtrl->GetParam_WinchNo();
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 15: Teufenkalibrierwert
	// --------------------------------
	StringBuffer.str("");
	StringBuffer << "DepthCal = ";
	StringBuffer << VarCtrl->GetParam_CalFactDepth();
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 16: Zugkraftkalibrierwert [N/mV]
	// --------------------------------------
	StringBuffer.str("");
	StringBuffer << "TensionCal = ";
	StringBuffer << (VarCtrl->GetParam_CalibTractionY() / VarCtrl->GetParam_CalibTractionX());
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 17: Zugkraftoffset [N]
	// --------------------------------
	StringBuffer.str("");
	StringBuffer << "TensionOffset = ";
	StringBuffer << VarCtrl->GetParam_CalibTractionZero();
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 18: Eingestellte Maximalzugkraft
	// -------------------------------------------
	StringBuffer.str("");
	StringBuffer << "MaxTension = ";
	switch (VarCtrl->GetParam_LOGFileTensionUnit()) {
	case 1: // [kp]
		Hfloat = static_cast<float>(VarCtrl->GetParam_MaxTraction()) / __GFORCE;
		StringBuffer << static_cast<int>(Hfloat);
		break;
	case 0:
	default: // [kN]
		StringBuffer << (VarCtrl->GetParam_MaxTraction() + 500) / 1000; // + 500 um korrekt aufzurunden
	}
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 19: End Of Parameter
	// --------------------------------
	StringBuffer.str("");
	StringBuffer << "[EOP]";
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 20: Leerzeile
	// ------------------
	LOGFile << endl;

	// Zeile 21: Start Of Logging
	// -------------------------------------------
	StringBuffer.str("");
	StringBuffer << "[SOL]";
	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 22: Spaltenüberschriften der Datensatzzeilen im CSV-Format
	// ----------------------------------------------------------------
	StringBuffer.str("");

	StringBuffer << "Elapsed time" << "\t";
	StringBuffer << "Depth" << "\t";
	StringBuffer << "Magnetic mark" << "\t";
	StringBuffer << "Velocity" << "\t";
	StringBuffer << "Tension" << "\t";
	StringBuffer << "Controller error" << "\t";
	StringBuffer << "Change oil strainer" << "\t";
	StringBuffer << "Hydraulic" << "\t";
	StringBuffer << "Joystick" << "\t";
	StringBuffer << "Automatic" << "\t";

	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

	// Zeile 23: Einheitenangabe der Datensatzzeilen im CSV-Format
	// ----------------------------------------------------------------
	StringBuffer.str("");

	StringBuffer << "s" << "\t";		// Elapsed time
	StringBuffer << "m" << "\t";		// Depth
	StringBuffer << "0/1" << "\t";		// Magnetic mark
	StringBuffer << "m/h" << "\t";		// Velocity

	switch (VarCtrl->GetParam_LOGFileTensionUnit()) {
	case 1: // [kp]
		StringBuffer << "kp" << "\t";		// Tension
		break;
	case 0:
	default: // [kN]
		StringBuffer << "kN" << "\t";		// Tension
		;
	}

	StringBuffer << "0/1" << "\t";		// Controller error
	StringBuffer << "0/1" << "\t";		// Change oil strainer
	StringBuffer << "0/1" << "\t";		// Hydraulic
	StringBuffer << "0/1" << "\t";		// Joystick
	StringBuffer << "0/1" << "\t";		// Automatic
	StringBuffer << "0/1";				// External regulator

	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

}
// ------------------------------------
// Schreibt einen Datensatz
bool WCLog::WriteDataSetGEOSYSV2()
{
	++DatasetWritten;	// Da Sekundenweise aufgerufen, wird hier die vergangene Zeit protokolliert

	stringstream StringBuffer;
	float Hfloat;

	StringBuffer.str("");

	StringBuffer << DatasetWritten << "\t";										// Elapsed time
	StringBuffer << float(static_cast<float>(VarCtrl->GetInfo_CurrDepth()) / 1000) << "\t";	// Depth
	StringBuffer << VarCtrl->GetInfo_MagneticMarkStaticLOG() << "\t";					// Magnetic mark
	StringBuffer << VarCtrl->GetInfo_CurrVelocity() << "\t";					// Velocity

	switch (VarCtrl->GetParam_LOGFileTensionUnit()) {
	case 1: // [kp]
		Hfloat = static_cast<float>(VarCtrl->GetInfo_CurrTraction()) / __GFORCE;
		StringBuffer << static_cast<int>(Hfloat) << "\t";						// Tension
		break;
	case 0:
	default: // [kN]
		StringBuffer << ((VarCtrl->GetInfo_CurrTraction() + 500) / 1000) << "\t";				// Tension
		;
	}

	StringBuffer << VarCtrl->GetInfo_ControllerError() << "\t";					// Controller error
	StringBuffer << (VarCtrl->GetInfo_OilStrainer() || VarCtrl->GetInfo_OilStrainer2()) << "\t"; // Change oil strainer
	StringBuffer << VarCtrl->GetInfo_HydraulicOK() << "\t";						// Hydraulic
	StringBuffer << VarCtrl->GetInfo_LeverOn() << "\t";							// Joystick
	StringBuffer << VarCtrl->GetControl_AutoRunOn() << "\t";					// Automatic

	LOGFile << StringBuffer.str() << endl;
#ifdef __WRITE_CRC_VALIDATION
	MakeCRC16_1(StringBuffer.str().c_str(), StringBuffer.str().length());
#endif

}


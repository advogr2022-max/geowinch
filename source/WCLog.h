//---------------------------------------------------------------------------
#ifndef WCLogH
#define WCLogH

#include "constants.h"
#include "structs.h"
#include "WCAnalysis.h"
#include <fstream.h>
//---------------------------------------------------------------------------
class WCLog
{
        private:
                        ofstream LOGFile;                                       // Datei-Handle
                        int DatasetWritten;                                     // Gibt an, ob mindestens ein Datensatz in die aktuelle Datei geschrieben wurde (true)
                        char FileCreationTime[15];                              // Puffer der Zeit, zu der die Aufzeichnung in die aktuelle Datei begonnen hat
                        int LANGUAGE;                                           // Die zu verwendende Sprache

                        ofstream ValidationFile;                                // Datei-Handle für Gültigkeitsprüfung  

                        int LAS_CurrTime;                                       // Zeit für Eintrag in LAS-Datei
                        unsigned int LAS_StopPosition;                          // Position des Eintrags für die Stopp(End)-Zeit
                        unsigned int ValidationFile_LAS_StopPosition;           // Position des Eintrags für Prüfsequenz für LAS-Stopp-Zeit-Zeile

                        WCGenOrganisationS *GOrg;                               // Zeiger auf die Organisations-Struktur
                        WCVarControl *VarCtrl;                                  // Zeiger auf die Datenquelle

                        int Col1Size, Col2Size, Col3Size, Col4Size, Col5Size;   // Die Breiten der jeweiligen Spalten

                        // Hilfsmethoden
                        // =============
                        char *ManageLOGFileName(char *oldLOGFileName, int Action); // Verwaltet den LOG-Datei-Namen
                        char *ManageLOGFilePath(char *LOGFileName, char *oldLOGFilePath, int Action); // Verwaltet den Namen des vollständigen Pfads der LOG-Datei
                        char *ManageValidationFilePath(char *LOGFileName, char *oldValidationFilePath, int Action); // Verwaltet den Namen des vollständigen Pfads der zur LOG-Datei gehörigen Validierungsdatei

                        // Dateiverwaltung
                        // ===============
                        // Validierungsdatei
                        bool MakeCRC16_1(const char *Data, int Length);         // Ermittelt eine 16-Bit CRC-Sequenz mit Vorgehensweise "1" und schreibt diese in die Validierungsdatei
                        bool OpenValidationFile(char *FilePath, char *ReferenceFile);  // Öffnet eine neue Datendatei für die Gültigkeitsüberprüfung
                        bool CloseValidationFile();                             // Schließt die Datendatei für die Gültigkeitsüberprüfung
                        bool CleanUpValidationFile(char *FilePath);             // Löscht Validierungs-Dateien

                        bool WriteHeader();                                     // Schreibt den DateiHeader

                        // Protokolldatei
                        bool OpenFile(char *FilePath);                          // Öffnet eine neue LOG-Datei
                        bool CloseFile();                                       // Schließt eine geöffnete LOG-Datei
                        bool CleanUpEmptyFiles(char *FilePath);                 // Löscht LOG-Dateien, die keinen LOG-Daten enthalten (wäre sonst Platzverschwendung und Unordnung)

                        // LOG-File-Format-spezifische Routinen
                        // ====================================
                        // GEOSYS-Standard
                        bool WriteHeaderGEOSYS();                               // Datei-Header schreiben
						bool WriteDataSetGEOSYS();                              // Schreibt einen Datensatz
						// ------------------------------------
						// LAS 2.0
						bool WriteHeaderLAS2_0();                               // Datei-Header schreiben
						bool WriteDataSetLAS2_0();                              // Schreibt einen Datensatz
						bool ConcludeFileLAS2_0();                              // Abschließen der LAS-Datei
						// ------------------------------------
						// GEOSYS-Standard V2
						bool WriteHeaderGEOSYSV2();								// Daten-Header schreiben
						bool WriteDataSetGEOSYSV2();                            // Schreibt einen Datensatz

		protected:

        public:
                        WCLog(WCGenOrganisationS *GenOrg,
                                  WCVarControl *VarControl);                    // Konstruktor
                        ~WCLog();                                               // Destruktor

                        bool StartLOG();                                        // Mit neuer LOG-Datei weiterarbeiten
                        bool StopLOG();                                         // Logging unterbrechen und Datei schließen

                        bool WriteDataSet();                                    // Schreibt einen Datensatz
};

#endif

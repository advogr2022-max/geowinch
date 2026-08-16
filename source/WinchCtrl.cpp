//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USEFORM("Splash.cpp", SplashForm);
USEFORM("GUIMain.cpp", MainForm);
USEFORM("DevelF.cpp", DevelForm);
//---------------------------------------------------------------------------
#include "Labelling.h"
#include "constants.h"
#include <iostream.h>
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        // EXTRA Parameter-Überprüfung, um Programmversionsausgabe-Anfrage festzustellen
        for (int i = 0; i <= ParamCount(); ++i)
        {
                if (ParamStr(i).c_str()[0] == CMDP_Entry)
                {
                        // ---------------
                        // Versionsinfo ausgeben
                        // Bsp.: "WinchControl 1.2.0ru November 22, 2005 0 
                        if (ParamStr(i).Pos(CMDP_PROGRAM_VERSION) == 2)
                        {
                                AnsiString VerInfo = (AnsiString)(__PROGNAME) + " " + __VERSION + " " + __DATEOFCOMPIL + " " + __NUMBERPERDATE;
                                ShowMessage(VerInfo);
//                                cout << endl << __PROGNAME << " " << __VERSION << " " << __DATEOFCOMPIL << " " << __NUMBERPERDATE << endl;
                                return 0;                                       // nach Ausgabe der Versionsinformation wird das Programm sofort wieder beendet.
                        };
                };

        };

        // Test, ob bereits eine Instanz der Anwendung existiert
        HANDLE hSem = CreateSemaphore(NULL, 0, 1, __PROGNAME);
        if (hSem)
        {
            if (GetLastError() == ERROR_ALREADY_EXISTS)
            {
                //Die Semaphore existiert bereits, also beenden
                ShowMessage(ERRMSG_PROGRAM_INSTANCE_EXISTS[__ENGLISH]); // Ausgabe erfolgt ausschließlich in englisch
                CloseHandle(hSem);
                return 0;
            }
        };

        // Programmstart-Verzögerung, bis exzessive Systemauslastung durch Anwendung eingeleitet wird
        if (PROGRAM_START_DELAY > 0) Sleep(PROGRAM_START_DELAY);

        // Programm-Priorität erhöhen
        SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
//        SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

        // Fenster-Arbeit beginnen
        try
        {
            Application->Initialize();
            Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->Run();
        }
        catch (Exception &exception)
        {
            Application->ShowException(&exception);
        }

        CloseHandle(hSem);
        return 0;
}
//---------------------------------------------------------------------------

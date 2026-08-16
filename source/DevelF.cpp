//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "DevelF.h"
#include "GUIMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDevelForm *DevelForm;

// Konfiguration
// -------------
const      DEVELVIEWLINES =                             12;
const char *EMPTY_LINE =                                "";

const char *CONTROLTHREADLPS_LABEL =                    "Control Thread LPS = ";
const char *MAXTENSION_LABEL =                          "Maximum Tension = ";
const char *MAXTENSION_UNIT =                           " N";
const char *TENSIONVOLTAGE_LABEL =                      "Traction Voltage = ";
const char *TENSIONVOLTAGE_UNIT =                       " mV";
const char *WINCHCONTROLVOLTAGE_LABEL =                 "Winch Control Voltage = ";
const char *WINCHCONTROLVOLTAGE_UNIT =                  " mV";
const char *WINCHCONTROLVOLTAGEMESSURED_LABEL =         "Messured Winch Control Voltage = ";
const char *WINCHCONTROLVOLTAGEMESSURED_UNIT =          " mV";
const char *WINCHCONTROLAUTOPID_E_LABEL =               "Automatic PID e = ";
const char *WINCHCONTROLAUTOPID_E_UNIT =                " m/h";
const char *WINCHCONTROLAUTOPID_EDIFF_LABEL =           "Automatic PID ediff = ";
const char *WINCHCONTROLAUTOPID_EDIFF_UNIT =            " m/h";
const char *WINCHCONTROLAUTOPID_ESUM_LABEL =            "Automatic PID esum = ";
const char *WINCHCONTROLAUTOPID_ESUM_UNIT =             " m/h";
const char *WINCHCONTROLAUTOPID_Y_LABEL =               "Automatic PID result = ";
const char *WINCHCONTROLAUTOPID_Y_UNIT =                " mV";
const char *WINCHCONTROLAUTOPID_OUTVOLTAGE_LABEL =      "Automatic PID out = ";
const char *WINCHCONTROLAUTOPID_OUTVOLTAGE_UNIT =       " mV";
const char *WINDOWCLIENTSIZE_LABEL =                    "Window Client Size = ";
const char *UNLOADCABELPIDCLOCK_LABEL =              	"Unload Cable PID Clock = ";
const char *UNLOADCABELPIDCLOCK_UNIT =               	" ms";
const char *UNLOADCABELPIDQ0_LABEL =                 	"Unload Cable PID q0 = ";
const char *UNLOADCABELPIDQ1_LABEL =                 	"Unload Cable PID q1 = ";
const char *UNLOADCABELPIDQ2_LABEL =                 	"Unload Cable PID q2 = ";
const char *UNLOADCABLETENSIONREFERENCE_LABEL =      	"Unload Cable Traction Reference = ";
const char *UNLOADCABLETENSIONREFERENCE_UNIT =       	" N";
const char *DISPLAYTHREADLPS_LABEL =                    "Display Thread LPS = ";
const char *STATIC_DEVELVIEW_LINE[DEVELVIEWLINES] =     {
                                                                "System",
                                                                "------------------------------------",
                                                                CONTROLTHREADLPS_LABEL,
																DISPLAYTHREADLPS_LABEL,
																WINDOWCLIENTSIZE_LABEL,
																EMPTY_LINE,
																"Tension",
																"------------------------------------",
																TENSIONVOLTAGE_LABEL,
																MAXTENSION_LABEL,
																UNLOADCABLETENSIONREFERENCE_LABEL,
																EMPTY_LINE
														};

//---------------------------------------------------------------------------
__fastcall TDevelForm::TDevelForm(TComponent* Owner)
		: TForm(Owner)
{
		// Voreinstellungen
		MakeStaticLayout();
}

//---------------------------------------------------------------------------
// Statisches Layout zusammenstellen
void __fastcall TDevelForm::MakeStaticLayout()
{
		// Aufräumen unnötiger Darstellungen
		Caption = "";
		DevelButtonPanel->Caption = "";
		StandardScreenResolutionButton->Caption = "1024x768";
		TerminateProgramPanel->Caption = "";
		TerminateProgramButton->Caption = "Exit WinchCtrl";
		MakeStaticDevelViewMemo();
};

//---------------------------------------------------------------------------
// Statische Zeilen in DevelViewMemo darstellen
void __fastcall TDevelForm::MakeStaticDevelViewMemo()
{
		DevelViewMemo->Clear();
		DevelViewMemo->Font->Pitch = TFontPitch::fpFixed;

		for (int i = 0; i < DEVELVIEWLINES; ++i)
		{
				DevelViewMemo->Lines->Append(STATIC_DEVELVIEW_LINE[i]);
		};
};

//---------------------------------------------------------------------------
// Aktualisiert die darzustellenden Parameter
void __fastcall TDevelForm::ActualizeDevelViewMemo(WCVarControl *Var, WCGenOrganisationS *GOrg)
{
		int HV;
		if ((Var == 0) || (GOrg == 0)) return;

		// Control Thread: LPS
		HV = 2;
		DevelViewMemo->Lines->Strings[HV++] = (AnsiString)CONTROLTHREADLPS_LABEL + FloatToStrF(GOrg->ControlThreadLoopsPerSecound, ffFixed, 7, 1);
		DevelViewMemo->Lines->Strings[HV++] = (AnsiString)DISPLAYTHREADLPS_LABEL + FloatToStrF(GOrg->DisplayThreadLoopsPerSecound, ffFixed, 7, 1);
		DevelViewMemo->Lines->Strings[HV++] = (AnsiString)WINDOWCLIENTSIZE_LABEL + IntToStr(GOrg->MainForm_ClientWidth) + "x" + IntToStr(GOrg->MainForm_ClientHeight);

		HV += 3;
		DevelViewMemo->Lines->Strings[HV++] = (AnsiString)TENSIONVOLTAGE_LABEL + IntToStr(Var->GetInfo_CurrTractionVoltage()) + TENSIONVOLTAGE_UNIT;
		DevelViewMemo->Lines->Strings[HV++] = (AnsiString)MAXTENSION_LABEL + IntToStr(Var->GetInfo_CurrTensionLimit_Upper()) + MAXTENSION_UNIT;
		DevelViewMemo->Lines->Strings[HV++] = (AnsiString)UNLOADCABLETENSIONREFERENCE_LABEL + IntToStr(Var->GetInfo_CurrTractionReference()) + UNLOADCABLETENSIONREFERENCE_UNIT;

};

void __fastcall TDevelForm::TerminateProgramButtonClick(TObject *Sender)
{
        MainForm->ExitButtonClick(this);
}
//---------------------------------------------------------------------------

void __fastcall TDevelForm::StandardScreenResolutionButtonClick(
      TObject *Sender)
{
        MainForm->WindowState = wsNormal;
        MainForm->ClientWidth = 1024;
        MainForm->ClientHeight = 768;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
#ifndef DevelFH
#define DevelFH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "VarControl.h"
#include "structs.h"
//---------------------------------------------------------------------------
class TDevelForm : public TForm
{
__published:	// Von der IDE verwaltete Komponenten
        TPanel *DevelButtonPanel;
        TMemo *DevelViewMemo;
        TPanel *TerminateProgramPanel;
        TButton *TerminateProgramButton;
        TButton *StandardScreenResolutionButton;
        void __fastcall TerminateProgramButtonClick(TObject *Sender);
        void __fastcall StandardScreenResolutionButtonClick(
          TObject *Sender);
private:	// Anwenderdeklarationen
public:		// Anwenderdeklarationen
        __fastcall TDevelForm(TComponent* Owner);
        void __fastcall MakeStaticLayout();                                     // Statisches Layout zusammenstellen
        void __fastcall MakeStaticDevelViewMemo();                              // Statische Zeilen in DevelViewMemo darstellen
        void __fastcall ActualizeDevelViewMemo(WCVarControl *Var, WCGenOrganisationS *GOrg); // Aktualisiert die darzustellenden Parameter
};
//---------------------------------------------------------------------------
extern PACKAGE TDevelForm *DevelForm;
//---------------------------------------------------------------------------
#endif

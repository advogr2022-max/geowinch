//---------------------------------------------------------------------------
#ifndef SplashH
#define SplashH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TSplashForm : public TForm
{
__published:	// Von der IDE verwaltete Komponenten
        TImage *SplashImage;
        TTimer *SplashTimer;
        void __fastcall TimerEvent(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormClicked(TObject *Sender);
private:	// Anwenderdeklarationen
        int DelayCounter;
        void __fastcall EndSplash();
public:		// Anwenderdeklarationen
        __fastcall TSplashForm(TComponent* Owner);
        int Delay;      // Wartezeit bis Abbruch in Sekunden
        AnsiString SplashText;
};
//---------------------------------------------------------------------------
extern PACKAGE TSplashForm *SplashForm;
//---------------------------------------------------------------------------
#endif

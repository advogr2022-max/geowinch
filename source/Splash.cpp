//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Splash.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSplashForm *SplashForm;
//---------------------------------------------------------------------------
__fastcall TSplashForm::TSplashForm(TComponent* Owner)
        : TForm(Owner)
{
        SplashTimer->Enabled = false;
        Delay = 5;      // Standard Wartezeit
        WindowState = wsMaximized;
        DelayCounter = 0;
}
//---------------------------------------------------------------------------
void __fastcall TSplashForm::TimerEvent(TObject *Sender)
{
        if (DelayCounter < Delay) ++DelayCounter;
        else EndSplash();       
}
//---------------------------------------------------------------------------

void __fastcall TSplashForm::FormShow(TObject *Sender)
{
        SplashImage->Canvas->Brush->Color = SplashImage->Canvas->Pixels[0][0];
        SplashImage->Canvas->Font->Color = clWhite;
        SplashImage->Canvas->Font->Style = SplashImage->Canvas->Font->Style << fsBold;
        SplashImage->Canvas->Font->Height = -SplashImage->Height / 8;
        SplashImage->Canvas->TextOut(SplashImage->Height / 8, SplashImage->Height / 8, SplashText);

        SplashTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TSplashForm::EndSplash()
{
        SplashTimer->Enabled = false;
        ModalResult = mrOk;
};

//---------------------------------------------------------------------------

void __fastcall TSplashForm::FormClicked(TObject *Sender)
{
        EndSplash();
}
//---------------------------------------------------------------------------


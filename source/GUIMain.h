//---------------------------------------------------------------------------
#ifndef GUIMainH
#define GUIMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
#include <ImgList.hpp>
#include "VirtualKeyBoard.h"
#include "DirectionViewVert01.h"
#include "TractionViewHor01.h"
#include <ToolWin.hpp>
#include "CSPIN.h"
#include <Mask.hpp>
#include "WCDisplayThread.h"
#include "WCControlThread.h"
#include "AnalogMeter01.h"
#include "CGAUGES.h"
#include "DepthVisualisation01.h"
#include "LED01.h"
//#include "VirtualKeyboard2.h"
#include <Graphics.hpp>
#include "DirectionViewVert02.h"
#include <Grids.hpp>
//#include "VirtualKeyBoard2.h";
//#include "VirtualKeyboard2.h"
#include "AnalogMeter01.h"
//#include "DepthVisualisation01.h"
#include "LED01.h"
#include <Graphics.hpp>
#include "CGAUGES.h"
#include "tools.h"
#include "AverageManager_float.h"
#include "VirtualKeyBoard2.h"
#include "Thermometer01.h"
#include "SpeedView01.h"
#include "GaugeVert01.h"
#include "VirtKeypad.h"
#include <ValEdit.hpp>
#include <ButtonGroup.hpp>
#include <CategoryButtons.hpp>
#include "JMAdvLabeledEdit.h"
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// Von der IDE verwaltete Komponenten
	TPanel *MainCtlPanel;
        TPanel *FunctionPanel;
        TPanel *MainViewPanel;
        TSpeedButton *StartStopButton;
        TSpeedButton *SignalHornButton;
	TSpeedButton *ResetButton;
        TPanel *MessagePanel;
        TPanel *KeyBoardPanel;
        TPanel *MainViewHelpPanel1;
        TLabel *ProgramModeLabel;
        TGroupBox *CurrDepthBox;
        TLabel *CurrDepthLabel;
        TPanel *NormalViewPanel;
        TPanel *ParameterPanel;
        TPanel *DialogPanel;
        TPanel *InfoPanel;
        TPanel *FileAdminPanel;
        TPanel *ChoicePanel;
        TSpeedButton *ChoiceButton1;
        TSpeedButton *ChoiceButton2;
        TSpeedButton *ChoiceButton3;
        TSpeedButton *ChoiceButton4;
        TSpeedButton *ChoiceButton5;
        TLabel *ChoiceLabel1;
        TLabel *ChoiceLabel2;
        TLabel *ChoiceLabel3;
        TLabel *ChoiceLabel4;
        TLabel *ChoiceLabel5;
        TSpeedButton *ChoiceAbortButton;
        TPanel *CalibDepthPanel;
        TPanel *CalibTractionPanel;
		TPanel *UnloadCablePanel;
        TLabel *ChoiceCaptionLabel;
        TSpeedButton *InfoOkButton;
        TLabel *InfoCaptionLabel;
        TLabel *TimeLabel;
        TMemo *InfoMemo;
        TSpeedButton *EngineButton;
        TSpeedButton *EngineRevolutionDownButton;
        TSpeedButton *EngineRevolutionUpButton;
        TTimer *Timer;
        TPageControl *ParamPageControl;
        TSpeedButton *FastRunButton;
        TSpeedButton *LostMotionImpulseButton;
        TTabSheet *WinchTabSheet;
        TTabSheet *RunTabSheet;
        TTabSheet *CableTabSheet;
        TTabSheet *DepthCalcTabSheet;
        TTabSheet *SuperVisionTabSheet;
	TSpeedButton *WinchTabSheetExitButton;
	TSpeedButton *RunTabSheetExitButton;
	TSpeedButton *CableTabSheetExitButton;
	TSpeedButton *DepthCalcTabSheetExitButton;
	TSpeedButton *SuperVisionTabSheetExitButton;
        TCheckBox *PIncEncErrorMonitCheckBox;
        TSpeedButton *PChangeDirectionButton;
        TMemo *MessageListBox;
        TLabel *CalibTractionCaptionLabel;
        TGroupBox *CalibTractionCompleteBox;
        TGroupBox *CalibTractionZeroBox;
        TSpeedButton *CalibTractionAbortButton;
        TLabel *CalibTractionCompleteInfoLabel;
        TLabel *CalibTractionZeroInfoLabel;
        TSpeedButton *CalibTractionMessure1Button;
        TSpeedButton *CalibTractionMessure2Button;
        TSpeedButton *CalibTractionGetZeroButton;
        TSpeedButton *CalibTractionCompleteSetButton;
        TPanel *CalibDepthFramePanel;
        TLabel *CalibDepthCaptionLabel;
        TLabel *CalibDepthInfoLabel;
        TSpeedButton *CalibDepthAcceptButton;
	TGroupBox *PCalibTensionGroupBox;
	TLabel *UnloadCablePanelCaptionLabel;
	TGroupBox *UnloadCableStretchGroupBox;
	TCheckBox *UnloadCabelStretchCheckBox;
	TSpeedButton *UnloadCableExitButton;
        TAnalogMeter01 *CurrTractionView;
        TGroupBox *Ext_cm_Imp_GroupBox;
        TRadioButton *PKARATRadioButton;
        TRadioButton *PONYXRadioButton;
        TRadioButton *PKEDRRadioButton;
        TPanel *LOGFilePanel;
        TLabel *LOGFileCaptionLabel;
        TPageControl *LOGFilePageControl;
        TTabSheet *LOGFileCharacteristicsTabSheet;
        TTabSheet *LOGFileFileAdminTabSheet;
        TLabel *LOGFileLocationLabel;
        TMaskEdit *LOGFileLocationMaskEdit;
	TLabel *LOGFileWellLabel;
        TLabel *LOGFileNameLabel;
        TGroupBox *LOGFileCommentsGroupBox;
	TMaskEdit *LOGFileWellMaskEdit;
        TMaskEdit *LOGFileNameMaskEdit;
        TGroupBox *LOGFileLanguageFormatGroupBox;
        TMemo *LOGFileCommentsMemo;
        TRadioButton *LOGFileLanguageGermanRadioButton;
        TRadioButton *LOGFileLanguageRussianRadioButton;
        TRadioButton *LOGFileLanguageEnglishRadioButton;
        TPanel *LOGFileButtonPanel;
        TSpeedButton *LOGFileOkButton;
        TSpeedButton *LOGFileCancelButton;
        TPanel *LOGFileAdminHelpPanel1;
        TPanel *LOGFileAdminHelpPanel2;
        TPanel *LOGFileAdminButtonPanel;
        TSpeedButton *LOGFileAdminCancelButton;
        TListBox *LOGFileAdminFileListBox;
        TGroupBox *LOGFileAdminLanguageGroupBox;
        TRadioButton *LOGFileAdminGermanRadioButton;
        TRadioButton *LOGFileAdminRussianRadioButton;
        TRadioButton *LOGFileAdminEnglishRadioButton;
        TComboBox *LOGFileAdminDestinationComboBox;
        TSpeedButton *LOGFileAdminCopyButton;
        TLabel *LOGFileAdminDestinationLabel;
        TCGauge *ProgressGauge;
        TPanel *SystemPanel;
        TGroupBox *SystemDatetimeGroupBox;
        TGroupBox *SystemControlGroupBox;
        TPanel *SystemHelpPanel;
        TSpeedButton *SystemCancelButton;
        TSpeedButton *SystemShutdownButton;
        TSpeedButton *SystemRestartButton;
        TSpeedButton *SystemProgramTerminationButton;
        TMaskEdit *SystemDateMaskEdit;
        TMaskEdit *SystemTimeMaskEdit;
        TLabel *SystemDateLabel;
        TLabel *SystemTimeLabel;
        TSpeedButton *SystemDateTimeAcceptButton;
	TScrollBox *UnloadCableStretchScrollBox;
	TPanel *UnloadCableHelpPanel;
        TTabSheet *ViewTabSheet;
	TSpeedButton *ViewTabSheetExitButton;
        TLabel *PTractionViewLabel;
        TComboBox *PTractionViewComboBox;
        TSpeedButton *DialogOkSpeedButton;
        TSpeedButton *DialogCancelSpeedButton;
        TLabel *DialogCaptionLabel;
        TLabel *IdentificationSystemLabel;
        TLabel *DialogInfoLabel;
	TCheckBox *PControllerErrorMonitCheckBox;
        TSpeedButton *AlarmSoundOnOffButton;
        TGroupBox *LOGFileTypeGroupBox;
	TRadioButton *LOGFileTypeGeoSysV1RadioButton;
        TRadioButton *LOGFileTypeLAS2RadioButton;
	TUpDown *UnloadCableMinTractionUpDown;
		TSpeedButton *SetParameterButton;
        TSpeedButton *CalibrateButton;
        TSpeedButton *FileAdminButton;
	TSpeedButton *UnloadCableButton;
        TSpeedButton *DevelFormButton;
        TSpeedButton *SystemButton;
        TLabel *ComputerNameLabel;
        TRadioButton *PKARAT2RadioButton;
        TRadioButton *PKARAT2NRadioButton;
        TLabel *DepthCalcSWVersionLabel;
        TMemo *LOGFileAdminCurrFileMemo;
        TSpeedButton *LOGFileAdminDeleteButton;
        TPanel *SystemHelpPanel1;
        TGroupBox *SystemTemperatureGroupBox;
        TLabel *SystemTempDepthCalcDescrLabel;
        TLabel *SystemTempADescrLabel;
        TLabel *SystemTempBDescrLabel;
        TLabel *SystemTempDepthCalcLabel;
        TLabel *SystemTempALabel;
        TLabel *SystemTempBLabel;
        TSpeedButton *WinchBrakeOffSpeedButton;
        TSpeedButton *EngineStopButton;
	TLabel *PCalibTensionCalibInfoLabel;
        TScrollBox *WinchTabSheetScrollBox;
        TScrollBox *RunTabSheetScrollBox;
        TScrollBox *CableTabSheetScrollBox;
        TScrollBox *DepthCalcTabSheetScrollBox;
        TScrollBox *SuperVisionTabSheetScrollBox;
        TScrollBox *ViewTabSheetScrollBox;
        TGroupBox *SpecialSignalModeGroupBox;
        TRadioButton *PSpecialSignalMode0RadioButton;
        TRadioButton *PSpecialSignalMode1RadioButton;
        TPanel *ControlLampPanel;
        TPanel *NormalViewHelpPanel1;
        TLabel *EmergencySwitchLabel;
        TLabel *MagneticMarkOnOffLabel;
        TLabel *ChangeOilFilterLabel;
        TLabel *AuxiallyDriveLabel;
        TLabel *DynamicTensionOnOffLabel;
        TLabel *AutomotiveErrorLabel;
        TPanel *NormalViewHelpPanel2;
        TDepthVisualisation01 *DepthVisualisation;
        TCheckBox *PEngineRevolutionChangeableCheckBox;
        TRadioButton *PKEDRNRadioButton;
    TSpeedButton *DynamicTensionOnOffButton;
        TImage *Ext_cm_Imp_Image;
        TSpeedButton *PowerFailCdResetSpeedButton;
        TLabel *PowerFailCdLabel;
        TVirtualKeyboard2 *VirtualKeyboard21;
        TSpeedButton *MagneticMarkOnOffButton;
    TSpeedButton *EmergencySwitch_DeadButton;
    TSpeedButton *AutomotiveError_DeadButton;
    TSpeedButton *ChangeOilFilter_DeadButton;
    TSpeedButton *AuxiallyDrive_DeadButton;
    TGroupBox *PProgramAutomGroupBox;
    TCheckBox *PProgramAutomCheckBox;
    TListBox *PProgramAutomFileListBox;
    TLabel *PProgramAutomPrgNameLabel;
    TPanel *AutoRunProgramItemListPanel;
    TLabel *AutoRunProgramItemListLabel;
    TListBox *AutoRunProgramItemListListBox;
    TGroupBox *AutoRunProgramItemListGroupBox;
    TCheckBox *PEngineStartStopEnableCheckBox;
    TRadioButton *PSpecialSignalMode2RadioButton;
	TCheckBox *PReverseDepthCheckBox;
	TPanel *MagneticMarkPanel;
	TPanel *LogFileLanguageFormatPanel;
	TScrollBox *LOGFileCharacteristicsScrollBox;
	TMaskEdit *LOGFileTeamMaskEdit;
	TMaskEdit *LOGFileWinchMachineryMaskEdit;
	TLabel *LOGFileTeamLabel;
	TLabel *LOGFileWinchMachineryLabel;
	TPanel *LOGFileTypePanel;
	TRadioButton *LOGFileTypeGeoSysV2RadioButton;
	TGroupBox *LOGFileTensionUnitGroupBox;
	TPanel *LogFileTensionUnitPanel;
	TRadioButton *LOGFileTensionUnit_kN_RadioButton;
	TRadioButton *LOGFileTensionUnit_kp_RadioButton;
	TRadioButton *PSpecialSignalMode3RadioButton;
	TCheckBox *PLostMotionEnableCheckBox;
	TCheckBox *PWinchBrakeInternalAutoCheckBox;
	TRadioButton *PSpecialSignalMode4RadioButton;
	TListBox *PWinchCfgListBox;
	TLabel *PVelocityViewLabel;
	TComboBox *PVelocityViewComboBox;
	TGroupBox *PControllerErrorGroupBox;
	TCheckBox *PControllerErrorLactiveCheckBox;
	TCheckBox *PBrakeOffEnableCheckBox;
	TRadioButton *PSpecialSignalMode5RadioButton;
	TCheckBox *PControllerErrorAtSMCheckBox;
	TSpeedButton *MainMenuSpeedButton;
	TPanel *MainMenuPanel;
	TSpeedButton *CalibDepthAbortButton;
	TSpeedButton *MessagePanelButton;
	TPanel *EngineCtlPanel;
	TPanel *WinchDriveCtlPanel;
	TScrollBox *LOGFileAdminScrollBox;
	TPanel *MessageButtonPanel;
	TButton *MessagePanelOkButton;
	TPanel *MessageLight;
	TLabel *CtlUnitSWVersionLabel;
	TPanel *HydraulicCtlPanel;
	TAnalogMeter01 *HydrWrkPressureView;
	TSpeedView01 *SpeedView;
	TAnalogMeter01 *EngineSpeedView;
	TSpeedButton *SpecialFuncButton;
	TLabel *SpecialFuncLabel;
	TGaugeVert01 *HydrTemperatureView;
	TPanel *UnloadCableTopHelpPanel;
	TPanel *LOGFileCharacteristicsHelpPanel;
	VirtKeypad *VirtKeypad1;
	TAdvLabeledEdit *PMinDepthEdit;
	TAdvLabeledEdit *PMaxDepthEdit;
	TAdvLabeledEdit *PVelocityEdit;
	TAdvLabeledEdit *PMaxTensionEdit;
	TAdvLabeledEdit *PRelTensionLimitLowerEdit;
	TAdvLabeledEdit *PRelTensionLimitUpperEdit;
	TAdvLabeledEdit *PCalibTensionXEdit;
	TAdvLabeledEdit *PCalibTensionYEdit;
	TAdvLabeledEdit *PCalibTensionZeroEdit;
	TAdvLabeledEdit *PSetDepthEdit;
	TAdvLabeledEdit *PDepthCalFactorEdit;
	TAdvLabeledEdit *PMMSyncForceRangeLimEdit;
	TAdvLabeledEdit *UnloadCableSetTensionEdit;
	TAdvLabeledEdit *UnloadCableStretchStartLenghtEdit;
	TAdvLabeledEdit *UnloadCableStretchEndLenghtEdit;
	TAdvLabeledEdit *UnloadCableStretchStartTensionEdit;
	TAdvLabeledEdit *UnloadCableStretchEndTensionEdit;
	TAdvLabeledEdit *CalibDepth_DepthEdit;
	TLabel *CalibDepthHint1_SwitchOffMM_Label;
	TLabel *CalibDepthHint2_SetDepth_Label;
	TLabel *CalibDepthHint3_MoveLine_Label;
	TLabel *CalibDepthHint4_InsertCorrectDepth_Label;
	TGroupBox *CalibDepthEdit_GroupBox;
	TLabel *CalibDepthInfo_ActiveCalFact_Label;
	TLabel *CalibDepthInfo_EarmarkedCalFact_Label;
	TLabel *CalibDepthInfo_ImpulseCount_Label;
	TAdvLabeledEdit *CalibTension_Tension1Edit;
	TAdvLabeledEdit *CalibTension_Tension2Edit;
	TCheckBox *PHydraulikBrakeEnableCheckBox;
	TTabSheet *AdditionalFuncTabSheet;
	TSpeedButton *AdditionalFuncTabSheetExitButton;
	TScrollBox *AdditionalFuncTabSheetScrollBox;
	TGroupBox *PLineLubGroupBox;
	TCheckBox *PLineLubAvailableCheckBox;
	TPanel *PLineLubTunePanel;
	TCheckBox *PLineLubPermanentOperationCheckBox;
	TTrackBar *PLineLubIntensitySlider;
	TPageScroller *XtraCtlPageScroller;
	TPanel *MainCtlSubPanel;
	TPanel *XtraCtlPagePanel;
	TPanel *XtraEngineCtlPanel;
	TGroupBox *XtraEngineCtlGroupBox;
	TGroupBox *HydraulicCtlGroupBox;
	TGroupBox *WinchDriveCtlGroupBox;
	TGaugeVert01 *HydrLvlView;
	TGaugeVert01 *EngineCoolantTemperatureView;
	TGroupBox *EngineCtlGroupBox;
	TGroupBox *CommonCtlGroupBox;
	TPanel *CommonCtlPanel;
//        TRadioButton *LOGFileTypeLAS2RadioButton;
        void __fastcall MainFormResize(TObject *Sender);                        // Hauptformular wird in seiner Größe geändert
        void __fastcall FormShow(TObject *Sender);                              // Hauptformular wird angezeigt
        void __fastcall ExitButtonClick(TObject *Sender);                       // Programm beenden wurde gedrückt
        void __fastcall CloseQuery(TObject *Sender, bool &CanClose);            // Das Ereignus CloseQuery tritt auf
        void __fastcall TimerEvent(TObject *Sender);                            // Timer-Ereignis tritt auf
		void __fastcall SetParameterButtonClick(TObject *Sender);               // "Parameter" - Menü betätigt
        void __fastcall FileAdminButtonClick(TObject *Sender);                  // "Dateiverwaltung" - Menü
        void __fastcall CalibrateButtonClick(TObject *Sender);                  // "Kalibrieren" - Menü betätigt
        void __fastcall UnloadCableButtonClick(TObject *Sender);             // "Kabel entladen" betätigt
        void __fastcall StartStopButtonClick(TObject *Sender);                  // Start/Stop-Taste betätigt
        void __fastcall PChangeDirectionButtonClick(TObject *Sender);           // Fahrtrichtungsauswahl betätigt
        void __fastcall ParameterExitClick(TObject *Sender);                  // Parametereingabe abbrechen betätigt
        void __fastcall ParameterOkClick(TObject *Sender);                      // Parameter übernehmen betätigt
        void __fastcall FastRunButtonClick(TObject *Sender);                    // Schnell/Langsam Taste betätigt
        void __fastcall LostMotionImpulseButtonClick(TObject *Sender);          // Freilauf-Taste wurde betätigt
        void __fastcall SignalHornButtonMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);                // Signal(Hupe) Taste wird gedrückt gehalten
        void __fastcall SignalHornButtonMouseUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);                // Signal(Hupe) Taste wird losgelassen
        void __fastcall ResetButtonClick(TObject *Sender);        // Freigabe-Taste wird gedrückt
        void __fastcall EngineButtonDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);                // Motor "Ein" Taste wird gedrückt gehalten
        void __fastcall EngineButtonUp(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);                // Motor "Ein" Taste wird losgelassen
        void __fastcall EngineRevolutionDownButtonDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);                // Drehzahlregler abwärts Taste wird gerade gedrückt
        void __fastcall EngineRevolutionHoldButton(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);                // Drehzahlregler Tasten (+ bzw. -) werden losgelassen
        void __fastcall EngineRevolutionUpMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);                // Drehzahlregler aufwärts Taste wird gerade gedrückt
        void __fastcall ChoicePanelButtonsClick(TObject *Sender);               // Eine Auswahltaste auf ChoicPanel wurde betätigt
        void __fastcall CalibTractionButtonsClick(TObject *Sender);             // Eine Taste, die mit dem Kalibrieren des Zugkraftmessers zu tun hat wurde
        void __fastcall CalibDepthButtonsClick(TObject *Sender);
        void __fastcall UnloadCableMinTractionUpDownClick(
          TObject *Sender, TUDBtnType Button);          // Ausgeführt, wenn auf LOGFile-Liste geklickt
        void __fastcall LOGFileAdminLanguageCheck(TObject *Sender);             // Sprachauswahl für LOG-Datei-Ansicht betätigt
        void __fastcall LOGFileAdminCopyButtonClick(TObject *Sender);           // LOG-Datei kopieren angeklickt
        void __fastcall SystemButtonClick(TObject *Sender);                     // Systemmenü-Auswahl gedrückt
        void __fastcall SystemShutdownButtonClick(TObject *Sender);             // System abschalten
        void __fastcall SystemRestartButtonClick(TObject *Sender);              // System neu starten
        void __fastcall SystemProgramTerminationButtonClick(                    // Programm beenden
          TObject *Sender);                   // Umleiten von Tastendrücken zu Sicherheitsabfrage
        void __fastcall AlarmSoundOnOffButtonClick(TObject *Sender);            // Akustische Alarm-/Warnungs-Signalisierung ein/aus

        // SICHERHEITSABFRAGE
        void __fastcall SecCheckRedirectionStart(TObject *Sender);              // Umleiten von Tastendrücken zu Sicherheitsabfrage
        void __fastcall SecCheckRedirectionEnd(TObject *Sender);
        void __fastcall LOGFileAdminFileListBoxDoubleClick(
          TObject *Sender);
        void __fastcall LOGFileAdminDeleteButtonClick(TObject *Sender);
        void __fastcall WinchBrakeOffSpeedButtonClick(TObject *Sender);
        void __fastcall ComponentEntered(TObject *Sender);
        void __fastcall DevelFormButtonClick(TObject *Sender);
        void __fastcall EngineStopButtonDown(TObject *Sender,                   // Motor "Aus" Taste gedrückt
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall EngineStopButtonUp(TObject *Sender,                     // Motor "Aus" Taste losgelassen
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall DynamicTensionOnOffButtonClick(TObject *Sender);
        void __fastcall Ext_cm_Imp_RadioButtonClick(TObject *Sender);
    void __fastcall PowerFailCdResetSpeedButtonClick(TObject *Sender);
    void __fastcall MagneticMarkOnOffButtonClick(TObject *Sender);
    void __fastcall PProgramAutomChoiseClick(TObject *Sender);
    void __fastcall AutoRunProgramItemListListBoxClick(TObject *Sender);
	void __fastcall MainMenuSpeedButtonClick(TObject *Sender);
	void __fastcall MessagePanelButtonClick(TObject *Sender);
	void __fastcall SpeedViewAutoDirChanged(TObject *Sender);
	void __fastcall SpeedViewAutoSpeedChanged(TObject *Sender);
	void __fastcall SpecialFuncButtonMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall SpecialFuncButtonMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall NumEditEntered(TObject *Sender);
	void __fastcall NumEditCanceled(TObject *Sender);
	void __fastcall PMinDepthEditAssumedInput(TObject *Sender);
	void __fastcall PMaxDepthEditAssumedInput(TObject *Sender);
	void __fastcall PVelocityEditAssumedInput(TObject *Sender);
	void __fastcall PWinchBrakeInternalAutoCheckBoxClick(TObject *Sender);
	void __fastcall PEngineRevolutionChangeableCheckBoxClick(TObject *Sender);
	void __fastcall PEngineStartStopEnableCheckBoxClick(TObject *Sender);
	void __fastcall PLostMotionEnableCheckBoxClick(TObject *Sender);
	void __fastcall PBrakeOffEnableCheckBoxClick(TObject *Sender);
	void __fastcall PMaxTensionEditAssumedInput(TObject *Sender);
	void __fastcall PRelTensionLimitLowerEditAssumedInput(TObject *Sender);
	void __fastcall PRelTensionLimitUpperEditAssumedInput(TObject *Sender);
	void __fastcall PCalibTensionXEditAssumedInput(TObject *Sender);
	void __fastcall PCalibTensionYEditAssumedInput(TObject *Sender);
	void __fastcall PCalibTensionZeroEditAssumedInput(TObject *Sender);
	void __fastcall PWinchCfgListBoxClick(TObject *Sender);
	void __fastcall PSetDepthEditAssumedInput(TObject *Sender);
	void __fastcall PDepthCalFactorEditAssumedInput(TObject *Sender);
	void __fastcall PMMSyncForceRangeLimEditAssumedInput(TObject *Sender);
	void __fastcall PIncEncErrorMonitCheckBoxClick(TObject *Sender);
	void __fastcall PControllerErrorMonitCheckBoxClick(TObject *Sender);
	void __fastcall PControllerErrorLactiveCheckBoxClick(TObject *Sender);
	void __fastcall PControllerErrorAtSMCheckBoxClick(TObject *Sender);
	void __fastcall PSpecialSignalModeRadioButtonClick(TObject *Sender);
	void __fastcall PTractionViewComboBoxChange(TObject *Sender);
	void __fastcall PVelocityViewComboBoxChange(TObject *Sender);
	void __fastcall PReverseDepthCheckBoxClick(TObject *Sender);
	void __fastcall ParameterInputExit(TObject *Sender);
	void __fastcall UnloadCableSetTensionEditAssumedInput(TObject *Sender);
	void __fastcall UnloadCableStretchStartLenghtEditAssumedInput(TObject *Sender);
	void __fastcall UnloadCableStretchEndLenghtEditAssumedInput(TObject *Sender);
	void __fastcall UnloadCableStretchStartTensionEditAssumedInput(TObject *Sender);
	void __fastcall UnloadCableStretchEndTensionEditAssumedInput(TObject *Sender);
	void __fastcall UnloadCabelStretchCheckBoxClick(TObject *Sender);
	void __fastcall CalibDepth_DepthEditAssumedInput(TObject *Sender);
	void __fastcall CalibDepth_RealDepthEditEnterd(TObject *Sender);
	void __fastcall CalibTension_TensionEditAssumedInput(TObject *Sender);
	void __fastcall PHydraulikBrakeEnableCheckBoxClick(TObject *Sender);
	void __fastcall SystemDateMaskEditExit(TObject *Sender);
	void __fastcall SystemTimeMaskEditExit(TObject *Sender);
	void __fastcall PLineLubAvailableCheckBoxClick(TObject *Sender);
	void __fastcall PLineLubPermanentOperationCheckBoxClick(TObject *Sender);
	void __fastcall PLineLubIntensitySliderChange(TObject *Sender);




private:	// Anwenderdeklarationen
        int oldLOGFileItem;
        float oldTemperatureSensorDepthCalc;                                    // zuletzt erfasste Temperatur des Sensors auf dem Teufenrechner      
        float oldTemperatureSensorA;                                            // zuletzt erfasste Temperatur des Sensors A
        float oldTemperatureSensorB;                                            // zuletzt erfasste Temperatur des Sensors B
        int SplashCount;
        TObject *RedirectionInitiator;                                          // Zeiger auf das Objekt, welches die Umleitungs-Start-Routine ursprünglich aufgerufen hat
//        char RedirectionStartFunctionPanel;                                     // Nummer des Fensters, welches beim Start der Umleitung sichtbar war

        int ControlThread_STARTUP_CNT;                                          // Zähler für Startup-Delay des ControlThreads
        int DisplayThread_STARTUP_CNT;                                          // Zähler für Startup-Delay des DisplayThreads

#ifdef  __TRACTION_VIEW_NOISE_DAMP
        unsigned int oldTractionViewDampTime;                                   // Zeitpunkt der letzen Aktualisierung der Zugkraftanzeige bezügl. des aktuellen Wertes
        JMGS_float_AverageManager TractionView_AverageManager;                  // Verwaltet den Mittelwert für die Zugkraftanzeige (zur Dämpfung)
#endif

public:		// Anwenderdeklarationen
        // HILFSMETHODEN
//        void DampNoise(float CurrVal, float *TargetVal, int Delay, unsigned int *LastTime);// Dämpft veränderliche Werte, indem sich der Istwert dem Sollwert annähert (durch stetiges Aufrufen dieser Methode mit den jeweiligen Folgewerten, wird nach einer einstellbaren Verzögerung der Sollwert erreicht)
//        long GetProcessTime_in_ms();                                            // ermittelt die seit Prozessbeginn verbrauchte CPU-Zeit in ms

        // PROGRAMM-VERWALTUNG
        __fastcall TMainForm(TComponent* Owner);

        void __fastcall SetWorkingDir();                                        // Setzt das aktuelle Arbeitsverzeichnis
        void __fastcall GetCmdParameter();                                      // Ermitteln der Kommandozeilenparameter vom Programmstart
    	void __fastcall SetDemoMode();											// Fenstereinstellungen für den Demo-Modus
		void __fastcall SetDevelMode();                                         // Darstellung und Einstellmöglichkeiten abh. vom Entwicklermodus anpassen
    	bool __fastcall InitEventMgr();											// Ereignismeldungsverwaltung initialisieren
		void __fastcall DelErrorMessage(int Code);        						// Löscht, wenn enthalten, die dem Fehlercode-zugeordnete Fehlermeldung aus der Meldungsliste
        void __fastcall AddErrorMessage(int Code);        						// Hängt die zum Fehler-Code gehörige Meldung an die Meldungsliste an
        short __fastcall DetermErrorMessageNotif(int Code);         			// Ermittelt, wie die Anzeige für den angegebenen Fehlerzustand aktualisiert werden muss

        // GUI-LAYOUT
        void __fastcall MakeStaticLayout();                                     // Layout-Einstellungen, die während der Programmlaufzeit statisch sind
        void __fastcall MakeLayout();                                           // Organisiert die Bildschirmaufteilung der Objekte
        void __fastcall MakeCalibDepthPanelLayout();                            // Organisiert die Bildschirmaufteilung der Unter-Objekte von CalibDepthPanel
        void __fastcall MakeCalibTractionPanelLayout();                         // Organisiert die Bildschirmaufteilung der Unter-Objekte von CalibTractionPanel
        void __fastcall MakeMainCtlPanelLayout();                               // Organisiert die Bildschirmaufteilung der Unter-Objekte von ControlPanel
        void __fastcall MakeControlLampPanelLayout();                           // Organisiert die Bildschirmaufteilung der Unter-Objekte von ControlLampPanel
        void __fastcall MakeChoicePanelLayout();                                // Organisiert die Bildschirmaufteilung der Unter-Objekte von ChoisePanel
        void __fastcall MakeDialogPanelLayout();                                // Organisiert die Bildschirmaufteilung der Unter-Objekte von DialogPanel
        void __fastcall MakeUnloadCablePanelLayout();                        	// Organisiert die Bildschirmaufteilung der Unter-Objekte von UnloadCablePanel
        void __fastcall MakeFileAdminPanelLayout();                             // Organisiert die Bildschirmaufteilung der Unter-Objekte von FileAdminPanel
        void __fastcall MakeInfoPanelLayout();                                  // Organisiert die Bildschirmaufteilung der Unter-Objekte von InfoPanel
        void __fastcall MakeMessagePanelLayout();                               // Organisiert die Bildschirmaufteilung der Unter-Objekte von MessagePanel
        void __fastcall MakeNormalViewPanelLayout();                            // Organisiert die Bildschirmaufteilung der Unter-Objekte von NormalViewPanel
        void __fastcall MakeParameterPanelLayout();                             // Organisiert die Bildschirmaufteilung der Unter-Objekte von ParameterPanel
		void __fastcall MakeSystemPanelLayout();                                // Organisiert die Bildschirmaufteilung der Unter-Objekte von SystemPanel
		void __fastcall MakeMainMenuPanelLayout();								// Organisiert die Bildschirmaufteilung der Unter-Objekte von MainMenuPanel
		void __fastcall MakeXtraCtlPageScrollerLayout();						// Organisiert die Bildschirmaufteilung der Unter-Objekte von XtraCtlPageScroller

        // SPRACHE ANPASSEN
        void __fastcall SetLabelling();                                         // Setzt alle Beschriftungen im GUI entsprechend der eingestellten Sprache

        // FUNKTIONSPANELVERWALTUNG
        void __fastcall ChoicePanelObjectsVisibility(unsigned int ObjectBits);  // Sorgt für die sichtbar/unsichtbar-Einstellungen der Objekte auf ChoicePanel
		void __fastcall ShowCurrFunctionPanel();                                // Macht die aktuelle Funktions-Panel sichtbar, welche in GenOrgan.CurrFunctionPanel angegeben sind
		void __fastcall ShowFavoriteFunctionPanel();                            // Macht das favorisierte Funktions-Panel, welches in GenOrgan.FavoriteFunctionPanel angegeben ist, zun aktuellen und sichtbar

        // KONVERTIERUNG ZUR DATENEDARSTELLUNG UND ZURÜCK
        AnsiString __fastcall MakeDepthString(int Value, int Digits, bool UnitOn);// erstellen des Strings für die Teufenanzeige mit Berücksichtigung der gewählten Einheit
        bool __fastcall MakeDepthValue(AnsiString TheString, int *Value);       // ermittelt den Wert aus einem String und gibt diesen im internen Standartformat aus
        AnsiString __fastcall MakeTimeString(int Value, bool UnitOn);           // erstellen des Strings für die Zugkraftanzeige mit Berücksichtigung der gewählten Einheit
        bool __fastcall MakeTimeValue(AnsiString TheString, int *Value);        // ermittelt den Wert aus einem String und gibt diesen im internen Standartformat aus
        AnsiString __fastcall MakeTensionString(int Value, bool UnitOn);       // erstellen des Strings für die Zugkraftanzeige mit Berücksichtigung der gewählten Einheit
        bool __fastcall MakeTractionValue(AnsiString TheString, int *Value);    // ermittelt den Wert aus einem String und gibt diesen im internen Standartformat aus
        UnicodeString __fastcall MakeVelocityString(int Value, bool UnitOn);    // erstellen des Strings für die Geschwindigkeitsanzeige mit Berücksichtigung der gewählten Einheit
		bool __fastcall MakeVelocityValue(UnicodeString TheString, int *Value); // ermittelt den Wert aus einem String und gibt diesen im internen Standartformat aus
		AnsiString __fastcall MakeWinchNameString(unsigned int Value);          // erstellen der Windenbezeichnung anhand der angegebenen Winden-Nummer

        float  __fastcall TractionViewApplyDimension(float OriginalValue);      // Wendet auf einen Standardzugkraftwert die Maßeinheit der Ausgabe an
        float  __fastcall TractionViewClearDimension(float OriginalValue);      // Bringt einen Zugkraftwert mit der  Maßeinheit der Ausgabe in den programminternen Standardzugkraftwert

        // DATENERFASSUNG IM SYSTEMUMFELD
        void __fastcall GetAutoRunProgramList();                                // Erfasst die Liste der Ablaufprogramme für die Programmautomatik

        // DATENDARSTELLUNG
        // Steuertasten
        void __fastcall SetClearAfterThreshold();                               // Abhängigkeit vom Zustand
        void __fastcall SetFastRun();                                           // Abhängigkeit vom Zustand
        void __fastcall SetLostMotionImpulse();                                 // Abhängigkeit vom Zustand
        void __fastcall SetStartStop();                                         // Abhängigkeit vom Zustand
        void __fastcall SetWinchBrakeOffButton();                               // Abhängigkeit vom Zustand

        // v-Keyboard
        void __fastcall VKeyBoardBlock(unsigned int VKBMask);                   // Blockierung des Keyboards

        // Anzeigen
        void __fastcall PlayAlarmSound();                                       // Initialisiert die Akustische Alarm-Signalisierung
        void __fastcall ShowAutoRunProgramable();                               // Zeigt die aktuelle Einstellung für die Programmautomatik an
        void __fastcall ShowAutoRunProgramItemList();                           // Zeigt die Ablaufprogrammpunktliste mit dem aktuellen Inhalt an, oder verbirgt ihn
        void __fastcall ShowAutoRunCurrProgramIndex();                          // Zeigt den aktuellen Ablaufprogrammpunkt an
        void __fastcall ShowAutomotiveError();                                  // Zeigt an, ob ein Fahrzeug-Fehler aufgetreten ist
		void __fastcall ShowHydraulicOKState();                               	// Zeigt an, ob die Hydraulik betriebsbereit ist
		void __fastcall ShowHydrLvl();											// Aktualisieren der Hydraulik-Niveau-Anzeige
		void __fastcall ShowHydrWrkPressure();									// Aktualisieren der Hydraulikarbeitsdruckanzeige
		void __fastcall ShowHydrChargePressure();  								// Aktualisieren der Hydraulikspeisedruckanzeige
		void __fastcall ShowHydrWrkPressureView_Scale();						// Aktualisieren der Skalierung der Hydraulikarbeitsdruckanzeige
		void __fastcall ShowBrakeOffEnable();									// Zeigt an, ob das manuelle Abschalten der Bremse Verfügbar ist, oder nicht
		void __fastcall ShowHydraulicBrake();									// Zeigt an, ob die Hydraulische Bremse ein- oder ausgeschaltet ist
		void __fastcall ShowCalcCurrTractionDifference();                       // Zeigt die Kraftdifferenz für die dynamische Maximal-Zugkraft-Ermittlung an
		void __fastcall ShowCalcCurrTractionsSignalisation();                   // Signalisiert, ob die Maximal-Zugkraft zur Zeit dynamisch ermittelt wird
		void __fastcall ShowCalFactorDepth();                                   // Zeigt den aktuellen Eichfaktor für den Teufenrechner an
    	void __fastcall ShowCalibDepthInfo();									// Zeigt den jeweils passenden Hinweis im Teufenkalibrierfenster an
		void __fastcall ShowCalValTraction();                                   // Zeigt die aktuellen Zugkraftkalibrierungswerte an
		void __fastcall ShowCalValTractionInfo();                               // Zeigt Informationen zur aktuellen Zugkraftkalibrierung an
		void __fastcall ShowComputerName();                                     // Ermittelt den Computernamen und zeigt diesen an
		void __fastcall ShowCtlUnitSWVersion();                               	// Zeigt die Software-Version des angeschlossenen Steuergeräts an
		void __fastcall ShowCurrDepth();                                        // Zeigt momentane Teufe an
		void __fastcall ShowCurrDirection();                                    // Zeigt die momentane Fahrtrichtung an
		void __fastcall ShowCurrTraction();                                     // Zeigt momentane Zugkraft an
		void __fastcall ShowCurrTensionLimits();                   				// Berechnet aus MaxValue die Darstellung der Grenzwerte
		void __fastcall ShowCurrTractionViewUnit();                             // Aktualisiert die Einheit in der Zugkraftanzeige an
		void __fastcall ShowCurrVelocity();                                     // Zeigt momentane Geschwindigkeit an
		void __fastcall ShowCurrWinchBrakeInternAutoStatus();					// Zeigt den Status der internen Automatischen Windenbremse an
		void __fastcall ShowDepthCalcSWVersion();                               // Zeigt die Software-Version des angeschlossenen Teufenrechners an
		void __fastcall ShowDevelInfo();                                        // Zeigt Informationen an, die nur im Entwicklermodus sichtbar sind
		void __fastcall ShowDirection(TObject *Sender);                         // Zeigt Soll-Fahrtrichtung
		void __fastcall ShowUnloadCableParameters();                         	// Zeigt die speziellen Parameter zum entladen des Kabels an
		void __fastcall ShowEmergencySwitchState();                             // Zeigt an, ob der NotausSchalter betätigt wurde
		void __fastcall ShowEngineCoolantTemp();								// Zeigt die Antriebsmotorkühlmitteltemperatur an
		void __fastcall ShowEngineRevolutionChangeable();                       // Zeigt den Status an, ob die Motordrehzahl verändert werden kann oder nicht
		void __fastcall ShowEngineSpeed();										// Zeigt die Antriebsmotordrehzahl an
		void __fastcall ShowEngineSpeedViewScale();								// Zeigt die Skalierung der Antriebsdrehzahl an
		void __fastcall ShowEngineStartStopEnable();                            // Zeigt den Status an, ob die Motor-Start-Stopp-Funktion aktiv ist oder nicht
		void __fastcall ShowErrorState();                                       // Zeigt den aktuellen Fehlerstatus an
		void __fastcall ShowExtCMImpulseFormat();                               // Zeigt das eingestellte Format für den externen cm-Impuls an
		void __fastcall ShowExtCMImpulseFormatImage(int FormatCode);            // Zeigt eine grafische Darstellung des eingestellten Formats für den externen cm-Impuls an
		void __fastcall ShowIncEncErrorMonitStatus();                           // Zeigt den Status der IGR-Fehlerüberwachung an (ein-/aus-geschaltet)
		void __fastcall	ShowLineLubParam();										// Zeigt die Parametereinstellung des Kabel-/Seilölers an
		void __fastcall ShowLOGCharacteristics();                               // Zeigt die Kenndaten für die LOG-Datei an
		void __fastcall ShowLOGFileAdminFileType();                             // Zeigt den momentan gewählten Dateityp an
		void __fastcall ShowLOGFileAdminFocusedFile();                          // Zeigt den Inhalt der LOG-Datei an, die gerade den Fokus besitzt
		void __fastcall ShowLOGFileAdminValues();                               // Aktualisiert die Werte für die LOG-Datei-Verwaltung
		void __fastcall ShowLostMotionEnable();									// Zeigt den Status an, ob der Freilauf verfügbar ist
		void __fastcall ShowMagneticMark(bool ONOFF);                           // Signalisiert die Magnetmarke
		void __fastcall ShowMagneticMarkStatus();                               // Zeigt den Status der Magnetmarkenüberwachung an (ein-/aus-geschaltet)
		void __fastcall ShowMaxDepth();                                         // Zeigt Mindestteufe an
		void __fastcall ShowMaxTraction();                                      // Zeigt die momentane Maximal-Zugkraft an
		void __fastcall ShowMaxTractionLimits();                                // Berechnet die Darstellung der Grenzwerte
		void __fastcall ShowMinDepth();                                         // Zeigt Mindestteufe an
		void __fastcall ShowMMSyncForceParam();									// Zeigt die Parameter zur MM-Zwangssynchronisation an
		void __fastcall ShowOilFilterState();                                   // Zeigt an, ob der Ölfilter ausgetauscht werden muss
		void __fastcall ShowPowerFailStatus();                                  // Ermitteln und Anzeigen des aktuellen Power-Fail-Zustandens
		void __fastcall ShowProgramMode();                                      // Zeigt momentanen Programm-Modus an
		void __fastcall ShowControllerErrorMonitAtSM();							// Zeigt den Status der Steuergerät - Fehlerüberwachung bei langsamer Fahrt an (ein-/aus-geschaltet)
		void __fastcall ShowControllerErrorMonitStatus();                       // Zeigt den Status der Steuergerät - Fehlerüberwachung an (ein-/aus-geschaltet)
		void __fastcall ShowControllerLactiveStatus();							// Zeigt den Status des Steuergerät Aktiv-Pegels an (L-aktive oder H-aktive Detektion)
		void __fastcall ShowReverseDepthStatus();                               // Zeigt den Satus der Teufenumkehrung an.
        void __fastcall ShowSetDepth();                                         // Zeigt die Teufe an, die dem Teufenrechner als Startteufe übergeben werden kann (StartDepth)
        void __fastcall ShowSpecialSignalMode();                                // Zeigt den Modus (Version) der Sondersignalverarbeitung an
        void __fastcall ShowSystemDateTime();                                   // Zeigt die aktuelle Uhrzeit an
        void __fastcall ShowTemperatures(bool ForceActualisation);              // Zeigt die Temperaturen der verschiedenen Sensoren an
		void __fastcall ShowTime();                                             // Zeigt die aktuelle Uhrzeit an
        void __fastcall ShowTractionDifference();                               // Zeigt Soll-Zugkraft-Differenz an
        void __fastcall ShowTractionDimensionUnit();                            // Zeigt die Maßeinheit der Zugkraft an, mit der die Anzeige und Interaktion mit dem Anwender erfolgt
		void __fastcall ShowVelocity(TObject *Sender);                          // Zeigt Soll-Geschwindigkeit an
		void __fastcall ShowVelocityDimensionUnit();                            // Zeigt die Maßeinheit der Geschwindigkeit an, mit der die Anzeige und Interaktion mit dem Anwender erfolgt
		void __fastcall ShowWinchBrakeInternalAuto();							// Zeigt an, ob die Windenbremse durch die Steuerungssoftware gehalten wird, oder nicht
//		void __fastcall ShowWinchCnt();                                         // Anzeigen der zu verwaltenden Windenanzahl
        void __fastcall ShowWinchNo();                                          // Anzeigen der momentanen Windennummer

        void __fastcall ActualizeAllInformations();                             // Liest alle Kontrollwerte/Parameter/Informationen aus und aktualisiert die Anzeige/Steuerelemente
        void __fastcall ActualizeUnloadCableInformation();                   	// Aktualisiert die Ansicht der Parameter, welche für das Entladen des Kabels wichtig sind
        void __fastcall ActualizeFileAdminView();                               // Aktualisieren aller in FileAdminPanel sichtbaren Angaben
        void __fastcall ActualizeParameterView();                               // Aktualisieren aller in ParameterPanel sichtbaren Angaben
        void __fastcall ActualizeSystemView();                                  // Aktualisieren aller in SystemPanel sichtbaren Angaben


        // PROGRAMMODUS-VERWALTUNG
        void __fastcall ProgramModeAdministrations(int ProgMode, int ControlState);// Führt entsprechende Schritte für die Programmmodi aus

        // Globale Objekte
        WCVarControl VarControl;                                                // Verwaltung Programmzweckgebundener Informationen
		WCGenOrganisationS GenOrgan;                                            // Generelle Programm-organisatorische Informationen/Einstellungen
		short Clock100ms_cnt;                                                   // Timer-Zähler

        // Threads
        WCDisplayThread *DisplayThread;
        WCControlThread *ControlThread;
        bool DisplayThreadTerminates;                                           // Gibt an, ob der DisplayThread seine Arbeit beendet
        bool ControlThreadTerminates;                                           // Gibt an, ob der ControlThread seine Arbeit beendet
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif


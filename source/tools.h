/*
WinchControl    Jörg Müller    structs.h

Enthält diverse unabhängige Hilfsfunktionen

Portierbarkeit: bedingt
                nicht - GetProcessTime_in_ms(...)
*/

#ifndef __TOOLS_H
#define __TOOLS_H

#include <time.h>

// --------------------------------------------------------------------------
// ermittelt die seit Prozessbeginn verbrauchte CPU-Zeit in ms
long GetProcessTime_in_ms()
{
        return ((float)(clock()) * (float)(1000) / (float)(CLOCKS_PER_SEC));
};

// --------------------------------------------------------------------------
// Prüft, ob die vergangene Zeit zwischen "PreviousTime_ms" und dem
// aktuellen Zeitpunkt größer oder gleich "TimeInterval_ms" ist.
// Die verwendete Zeiteinheit ist für alle Werte ms.
//
// Zeitdifferenz >= als "TimeInterval_ms":      - Rückgabewert: true
//                                              - "PreviousTime_ms" wird
//                                                auf aktuelle Zeit gesetzt
// Zeitdifferenz < als "TimeInterval_ms":       - Rückgabewert: false
//                                              - "PreviousTime_ms"
//                                                verbleibt unverändert
//
// "OverflowAction" definiert, wie bei einem Überlauf des Zeitzählers reagiert
// werden soll: 0 ... Standard: Rückgabewert true, "PreviousTime_ms"="CurrClock"
//              1 ... Rückgabewert false, "PreviousTime_ms"="CurrClock"
//              x ... wie 0
// Ein Überlauf erfolgt erst, wenn das Programm mehr als 24Tage lang läuft und
// ist oft irrelevant. (Bedingung: Clock_t 32-Bit-Wert, signed)
//
// "Initialise" bestimmt, wenn "true", dass die Funktion true zurückgibt und
// "PreviousTime_ms" auf jeden Fall auf "CurrTime" setzt. Damit kann
// "PreviousTime_ms" initialisiert werden, ohne dass der Programmierer eine
// eigene Implementationen integrieren muss.
// Hier wird keinerlei weitere Prüfung vorgenommen.
// Countdown_ms gibt die verbleibende Zeit in ms zurück
bool IsTimeElapsed(clock_t *PreviousTime_ms, clock_t *Countdown_ms, clock_t TimeInterval_ms, int ClockOverflowAction, bool Initialise)
{
        clock_t HV, CurrTime = GetProcessTime_in_ms();

        // Ausschließlich Initialisierung des Werts "PreviousTime_ms"
        if (Initialise)
        {
                *PreviousTime_ms = CurrTime;
                *Countdown_ms = TimeInterval_ms;
                return true;
        };

        // Behandlung Überlauf
        if (CurrTime < *PreviousTime_ms)
        {
                switch (ClockOverflowAction)
                {
                        case 1:         *PreviousTime_ms = CurrTime;
                                        *Countdown_ms = TimeInterval_ms;
                                        return false;

                        default:        // auch 0
                                        *PreviousTime_ms = CurrTime;
                                        *Countdown_ms = TimeInterval_ms;
                                        return true;
                };
        };

        // Normal-Ablauf
        *Countdown_ms = TimeInterval_ms - (CurrTime - *PreviousTime_ms);
        if (*Countdown_ms > 0) return false;

        *PreviousTime_ms = CurrTime;
        *Countdown_ms = TimeInterval_ms;
        return true;
};

// --------------------------------------------------------------------------
// Dämpft veränderliche Werte, indem sich der Istwert dem Sollwert annähert
// (durch stetiges Aufrufen dieser Methode mit den jeweiligen Folgewerten,
// wird nach einer einstellbaren Verzögerung der Sollwert erreicht)
// Wie bei I-Regler.
// CurrVal      ... der bisherige Wert.
// TargetVal    ... Der zu erreichende Soll-Wert. In diesem wird auch das Ergebnis der Methode geliefert.
// Delay        ... Die Verzögerungszeit
// LastTime     ... Die beim letzten Aufruf vorhandene Zeit (Zeitbasis für Berechnung)
// Ist der Zeitunterschied gleich 0, wird *TargetVal = CurrVal.
void DampNoise(float CurrVal, float *TargetVal, int Delay, unsigned int *LastTime)
{
        // Imaginäre Zugkraft-Dämpfung (Filtern der Zugkraft)
        unsigned int HVuint = GetProcessTime_in_ms();                           // momentane Zeit
        unsigned int Steps;                                                     // noch zu ermittelne Annäherungsstufen

        if ((HVuint - *LastTime) != 0)
        {
                // Anzahl der Annäherungsstufen erfassen
                Steps = Delay / (HVuint - *LastTime);
                if (Steps == 0) Steps = 1;                                      // wenn mehr als die erlaubte Zeit vergangen ist, dann wird der Wert direkt gesetzt
                *LastTime = HVuint;                                             // für nächsten Durchgang merken

                *TargetVal = CurrVal + ((*TargetVal - CurrVal) / Steps);

        }else *TargetVal = CurrVal;
};

#endif __TOOLS_H

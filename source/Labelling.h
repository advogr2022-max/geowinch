/*
WinchControl    Jörg Müller    Labelling.h

Spracheinstellungen
Portierbarkeit: vollständig
*/

#ifndef __LABELLING_H
#define __LABELLING_H

// Sprachdefinition
const __GERMAN = 0;
const __RUSSIAN = 1;
const __ENGLISH = 2;

const __CURR_LNG = __RUSSIAN;

const char      LNG_CODE[3]
                = {0, 204, 0};

const wchar_t *LANGUAGELABEL[3]                       = {L"Deutsch", L"Русский", L"English"};

// Platzhalter
const char      *__SEPARATOR01[3]                       = {" ", " ", " "};      // Trennzeichen: Leerzeichen
const char      *__SEPARATOR02[3]                       = {"/", "/", "/"};      // Trennzeichen: Slash
const char      *__SEPARATOR03[3]                       = {":", ":", ":"};      // Trennzeichen: Doppelpunkt
const char      *__SEPARATOR04[3]                       = {".", ".", "."};      // Trennzeichen: Punkt   

// Maßeinheiten
const wchar_t   *UNIT_BAR[3]                            = {L"bar", L"бар", L"bar"};
const wchar_t *UNIT_DEGREE_CELSIUS[3]                 = {L"°C", L"°C", L"°C"};
const wchar_t *UNIT_HOUR[3]                           = {L"h", L"ч", L"h"};
const wchar_t *UNIT_NEWTON[3]                         = {L"N", L"Н", L"N"};
const wchar_t *UNIT_KILONEWTON[3]                     = {L"kN", L"кН", L"kN"};
const wchar_t *UNIT_KILOPOND[3]                       = {L"kp", L"кгс", L"kp"};
const wchar_t *UNIT_NEWTON_PER_MILLIVOLT[3]           = {L"N/mV", L"Н/мВ", L"N/mV"};
const wchar_t *UNIT_METER[3]                          = {L"m", L"м", L"m"};
const wchar_t *UNIT_METER_PER_HOUR[3]                 = {L"m/h", L"м/ч", L"m/h"};
const wchar_t   *UNIT_METER_PER_MINUTE[3]               = {L"m/min", L"м/мин", L"m/min"};
const wchar_t *UNIT_MILLIMETER_PER_IMPULSE[3]         = {L"mm/Impuls", L"мм/импульс", L"mm/Impulse"};
const wchar_t *UNIT_MILLISECOUND[3]                   = {L"ms", L"мс", L"ms"};
const wchar_t *UNIT_MILLIVOLT[3]                      = {L"mV",L"мВ",L"mV"};
const wchar_t   *UNIT_PERC[3]               			= {L"%", L"%", L"%"};
const wchar_t   *UNIT_RPM[3]               				= {L"U/min", L"об/мин", L"rpm"};
const wchar_t *UNIT_SECOUND[3]                        = {L"s", L"с", L"s"};

const wchar_t *UNIT_EXPL_NEWTON[3]                    = {L"1N (1 Newton)", L"1Н (1 Нютон)", L"1N (1 Newton)"};
const wchar_t *UNIT_EXPL_KILONEWTON[3]                = {L"1kN (1 Kilonewton)", L"1кН (1 Килонютон)", L"1kN (1 Kilonewton)"};
const wchar_t *UNIT_EXPL_KILOPOND[3]                  = {L"1kp (1 Kilopond)", L"1кгс (1 Кило грамм-сила)", L"1kp (1 Kilopond)"};

const wchar_t	*UNIT_EXPL_METER_PER_HOUR[3]			= {L"1m/h (1 Meter pro Stunde)", L"1м/ч (1 метр в час)", L"1 m/h (1 meter per hour)"};
const wchar_t	*UNIT_EXPL_METER_PER_MINUTE[3]			= {L"1m/min (1 Meter pro Minute)", L"1м/мин (1 метр в минуту)", L"1 m/min (1 meter per minute)"};

// Fehlermeldungen kurz
const wchar_t *SHORT_MSG_ERROR[3]                     = {L"Fehler",L"Ошибка", L"Error"};
const wchar_t *SHORT_MSG_NOERROR[3]                   = {L"", L"Нет ошибок", L""};
const wchar_t *SHORT_MSG_ATTENTION[3]                 = {L"Achtung", L"Внимание", L"Attention"};

// Beschriftungen
const wchar_t *LABEL_ABORT[3]                         = {L"Abbrechen", L"Отмена", L"Abort"};
const wchar_t *LABEL_ACCEPT[3]                        = {L"Übernehmen", L"Сохранить", L"Accept"};
const wchar_t *LABEL_ACQUIREZEROPOINT[3]              = {L"Nullpunkt ermitteln", L"Устанавливать нейтраль", L"Acquire zero point"};
const wchar_t *LABEL_ADDITIONAL_OILSTRAINER_MONIT[3]	= {L"Zusätzl. Ölfilterüberw.", L"Дополн. контроль масл. фильтра", L"Add. oil strainer monit."};
const wchar_t	*LABEL_ADDITIONAL_FUNCTIONS[3]			= {L"Zusatzfunktionen", L"Дополнительные функции", L"Additional functions"};
const wchar_t *LABEL_AMPLIFICATIONFACTOR_KR[3]        = {L"Verstärkungsfaktor KR", L"Коэффициент усиления KR", L"Amplification factor KR"};
const wchar_t *LABEL_AUTOMATIC_BRAKE[3]               = {L"Automatische\nBremse", L"Автоматич.\nтормоз", L"Automatic\nbrake"};
const wchar_t *LABEL_AUTOMOTIVE_ERROR[3]              = {L"Fahrzeug-Fehler", L"Ошибка автомобиля", L"Automotive Error"};
const wchar_t *LABEL_AUXIALLYDRIVE[3]                 = {L"Nebenantrieb", L"Коробка отбора мощности", L"Auxially drive"};
const wchar_t	*LABEL_AVAILABLE[3]						= {L"Verfügbar", L"Доступный", L"Available"};
const wchar_t *LABEL_BEGINLENGHT_OF_CABLE[3]          = {L"Anfangs-Länge des Kabels", L"Начальная длина кабеля", L"Begin-lenght of the cable"};
const wchar_t *LABEL_BEGINTENSION[3]                 	= {L"Anfangs-Zugkraft", L"Начальное натяжение", L"Begin-tension"};
const wchar_t	*LABEL_BRAKEOFFENABLE[3]				= {L"Bremse manuell abschaltbar", L"Возможность мануального отключения тормоза", L"Manually sheddable brake"};
const wchar_t *LABEL_CABLE[3]                         = {L"Kabel", L"Кабель", L"Cable"};
const wchar_t *LABEL_CALCTENSIONDYN[3]               	= {L"Maximale Zugkraft dynamisch berechnen", L"Рассчитать натяжение", L"Calculate maximal tension dynamic"}; // Gleitende Maximal-Zugkraft
//const char      *LABEL_CALCTENSIONDYNDIFFERENCE[3]     	= {"Zugkraftabstufung", "Фракционирование натяжение", "Tension gradation"};
//const char      *LABEL_CALCULATE[3]                     = {"Berechnen", "Рассчитывать", "Calculate"};
const wchar_t   *LABEL_CALFACTOR[3]                 	= {L"Kalibrierwert", L"Калибров. знач.", L"Calibration factor"};
const wchar_t   *LABEL_CALFACTOR_ACTIVE[3]             	= {L"Aktiver Kalibrierwert", L"Действующий калибров. знач.", L"Active calibration factor"};
const wchar_t   *LABEL_CALFACTOR_EARMARKED[3]          	= {L"Vorgemerkter Kalibrierwert", L"Намеченный калибров. знач.", L"Earmarked calibration factor"};
const wchar_t *LABEL_CALIB_TENSION_X[3]              	= {L"Spannungsdifferenz für Anstieg", L"Начальное различие напряжения", L"Voltage difference for rising"};
const wchar_t *LABEL_CALIB_TENSION_Y[3]              	= {L"Zugkraftdifferenz für Anstieg", L"Начальное различие натяжения", L"Tension difference for rising"};
const wchar_t *LABEL_CALIB_TENSION_ZERO[3]           	= {L"Offset (Null-Wert)", L"Сдвиг (Нулевое значение)", L"Offset (Zero)"};
const wchar_t *LABEL_CALIB_VALUES_TENSION[3]          = {L"Kalibrierwerte für Zugkraftbestimmung", L"Калибровочные значения для определения силы натяжения", L"Calibration values for tension acquisition"};
const wchar_t *LABEL_CALIBRATE[3]                   	= {L"Kalibrierung", L"Калибровка", L"Calibrate"};
const wchar_t	*LABEL_COMMON[3]						= {L"Allgemein", L"Общий", L"Common"};
const wchar_t *LABEL_CHANGE_OILFILTER[3]              = {L"Ölfilter austauschen", L"Замена масляного фильтра", L"Change oil-filter"};
const wchar_t *LABEL_CHARACTERISTICS[3]               = {L"Kenndaten", L"Параметры", L"Characteristics"};
const wchar_t *LABEL_CHARSET_OF_VIEW[3]               = {L"Zeichensatz der Ansicht", L"Символы вида", L"Character set of view"};
const wchar_t *LABEL_CLEARING[3]                      = {L"Freigabe", L"Сброс", L"Clear"};
const wchar_t *LABEL_COMMENT[3]                       = {L"Bemerkungen", L"Комментарий", L"Comment"};
const wchar_t *LABEL_COMPLETE[3]                      = {L"Vollständig", L"Полный", L"Complete"};
const wchar_t 	*LABEL_CONFIGURATION[3]					= {L"Konfiguration", L"Конфигурация", L"Configuration"};
const wchar_t *LABEL_CONTROLLERERRORMONIT[3]          = {L"Überwachung Steuergerät", L"Наблюдение контроллера", L"Controller Monitoring"};
const wchar_t	*LABEL_CONTROLLERERRORMONIT_ATSM_ENABLE[3] = {L"Überwachung Steuergerät in langs. Fahrstufe", L"Наблюдение контроллера в медл. режиме", L"Controller monitoring at slow motion"};
const wchar_t   *LABEL_CONTROLLER[3]          			= {L"Steuergerät", L"Контроллер", L"Controller"};
const wchar_t *LABEL_DATE[3]                          = {L"Datum", L"Число", L"Date"};
const wchar_t *LABEL_DATETIMETUNING[3]                = {L"Datum und Uhrzeit", L"Число и час", L"Date and time"};
const wchar_t *LABEL_DEACTIVATE_BRAKE[3]              = {L"Bremse\nlösen", L"Выключить\nтормоз", L"Brake\noff"};
const wchar_t *LABEL_DELETE[3]                        = {L"Löschen", L"Удаление", L"Delete"};
const wchar_t *LABEL_DELETE_LOG_FILES[3]              = {L"Löschen von LOG-Dateien", L"Удаление LOG-файлов", L"Deleting LOG-Files"};
const wchar_t *LABEL_DEPTH[3]                         = {L"Teufe", L"Глубина", L"Depth"};
const wchar_t *LABEL_DEPTHCALC[3]                     = {L"Teufenrechner", L"Счетчик глубины", L"Depth Computer"};
const wchar_t	*LABEL_DEPTHIMPULSESCOUNT[3] 			= {L"Erfasste Teufenimpulse", L"Количество импульсов глубины", L"Depth impulses count"};
const wchar_t *LABEL_DERIVATIVE_TIME_TD[3]            = {L"Vorhaltezeit TD", L"Предварительное время TD", L"Derivative time TD"};      // auf Regelungstechnik bezog.
const wchar_t *LABEL_DEVELOPER[3]                     = {L"Entwickler", L"Разрабочик", L"Developer"};
const wchar_t *LABEL_DIFFERENCE_PARAM[3]              = {L"Differenz-Einstellungen", L"Установка разности", L"Difference Settings"};
const wchar_t *LABEL_DIRECTION[3]                     = {L"Fahrtrichtung", L"Направление", L"Direction"};
const wchar_t *LABEL_DISCARD[3]                       = {L"Verwerfen", L"Отмена", L"Discard"};
const wchar_t *LABEL_DOUBLEWINCH[3]                   = {L"Doppelwinde", L"Двойная лебедка", L"Double winch"};
const wchar_t *LABEL_DOWNWARD[3]                      = {L"abwärts", L"вниз", L"down"};
const wchar_t *LABEL_DRIVE_CONTROL[3]                 = {L"Fahrsteuerung", L"Управление лебедкой", L"Drive control"};
const wchar_t *LABEL_DURATIONUNTILSHUTDOWN[3]         = {L"Systemabschaltung in", L"Срок до автом. выключения", L"shutdown in"};
const wchar_t *LABEL_DYNAMIC_MAX_TENSION[3]           = {L"Dynamische Max-Zugkraft", L"Динамическое макс. натяжение", L"Sliding Max-Tension"}; // Gleitende Maximalzugkraft
const wchar_t *LABEL_ENDLENGHT_OF_CABLE[3]            = {L"End-Länge des Kabels", L"Конечная длина кабеля", L"End-lenght of the Cable"};
const wchar_t *LABEL_ENDTENSION[3]                   	= {L"End-Zugkraft", L"Конечное натяжение", L"End-tension"};
const wchar_t *LABEL_EMERGENCYSWITCH[3]               = {L"Not-Aus-Schalter", L"Аварийный выключатель", L"Emergency-Switch"};
const wchar_t   *LABEL_ENGINE[3]                        = {L"Motor", L"Двигатель", L"Engine"};
const wchar_t *LABEL_ENGINEOFF[3]                     = {L"Motor aus", L"Выключить\nмотор", L"Engine Off"};
const wchar_t *LABEL_ENGINEON[3]                      = {L"Motor ein", L"Запуск\nмотора", L"Engine On"};
const wchar_t *LABEL_ENGINEREVOLUTION[3]              = {L"Motordrehzahl", L"Обороты\nавтомотора", L"Engine Rev."};
const wchar_t *LABEL_ENGINEREVOLUTIONCHANGEABLE[3]    = {L"Motordrehzahl änderbar", L"Обороты двигателя подвергаются регулируванию", L"Engine revolution changeable"};
const wchar_t *LABEL_ENGINESTARTSTOPENABLE[3]         = {L"Motor Start-Stopp aktiviert", L"Функция двигатель Старт-Стоп активна", L"Engine Start-Stop activated"};
const wchar_t *LABEL_ENGLISH[3]                       = {L"Englisch", L"Английский", L"English"};
const wchar_t *LABEL_ENFORCE_MM_SYNC_UNTIL[3] 		= {L"MM-Synchr. erzwingen bis", L"Форсировать синхронизацию ММ до", L"Enforce MM sync. until"};
const wchar_t *LABEL_EXIT[3]                          = {L"Beenden", L"Заканчивать", L"Exit"};
const wchar_t *LABEL_EXIT_PROGRAM[3]                  = {L"WinchControl beenden", L"Закончить управление лебедкой", L"Exit WinchControl"};
const wchar_t	*LABEL_EXT_BRAKE_MONITORING[3]			= {L"Überwachung einer externen Bremse", L"Контроль дополнительного тормоза", L"External brake monitoring"};
const wchar_t *LABEL_EXT_CM_IMPULSE[3]                = {L"Externer cm-Impuls", L"Наружный см-импульс", L"External cm-impulse"};
const wchar_t *LABEL_FAST[3]                          = {L"Schnell", L"Быстро", L"Fast"};
const wchar_t *LABEL_FILE_LANGUAGE_FORMAT[3]          = {L"Sprachformat", L"Формат языков", L"Language format"};
const wchar_t *LABEL_FILEADMIN[3]                     = {L"Dateiverwaltung", L"Управление файлов", L"File Administration"};
const wchar_t *LABEL_FILETYPE[3]                      = {L"Dateityp", L"Тип файла", L"Type of file"};
const wchar_t *LABEL_GERMAN[3]                        = {L"Deutsch", L"Немецкий", L"German"};
const wchar_t *LABEL_HYDRAULIC[3]                     = {L"Hydraulik", L"Гидравлика", L"Hydraulic"};
const wchar_t	*LABEL_HYDRAULIC_BRAKE[3]				= {L"Hydraulische Bremse", L"Гидравлический тормоз", L"Hydraulic Brake"};
const wchar_t *LABEL_IGNORE[3]                        = {L"Ignorieren", L"Игнорировать", L"Ignore"};
const wchar_t *LABEL_INCENCERRORMONIT[3]              = {L"IGR-Überwachung", L"Контроль датчика глубины", L"Incremental Encorder Monitoring"};
const wchar_t	*LABEL_INSERT_REAL_DEPTH[3]				= {L"Tatsächliche Teufe eingeben!",L"Вводить фактическую глубину!", L"Insert real depth!"};
//const char      *LABEL_JOYSTICK_VOLTAGE[3]              = {"Spannung am HSG", "Напряжение на джойстике", "Joystick voltage"};
const wchar_t *LABEL_LEFT_WINCHCHOISE[3]     			= {L"links", L"Левая", L"left"};
const wchar_t	*LABEL_LUBRICATE_LINE[3]                = {L"Kabel ölen", L"Смазка кабеля", L"Cable lubrication"};
const wchar_t *LABEL_LOCATION[3]                      = {L"Ort", L"Место", L"Location"};
const wchar_t *LABEL_LOSTMOTIONOFF[3]                 = {L"Freilauf Aus", L"Выключить\nсвободный ход", L"Freewheel Off"};       // Beschreibt Taste um etwas zu tun, keinen Zustand
const wchar_t *LABEL_LOSTMOTIONON[3]                  = {L"Freilauf Ein", L"Включить\nсвободный ход", L"Freewheel On"};
const wchar_t *LABEL_LOSTMOTIONENABLE[3]				= {L"Freilauf verfügbar", L"Свободный ход в распоряжении", L"Lost motion available"};
const wchar_t *LABEL_KARAT[3]                         = {L"KARAT", L"КАРАТ", L"KARAT"};
const wchar_t *LABEL_KARAT2[3]                        = {L"KARAT-2", L"КАРАТ-2", L"KARAT-2"};
const wchar_t *LABEL_KARAT2N[3]                       = {L"KARAT-2 negiert", L"КАРАТ-2 с отрицанием", L"KARAT-2 negatived"};
const wchar_t *LABEL_KEDR[3]                          = {L"KEDR", L"КЕДР", L"KEDR"};
const wchar_t *LABEL_KEDRN[3]                         = {L"KEDR negiert", L"КЕДР с отрицанием", L"KEDR negatived"};
const wchar_t	*LABEL_L_ACTIVE[3]						= {L"L-aktiv", L"Cигнал с активным низким уровнем", L"L-active"};
const wchar_t *LABEL_LAS2[3]                          = {L"LAS 2.0", L"ЛАС 2.0", L"LAS 2.0"};
const wchar_t	*LABEL_LINELUBRICATOR[3]				= {L"Kabel-/Seilöler", L"Лубрикатор каната/троса", L"Line lubricator"};
const wchar_t *LABEL_LOG_FILE[3]                      = {L"LOG-Datei", L"LOG-файл", L"LOG-File"};
const wchar_t *LABEL_LOG_FILE_ADMINISTRATION[3]       = {L"LOG-Dateien verwalten", L"Управление LOG-файлов", L"Administrate LOG-files"};
const wchar_t	*LABEL_MESSAGES[3]						= {L"Meldungen", L"Сообщения", L"Messages"};
const wchar_t *LABEL_MINDEPTH[3]                      = {L"Minimale Teufe", L"Минимальная глубина", L"Minimal Depth"};
const wchar_t *LABEL_MINTENSION[3]                    = {L"Mindest-Zugkraft", L"Минималное натяжение", L"Min. Tension"};
const wchar_t *LABEL_MAXDEPTH[3]                      = {L"Maximale Teufe", L"Максимальная глубина", L"Maximal Depth"};
const wchar_t *LABEL_MAXTENSION[3]                   	= {L"Maximal-Zugkraft", L"Максимальное натяжение", L"Max. Tension"};
const wchar_t *LABEL_MAGNETICMARK[3]                  = {L"Magnetmarke", L"Магнитная метка", L"Magnetic Marker"};
const wchar_t *LABEL_MAGNETICMARKSHORT[3]             = {L"MM", L"ММ", L"MM"};                                  // MM...Magnetmarke
const wchar_t *LABEL_MESSURINGPOINT1[3]               = {L"Messpunkt 1", L"Пункт измерения 1", L"Messuring point 1"};
const wchar_t *LABEL_MESSURINGPOINT2[3]               = {L"Messpunkt 2", L"Пункт измерения 2", L"Messuring point 2"};
//const wchar_t	*LABEL_MULTIPLEWINCHCONFIGS[3]			= {L"Mehrere Windenkonfigurationen", L"Многократные конфигурации лебедки", L"Multiple winch configurations"};
const wchar_t *LABEL_MM_SYNC[3]                       = {L"MM-Synchronisation", L"Синхронизация ММ", L"MM-Synchronisation"};  // MM...Magnetmarke
const wchar_t   *LABEL_MM_SYNC_SWITCHOFF[3]             = {L"MM-Synchronisation abschalten!", L"Выкл. синхронизация магн. меток!", L"Switch off MM-Synchronisation!"};  // MM...Magnetmarke
const wchar_t *LABEL_NAME[3]                          = {L"Name", L"Имя", L"Name"};
const wchar_t *LABEL_NO[3]                            = {L"Nein", L"Нет", L"No"};
const wchar_t *LABEL_OK[3]                            = {L"OK", L"ОК", L"OK"};
const wchar_t *LABEL_ON[3]                            = {L"Ein", L"Включить", L"On"};
const wchar_t *LABEL_ON_V2[3]                         = {L"Ein", L"Запуск", L"On"};
const wchar_t *LABEL_ONYX[3]                          = {L"ONYX", L"ОНИКС", L"ONYX"};
const wchar_t *LABEL_OFF[3]                           = {L"Aus", L"Выключить", L"Off"};
const char      *LABEL_PER_THOUSAND[3]                  = {"/ 1000", "/ 1000", "/ 1000"};
const wchar_t	*LABEL_PERMANENTOPERATION[3]			= {L"Dauerbetrieb", L"Непрерывный режим", L"Permanent operation"};
const wchar_t *LABEL_PID[3]                           = {L"PID", L"ПИД", L"PID"};
const wchar_t *LABEL_PROGRAMABLEAUTOMATIC[3]          = {L"Programmautomatik", L"Программируемый автоматический режим", L"Programable automatic"};
const wchar_t *LABEL_PROGRAMABLEAUTOMATIC_SHORT[3]    = {L"Programmautomatik", L"Программируемый автоматич. режим", L"Programable automatic"};
//const char      *LABEL_QUIESCENTVOLTAGE[3]              = {"Ruhespannung", "Напряжение холостого хода", "Quiescent voltage"};
const wchar_t *LABEL_READY[3]                         = {L"Fertig", L"Готовый", L"Ready"};
//const char      *LABEL_REF_VOLT_TO_0MPH[3]              = {"U für v=0", "U для v=0", "V for v=0"};
const wchar_t	*LABEL_RELTENSIONLIMIT_LOWER[3]			= {L"Untere rel. Zugkraftgrenze", L"Нижний рел. лимит натяжения", L"Lower rel. tension limit"};
const wchar_t	*LABEL_RELTENSIONLIMIT_UPPER[3]			= {L"Obere rel. Zugkraftgrenze", L"Верхний рел. лимит натяжения", L"Upper rel. tension limit"};
//const char      *LABEL_REPLACE[3]                       = {"Erneuern", "Заменить", "Replace"};
const wchar_t *LABEL_REV_TENSION[3]                   = {L"Soll-Zugkraft", L"Заданное натяжение", L"Reverence Tension"};
const wchar_t *LABEL_REV_VELOCITY[3]                  = {L"Soll-Geschwindigkeit", L"Заданная скорость", L"Reverence Velocity"};
const wchar_t *LABEL_REVERSEDEPTH[3]                  = {L"Teufe umkehren", L"Переставлять Глубину", L"Reverse Depth"};
const wchar_t *LABEL_REVOLUTION[3]                    = {L"Drehzahl", L"Обороты", L"Revolution"};
const wchar_t *LABEL_RESET_TIME_TI[3]                 = {L"Nachstellzeit TI", L"Регулировка времени TI", L"Reset time TI"};  // auf Reglungstechnik bezog.
const wchar_t *LABEL_RIG_LOCK_MONITOR[3]              = {L"Überwachung der Mastverrieglung 2x", L"Наблюдение фиксатора мачты 2x", L"Rig lock monitor 2x"};  // auf Reglungstechnik bezog.
const wchar_t *LABEL_RIGHT_WINCHCHOISE[3]             = {L"rechts", L"Правая", L"right"};
const wchar_t *LABEL_RUN[3]                           = {L"Fahrt", L"Движение", L"Run"};
const wchar_t *LABEL_RUSSIAN[3]                       = {L"Russisch", L"Русский", L"Russian"};
const wchar_t *LABEL_SAVE_SETTINGS[3]                 = {L"Einstellungen speichern", L"Запись данных в память", L"Save settings"};
const wchar_t *LABEL_SENSOR_A[3]                      = {L"Sensor A", L"Сенсор A", L"Sensor A"};
const wchar_t *LABEL_SENSOR_B[3]                      = {L"Sensor B", L"Сенсор B", L"Sensor B"};
const wchar_t *LABEL_SENSOR_DEPTHCALC[3]              = {L"TR", L"Счетчик глуб.", L"Depth Calc."};
//const char      *LABEL_SET_CALFACTOR[3]                 = {"Kalibrierwert setzen", "Определить калибровочный фактор", "Set calibration factor"};
const wchar_t *LABEL_SET_CALVAL_TENSION[3]            = {L"Kalibrierwerte setzen", L"Определить калибров. знач.", L"Set calibration values"};
//const char      *LABEL_SET_CALIB_TENSION[3]             = {"Kalibrierwerte setzen", "Определить калибров. знач.", "Set calibration values"};
const wchar_t *LABEL_SET_DEPTH[3]                     = {L"Teufe setzen", L"Определить глубину", L"Set Depth"};
const wchar_t   *LABEL_SET_NEW_DEPTH[3]                 = {L"Neue Teufe setzen!", L"Определить новую глубину!", L"Set new Depth!"};
const wchar_t *LABEL_SET_PARAMETER[3]                 = {L"Parameter", L"Параметр", L"Parameter"};
const wchar_t *LABEL_SETTINGS_FOR_AUTOMATIC[3]        = {L"Einstellungen für Automatikfahrt", L"Управление автоматич. движения", L"Settings for automatic drive"};
const wchar_t   *LABEL_SHUT_DEPTH_DISTANCE_DOWN[3] 		= {L"Eine Teufenstrecke abfahren!", L"Отправляться участок глубины!", L"Shut a depth distance down!"};  // Teufenstrecke abfahren
const wchar_t *LABEL_SIGNAL[3]                        = {L"Hupe", L"Сигнал", L"Horn"};
const wchar_t *LABEL_SLOW[3]                          = {L"Langsam", L"Медленно", L"Slow"};
const wchar_t *LABEL_SPECIAL[3]                       = {L"Spezial", L"Специально", L"Special"};
const wchar_t *LABEL_SPECIAL_SIGNALS[3]               = {L"Sondersignale", L"Специальные сигналы", L"Special signals"};
const wchar_t *LABEL_STANDARD[3]                      = {L"Standard", L"Стандарт", L"Standard"};
const wchar_t *LABEL_STANDARD_OBSOLETE[3]             = {L"Standard (veraltet)", L"Стандарт (устаревший)", L"Standard (obsolete)"};
//const char      *LABEL_START[3]                         = {"Start", "Старт", "Start"};
const wchar_t *LABEL_START_AUTO[3]                    = {L"Automatik Start", L"Автоматич. старт", L"Automatic Start"};
const wchar_t *LABEL_START_UNLOAD_CABLE[3]         	= {L"Zugkraft-Reg. Ein", L"Контроль натяж.вкл.", L"Tension-Ctrl. On"};
const wchar_t *LABEL_STOP[3]                          = {L"Stopp", L"Стоп", L"Stop"};
const wchar_t *LABEL_STOP_AUTO[3]                     = {L"Automatik Stopp", L"Автоматич. стоп", L"Automatic Stop"};
const wchar_t *LABEL_STOP_UNLOAD_CABLE[3]          	= {L"Zugkraft-Reg. aus", L"Контроль натяж.выкл.", L"Tension-Ctrl. Off"};
const wchar_t *LABEL_STRETCHING[3]                    = {L"Stretching", L"Задержка", L"Stretching"};
const wchar_t *LABEL_STRETCHING_ON[3]                 = {L"Stretching ein", L"Включить задержку", L"Stretching on"};
const wchar_t *LABEL_SUPERVISION[3]                   = {L"Überwachung", L"Контроль", L"Monitoring"};
const wchar_t *LABEL_SYSTEM[3]                        = {L"System", L"Система", L"System"};
const wchar_t *LABEL_SYSTEM_RESTART[3]                = {L"System neu starten", L"Перезапуск системы", L"Restart the system"};
const wchar_t *LABEL_SYSTEM_TURNOFF[3]                = {L"System ausschalten", L"Выключить системы", L"Switch off the system"};
const wchar_t   *LABEL_TANK[3]                  		= {L"Tank", L"Бак", L"Tank"};
const wchar_t *LABEL_TEAM[3]                  		= {L"Trupp", L"Партия", L"Team"};
const wchar_t *LABEL_TEMPERATURES[3]                  = {L"Temperaturen", L"Температуры", L"Temperatures"};
const wchar_t *LABEL_TIME[3]                          = {L"Zeit", L"Время", L"Time"};
//const char      *LABEL_TIMEDIFFERENCE[3]                = {"Zeit-Differenz", "Разность времени", "Time Difference"};
const wchar_t *LABEL_TIMEOFDAY[3]                     = {L"Uhrzeit", L"Час", L"Time"};
const wchar_t *LABEL_TENSION[3]                       = {L"Zugkraft", L"Натяжение", L"Tension"};
const wchar_t *LABEL_TENSION_DIMENSIONUNIT[3]         = {L"Maßeinheit der Zugkraft", L"Единица измерения натяжения", L"Dimension unit of tension"};
//const char      *LABEL_TENSIONDIFFERENCE[3]             = {"Zugkraft-Differenz", "Разность натяжения", "Tension Difference"};
const wchar_t *LABEL_TRANSFUSE[3]                     = {L"Übertragen", L"Перенесение", L"Transmit"};
const wchar_t *LABEL_TRANSFUSEDESTINATION[3]          = {L"Übertragungsziel", L"Цель переноса", L"Transmission Destination"};
const wchar_t *LABEL_TWO_EXT_EQUIPMENT_ERRORSIGNALS[3]= {L"Doppelfehlerüberwachung externer Geräte", L"Двойной контроль ожибок наружих приборов", L"Double error monitoring for external equipment"};
const wchar_t *LABEL_TWO_SENSOR_GEARSHIFT_CTRL[3]     = {L"2-Sensoren-Schalthebelüberwachung", L"2-x сенсорный контроль переключающего рычага", L"2 sensors gearshift control"};
const wchar_t *LABEL_UNCOILED_CABLE_LENGHT_IN_METER[3]= {L"Herausgezogene Kabellänge in Metern", L"Вытягиванная длина кабеля в метрах", L"Uncoiled cable length in meter"};
const wchar_t *LABEL_UNLOADCABLE[3]                	= {L"Kabel Entladen", L"Разгрузка кабеля", L"Unload cable"};
const wchar_t *LABEL_UPWARD[3]                        = {L"aufwärts", L"вверх", L"up"};
const wchar_t *LABEL_VELOCITY[3]                      = {L"Geschwindigkeit", L"Скорость", L"Velocity"};
const wchar_t   *LABEL_VELOCITY_DIMENSIONUNIT[3]        = {L"Maßeinheit der Geschwindigkeit", L"Единица измерения скорости", L"Dimension unit of Velocity"};
const wchar_t *LABEL_VIEW[3]                          = {L"Ansicht", L"Вид", L"View"};
const wchar_t *LABEL_VOLT_STEP_NORMAL[3]              = {L"dU für Regelung v (normaler Schritt)",L"dU для регулирования v (нормальный шаг)",L"dV for regulation of v (normal step)"};
const wchar_t *LABEL_VOLT_STEP_SMALL[3]               = {L"dU für Regelung v (kleiner Schritt)",L"dU для регулирования v (маленький шаг)",L"dV for regulation of v (small step)"};
const wchar_t *LABEL_WELL[3]                      	= {L"Bohrloch", L"Скважина", L"Well"};
const wchar_t *LABEL_WINCH[3]                         = {L"Winde", L"Лебедка", L"Winch"};
const wchar_t *LABEL_WINCHBRAKE_CONTROLED_BY_SW[3]	= {L"Software-gesteuerte Windenbremse", L"Тормоз барабана управляемый программным обеспечением ", L"Winch brake controlled by software"};
const wchar_t *LABEL_WINCHMACHINERY[3]                = {L"Windenanlage", L"Лебедочная установка", L"Winch machinery"};
const wchar_t *LABEL_YES[3]                           = {L"Ja", L"Да", L"Yes"};
const wchar_t *LABEL_ZEROPOINT[3]                     = {L"Nullpunkt", L"нулевая точка", L"Zero Point"};

// Programm-Modi
const wchar_t *PROG_MODE_0[3]                         = {L"Undefiniert", L"Неопределенный", L"Undefined"};
const wchar_t *PROG_MODE_1[3]                         = {L"Programm-Start", L"Старт программы", L"Start program"};
const wchar_t *PROG_MODE_2[3]                         = {L"Programm-Ende", L"Стоп программы", L"End program"};
const wchar_t *PROG_MODE_3[3]                         = {L"Manueller Betrieb", L"Ручное управление", L"Manually Mode"};
const wchar_t *PROG_MODE_4[3]                         = {L"Automatischer Betrieb", L"Автоматич. управл.", L"Automatic Mode"};
const wchar_t *PROG_MODE_5[3]                         = {L"Kalibrieren", L"Калибровка", L"Calibration"};     // allgemein Kalibrieren
const wchar_t *PROG_MODE_6[3]                         = {L"Kalibrieren", L"Калибровка", L"Calibration"};     // Teufenrechner
const wchar_t *PROG_MODE_7[3]                         = {L"Kalibrieren", L"Калибровка", L"Calibration"};     // Zugkraftmesser
const wchar_t *PROG_MODE_8[3]                         = {L"Kabel entladen", L"Разгрузка кабеля", L"Unload cable"};
const wchar_t *PROG_MODE_9[3]                         = {L"Freilauf an", L"Свободный ход", L"Freewheel on"};

// Fehlermeldungen
const wchar_t *ERRMSG_AUTOMOTIVEERROR[3]              = {L"Fahrzeug-Fehler", L"Ошибка автомобиля", L"Automotive Error"};
const wchar_t *ERRMSG_CONTROLLERERROR[3]              = {L"Steuergerät Fehler", L"Ошибка контроллера", L"Controller Error"};
const wchar_t *ERRMSG_COPY_LOG_FILE_FAIL[3]           = {L"Kopieren der LOG-Datei(en) fehlgeschlagen", L"Копия LOG-файл(ов) не удался", L"Copy of LOG-File(s) fails"};
const wchar_t *ERRMSG_COULDNT_READ_PARAM[3]           = {L"Fehler beim Auslesen der Parameterdatei", L"Ошибка при считывании файла параметров", L"Error by reading the parameter file"};
const wchar_t *ERRMSG_COULDNT_WRITE_PARAM[3]          = {L"Fehler beim Schreiben in die Parameterdatei", L"Ошибка при записи в файл параметров", L"Error by writing in parameter file"};
const wchar_t *ERRMSG_COULDNT_WRITE_SPECIAL_PARAM[3]  = {L"Fehler beim Schreiben in die spezielle Parameterdatei", L"Ошибка при записи в специальный файл параметров", L"Error by writing in special parameter file"};
const wchar_t	*ERRMSG_CTL_CAN_INTERFACE_COM_ERROR[3]  = {L"Steuergeräte-CAN: Kommunikationsfehler", L"CAN контроллера: Ошибка обмена данных", L"Controller CAN: Communication error"};
const wchar_t	*ERRMSG_CTL_CAN_INTERFACE_INIT_FAILURE[3] = {L"Steuergeräte-CAN: Schnittstelle konnte nicht initialisiert werden", L"CAN контроллера: Интерфайс не подготавливается", L"Controller CAN: Could not initialize interface"};
const wchar_t	*ERRMSG_CTL_CAN_LIBNOTFOUND[3]			= {L"Steuergeräte-CAN: Bibliothek nicht gefunden", L"CAN контроллера: Библиотека не доступный", L"Controller CAN: Library not found"};
const wchar_t *ERRMSG_DAC_CARD_NO_MEMORY[3]           = {L"Speicherreservierung für DAC-Karte fehlgeschlagen", L"Резервирование памяти для DAC-платы не удалось", L"Memory allocation for DAC-Card fails"};
const wchar_t *ERRMSG_DAC_CARD_NOT_FOUND[3]           = {L"Keine passende DAC-Karte gefunden", L"Нет согласованной DAC-платы", L"No compatibly DAC-Card found"};
const wchar_t *ERRMSG_DELETE_LOG_FILE_FAIL[3]         = {L"Löschen der LOG-Datei(en) fehlgeschlagen", L"Стирание LOG-файл(ов) не удался", L"Deleting LOG-File(s) fails"};
const wchar_t *ERRMSG_DEPTHCALC_PORT_OPEN_FAILURE[3]  = {L"Schnittstelle zum Teufenrechner konnte nicht geöffnet werden", L"Интерфейс на счетчик глубины не открывается", L"Interface to depth computer could not be opened"};
const wchar_t *ERRMSG_DEPTHCALC_COMM_FAILURE[3]       = {L"Fehler bei Kommunikation mit Teufenrechner", L"Ошибка при коммуникации с счетчиком глубины", L"Error by communication with depth computer"};
const wchar_t *ERRMSG_DEPTHRANGE_EXCEEDED[3]          = {L"Außerhalb des angegebenen Teufenbereichs", L"Снаружи заданного диапазона глубины", L"Beyond declared depth range"};
const wchar_t *ERRMSG_DIO_RELAY_CARD_NO_MEMORY[3]     = {L"Speicherreservierung für DIO-Relay-Karte fehlgeschlagen", L"Резервирование памяти для DIO-Relay-платы не удалось", L"Memory allocation for DIO-Relay-Card fails"};
const wchar_t *ERRMSG_DIO_RELAY_CARD_NOT_FOUND[3]     = {L"Keine passende DIO-Relais-Karte gefunden", L"Нет согласованной DIO-Relay-платы", L"No compatibly DIO-Relay-Card found"};
const wchar_t *ERRMSG_ERROR_SIGNAL_1_FROM_EXT_EQUIPMENT[3] = {L"Fehlersignal 1 von externem Gerät", L"Сигнал несправности 1 от наружного прибора", L"Error signal 1 from external equipment"};
const wchar_t *ERRMSG_ERROR_SIGNAL_2_FROM_EXT_EQUIPMENT[3] = {L"Fehlersignal 2 von externem Gerät", L"Сигнал несправности 2 от наружного прибора", L"Error signal 2 from external equipment"};
const wchar_t *ERRMSG_GEAR_SHIFT_FAIL[3]              = {L"Getriebeumschaltung fehlgeschlagen", L"Переключение редуктора передач не удалось", L"Gear shifting failure"};
const wchar_t *ERRMSG_INC_ENC_ERROR[3]                = {L"Inkremental-Geber ausgefallen", L"Датчик ИГР вышел из строя", L"Incremental Encoder cancelled"};
const wchar_t *ERRMSG_INFOOUT_PORT_OPEN_FAILURE[3]    = {L"Schnittstelle zur Informationsausgabe konnte nicht geöffnet werden", L"Интерфейс к выводу информации не открывается", L"Interface for information output could not be opened"};
const wchar_t   *ERRMSG_LEVER_FAULTY[3]         		= {L"Bedienhebel Winde gestört", L"Ручка управления лебёдкой повреждённа", L"Winch control lever faulty"};
const wchar_t *ERRMSG_LOG_DISKSPACE_FAIL[3]           = {L"Zu wenig Speicherplatz für Protokollierung", L"Слишком мало память для протоколирования", L"Disk space to small for logging"};
const wchar_t *ERRMSG_LOG_DATA_FAIL[3]                = {L"Fehler beim Schreiben der LOG-Daten", L"Ошибка при записи LOG-файлов", L"Error during writing LOG-data"};
const wchar_t *ERRMSG_LOG_INTERNAL_DISKSPACE_FAIL[3]  = {L"Zu wenig Speicherplatz für interne Protokollierung", L"Слишком мало память для внутреннего протоколирования", L"Disk space to small for internal logging"};
const wchar_t *ERRMSG_MAXTENSION_EXCEEDED[3]          = {L"Max. Zugkraft überschritten", L"Превышение максимального натяжения", L"Max. Tension exceeded"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_BRAKE[3]         	= {L"Ausgang gestört: Bremse", L"Выход повреждён: Тормоз", L"Output faulty: Brake"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_SPOOLER_HOR_CLUTCH[3] = {L"Ausgang gestört: Spooler horizontal - Kupplung", L"Выход повреждён: Кабелеукладчик горизонтальный - муфта", L"Output faulty: Spooler horizontal - clutch"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_SPOOLER_HOR_LEFT[3] = {L"Ausgang gestört: Spooler horizontal links", L"Выход повреждён: Кабелеукладчик горизонтальный влево", L"Output faulty: Spooler horizontal left"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_SPOOLER_HOR_RIGHT[3] = {L"Ausgang gestört: Spooler horizontal rechts", L"Выход повреждён: Кабелеукладчик горизонтальный направо", L"Output faulty: Spooler horizontal right"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_FREEWHEEL[3] 		= {L"Ausgang gestört: Freilauf", L"Выход повреждён: Свободный ход", L"Output faulty: Freewheeling"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_FREEWHEEL_INV[3]	= {L"Ausgang gestört: Freilauf invertiert", L"Выход повреждён: Инверсный свободный ход", L"Output faulty: Freewheeling inverted"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_HYDRAULIC_ACTIVE[3] = {L"Ausgang gestört: Hydraulik aktiv", L"Выход повреждён: Гидравлика в действии", L"Output faulty: Hydraulic active"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_PUMP1_FWD[3] 		= {L"Ausgang gestört: Pumpe 1 - vorwärts", L"Выход повреждён: Насос 1 - вперёд", L"Output faulty: Pump 1 - forward"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_PUMP1_REVERSE[3] 	= {L"Ausgang gestört: Pumpe 1 - zurück", L"Выход повреждён: Насос 1 - назад", L"Output faulty: Pump 1 - reverse"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_PUMP2_FWD[3] 		= {L"Ausgang gestört: Pumpe 1 - vorwärts", L"Выход повреждён: Насос 1 - вперёд", L"Output faulty: Pump 1 - forward"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_PUMP2_REVERSE[3] 	= {L"Ausgang gestört: Pumpe 1 - zurück", L"Выход повреждён: Насос 1 - назад", L"Output faulty: Pump 1 - reverse"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_SLOWMOTION[3]   	= {L"Ausgang gestört: Fahrstufenumschaltung", L"Выход повреждён: Переключение скорости хода", L"Output faulty: running range shift"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_SPECIAL1[3]		= {L"Ausgang gestört: Sonderfunktion 1", L"Выход повреждён: Cпециальная функция 1", L"Output faulty: Special function 1"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_SPECIAL2[3]		= {L"Ausgang gestört: Sonderfunktion 2", L"Выход повреждён: Cпециальная функция 2", L"Output faulty: Special function 2"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_SPOOLER_TELESCOP_RUNIN[3] = {L"Ausgang gestört: Spooler - Teleskopierung einfahren", L"Выход повреждён: Кабелеукладчик - вдвигание телескопирования", L"Output faulty: Spooler - Telescope run-in"};
const wchar_t   *ERRMSG_OUTPUT_FAULTY_SPOOLER_VERT[3]   = {L"Ausgang gestört: Spooler vertikal", L"Выход повреждён: Кабелеукладчик вертикальный", L"Output faulty: Spooler vertical"};
const wchar_t *ERRMSG_POWERFAIL[3]                    = {L"Stromversorgung unterbrochen, System wird in Kürze heruntergefahren", L"Электроснабжение прерванно, система вскоре прекратит работу", L"Power fail, system shutdown briefly"};
const wchar_t *ERRMSG_PROGRAM_INSTANCE_EXISTS[3]      = {L"WinchControl scheint bereits aktiv zu sein.", L"Управление лебедкой возможно в активном режиме", L"WinchControl already seems to be active."};
const wchar_t *ERRMSG_RIG_UNSECURED[3]                = {L"Mast ungesichert",L"Мачта не стопорная",L"Rig unsecured"};
const wchar_t   *ERRMSG_SPOOLER_HOR_LEVER_FAULTY[3] 	= {L"Spooler horizontal - Bedienhebel gestört", L"Кабелеукладчик горизонтальный - Ручка управления повреждённа", L"Spooler horizontal - Control lever faulty"};
const wchar_t *ERRMSG_START_FAILURE[3]                = {L"Ressourcen-Fehler!!!\nProgramm-Start abgebrochen.", L"Ошибка ресурса!!!\nСтарт программы аварийно завершен", L"Ressource Error!\nStartup aborted."};

// Texte/Erläuterungen
const wchar_t *EXPLAN_CALIBDEPTH[3]                   = {L"Eine möglichst große Kabellänge muss abgewickelt, gemessen und angegeben werden.",
														   L"Вымотать, измерять и задавать по возможности большую длину кабеля.",
                                                           L"The possible amount of cable must be uncoiled, be measured and be entered."};
const wchar_t *EXPLAN_CALIBTENSIONCOMPLETE[3]         = {L"Es müssen zwei Zugkraftmessungen mit unterschiedlicher Last\ndurchgeführt werden und die bei der jeweiligen Messung\ntatsächlich vorhandene Zugkraft angegeben werden.",
														   L"Провести 2 измерения натяжения с разной нагрузкой\n и задать действительное натяжение во время измерения.",
														   L"Two tension measurements with different loads must be carried out\nand the tension with the respective measurement available has to be entered."};
const wchar_t *EXPLAN_CALIBTENSIONZERO[3]             = {L"Vor der Nullpunktermittlung muss der Zugkraftmesser vollständig\nentlastet werden.",
														   L"Перед измеренией нулевой точки должен быть разгружен счетчик натяжения.",
														   L"Before acquisition of zero point the dynamometer must be relieved completely."};
const wchar_t *EXPLAN_DELETELOGFILES[3]                = {L"Wollen Sie die markierten LOG-Dateien\nwirklich löschen ?",
                                                            L"Удалить маркированые файлы ?",
                                                            L"Do you really want to delete\nthe marked LOG-files ?"};
const wchar_t *EXPLAN_LOSTMOTIONONQUEST[3]            = {L"Soll der Freilauf wirklich eingeschaltet werden?",
                                                           L"Действительно включить свободный ход?",
                                                           L"Should the freewheel really be switched on?"};
const wchar_t *EXPLAN_LOSTMOTIONOFFQUEST[3]           = {L"Soll der Freilauf wirklich ausgeschaltet werden?",
                                                           L"Действительно выключить свободный ход?",
                                                           L"Should the freewheel really be switched off?"};
const wchar_t *EXPLAN_SYSTEM_RESTART[3]               = {L"Soll das System wirklich neu gestartet werden?",
                                                           L"Действительно проводить перезагрузку системы?",
                                                           L"Should the system really be restarted?"};
const wchar_t *EXPLAN_SYSTEM_TERMINATE[3]             = {L"Soll das Programm wirklich beendet werden?",
                                                           L"Действительно заканчивать программу?",
                                                           L"Should the application really be closed?"};
const wchar_t *EXPLAN_SYSTEM_TURNOFF[3]               = {L"Soll das System wirklich heruntergefahren werden?",
                                                           L"Действительно переходить на более низкие\nрежимные параметры?",
                                                           L"Should the system really be shut down?"};              
#endif //__LABELLING_H

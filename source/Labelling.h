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

const char      *LANGUAGELABEL[3]                       = {"Deutsch", "Русский", "English"};

// Platzhalter
const char      *__SEPARATOR01[3]                       = {" ", " ", " "};      // Trennzeichen: Leerzeichen
const char      *__SEPARATOR02[3]                       = {"/", "/", "/"};      // Trennzeichen: Slash
const char      *__SEPARATOR03[3]                       = {":", ":", ":"};      // Trennzeichen: Doppelpunkt
const char      *__SEPARATOR04[3]                       = {".", ".", "."};      // Trennzeichen: Punkt   

// Maßeinheiten
const wchar_t   *UNIT_BAR[3]                            = {L"bar", L"бар", L"bar"};
const char      *UNIT_DEGREE_CELSIUS[3]                 = {"°C", "°C", "°C"};
const char      *UNIT_HOUR[3]                           = {"h", "ч", "h"};
const char      *UNIT_NEWTON[3]                         = {"N", "Н", "N"};
const char      *UNIT_KILONEWTON[3]                     = {"kN", "кН", "kN"};
const char      *UNIT_KILOPOND[3]                       = {"kp", "кгс", "kp"};
const char      *UNIT_NEWTON_PER_MILLIVOLT[3]           = {"N/mV", "Н/мВ", "N/mV"};
const char      *UNIT_METER[3]                          = {"m", "м", "m"};
const char      *UNIT_METER_PER_HOUR[3]                 = {"m/h", "м/ч", "m/h"};
const wchar_t   *UNIT_METER_PER_MINUTE[3]               = {L"m/min", L"м/мин", L"m/min"};
const char      *UNIT_MILLIMETER_PER_IMPULSE[3]         = {"mm/Impuls", "мм/импульс", "mm/Impulse"};
const char      *UNIT_MILLISECOUND[3]                   = {"ms", "мс", "ms"};
const char      *UNIT_MILLIVOLT[3]                      = {"mV","мВ","mV"};
const wchar_t   *UNIT_PERC[3]               			= {L"%", L"%", L"%"};
const wchar_t   *UNIT_RPM[3]               				= {L"U/min", L"об/мин", L"rpm"};
const char      *UNIT_SECOUND[3]                        = {"s", "с", "s"};

const char      *UNIT_EXPL_NEWTON[3]                    = {"1N (1 Newton)", "1Н (1 Нютон)", "1N (1 Newton)"};
const char      *UNIT_EXPL_KILONEWTON[3]                = {"1kN (1 Kilonewton)", "1кН (1 Килонютон)", "1kN (1 Kilonewton)"};
const char      *UNIT_EXPL_KILOPOND[3]                  = {"1kp (1 Kilopond)", "1кгс (1 Кило грамм-сила)", "1kp (1 Kilopond)"};

const wchar_t	*UNIT_EXPL_METER_PER_HOUR[3]			= {L"1m/h (1 Meter pro Stunde)", L"1м/ч (1 метр в час)", L"1 m/h (1 meter per hour)"};
const wchar_t	*UNIT_EXPL_METER_PER_MINUTE[3]			= {L"1m/min (1 Meter pro Minute)", L"1м/мин (1 метр в минуту)", L"1 m/min (1 meter per minute)"};

// Fehlermeldungen kurz
const char      *SHORT_MSG_ERROR[3]                     = {"Fehler","Ошибка", "Error"};
const char      *SHORT_MSG_NOERROR[3]                   = {"", "Нет ошибок", ""};
const char      *SHORT_MSG_ATTENTION[3]                 = {"Achtung", "Внимание", "Attention"};

// Beschriftungen
const char      *LABEL_ABORT[3]                         = {"Abbrechen", "Отмена", "Abort"};
const char      *LABEL_ACCEPT[3]                        = {"Übernehmen", "Сохранить", "Accept"};
const char      *LABEL_ACQUIREZEROPOINT[3]              = {"Nullpunkt ermitteln", "Устанавливать нейтраль", "Acquire zero point"};
const char      *LABEL_ADDITIONAL_OILSTRAINER_MONIT[3]	= {"Zusätzl. Ölfilterüberw.", "Дополн. контроль масл. фильтра", "Add. oil strainer monit."};
const wchar_t	*LABEL_ADDITIONAL_FUNCTIONS[3]			= {L"Zusatzfunktionen", L"Дополнительные функции", L"Additional functions"};
const char      *LABEL_AMPLIFICATIONFACTOR_KR[3]        = {"Verstärkungsfaktor KR", "Коэффициент усиления KR", "Amplification factor KR"};
const char      *LABEL_AUTOMATIC_BRAKE[3]               = {"Automatische\nBremse", "Автоматич.\nтормоз", "Automatic\nbrake"};
const char      *LABEL_AUTOMOTIVE_ERROR[3]              = {"Fahrzeug-Fehler", "Ошибка автомобиля", "Automotive Error"};
const char      *LABEL_AUXIALLYDRIVE[3]                 = {"Nebenantrieb", "Коробка отбора мощности", "Auxially drive"};
const wchar_t	*LABEL_AVAILABLE[3]						= {L"Verfügbar", L"Доступный", L"Available"};
const char      *LABEL_BEGINLENGHT_OF_CABLE[3]          = {"Anfangs-Länge des Kabels", "Начальная длина кабеля", "Begin-lenght of the cable"};
const char      *LABEL_BEGINTENSION[3]                 	= {"Anfangs-Zugkraft", "Начальное натяжение", "Begin-tension"};
const wchar_t	*LABEL_BRAKEOFFENABLE[3]				= {L"Bremse manuell abschaltbar", L"Возможность мануального отключения тормоза", L"Manually sheddable brake"};
const char      *LABEL_CABLE[3]                         = {"Kabel", "Кабель", "Cable"};
const char      *LABEL_CALCTENSIONDYN[3]               	= {"Maximale Zugkraft dynamisch berechnen", "Рассчитать натяжение", "Calculate maximal tension dynamic"}; // Gleitende Maximal-Zugkraft
//const char      *LABEL_CALCTENSIONDYNDIFFERENCE[3]     	= {"Zugkraftabstufung", "Фракционирование натяжение", "Tension gradation"};
//const char      *LABEL_CALCULATE[3]                     = {"Berechnen", "Рассчитывать", "Calculate"};
const wchar_t   *LABEL_CALFACTOR[3]                 	= {L"Kalibrierwert", L"Калибров. знач.", L"Calibration factor"};
const wchar_t   *LABEL_CALFACTOR_ACTIVE[3]             	= {L"Aktiver Kalibrierwert", L"Действующий калибров. знач.", L"Active calibration factor"};
const wchar_t   *LABEL_CALFACTOR_EARMARKED[3]          	= {L"Vorgemerkter Kalibrierwert", L"Намеченный калибров. знач.", L"Earmarked calibration factor"};
const char      *LABEL_CALIB_TENSION_X[3]              	= {"Spannungsdifferenz für Anstieg", "Начальное различие напряжения", "Voltage difference for rising"};
const char      *LABEL_CALIB_TENSION_Y[3]              	= {"Zugkraftdifferenz für Anstieg", "Начальное различие натяжения", "Tension difference for rising"};
const char      *LABEL_CALIB_TENSION_ZERO[3]           	= {"Offset (Null-Wert)", "Сдвиг (Нулевое значение)", "Offset (Zero)"};
const char      *LABEL_CALIB_VALUES_TENSION[3]          = {"Kalibrierwerte für Zugkraftbestimmung", "Калибровочные значения для определения силы натяжения", "Calibration values for tension acquisition"};
const char      *LABEL_CALIBRATE[3]                   	= {"Kalibrierung", "Калибровка", "Calibrate"};
const wchar_t	*LABEL_COMMON[3]						= {L"Allgemein", L"Общий", L"Common"};
const char      *LABEL_CHANGE_OILFILTER[3]              = {"Ölfilter austauschen", "Замена масляного фильтра", "Change oil-filter"};
const char      *LABEL_CHARACTERISTICS[3]               = {"Kenndaten", "Параметры", "Characteristics"};
const char      *LABEL_CHARSET_OF_VIEW[3]               = {"Zeichensatz der Ansicht", "Символы вида", "Character set of view"};
const char      *LABEL_CLEARING[3]                      = {"Freigabe", "Сброс", "Clear"};
const char      *LABEL_COMMENT[3]                       = {"Bemerkungen", "Комментарий", "Comment"};
const char      *LABEL_COMPLETE[3]                      = {"Vollständig", "Полный", "Complete"};
const wchar_t 	*LABEL_CONFIGURATION[3]					= {L"Konfiguration", L"Конфигурация", L"Configuration"};
const char      *LABEL_CONTROLLERERRORMONIT[3]          = {"Überwachung Steuergerät", "Наблюдение контроллера", "Controller Monitoring"};
const wchar_t	*LABEL_CONTROLLERERRORMONIT_ATSM_ENABLE[3] = {L"Überwachung Steuergerät in langs. Fahrstufe", L"Наблюдение контроллера в медл. режиме", L"Controller monitoring at slow motion"};
const wchar_t   *LABEL_CONTROLLER[3]          			= {L"Steuergerät", L"Контроллер", L"Controller"};
const char      *LABEL_DATE[3]                          = {"Datum", "Число", "Date"};
const char      *LABEL_DATETIMETUNING[3]                = {"Datum und Uhrzeit", "Число и час", "Date and time"};
const char      *LABEL_DEACTIVATE_BRAKE[3]              = {"Bremse\nlösen", "Выключить\nтормоз", "Brake\noff"};
const char      *LABEL_DELETE[3]                        = {"Löschen", "Удаление", "Delete"};
const char      *LABEL_DELETE_LOG_FILES[3]              = {"Löschen von LOG-Dateien", "Удаление LOG-файлов", "Deleting LOG-Files"};
const char      *LABEL_DEPTH[3]                         = {"Teufe", "Глубина", "Depth"};
const char      *LABEL_DEPTHCALC[3]                     = {"Teufenrechner", "Счетчик глубины", "Depth Computer"};
const wchar_t	*LABEL_DEPTHIMPULSESCOUNT[3] 			= {L"Erfasste Teufenimpulse", L"Количество импульсов глубины", L"Depth impulses count"};
const char      *LABEL_DERIVATIVE_TIME_TD[3]            = {"Vorhaltezeit TD", "Предварительное время TD", "Derivative time TD"};      // auf Regelungstechnik bezog.
const char      *LABEL_DEVELOPER[3]                     = {"Entwickler", "Разрабочик", "Developer"};
const char      *LABEL_DIFFERENCE_PARAM[3]              = {"Differenz-Einstellungen", "Установка разности", "Difference Settings"};
const char      *LABEL_DIRECTION[3]                     = {"Fahrtrichtung", "Направление", "Direction"};
const char      *LABEL_DISCARD[3]                       = {"Verwerfen", "Отмена", "Discard"};
const char      *LABEL_DOUBLEWINCH[3]                   = {"Doppelwinde", "Двойная лебедка", "Double winch"};
const char      *LABEL_DOWNWARD[3]                      = {"abwärts", "вниз", "down"};
const char      *LABEL_DRIVE_CONTROL[3]                 = {"Fahrsteuerung", "Управление лебедкой", "Drive control"};
const char      *LABEL_DURATIONUNTILSHUTDOWN[3]         = {"Systemabschaltung in", "Срок до автом. выключения", "shutdown in"};
const char      *LABEL_DYNAMIC_MAX_TENSION[3]           = {"Dynamische Max-Zugkraft", "Динамическое макс. натяжение", "Sliding Max-Tension"}; // Gleitende Maximalzugkraft
const char      *LABEL_ENDLENGHT_OF_CABLE[3]            = {"End-Länge des Kabels", "Конечная длина кабеля", "End-lenght of the Cable"};
const char      *LABEL_ENDTENSION[3]                   	= {"End-Zugkraft", "Конечное натяжение", "End-tension"};
const char      *LABEL_EMERGENCYSWITCH[3]               = {"Not-Aus-Schalter", "Аварийный выключатель", "Emergency-Switch"};
const wchar_t   *LABEL_ENGINE[3]                        = {L"Motor", L"Двигатель", L"Engine"};
const char      *LABEL_ENGINEOFF[3]                     = {"Motor aus", "Выключить\nмотор", "Engine Off"};
const char      *LABEL_ENGINEON[3]                      = {"Motor ein", "Запуск\nмотора", "Engine On"};
const char      *LABEL_ENGINEREVOLUTION[3]              = {"Motordrehzahl", "Обороты\nавтомотора", "Engine Rev."};
const char      *LABEL_ENGINEREVOLUTIONCHANGEABLE[3]    = {"Motordrehzahl änderbar", "Обороты двигателя подвергаются регулируванию", "Engine revolution changeable"};
const char      *LABEL_ENGINESTARTSTOPENABLE[3]         = {"Motor Start-Stopp aktiviert", "Функция двигатель Старт-Стоп активна", "Engine Start-Stop activated"};
const char      *LABEL_ENGLISH[3]                       = {"Englisch", "Английский", "English"};
const char      *LABEL_ENFORCE_MM_SYNC_UNTIL[3] 		= {"MM-Synchr. erzwingen bis", "Форсировать синхронизацию ММ до", "Enforce MM sync. until"};
const char      *LABEL_EXIT[3]                          = {"Beenden", "Заканчивать", "Exit"};
const char      *LABEL_EXIT_PROGRAM[3]                  = {"WinchControl beenden", "Закончить управление лебедкой", "Exit WinchControl"};
const wchar_t	*LABEL_EXT_BRAKE_MONITORING[3]			= {L"Überwachung einer externen Bremse", L"Контроль дополнительного тормоза", L"External brake monitoring"};
const char      *LABEL_EXT_CM_IMPULSE[3]                = {"Externer cm-Impuls", "Наружный см-импульс", "External cm-impulse"};
const char      *LABEL_FAST[3]                          = {"Schnell", "Быстро", "Fast"};
const char      *LABEL_FILE_LANGUAGE_FORMAT[3]          = {"Sprachformat", "Формат языков", "Language format"};
const char      *LABEL_FILEADMIN[3]                     = {"Dateiverwaltung", "Управление файлов", "File Administration"};
const char      *LABEL_FILETYPE[3]                      = {"Dateityp", "Тип файла", "Type of file"};
const char      *LABEL_GERMAN[3]                        = {"Deutsch", "Немецкий", "German"};
const char      *LABEL_HYDRAULIC[3]                     = {"Hydraulik", "Гидравлика", "Hydraulic"};
const wchar_t	*LABEL_HYDRAULIC_BRAKE[3]				= {L"Hydraulische Bremse", L"Гидравлический тормоз", L"Hydraulic Brake"};
const char      *LABEL_IGNORE[3]                        = {"Ignorieren", "Игнорировать", "Ignore"};
const char      *LABEL_INCENCERRORMONIT[3]              = {"IGR-Überwachung", "Контроль датчика глубины", "Incremental Encorder Monitoring"};
const wchar_t	*LABEL_INSERT_REAL_DEPTH[3]				= {L"Tatsächliche Teufe eingeben!",L"Вводить фактическую глубину!", L"Insert real depth!"};
//const char      *LABEL_JOYSTICK_VOLTAGE[3]              = {"Spannung am HSG", "Напряжение на джойстике", "Joystick voltage"};
const char      *LABEL_LEFT_WINCHCHOISE[3]     			= {"links", "Левая", "left"};
const wchar_t	*LABEL_LUBRICATE_LINE[3]                = {L"Kabel ölen", L"Смазка кабеля", L"Cable lubrication"};
const char      *LABEL_LOCATION[3]                      = {"Ort", "Место", "Location"};
const char      *LABEL_LOSTMOTIONOFF[3]                 = {"Freilauf Aus", "Выключить\nсвободный ход", "Freewheel Off"};       // Beschreibt Taste um etwas zu tun, keinen Zustand
const char      *LABEL_LOSTMOTIONON[3]                  = {"Freilauf Ein", "Включить\nсвободный ход", "Freewheel On"};
const char      *LABEL_LOSTMOTIONENABLE[3]				= {"Freilauf verfügbar", "Свободный ход в распоряжении", "Lost motion available"};
const char      *LABEL_KARAT[3]                         = {"KARAT", "КАРАТ", "KARAT"};
const char      *LABEL_KARAT2[3]                        = {"KARAT-2", "КАРАТ-2", "KARAT-2"};
const char      *LABEL_KARAT2N[3]                       = {"KARAT-2 negiert", "КАРАТ-2 с отрицанием", "KARAT-2 negatived"};
const char      *LABEL_KEDR[3]                          = {"KEDR", "КЕДР", "KEDR"};
const char      *LABEL_KEDRN[3]                         = {"KEDR negiert", "КЕДР с отрицанием", "KEDR negatived"};
const wchar_t	*LABEL_L_ACTIVE[3]						= {L"L-aktiv", L"Cигнал с активным низким уровнем", L"L-active"};
const char      *LABEL_LAS2[3]                          = {"LAS 2.0", "ЛАС 2.0", "LAS 2.0"};
const wchar_t	*LABEL_LINELUBRICATOR[3]				= {L"Kabel-/Seilöler", L"Лубрикатор каната/троса", L"Line lubricator"};
const char      *LABEL_LOG_FILE[3]                      = {"LOG-Datei", "LOG-файл", "LOG-File"};
const char      *LABEL_LOG_FILE_ADMINISTRATION[3]       = {"LOG-Dateien verwalten", "Управление LOG-файлов", "Administrate LOG-files"};
const wchar_t	*LABEL_MESSAGES[3]						= {L"Meldungen", L"Сообщения", L"Messages"};
const char      *LABEL_MINDEPTH[3]                      = {"Minimale Teufe", "Минимальная глубина", "Minimal Depth"};
const char      *LABEL_MINTENSION[3]                    = {"Mindest-Zugkraft", "Минималное натяжение", "Min. Tension"};
const char      *LABEL_MAXDEPTH[3]                      = {"Maximale Teufe", "Максимальная глубина", "Maximal Depth"};
const char      *LABEL_MAXTENSION[3]                   	= {"Maximal-Zugkraft", "Максимальное натяжение", "Max. Tension"};
const char      *LABEL_MAGNETICMARK[3]                  = {"Magnetmarke", "Магнитная метка", "Magnetic Marker"};
const char      *LABEL_MAGNETICMARKSHORT[3]             = {"MM", "ММ", "MM"};                                  // MM...Magnetmarke
const char      *LABEL_MESSURINGPOINT1[3]               = {"Messpunkt 1", "Пункт измерения 1", "Messuring point 1"};
const char      *LABEL_MESSURINGPOINT2[3]               = {"Messpunkt 2", "Пункт измерения 2", "Messuring point 2"};
//const wchar_t	*LABEL_MULTIPLEWINCHCONFIGS[3]			= {L"Mehrere Windenkonfigurationen", L"Многократные конфигурации лебедки", L"Multiple winch configurations"};
const char      *LABEL_MM_SYNC[3]                       = {"MM-Synchronisation", "Синхронизация ММ", "MM-Synchronisation"};  // MM...Magnetmarke
const wchar_t   *LABEL_MM_SYNC_SWITCHOFF[3]             = {L"MM-Synchronisation abschalten!", L"Выкл. синхронизация магн. меток!", L"Switch off MM-Synchronisation!"};  // MM...Magnetmarke
const char      *LABEL_NAME[3]                          = {"Name", "Имя", "Name"};
const char      *LABEL_NO[3]                            = {"Nein", "Нет", "No"};
const char      *LABEL_OK[3]                            = {"OK", "ОК", "OK"};
const char      *LABEL_ON[3]                            = {"Ein", "Включить", "On"};
const char      *LABEL_ON_V2[3]                         = {"Ein", "Запуск", "On"};
const char      *LABEL_ONYX[3]                          = {"ONYX", "ОНИКС", "ONYX"};
const char      *LABEL_OFF[3]                           = {"Aus", "Выключить", "Off"};
const char      *LABEL_PER_THOUSAND[3]                  = {"/ 1000", "/ 1000", "/ 1000"};
const wchar_t	*LABEL_PERMANENTOPERATION[3]			= {L"Dauerbetrieb", L"Непрерывный режим", L"Permanent operation"};
const char      *LABEL_PID[3]                           = {"PID", "ПИД", "PID"};
const char      *LABEL_PROGRAMABLEAUTOMATIC[3]          = {"Programmautomatik", "Программируемый автоматический режим", "Programable automatic"};
const char      *LABEL_PROGRAMABLEAUTOMATIC_SHORT[3]    = {"Programmautomatik", "Программируемый автоматич. режим", "Programable automatic"};
//const char      *LABEL_QUIESCENTVOLTAGE[3]              = {"Ruhespannung", "Напряжение холостого хода", "Quiescent voltage"};
const char      *LABEL_READY[3]                         = {"Fertig", "Готовый", "Ready"};
//const char      *LABEL_REF_VOLT_TO_0MPH[3]              = {"U für v=0", "U для v=0", "V for v=0"};
const wchar_t	*LABEL_RELTENSIONLIMIT_LOWER[3]			= {L"Untere rel. Zugkraftgrenze", L"Нижний рел. лимит натяжения", L"Lower rel. tension limit"};
const wchar_t	*LABEL_RELTENSIONLIMIT_UPPER[3]			= {L"Obere rel. Zugkraftgrenze", L"Верхний рел. лимит натяжения", L"Upper rel. tension limit"};
//const char      *LABEL_REPLACE[3]                       = {"Erneuern", "Заменить", "Replace"};
const char      *LABEL_REV_TENSION[3]                   = {"Soll-Zugkraft", "Заданное натяжение", "Reverence Tension"};
const char      *LABEL_REV_VELOCITY[3]                  = {"Soll-Geschwindigkeit", "Заданная скорость", "Reverence Velocity"};
const char      *LABEL_REVERSEDEPTH[3]                  = {"Teufe umkehren", "Переставлять Глубину", "Reverse Depth"};
const char      *LABEL_REVOLUTION[3]                    = {"Drehzahl", "Обороты", "Revolution"};
const char      *LABEL_RESET_TIME_TI[3]                 = {"Nachstellzeit TI", "Регулировка времени TI", "Reset time TI"};  // auf Reglungstechnik bezog.
const char      *LABEL_RIG_LOCK_MONITOR[3]              = {"Überwachung der Mastverrieglung 2x", "Наблюдение фиксатора мачты 2x", "Rig lock monitor 2x"};  // auf Reglungstechnik bezog.
const char      *LABEL_RIGHT_WINCHCHOISE[3]             = {"rechts", "Правая", "right"};
const char      *LABEL_RUN[3]                           = {"Fahrt", "Движение", "Run"};
const char      *LABEL_RUSSIAN[3]                       = {"Russisch", "Русский", "Russian"};
const char      *LABEL_SAVE_SETTINGS[3]                 = {"Einstellungen speichern", "Запись данных в память", "Save settings"};
const char      *LABEL_SENSOR_A[3]                      = {"Sensor A", "Сенсор A", "Sensor A"};
const char      *LABEL_SENSOR_B[3]                      = {"Sensor B", "Сенсор B", "Sensor B"};
const char      *LABEL_SENSOR_DEPTHCALC[3]              = {"TR", "Счетчик глуб.", "Depth Calc."};
//const char      *LABEL_SET_CALFACTOR[3]                 = {"Kalibrierwert setzen", "Определить калибровочный фактор", "Set calibration factor"};
const char      *LABEL_SET_CALVAL_TENSION[3]            = {"Kalibrierwerte setzen", "Определить калибров. знач.", "Set calibration values"};
//const char      *LABEL_SET_CALIB_TENSION[3]             = {"Kalibrierwerte setzen", "Определить калибров. знач.", "Set calibration values"};
const char      *LABEL_SET_DEPTH[3]                     = {"Teufe setzen", "Определить глубину", "Set Depth"};
const wchar_t   *LABEL_SET_NEW_DEPTH[3]                 = {L"Neue Teufe setzen!", L"Определить новую глубину!", L"Set new Depth!"};
const char      *LABEL_SET_PARAMETER[3]                 = {"Parameter", "Параметр", "Parameter"};
const char      *LABEL_SETTINGS_FOR_AUTOMATIC[3]        = {"Einstellungen für Automatikfahrt", "Управление автоматич. движения", "Settings for automatic drive"};
const wchar_t   *LABEL_SHUT_DEPTH_DISTANCE_DOWN[3] 		= {L"Eine Teufenstrecke abfahren!", L"Отправляться участок глубины!", L"Shut a depth distance down!"};  // Teufenstrecke abfahren
const char      *LABEL_SIGNAL[3]                        = {"Hupe", "Сигнал", "Horn"};
const char      *LABEL_SLOW[3]                          = {"Langsam", "Медленно", "Slow"};
const char      *LABEL_SPECIAL[3]                       = {"Spezial", "Специально", "Special"};
const char      *LABEL_SPECIAL_SIGNALS[3]               = {"Sondersignale", "Специальные сигналы", "Special signals"};
const char      *LABEL_STANDARD[3]                      = {"Standard", "Стандарт", "Standard"};
const char      *LABEL_STANDARD_OBSOLETE[3]             = {"Standard (veraltet)", "Стандарт (устаревший)", "Standard (obsolete)"};
//const char      *LABEL_START[3]                         = {"Start", "Старт", "Start"};
const char      *LABEL_START_AUTO[3]                    = {"Automatik Start", "Автоматич. старт", "Automatic Start"};
const char      *LABEL_START_UNLOAD_CABLE[3]         	= {"Zugkraft-Reg. Ein", "Контроль натяж.вкл.", "Tension-Ctrl. On"};
const char      *LABEL_STOP[3]                          = {"Stopp", "Стоп", "Stop"};
const char      *LABEL_STOP_AUTO[3]                     = {"Automatik Stopp", "Автоматич. стоп", "Automatic Stop"};
const char      *LABEL_STOP_UNLOAD_CABLE[3]          	= {"Zugkraft-Reg. aus", "Контроль натяж.выкл.", "Tension-Ctrl. Off"};
const char      *LABEL_STRETCHING[3]                    = {"Stretching", "Задержка", "Stretching"};
const char      *LABEL_STRETCHING_ON[3]                 = {"Stretching ein", "Включить задержку", "Stretching on"};
const char      *LABEL_SUPERVISION[3]                   = {"Überwachung", "Контроль", "Monitoring"};
const char      *LABEL_SYSTEM[3]                        = {"System", "Система", "System"};
const char      *LABEL_SYSTEM_RESTART[3]                = {"System neu starten", "Перезапуск системы", "Restart the system"};
const char      *LABEL_SYSTEM_TURNOFF[3]                = {"System ausschalten", "Выключить системы", "Switch off the system"};
const wchar_t   *LABEL_TANK[3]                  		= {L"Tank", L"Бак", L"Tank"};
const char      *LABEL_TEAM[3]                  		= {"Trupp", "Партия", "Team"};
const char      *LABEL_TEMPERATURES[3]                  = {"Temperaturen", "Температуры", "Temperatures"};
const char      *LABEL_TIME[3]                          = {"Zeit", "Время", "Time"};
//const char      *LABEL_TIMEDIFFERENCE[3]                = {"Zeit-Differenz", "Разность времени", "Time Difference"};
const char      *LABEL_TIMEOFDAY[3]                     = {"Uhrzeit", "Час", "Time"};
const char      *LABEL_TENSION[3]                       = {"Zugkraft", "Натяжение", "Tension"};
const char      *LABEL_TENSION_DIMENSIONUNIT[3]         = {"Maßeinheit der Zugkraft", "Единица измерения натяжения", "Dimension unit of tension"};
//const char      *LABEL_TENSIONDIFFERENCE[3]             = {"Zugkraft-Differenz", "Разность натяжения", "Tension Difference"};
const char      *LABEL_TRANSFUSE[3]                     = {"Übertragen", "Перенесение", "Transmit"};
const char      *LABEL_TRANSFUSEDESTINATION[3]          = {"Übertragungsziel", "Цель переноса", "Transmission Destination"};
const char      *LABEL_TWO_EXT_EQUIPMENT_ERRORSIGNALS[3]= {"Doppelfehlerüberwachung externer Geräte", "Двойной контроль ожибок наружих приборов", "Double error monitoring for external equipment"};
const char      *LABEL_TWO_SENSOR_GEARSHIFT_CTRL[3]     = {"2-Sensoren-Schalthebelüberwachung", "2-x сенсорный контроль переключающего рычага", "2 sensors gearshift control"};
const char      *LABEL_UNCOILED_CABLE_LENGHT_IN_METER[3]= {"Herausgezogene Kabellänge in Metern", "Вытягиванная длина кабеля в метрах", "Uncoiled cable length in meter"};
const char      *LABEL_UNLOADCABLE[3]                	= {"Kabel Entladen", "Разгрузка кабеля", "Unload cable"};
const char      *LABEL_UPWARD[3]                        = {"aufwärts", "вверх", "up"};
const char      *LABEL_VELOCITY[3]                      = {"Geschwindigkeit", "Скорость", "Velocity"};
const wchar_t   *LABEL_VELOCITY_DIMENSIONUNIT[3]        = {L"Maßeinheit der Geschwindigkeit", L"Единица измерения скорости", L"Dimension unit of Velocity"};
const char      *LABEL_VIEW[3]                          = {"Ansicht", "Вид", "View"};
const char      *LABEL_VOLT_STEP_NORMAL[3]              = {"dU für Regelung v (normaler Schritt)","dU для регулирования v (нормальный шаг)","dV for regulation of v (normal step)"};
const char      *LABEL_VOLT_STEP_SMALL[3]               = {"dU für Regelung v (kleiner Schritt)","dU для регулирования v (маленький шаг)","dV for regulation of v (small step)"};
const char      *LABEL_WELL[3]                      	= {"Bohrloch", "Скважина", "Well"};
const char      *LABEL_WINCH[3]                         = {"Winde", "Лебедка", "Winch"};
const char      *LABEL_WINCHBRAKE_CONTROLED_BY_SW[3]	= {"Software-gesteuerte Windenbremse", "Тормоз барабана управляемый программным обеспечением ", "Winch brake controlled by software"};
const char      *LABEL_WINCHMACHINERY[3]                = {"Windenanlage", "Лебедочная установка", "Winch machinery"};
const char      *LABEL_YES[3]                           = {"Ja", "Да", "Yes"};
const char      *LABEL_ZEROPOINT[3]                     = {"Nullpunkt", "нулевая точка", "Zero Point"};

// Programm-Modi
const char      *PROG_MODE_0[3]                         = {"Undefiniert", "Неопределенный", "Undefined"};
const char      *PROG_MODE_1[3]                         = {"Programm-Start", "Старт программы", "Start program"};
const char      *PROG_MODE_2[3]                         = {"Programm-Ende", "Стоп программы", "End program"};
const char      *PROG_MODE_3[3]                         = {"Manueller Betrieb", "Ручное управление", "Manually Mode"};
const char      *PROG_MODE_4[3]                         = {"Automatischer Betrieb", "Автоматич. управл.", "Automatic Mode"};
const char      *PROG_MODE_5[3]                         = {"Kalibrieren", "Калибровка", "Calibration"};     // allgemein Kalibrieren
const char      *PROG_MODE_6[3]                         = {"Kalibrieren", "Калибровка", "Calibration"};     // Teufenrechner
const char      *PROG_MODE_7[3]                         = {"Kalibrieren", "Калибровка", "Calibration"};     // Zugkraftmesser
const char      *PROG_MODE_8[3]                         = {"Kabel entladen", "Разгрузка кабеля", "Unload cable"};
const char      *PROG_MODE_9[3]                         = {"Freilauf an", "Свободный ход", "Freewheel on"};

// Fehlermeldungen
const char      *ERRMSG_AUTOMOTIVEERROR[3]              = {"Fahrzeug-Fehler", "Ошибка автомобиля", "Automotive Error"};
const char      *ERRMSG_CONTROLLERERROR[3]              = {"Steuergerät Fehler", "Ошибка контроллера", "Controller Error"};
const char      *ERRMSG_COPY_LOG_FILE_FAIL[3]           = {"Kopieren der LOG-Datei(en) fehlgeschlagen", "Копия LOG-файл(ов) не удался", "Copy of LOG-File(s) fails"};
const char      *ERRMSG_COULDNT_READ_PARAM[3]           = {"Fehler beim Auslesen der Parameterdatei", "Ошибка при считывании файла параметров", "Error by reading the parameter file"};
const char      *ERRMSG_COULDNT_WRITE_PARAM[3]          = {"Fehler beim Schreiben in die Parameterdatei", "Ошибка при записи в файл параметров", "Error by writing in parameter file"};
const char      *ERRMSG_COULDNT_WRITE_SPECIAL_PARAM[3]  = {"Fehler beim Schreiben in die spezielle Parameterdatei", "Ошибка при записи в специальный файл параметров", "Error by writing in special parameter file"};
const wchar_t	*ERRMSG_CTL_CAN_INTERFACE_COM_ERROR[3]  = {L"Steuergeräte-CAN: Kommunikationsfehler", L"CAN контроллера: Ошибка обмена данных", L"Controller CAN: Communication error"};
const wchar_t	*ERRMSG_CTL_CAN_INTERFACE_INIT_FAILURE[3] = {L"Steuergeräte-CAN: Schnittstelle konnte nicht initialisiert werden", L"CAN контроллера: Интерфайс не подготавливается", L"Controller CAN: Could not initialize interface"};
const wchar_t	*ERRMSG_CTL_CAN_LIBNOTFOUND[3]			= {L"Steuergeräte-CAN: Bibliothek nicht gefunden", L"CAN контроллера: Библиотека не доступный", L"Controller CAN: Library not found"};
const char      *ERRMSG_DAC_CARD_NO_MEMORY[3]           = {"Speicherreservierung für DAC-Karte fehlgeschlagen", "Резервирование памяти для DAC-платы не удалось", "Memory allocation for DAC-Card fails"};
const char      *ERRMSG_DAC_CARD_NOT_FOUND[3]           = {"Keine passende DAC-Karte gefunden", "Нет согласованной DAC-платы", "No compatibly DAC-Card found"};
const char      *ERRMSG_DELETE_LOG_FILE_FAIL[3]         = {"Löschen der LOG-Datei(en) fehlgeschlagen", "Стирание LOG-файл(ов) не удался", "Deleting LOG-File(s) fails"};
const char      *ERRMSG_DEPTHCALC_PORT_OPEN_FAILURE[3]  = {"Schnittstelle zum Teufenrechner konnte nicht geöffnet werden", "Интерфейс на счетчик глубины не открывается", "Interface to depth computer could not be opened"};
const char      *ERRMSG_DEPTHCALC_COMM_FAILURE[3]       = {"Fehler bei Kommunikation mit Teufenrechner", "Ошибка при коммуникации с счетчиком глубины", "Error by communication with depth computer"};
const char      *ERRMSG_DEPTHRANGE_EXCEEDED[3]          = {"Außerhalb des angegebenen Teufenbereichs", "Снаружи заданного диапазона глубины", "Beyond declared depth range"};
const char      *ERRMSG_DIO_RELAY_CARD_NO_MEMORY[3]     = {"Speicherreservierung für DIO-Relay-Karte fehlgeschlagen", "Резервирование памяти для DIO-Relay-платы не удалось", "Memory allocation for DIO-Relay-Card fails"};
const char      *ERRMSG_DIO_RELAY_CARD_NOT_FOUND[3]     = {"Keine passende DIO-Relais-Karte gefunden", "Нет согласованной DIO-Relay-платы", "No compatibly DIO-Relay-Card found"};
const char      *ERRMSG_ERROR_SIGNAL_1_FROM_EXT_EQUIPMENT[3] = {"Fehlersignal 1 von externem Gerät", "Сигнал несправности 1 от наружного прибора", "Error signal 1 from external equipment"};
const char      *ERRMSG_ERROR_SIGNAL_2_FROM_EXT_EQUIPMENT[3] = {"Fehlersignal 2 von externem Gerät", "Сигнал несправности 2 от наружного прибора", "Error signal 2 from external equipment"};
const char      *ERRMSG_GEAR_SHIFT_FAIL[3]              = {"Getriebeumschaltung fehlgeschlagen", "Переключение редуктора передач не удалось", "Gear shifting failure"};
const char      *ERRMSG_INC_ENC_ERROR[3]                = {"Inkremental-Geber ausgefallen", "Датчик ИГР вышел из строя", "Incremental Encoder cancelled"};
const char      *ERRMSG_INFOOUT_PORT_OPEN_FAILURE[3]    = {"Schnittstelle zur Informationsausgabe konnte nicht geöffnet werden", "Интерфейс к выводу информации не открывается", "Interface for information output could not be opened"};
const wchar_t   *ERRMSG_LEVER_FAULTY[3]         		= {L"Bedienhebel Winde gestört", L"Ручка управления лебёдкой повреждённа", L"Winch control lever faulty"};
const char      *ERRMSG_LOG_DISKSPACE_FAIL[3]           = {"Zu wenig Speicherplatz für Protokollierung", "Слишком мало память для протоколирования", "Disk space to small for logging"};
const char      *ERRMSG_LOG_DATA_FAIL[3]                = {"Fehler beim Schreiben der LOG-Daten", "Ошибка при записи LOG-файлов", "Error during writing LOG-data"};
const char      *ERRMSG_LOG_INTERNAL_DISKSPACE_FAIL[3]  = {"Zu wenig Speicherplatz für interne Protokollierung", "Слишком мало память для внутреннего протоколирования", "Disk space to small for internal logging"};
const char      *ERRMSG_MAXTENSION_EXCEEDED[3]          = {"Max. Zugkraft überschritten", "Превышение максимального натяжения", "Max. Tension exceeded"};
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
const char      *ERRMSG_POWERFAIL[3]                    = {"Stromversorgung unterbrochen, System wird in Kürze heruntergefahren", "Электроснабжение прерванно, система вскоре прекратит работу", "Power fail, system shutdown briefly"};
const char      *ERRMSG_PROGRAM_INSTANCE_EXISTS[3]      = {"WinchControl scheint bereits aktiv zu sein.", "Управление лебедкой возможно в активном режиме", "WinchControl already seems to be active."};
const char      *ERRMSG_RIG_UNSECURED[3]                = {"Mast ungesichert","Мачта не стопорная","Rig unsecured"};
const wchar_t   *ERRMSG_SPOOLER_HOR_LEVER_FAULTY[3] 	= {L"Spooler horizontal - Bedienhebel gestört", L"Кабелеукладчик горизонтальный - Ручка управления повреждённа", L"Spooler horizontal - Control lever faulty"};
const char      *ERRMSG_START_FAILURE[3]                = {"Ressourcen-Fehler!!!\nProgramm-Start abgebrochen.", "Ошибка ресурса!!!\nСтарт программы аварийно завершен", "Ressource Error!\nStartup aborted."};

// Texte/Erläuterungen
const char      *EXPLAN_CALIBDEPTH[3]                   = {"Eine möglichst große Kabellänge muss abgewickelt, gemessen und angegeben werden.",
														   "Вымотать, измерять и задавать по возможности большую длину кабеля.",
                                                           "The possible amount of cable must be uncoiled, be measured and be entered."};
const char      *EXPLAN_CALIBTENSIONCOMPLETE[3]         = {"Es müssen zwei Zugkraftmessungen mit unterschiedlicher Last\ndurchgeführt werden und die bei der jeweiligen Messung\ntatsächlich vorhandene Zugkraft angegeben werden.",
														   "Провести 2 измерения натяжения с разной нагрузкой\n и задать действительное натяжение во время измерения.",
														   "Two tension measurements with different loads must be carried out\nand the tension with the respective measurement available has to be entered."};
const char      *EXPLAN_CALIBTENSIONZERO[3]             = {"Vor der Nullpunktermittlung muss der Zugkraftmesser vollständig\nentlastet werden.",
														   "Перед измеренией нулевой точки должен быть разгружен счетчик натяжения.",
														   "Before acquisition of zero point the dynamometer must be relieved completely."};
const char      *EXPLAN_DELETELOGFILES[3]                = {"Wollen Sie die markierten LOG-Dateien\nwirklich löschen ?",
                                                            "Удалить маркированые файлы ?",
                                                            "Do you really want to delete\nthe marked LOG-files ?"};
const char      *EXPLAN_LOSTMOTIONONQUEST[3]            = {"Soll der Freilauf wirklich eingeschaltet werden?",
                                                           "Действительно включить свободный ход?",
                                                           "Should the freewheel really be switched on?"};
const char      *EXPLAN_LOSTMOTIONOFFQUEST[3]           = {"Soll der Freilauf wirklich ausgeschaltet werden?",
                                                           "Действительно выключить свободный ход?",
                                                           "Should the freewheel really be switched off?"};
const char      *EXPLAN_SYSTEM_RESTART[3]               = {"Soll das System wirklich neu gestartet werden?",
                                                           "Действительно проводить перезагрузку системы?",
                                                           "Should the system really be restarted?"};
const char      *EXPLAN_SYSTEM_TERMINATE[3]             = {"Soll das Programm wirklich beendet werden?",
                                                           "Действительно заканчивать программу?",
                                                           "Should the application really be closed?"};
const char      *EXPLAN_SYSTEM_TURNOFF[3]               = {"Soll das System wirklich heruntergefahren werden?",
                                                           "Действительно переходить на более низкие\nрежимные параметры?",
                                                           "Should the system really be shut down?"};              
#endif //__LABELLING_H

# TumblerHL
24.03 Initial Revision

In  \TumblerPrj ist die aktuelle Version von  libRtEnvHL  als compilierbares
ESP-IDF  Projekt

idf.py set-target esp32            idf.py build

Im  \App  Verzeichniss sind verschiedene App's wie z.B.
RpmTest.cpp  KalmTest.cpp .........

In jedem \App Unterverzeichniss befindet sich das Haupprogramm z.B KalmTest.cpp
und dazu passende SvVisConfig.h und CmdList.txt

Zu den meisten App's gibt es Erklärungs ( Schulungs ) Videos
https://drive.google.com/drive/folders/169c7sWOlF6tPdowq96Qbc9-Uq1vyFpqL?usp=sharing

Next Steps:
Bei der derzeitige Drehzal-Messung mit nur einem Quadratur-Kanal
wird das Vorzeichen manchmal falsch erkannt, weshalb der SegWay dann umfällt.
Ich werde die Drehzal-Messung auf 2-Kanäle erweitern, dann sollte der SegWay
absolut stabil werden.

25.03: Neues Video CodeFolding.avi  
26.03: Aktuelle ESP32 Pin-Belegung in \doc mit Quadratur-Encoder  
26.03 added 2-Chan Encoder SW  LibVers: V1.7  
27.03 SegWay update auf 2-Chan Encoder  
27.03 Neues Video Using_SvVis.avi





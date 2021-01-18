# GUIDE-Walk v2.0
Alle Dateien, die für das Projekt "GUIDE-Walk v2.0: Autonomes Blindenführersystem mit KI" entstanden sind, sind hier organisiert. Erstellt für den [Regionalwettbewerb Jugend forscht 2021](https://www.jugend-forscht.de/).

### Allgemeines

1. Der Quellcode wird auf fremden Rechnern vielleicht kompilierbar, aber nicht ausführbar sein, da es auf spezifische Hardware (Sensoren) zugreift. Für Demo-Videos und Visualisierung der Funktionsweise siehe ```/demo_video```.
2. Der Quellcode dient lediglich zu Zwecken der Veranschaulichung und Nachvollziehbarkeit. Das Verändern oder Weiterverbreiten des Codes ist untersagt. Diese Bedingungen gelten zusätzlich zur angegebenen Lizenz.

### Legende

Die Dateien sind in folgenden Ordnern organisiert:

- ```/src```: C++-Quellcode mit Header-Dateien; Dateien mit fremden Code sind eindeutig gekennzeichnet, andernfalls kann der Code als Eigenarbeit betrachtet werden.
- ```/model```: Modell-Konfiguration (.prototxt) mit Gewichten (.caffemodel)
- ```/audio```: Voicelines für die Sprachausgabe
- ```/demo_video```: Beispielvideos; Rohe und ausgewertete Testaufnahmen der internen Kamera
- ```/python```: Python-Code zum Bearbeiten und Organisieren des Datasets

### Downloads

Verwendete Datasets:

Dataset|Download|Download
:---:|:---:|:---:
[MS COCO](https://cocodataset.org/#home) und [PascalVoc](host.robots.ox.ac.uk/pascal/VOC/)|[Images](https://drive.google.com/file/d/1CwUXG9AGKLxLxmITtofHA1-ptPUzoFeU/view?usp=sharing)|[Labels](https://drive.google.com/file/d/1Q_okseGIdyyqTtAx7CrgtfiZllCY8zM8/view?usp=sharing)
[Ampelpilot](https://github.com/patVlnta/Ampel-Pilot)|[Images](https://drive.google.com/file/d/10geTRHlrE-jTRdDPCw0VNrQEjTe_GV7u/view?usp=sharing)|[Labels](https://drive.google.com/file/d/10RGl02U1T-5gYq2nO2-GiREz-TjRK9_p/view?usp=sharing)
Eigene Aufnahmen|[Images](https://drive.google.com/file/d/1IRKaGkQdRIb97vfSxGQV2koyMVyhsk9F/view?usp=sharing)|[Labels](https://drive.google.com/file/d/1a30W06y9Maws8e8dqi4THVXOFuic8dGU/view?usp=sharing)
Eigene Aufnahmen (Test)|[Images](https://drive.google.com/file/d/1gJ_HtiwCqTB-t3HbrtVZn6DOOiu1gB9I/view?usp=sharing)|[Labels](https://drive.google.com/file/d/12a9DQeHYzYznV0H8E5NWmcaciklSdrFI/view?usp=sharing)

### Danksagungen

Vielen Dank an all diejenigen, die mich bei der Planung und Verwirklichung des Projekts unterstützten und weiterhalfen! Diese sind insbesondere:

*Mama und Papa, René Grünbauer, Christine Lohse, Rainer Schliermann, Rudolf Pichlmeier, Ulrike Weimer, Moritz Walker, Christoph Högl, Yannick Rittner, Paul Kutzer*

@simondlevy for [CrossPlatformDataBus](https://github.com/simondlevy/CrossPlatformDataBus) library and [USFS](https://github.com/simondlevy/USFS) library

@garmin for [LIDARLite_RaspberryPi_Library](https://github.com/garmin/LIDARLite_RaspberryPi_Library)

## Mit freundlicher Unterstützung von:
<a href="https://www.bbsb.org"><img src="https://bbsb.org/wp-content/uploads/2020/01/BBSB_Logo_100Jahre_rgb.png" width=200></a> <a href="https://www.blindeninstitut.de/de/regensburg/rund-ums-institut/begruessung/"><img src="https://upload.wikimedia.org/wikipedia/commons/d/d4/Blindeninstitutsstiftung_Logo.JPG" width=200></a> <a href="https://www.oth-regensburg.de"><img src="https://upload.wikimedia.org/wikipedia/commons/c/c2/Logo-OTH-Regensburg.png" width=300></a> <a href="https://www.domspatzen.de/"><img src="https://upload.wikimedia.org/wikipedia/de/thumb/4/44/Regensburger_Domspatzen_Logo_blau.svg/1200px-Regensburger_Domspatzen_Logo_blau.svg.png" width=130></a> <a href="http://www.sponsorpool-bayern.de/"><img src="http://www.sponsorpool-bayern.de/images/content/foerder-sticker-druckvorlage-4bb8c8af.png" width=200></a> <a href="http://www.edisys.hu/de/"><img src="http://www.edisys.hu/images/logoproba.png" width=200></a>

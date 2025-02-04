# gaLib Version of core_war




Genetic Algorithm zur Erstellung eines Warriors für das Spiel "Core War".
Initialisiert wird mit einem Warrior mit zufälligen Commands.
In jeder Generation tritt dann der Warrior gegen 570 Warrior aus folgendem Datensatz an:
https://github.com/n1LS/redcode-warriors


# Installation

Das Projekt verwendet CMake und wurde auf Ubuntu entwickelt.
Im File *CMakeLists.txt* müssen die Pfade zur Galib-Library (Headerordner und Library) und PMARS (Ausführungsdatei) bei Bedarf gesetzt werden.
Die aktuelle Einstellung geht von einer parallelen Installation aus, in der GaLib mit CMake und PMARS mit Make gebaut wurden.

# Ergebnis

Unser trainierter Warrior *warrior_1000gen_extopponents.red* ist in diesem Ordner zu finden.
Es wurden folgende Fitnessscores während dem Training erreicht:

	Generation: 50, Best fitness: 2710
	Generation: 100, Best fitness: 2712
	Generation: 150, Best fitness: 2714
	Generation: 200, Best fitness: 2714
	Generation: 250, Best fitness: 2743
	Generation: 300, Best fitness: 2743
	Generation: 350, Best fitness: 2743
	Generation: 400, Best fitness: 2760
	Generation: 450, Best fitness: 2796
	Generation: 500, Best fitness: 2799
	Generation: 550, Best fitness: 2799
	Generation: 600, Best fitness: 2807
	Generation: 650, Best fitness: 2807
	Generation: 700, Best fitness: 2816
	Generation: 750, Best fitness: 2816
	Generation: 800, Best fitness: 2857
	Generation: 850, Best fitness: 2857
	Generation: 900, Best fitness: 2857
	Generation: 950, Best fitness: 2857
	Generation: 1000, Best fitness: 2857
	
Abschließend wurde der Warrior wieder nocheinmal gegen die Gegnern mit folgenden PMARS Parametern getestet:
  * -F 4000 
  * -r 3 
  * -b

**C++ Skript:** *testwarrior.cpp*

Bei diesem Test konnte er 3108 Punkte erreichen, die Gegner in Summe 1701.

## Analyse und Verbesserungsmöglichkeiten

Da wir nur einen Durchlauf durchgeführt haben, der erzeugte Warriorcode nicht alle "Kommandos" die möglich wären enthält und der Code generell sehr einfach aufgebaut ist (keine BErücksichtigung von komplexeren Strukturen,...) ist das Ergebnis in unseren Augen unter diesen Voraussetzungen sehr zufriedenstellend.  

Verbesserungsmöglichkeiten sehen wir bei einer Anpassung dieser drei genannten Punkten.

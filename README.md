Dokumentacja projektu z grafiki komputerowej
==========

Zespół
-----
- Aleksander Trafimczyk
- Marcin Marlewski
- Paweł Łączkowski
- Piotr Szymkowiak
- Wiktor Leszczyński

Wybrane technologie
-----
- parallel transport frames ( smok )
- fire rendering ( pochodnie )

Modele
-----
- drzwi (gotowy model, tekstury z Poly Haven)
- kolumna (model własny wykonany w Blenderze, tekstury z Poly Haven)
- pochodnie (gotowy model, przerobiony w Blenderze, tesktury z Poly Haven)
- lampy (gotowy model i tekstury)
- włączniki lamp (gotowy model, tekstury z Poly Haven)
- segmentowany smok lata po całym pokoju (gotowy model i tekstury, przerobiony w Blenderze)
- pokój (model własny wykonany w Blenderze, tekstury z Poly Haven)
- kusza (gotowy model, przerobiony w Blenderze, tekstury z Poly Haven)
- strzała (gotowy model i tekstury)

Interakcje
-----
- strzelanie z kuszy strzałami ( strzały to obiekty )
- strzały znikają po trafieniu przeszkody i oddziałuje na nie grawitacja
- trafienie strzałą w pochodnię niszczy ją ( pochodnia znika )
- zapalanie i gaszenie lamp strzelając strzałami we włączniki lamp
- trafienie strzałą w specjalny segment smoka zabija go ( smok znika )

Estetyka
-----
- kusza widoczna przed kamerą jak w FPS
- pokój ciemny by zmienianie oświetlenia dawało widoczne efekty
- przedmioty z możliwością interakcji widoczne różnią się na tle reszty
- smok ma specjalny, wyróżniony segment
- smok owija się wokół kolumny
- wariacje ścieżki smoka za każdym uruchomieniem
- dać model z teksturą metaliczności
- ( nie ma skyboxa )

Narzędzia
-----
- Visual Studio
- Blender

Fire rendering
-----
Fire rendering zostal stworzony przez system generowania particles. Particles są pokolorawane w colory ognia. Mają swój czas życia po którym umierają i tworzą się nowe. Są generowane wokół koordynat pochodni, gdzie musi być ogień. Można w kodzie zobaczyć jak tworzona gęstość, liczba tworzonych particles, a także liczba tworzonych nowych particles, która wpływa na częstotliwość migotania ognia.

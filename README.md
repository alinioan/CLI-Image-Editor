# Tema #3 PCLP - Editor de Imagini

Nume: Alexandru Alin-Ioan

Seria: CA

Grupa: 312CA

## Descrierea temei

Gigel isi poate lansa in mod oficial cariera de influencer, si asta doar cu
ajutorul editorului de imagini realizat in acest proiect.

Acest editor poate sa faca operatii simple pe imagini in format .ppm sau .pgm.

## Rulare

### Comenzile programului

LOAD [fisier] - incarca fisierul in memorie.

SELECT [x1] [y1] [x2] [y2] - selecteaza zona delimitata de punctele (x1, y1),
(x2, y2).

SELECT ALL - selecteaza intreaga imagine.

CROP - taie imaginea la selectie.

ROTATE [unghi] - roteste imaginea cu [unghi] grade (doar 0, 90, 180, 270, 360,
-90, -180, -270, -360).

HISTOGRAM [nr_stele] [nr_binuri] - Realizeaza histograma imaginii (doar pentru
imagini greyscale)

EQUALIZE - Egalizeaza imaginea (doar pentru imagini greyscale)

APPLY [filtru] - Aplica filtrul pe selectie. [filtru] poate fi EDGE, SHARPEN,
BLUR sau GAUSSIAN_BLUR  

SAVE [nume] {ascii} - Salveaza imaginea. Daca apare parametrul ascii atunci se
va salva in formt ascii

EXIT - opreste programul

## Structura proiectului

### Makefile

* Contine regulile build si clean

### image_lib.h

* Contine declararea structurilor folosite pentru realizarea tipului de date
imgaine.
* Contine declararea celor mai importante functii folosite.

### image_editor.c

* Main-ul proiectului.
* Contine parserul de comenzi.
	* Se citeste initial intreaga linie, dupa care se separa cuvintele, primul
cuvant fiind cel dupa care se determina comanda, iar restul fiind parametrii.

### load.c

* Realizeaza operatia de incarcare a unui fisier in memorie
* Primul pas este sa extraga metadatele (magic number-ul, latimea si inaltimea
si valoarea maxima a pixelilor)
* Se aloca memorie pentru matricea ce retine valorile pixelilor
* Se citeste aceasta matrice (datele)

### free_memory.c

* Contine functiile de eliberare a memoriei pentru imagine si pentru matricea
cu datele acesteia.

### save.c

* Realizeaza operatia de salvare a unui fisier
* Pentru metadate se afiseaza in fisier un string format prin concatenarea
valorilor.
* Din cauza modului in care este facuta alocarea scrierea in binar a matricei se
poate face doar linie cu linie.

### actions.c

* Contine functiile pentru restul comenzilor
* SELECT ALL este realizat in aceiasi functie ca si SELECT, ALL fiind tratat
ca un argumnet al comenzii SELECT
* CROP
	* Se aloca memorie pentru matrice cu dimensiunile selectiei
	* Se copiaza selectia din matricea initiala tinand cont ca indicii nu sunt
aceiasi pentru ambele matrice.
* ROTATE
	* Sunt doar 3 cazuri (teoretic 4, dar la 360 nu se intampla nimic)
	* Pentru 90 si 270 se aloca memorie pentru o noua matrice cu inaltimea si
latimea inversate si apoi se copiaza elementele unul cate unul tinand cont de
indici.
	* Pentru 180 se inverseaza direct valorile in matricea cu date
* HISTOGRAM
	* Se calculeaza frecventa pixelilor.
	* Se realizeaza un alt sir ce contine valorile bin-urilor.
	* Se foloseste formula pentru a determina numarul de stelute.
* EQUALIZE
	* Se calculeaza frecventa pixelilor si se mai calculeaza un sir cu sumele
partiale ale acestora.
	* Se aplica formula data pentru a calcula noua valoare.
* APPLY
	* Se declara matricele pentru kernel-uri.
	* Se calculeaza zona pentru care sa aplica filtrul (trebuie sa nu contina
pixeli de pe margine).
	* Se aloca memorie pentru o noua matrice unde se calculeaza noile valori.
	* Se copiaza rezultatul in zona de apply.

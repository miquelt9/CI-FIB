READ ME

Bàsicament el programa s'estructura de la següent manera:
Al inici es fa tota la configuració necessaria: glcd, botons, consola, pwm, adc,...
En el bucle principal es comença llegint (per botons o per terminal) els moviments.
Segons el moviment, es dibuixa la nau en el seu lloc, es mou el combustible, si es pertinent.
En el cas que haguem deixat el combustible, actualitzem la barra de nivell i en generem un altre a un lloc aleatori.
Després comprovem si ens ha tocat un enemic, i restem una vida en el cas afirmatiu. 
Movem els enemics (en el cas que no s'estigui en el nivell 0), i actualitzem el cronòmetre i el nivell en el cas que es modifiqui el potenciòmetre.

Alguns dels problemes que hem tingut amb les seves solucions són:
En un inici, quan la nau o els enemics passaven per sobre de la plataforma o del dipòsit, s'esborraven. Ho hem solucionat fent que el dipòsit i la plataforma es vagin repintant a cada segon.
A la hora de fer desaparèixer els enemics quan es canviava de nivell a mitja partida hem tingut bastants problemes. Hem fet que cada vegada que es canvii de nivell, els enemics que desapareixin s'esborrin i es moguin a una posició fora de la pantalla per evitar que la nau colisioni amb enemics invisibles.
Un dels problemes que ens ha donat més mals de caps ha sigut fer moviments llegint de la consola. Cada vegada que movíem la nau, s'ens esborrava. La única manera que hem trobat per poder arreglar-ho ha sigut limitant el nombre d'interrupcions. Hem fet que només es pugui donar un sol moviment per iteració del bucle principal mentres no es faci cap instrucció de dibuixat. 
A més a més, es dona que si rebem moltes interrupcions per teclat, el programa es col·lapsa, és per això que recomenem utilitzar els botons o bé no enviar moltes iterrupcions per teclat de forma continuada.

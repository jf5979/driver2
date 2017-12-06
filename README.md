Laboratoire 2 par Jean-Francois Penven, Vincent Gosselin, 2017.

Usb driver in linux for a Camera.

Pour lancer l'application, naviguer dans le dossier src et lancer le script test.sh



Au moment de la remise, l'etat de la situation est :

-Capable de prendre une photo et de l'enregistrer en format jpeg.
	La deuxieme photo consecutive fait crasher le pilote cependant 
	car le callback plante avec le code d'erreur -71 soit Protocol error

-Capable de bouger la camera sur 2 axes a l'aide des touches WASD

-Capable de reinitialiser la position de la camera

-Capable d'aller chercher la valeur dans les registres disponibles sur la camera

-Capable d'aller ecrire dans un registre. Cependant pour une raison inconnue, c'est la valeur maximum
	qui est modifiee.

-Bien sur capable de detecter la connection/deconnection d'une camera.


Note :

Bien que cela fut exiger, le pilote creer un char driver avant la connection de la camera.
Pour cette raison, il est essentiel de brancher la camera avant de lancer le programme de test.
Sinon cela entrainera l'echec du pilote lorsqu'il tentera d'acceder a une interface inexistante.

Ce defaut fut regler dans un commit, cepandant, la stabilite du pilote etait moindre (il plantait une fois sur 2)
Cest pour cela que le commit fut ignorer dans la remise.


Pour voir le code, il est disponible sur github user jf5979 projet driver2





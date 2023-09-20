.. _lancement:

Lancement
*********

Depuis un terminal de son poste de travail, Magix3D se lance comme
suit :

Magix3D [arguments]

où arguments sont des *arguments* optionnels de Magix3D parmi les
suivants :

-  **--std** : lancement de la version standard,

-  **--new** : lancement de la dernière version,

-  **--old** : lancement de l’ancienne version standard,

-  **-userConfig** *fichier* : utilise *fichier* comme fichier de
   configuration personnelle (voir :ref:`preferences`), en
   remplacement du *~/.magix3d* par défaut,

-  **-scripts** *fichiers python* : exécute les fichiers python transmis
   en arguments au lancement de l'application.

Des arguments supplémentaires sont dédiés au mode client/serveur :

-  **-graphicalWindowWidth** *largeur* : affecte *largeur* à la largeur
   de la fenêtre graphique

-  **-graphicalWindowHeight** *hauteur* : affecte *hauteur* à la hauteur
   de la fenêtre graphique,

-  **-graphicalWindowFixedSize** : la fenêtre graphique ne peut pas être
   redimensionnée,

-  **-stillFrameRate** *temps* : nombre d'images/seconde hors
   interactions,

-  **-desiredFrameRate** *temps* : nombre d'images/seconde durant les
   interactions,

-  **-timeout** *temps* : timeout (connexion/appel distant), en
   millisecondes, des communications client/serveur,

-  **-serverHostName** *nom* : utilise pour serveur celui sur la machine
   *nom* ou d'adresse TCP/IP *nom*,

-  **-connectionPort** *port* : se connecte au port *port* du serveur,

-  **-listeningPort** *port* : option serveur, écoute sur le port
   *port*,

-  **-csPolicy** *mode* : mode de fonctionnement client/serveur utilisé.

Modes possibles :

-  **station** : non en client/serveur,

-  **noLimit** : serveur sur un nœud fortement doté en mémoire du
   calculateur,

-  **parallel** : utilisation d'un ou plusieurs nœuds du calculateur,

-  **-csRendering** *mode* : mode de fonctionnement des affichages 3D.

-  Modes possibles :

-  **station** : les données affichées sont rapatriées sur la station de
   travail,

-  **remote** : la vue 3D est calculée sur le serveur, et l'image est
   rapatriée sur la station de travail

-  **-imageCompressionAlgorithm** *algo* : algorithme de compression
   d'images utilisé pour transférer des images du serveur vers le client
   lorsque la vue 3D est calculée sur le serveur.

   Algorithmes disponibles :

   1. **none** : absence de compression,

   2. **zlib** : compression avec la zlib,

-  **-imageCompressionAlgorithmParameters** *parameters* : éventuels
   paramètres pour l'algorithme de compression défini par l'option
   -imageCompressionAlgorithm.

   Point commun : le premier entier définit le niveau de compression (0 par
   défaut).

.. note::
   Spécificité zlib : niveau de compression allant de 0 (absence de
   compression) à 9 (meilleure compression) en passant par 1 (meilleure
   rapidité).

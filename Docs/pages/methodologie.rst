.. toctree::
   :titlesonly:

.. _methodologie:

Méthodologie
============

Magix3D permet la création de maillage en suivant plusieurs méthodes de
construction.

Nous ne détaillons pas ici comment effectuer un tel maillage, mais nous
vous guidons dans le choix de la méthode la plus adaptée à votre
problématique. Pour comprendre comment réaliser de tels maillages, nous
vous renvoyons vers les :ref:`tutoriels<tuto-2BoitesConformes>`

Création par assemblage d’éléments simples
------------------------------------------

Cette méthode d’assemblage d’éléments simple est adaptée aux cas
d’études demandant des maillages de :

-  parallélépipèdes

-  sphères (entières ou portions)

-  cylindres (entiers ou portions)


Dans ces cas, vous allez pouvoir utiliser les fonctions de création de
ces éléments avec la topologie associée (:ref:`creation-boite`, :ref:`creation-sphere`,
:ref:`creation-cylindre`, ...) et simplement
effectuer un collage grâce aux :ref:`operations-booleennes`.

Le tutorial :ref:`Maillage de 2 boites conformes <tuto-2BoitesConformes>` explique précisément comment faire.

Création d’un modèle 3D par révolution d’un cas 2D
--------------------------------------------------

Cette méthode de création du modèle et de la topologie associée permet
de créer très facilement un maillage bloc structuré 3D.

Vous aurez besoin d’utiliser :

-  La commande :ref:`importer`

-  La commande :ref:`decoupage-face-arete`

-  La commande :ref:`creation-bloc-revolution`

Le tutorial :ref:`Maillage par révolution d'un modèle 2D <tuto-revolution>` explique précisement comment faire.

Création d’un maillage à partir d’un modèle géométrique 3D
----------------------------------------------------------

Cette méthode de création de la modélisation dans un premier temps, puis
de la topologie dans un second temps, est la plus générale. Elle peut
également faire suite à une construction suivant les 2 précédentes
méthodes.

La modélisation peut s’obtenir par une des méthodes suivantes ou un
mélange d’entre elles :

-  par importation d’un modèle 2D suivi d’une :ref:`creation-par-revolution`,

-  par importation d’une modélisation 3D (pour laquelle il est
   recommandé de construire des volumes si ce n’est pas le cas, voir
   :ref:`creation-volume`),

-  à l’aide de la fonction :ref:`operations-booleennes` sur des objets élémentaires (sans
   topologie).

Dans un second temps, il faut créer la topologie associée à chacun des
volumes. Il est aussi bien possible de procéder par décomposition d’un
bloc structuré (approche d’*ICEM-CFD*) ou par agglomération de blocs.

Le tutorial  :ref:`Maillage de 2 cylindres creux <tuto-2CylindresCreux>` explique précisément comment faire.

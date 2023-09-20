.. _operations-reperes:

Opérations sur les repères
~~~~~~~~~~~~~~~~~~~~~~~~~~

Les repères sont vus comme des entités de Magix3D ce qui permet leur
utilisation dans certaines commandes ou simplement dans certains
panneaux.

.. _creation-repere:

Créer un repère |image222|
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

La création d’un repère peut se faire suivant 3 méthodes
mais tous les cas il est demandé de choisir un nom de groupe :

-  Sans paramètre particulier, on obtient le repère global.

-  Avec un point qui définit le centre, le repère est alors le même que
   le global à une translation près.

-  Avec 3 points, un centre et 2 autres points qui définissent la base
   pour le trièdre.

.. _translation-repere:

Réaliser une translation d’un repère |image223|
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Il faut renseigner le nom du repère à translater. Il faut
également renseigner le vecteur pour la translation. Ce vecteur peut
être initialisé avec une arête topologique, un segment ou plus
généralement avec 2 points. Il est possible optionnellement de faire la
translation sur une copie de la sélection, il faut alors renseigner le
nom du groupe dans lequel sera mis ce nouveau repère.

.. _rotation-repere:

Réaliser une rotation d’un repère |image224|
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Il faut renseigner les noms du repère à tourner. Il faut
également renseigner l’angle de rotation suivant des valeurs soit
prédéfinies (demi pour 180°) soit suivant un nombre de degrés souhaité.

L’axe de rotation est par défaut l’axe des X mais il est possible d’en
sélectionner un autre (axe des Y ou Z, voir même un axe défini par une
courbe ou par deux points).

Comme pour la translation, il est possible de faire une copie.

.. include:: substitution-images.rst

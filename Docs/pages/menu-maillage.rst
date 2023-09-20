.. _menu-maillage:

Menu Maillage
~~~~~~~~~~~~~

Contrairement à Magix [2D] où vous passiez d’un mode modélisation à un
mode maillage et inversement, avec Magix3D il est nécessaire d’exécuter
une commande pour obtenir un maillage. Il est donc préférable d’annuler
cette création du maillage si vous souhaitez faire évoluer la
modélisation ou la topologie.

.. _mailler-selection:

Mailler la sélection
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cette commande permet de mailler les blocs et les faces
topologiques sélectionnés.

.. _mailler-visible:

Mailler ce qui est visible
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cette commande permet de mailler les blocs et les faces
topologiques dont les groupes sont visibles (suivant l’activation des
groupes dans le gestionnaire d’entités).

.. _tout-mailler:

Tout mailler
^^^^^^^^^^^^^^^^^^^

Cette commande crée un maillage pour toutes les faces ou tous
les blocs.

Représentations/Déraffiner
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cette fonction ouvre une boite de dialogue dans laquelle vous pouvez
spécifier un facteur de déraffinement de la représentation du maillage.
Cette fonction n’a d’effet que sur les maillages structurés pour le
moment. Il est alors affiché une vue dégradée du maillage avec moins de
mailles (une maille sur le facteur sélectionné).

.. _exploration-feuillets:

Exploration de feuillets
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

L’activation de l’exploration de feuillets ouvre un panneau
dans un onglet du panneau Opérations (à côté de l’onglet Opération
courante).

|image28|

Il vous faut alors sélectionner une arête le long de laquelle
sera construit le feuillet.

Ce feuillet permet de voir une couche de mailles en suivant la
structuration orthogonalement à une arête, ce qui est très pratique pour
explorer l’intérieur d’un maillage. Remarque : l’affichage du feuillet
désactive automatiquement la représentation des volumes du maillage pour
permettre d’observer le feuillet.

|image29|

.. _evaluation-qualite:

Evaluation de la qualité du maillage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

L’activation de l’évaluation de la qualité du maillage ouvre
un panneau dans un onglet du panneau Opérations (à côté de l’onglet
Opération courante).

Cette évaluation se fait sur les groupes de mailles sélectionnés (voir
:ref:`selection`). Il est similaire à celui disponible dans Lem.

.. _info-maillage:

Informations sur le maillage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cette action permet l’ouverture d’un panneau qui donne les informations
suivantes sur le maillage :

-  Nombre de blocs maillés et nombre de bloc total ;

-  Nombre de mailles créées et nombre de mailles prévisibles ;

Comme il n’est pas possible de connaitre le nombre de mailles dans un
bloc non-structurés, ces blocs ne sont pas pris en compte dans la
prévision.

Ce panneau est réactualisé après chaque nouvelle commande.

.. include:: substitution-images.rst

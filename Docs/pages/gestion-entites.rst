.. _gestion-entites:

Gestionnaire d'entités
======================

.. _onglet-groupes:

Onglet Groupes
--------------

Cet onglet est le moyen le plus simple pour sélectionner les
entités à afficher dans le panneau de représentation.

En utilisant le menu contextuel (bouton droit de la souris), il permet
également de sélectionner ou de désélectionner certaines de ces entités
suivant le fait qu’elles soient à afficher. Il est aussi possible de
modifier la représentation d’un ensemble de types d’entités.

Cet onglet se décompose en 2 parties, en haut les types d’entités et en
bas les groupes rassemblés par dimension.

La partie « **Types d’entités** » permet d’activer ou non la
représentation des entités suivant leur type. Les secondes et troisièmes
bouton gauche controlant respectivement l'affichage en mode filaire et
en mode solide des entités correspondantes.

.. image:: ../images/groupes_raccourci_affichage.png
    :width: 400px

Ces boutons sont de type ON/OFF, les réglages fins des mode filaire
(rien/filaire/isofilaire/filaire + isofilaire) et solide 
(rien/surfacique) s'effectuant à l'aide de la boîte de dialogue
prévue à cet effet.
Cela signifie par exemple que dans la boite de dialogue de paramétrage 
des propriétés d'affichage, si la case à cocher "Surfaces" n'est pas 
cochée pour les volumes géométriques, activer le bouton "solide" des
volumes géométriques sera sans effet.

La partie « **Groupes** » permet de sélectionner les groupes (nom de
volume par exemple, nom de matériau) à afficher.

L'option **Propager l'affichage** lorsqu'elle est active entraine la
représentation des entités de dimension inférieure dès lors qu'elles
font partie d'un groupe à afficher.

Les Types d’entités
^^^^^^^^^^^^^^^^^^^

Il est non seulement possible d’activer la représentation d’un type
particulier d’entités, mais il est également possible de sélectionner
soit toutes les entités, soit celles visibles, pour un type donné. Il
est aussi possible d’agir sur la représentation des entités de ce type.

Les Groupes
^^^^^^^^^^^

Les groupes peuvent être des regroupements d’entités géométriques,
topologiques ou des sous-ensembles du maillage, mais il ne peut y avoir
de mélange de type dans un même matériau. Ils peuvent correspondre à une
pièce mécanique, à un matériau ou tout autre chose. Ils sont là pour
permettre d’identifier des ensembles d’entités au sens large et d’agir
sur leur représentation, leur sélection.

Il est prévu de séparer les groupes suivant différents niveaux pour
aider à s’y retrouver pour les cas avec beaucoup de groupes. Aussi le
menu contextuel permet d’activer la « vue multi-niveaux ». Il ensuite
possible de changer des groupes de niveaux. L’idée étant de mettre dans
un même niveau des groupes de signification (volumes élémentaires,
matériaux, regroupements de matériaux...). Vous avez une totale liberté
sur son utilisation....

.. _onglet-entites:

Onglet Entités
--------------

Cet onglet permet d'accéder aux entités suivant leur nom.

Les entités sont simplement réparties suivant leur type et leur
dimension.

Les entités peuvent être rendues visibles ou non, individuellement ou
par groupe.

Un menu contextuel permet la modification de la représentation de la
sélection.

.. include:: cea_onglet-pointages-laser.rst

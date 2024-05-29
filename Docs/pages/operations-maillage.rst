.. _operations-maillage:

Opérations sur les maillages
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Opérations associées aux nuages de maillage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sélectionner la sous-famille d’opérations associés aux nuages avec
|image213|

Actuellement, aucune opération.

Opérations associées aux surfaces de maillage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sélectionner la sous-famille d’opérations associés aux surfaces de
maillage avec |image214|

.. _mailler-surfaces:

Mailler des surfaces |image215|
'''''''''''''''''''''''''''''''

Ce panneau permet de sélectionner les faces à mailler ou de
toutes les mailler.

Pour cela soit vous cochez *Tout mailler*, soit vous renseignez les noms
des faces à mailler.

Voir :ref:`boutons-creation-maillage` pour un accès plus rapide.

.. _projeter-surfaces:

Projeter des surfaces sur un plan |image216|
''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de s’assurer que les nœuds d’une surface
seront bien sur un plan de construction (z=0 par exemple).

.. _lisser-surfaces:

Lisser des surfaces |image217|
''''''''''''''''''''''''''''''

Ce panneau permet d’affecter une méthode de lissage à un
ensemble de surfaces.

Pour cela sélectionner le groupe auquel appliquer le lissage.

Cela doit être fait avant le maillage.

Il est possible d’utiliser une méthode proposée par défaut ou de
sélectionner une des méthodes de lissages et de modifier les paramètres.

Le nombre d’itérations est par défaut de 10.

Les méthodes de lissage sont les suivantes :

-  Optimisation suivant la normale

-  Optimisation suivant l’orthogonalité des bras

-  Optimisation elliptique suivant l’orthogonalité des bras *[par
   défaut]*

Une explication sur ces diverses méthodes devrait voir le jour
prochainement.

Il est également proposé 2 solveurs :

-  Gradient conjugué *[par défaut]*

-  Newton

Actuellement, seule la méthode par défaut semble fonctionner
correctement. Les autres bloquent sur la recherche d’une normale en
bordure de surface.

Opérations associées aux volumes de maillage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sélectionner la sous-famille d’opérations associés aux volumes de
maillage avec |image218|

.. _mailler-blocs:

Mailler des blocs |image219|
''''''''''''''''''''''''''''

Ce panneau permet de sélectionner les blocs à mailler ou de
tous les mailler.

Pour cela soit vous cochez *Tout mailler*, soit vous renseignez les noms
des blocs à mailler.

Voir :ref:`boutons-creation-maillage` pour un accès plus rapide.

.. _lisser-volumes:

Lisser des volumes |image220|
'''''''''''''''''''''''''''''

Ce panneau permet d’affecter une méthode de lissage à un
ensemble de volumes.

Pour cela sélectionner le groupe auquel appliquer le lissage.

Il est possible d’utiliser une méthode proposée par défaut ou de
sélectionner une des méthodes de lissages et de modifier les paramètres.

Le nombre d’itérations est par défaut de 10.

Les méthodes de lissage sont les suivantes :

-  Laplace *[par défaut]*

-  Tipton

-  Jun

-  Condition number

-  Inverse mean ratio

Une explication sur ces diverses méthodes est à faire.

.. _extraire-volume-feuillets:

Extraire un sous-volume délimité par 2 feuillets |image221|
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet la création d’un volume à l’intérieur d’un
ensemble de blocs sans avoir à découper ces derniers. Il suffit de
sélectionner les blocs concernés, une arête et des indices de bras sur
cette arête pour obtenir la création d’un volume entre les deux
feuillets qui passent par cette arête aux indices choisis. Pour les
feuillets, voir :ref:`exploration-feuillets`

.. include:: substitution-images.rst
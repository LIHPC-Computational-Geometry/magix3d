.. _menu-topologie:

Menu Topologie
~~~~~~~~~~~~~~

.. _modifier-discretisation-aretes:

Modifier la discrétisation des arêtes par défaut
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cette commande permet de changer le nombre de bras par défaut pour les
nouvelles arêtes topologiques. La valeur initiale par défaut est de 10.

Raffiner la discrétisation des arêtes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cette commande permet de modifier la discrétisation en nombre de bras
pour toutes les arêtes existantes en appliquant un facteur de
raffinement.

Optimiser la méthode de maillage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cette commande remplace autant que possible la méthode Transfini par une
plus rapide du type directionnel dans toutes les faces et tous les blocs
existants. Elle est à utiliser une fois la topologie stabilisée et avant
de mailler.

.. _info-topologie:

Informations sur la topologie
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cette action permet l’ouverture d’un panneau qui donne les informations
suivantes sur la topologie :

-  Nombre de blocs

-  Nombre de blocs avec une méthode structurée suivant une direction

-  Nombre de blocs avec une méthode structurée suivant une rotation

-  Nombre de blocs avec la méthode structurée transfini (plus générale
   mais plus couteuse en temps)

-  Nombre de blocs avec une méthode non structurée

-  Nombre de mailles générées avec une méthode structurée suivant une
   direction

-  Nombre de mailles générées avec une méthode structurée suivant une
   rotation

-  Nombre de mailles générées avec la méthode structurée transfini

-  Nombre de mailles générées avec une méthode non structurée

-  Nombre de faces

-  Nombre de faces avec une méthode structurée suivant une direction

-  Nombre de faces avec une méthode structurée suivant une rotation

-  Nombre de faces avec la méthode structurée transfini

-  Nombre de faces avec une méthode non structurée

Ce panneau est réactualisé après chaque nouvelle commande.

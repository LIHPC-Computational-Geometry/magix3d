Tutoriel pour mieux comprendre la notion de groupes
###################################################

Cas test
********

Pour illustrer l’intérêt et l'utilisation des groupes, nous allons prendre comme exemple un cylindre et une plaque.

|PlaqueEtCylindre|

Pour les construire, vous pouvez rejouer les 2 commandes suivantes:

.. code-block:: python

  ctx.getTopoManager().newCylinderWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(2, 0, 0), 360, True, .5, 10, 10, 10, "CYL")
  ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(-.2, -2, -2), Mgx3D.Point(0, 2, 2), 10, 10, 10, "PLAQUE")

Les groupes comme matériaux pour le jeu de données
**************************************************

Indépendamment du nom des pièces, un matériau qui sera référencé dans votre jeu de donnés, correspond à un groupe de mailles 
(volume de mailles), et pour regrouper les mailles qui ont les mêmes caractéristiques il est utile de leur attribuer un groupe commun 
(comme avec Magix et la notion de Groupes).

C'est donc au niveau des volumes que l'on attribue des groupes. Un volume se retrouve ainsi dans plusieurs groupes. 
Cette modification des groupes se fait en sélectionnant les boutons suivants:

.. taboperation:: 
      :famille: géométrie
      :sousfamille: volumes
      :operation: geomtogroupe

Modifier les champs suivants et *Appliquer* :

.. taboperationparams::
      :valeurs: Groupe, ACIER
                Opération, Ajouter
                Entités géométriques, Vol0000 Vol0001

La commande équivalente est la suivante : 

.. code-block:: python

  ctx.getGeomManager().addToGroup (["Vol0000", "Vol0001"], 3, "ACIER")

L'opération **Enlever**, fait l'inverse d\'**Ajouter**.

Il est également possible d\'**Affecter** un groupe à un ou plusieurs volumes, ces derniers se retrouvent alors exclusivement dans ce groupe 
(et quittent donc les autres groupes dans lesquels ils pouvaient être).

Tout ce qui a été fait ici pour les volumes peut également se faire sur les entités de dimensions inférieures, ainsi il est possible 
d'ajouter, enlever ou affecter un groupe pour des surfaces, des courbes ou des points. 

Les groupes comme outils de travail dans Magix3D
************************************************

Dans l'étape précédente, la séparation en 2 groupes (*CYL* et *PLAQUE*) des 2 volumes est déjà un bon moyen de limiter le nombre d'entités 
affichées ce qui s'avère nécessaire pour les cas complexes.

Néanmoins, il peut s'avérer utile de ne travailler que sur une partie des blocs d'un volume. Il est alors possible de mettre ces blocs 
dans un groupe. Pour cela sélectionner les boutons suivants :

.. taboperation:: 
      :famille: topologie
      :sousfamille: blocs
      :operation: topotogroupe

Les blocs étant dans un groupe supplémentaire, le maillage final contiendra en plus ces volumes de mailles. 
Si ces volumes de mailles gênent, il est possible de les retirer (les blocs du groupe) avant de mailler 
(ou de supprimer les commandes associées dans le script sauvegardé puis de le rejouer).

Tout ce qui a été fait ici pour les blocs peut également se faire sur les entités de dimensions inférieures, ainsi il est possible d'ajouter, 
enlever ou affecter un groupe pour des faces, des arêtes ou des sommets.

Ajoutons le bloc central du cylindre au groupe *CENTRE_CYL*, avec la commande :

.. code-block:: python

  ctx.getTopoManager().addToGroup (["Bl0004"], 3, "CENTRE_CYL")

Notre bloc *Bl0004* n'appartient qu'à un unique groupe *CENTRE_CYL* mais si on le maille, les mailles créées seront dans les groupes *CENTRE_CYL*, 
*CYL* et *ACIER* car le bloc est associé au volume *Vol0000* qui appartient lui-même aux groupes *CYL* et *ACIER*.

.. note::
  En résumé, l'utilisation des groupes permet de créer les groupes de mailles de même noms et/ou de filtrer l'affichage des entités à l'écran. 
  Il peut s'appliquer aux entités géométriques et/ou aux entités topologiques.

.. |plaqueEtCylindre| image:: ../images/PlaqueEtCylindre.png


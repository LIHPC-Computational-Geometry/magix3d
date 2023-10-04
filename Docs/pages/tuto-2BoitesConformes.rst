.. _tuto-2BoitesConformes:

Maillage de 2 boites conformes
###############################

Pré-requis
**********

:ref:`Lancer l'IHM<lancement>` de Magix3D. Si nécessaire, :ref:`modifier son aspect<modification-aspect>`. 

Construction de la 1ère boite
*****************************
Ouvrir le panneau *Création de boite* qui est accessible depuis le panneau :ref:`operations` 
en sélectionnant les boutons suivants :

.. taboperation:: 
      :famille: géométrie
      :sousfamille: volumes
      :operation: boite

|selectionCreationBoite|

Pour notre exemple, nous vous proposons de modifier les champs suivants :

.. taboperationparams::
      :valeurs: Groupe, Mat1
                Point 2 / y, 1.2
                nj, 12

Pour valider la commande et l'exécuter, cliquer ensuite sur le bouton *Appliquer*.

Certaines des entités créées sont alors affichées suivant les filtres d'affichage (voir :ref:`filtre-dimension-entites`).

Le panneau *Commandes python* fait alors apparaître la commande équivalente : 

.. code-block:: python

  ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1.2, 1), 10, 12, 10, "Mat1")

Construction de la 2ème boite
*****************************

Deux possibilités pour arriver au même résultat.
Vous pouvez essayer la première puis annuler la commande avant d'essayer la deuxième. 

En utilisant le panneau *Création de boite*
===========================================

Nous allons modifier les champs suivants :

.. taboperationparams::
      :valeurs: Groupe, Mat2
                Point 1 / x, 1
                Point 2 / x, 1.5

Cliquer ensuite sur le bouton Appliquer.

Le panneau *Commandes python* fait alors apparaître la commande équivalente : 

.. code-block:: python

  ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(1.5, 1.2, 1), 10, 12, 10, "Mat2")

En utilisant le panneau *Commandes python*
==========================================

Aller dans le panneau *Commandes python* et cliquer sur la ligne avec les derniers *>>*

Vous pouvez

* soit faire apparaître la commande de création de la première boite (en utilisant les flèches haut et bas)
* soit y copier la commande précédente (remonter dans cette fenêtre pour la trouver si elle n'est plus visible)

Puis vous modifiez la ligne pour qu'elle soit équivalente au résultat vu ci-dessus avec le panneau et vous l'exécutez 
en appuyant sur le retour chariot de votre clavier.

.. _collage-2-boites:

Collage des 2 boites
********************

Il vous faut ouvrir le panneau *Opérations booléennes sur des volumes* qui est accessible depuis le panneau *Opérations* 
en sélectionnant les boutons suivants :

.. taboperation:: 
      :famille: géométrie
      :sousfamille: volumes
      :operation: operationsbooléennes

Ensuite, sélectionner comme type d'opération le *Collage*. 
Sélectionner les deux volumes présents à l'aide du champ de sélection. 
Terminer par *Appliquer*.

Le panneau *Commandes python* fait alors apparaître la commande équivalente:

.. code-block:: python

  ctx.getGeomManager().glue(["Vol0000", "Vol0001"])

Les 2 volumes et les 2 topologies ont été collés. C'est à dire qu'à la frontière il n'y a plus qu'une surface partagée 
et une coface partagée. Il en est de même pour les entités de dimensions inférieures (sommets géométriques et topologiques, 
courbes et arêtes), une seule des deux entités superposées a été conservée. 

Modification d'une discrétisation
*********************************

Il vous faut ouvrir le panneau *Discrétisation des arêtes* qui est accessible depuis le panneau *Opérations* 
en sélectionnant les boutons suivants :

.. taboperation:: 
      :famille: topologie
      :sousfamille: arêtes
      :operation: discrétisation

Vous allez ensuite modifier les champs comme suit :

.. taboperationparams::
      :valeurs: Méthode, Discrétisation d'arêtes parallèles
                Algorithme, Par discrétisation avec progression géométrique
                Nombre de bras, 13
                Raison, 1.2
                Arêtes, Ar0022

Exécuter la commande avec le bouton *Appliquer*.

Le panneau *Commandes python* fait alors apparaître la commande équivalente : 

.. code-block:: python

  ctx.getTopoManager().setParallelMeshingProperty (Mgx3D.EdgeMeshingPropertyGeometric(13,1.2),"Ar0022")

.. _maillage-cas-conforme:

Création du maillage et sauvegarde
**********************************

Il faut ensuite exécuter la :ref:`commande de création du maillage<menu-maillage>` puis :ref:`celle de sauvegarde<exporter>`.
Le maillage obtenu a cet aspect (à condition que la visibilité des volumes de mailles soit activée).

|deuxBoitesMaillage|

Le panneau *Commandes python* fait alors apparaître les commandes suivantes :

.. code-block:: python

  ctx.getMeshManager().newAllBlocksMesh()
  ctx.getMeshManager().writeMli("deuxBoites.mli2")

.. _sauvegarde-cas-conforme:

Sauvegarde du script
********************

Il est alors possible de :ref:`sauvegarder<exporter-script>` l'ensemble des commandes utiles.
Le fichier obtenu sera équivalent à ceci :

.. code-block:: python

  # -*- coding: utf-8 -*-
  import sys
  import pyMagix3D as Mgx3D
  ctx = Mgx3D.getStdContext()

  # Création d'une boite avec une topologie
  ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1.2, 1), 10, 12, 10, "Mat1")
  # Création d'une boite avec une topologie
  ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(1.5, 1.2, 1), 10, 12, 10, "Mat2")
  # Collage entre géométries avec topologies
  ctx.getGeomManager().glue(["Vol0000", "Vol0001"])
  # Changement de discrétisation pour Ar0022
  ctx.getTopoManager().setParallelMeshingProperty (Mgx3D.EdgeMeshingPropertyGeometric(13,1.2),"Ar0022")
  # Création du maillage pour tous les blocs
  ctx.getMeshManager().newAllBlocksMesh()
  # Sauvegarde du maillage (mli)
  ctx.getMeshManager().writeMli("deuxBoites.mli2")

.. include:: substitution-images.rst

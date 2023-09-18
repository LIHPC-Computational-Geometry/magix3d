Commandes et sorties Python
***************************

Commandes Python
----------------

Ce panneau fait d’une part apparaitre les commandes créées depuis l’IHM
sous forme équivalente en Python. Ces commandes sont précédées du
commentaire associé à la commande.

D’autre part, ce panneau dispose de lignes interactives Python avec
accès à l’historique des commandes. Ainsi vous pouvez aussi bien y
copier une ligne, que faire défiler l’historique des commandes en
plaçant le curseur sur la ligne active (flèche rouge devant) ou une
après. Faire défiler les commandes avec Shift + les flèches de votre
clavier.

Pour faire apparaitre la complétion possible pour une commande :
**CTRL**-**Tab**, mais cette complétion n’est pas possible pour le
résultat d’une fonction. Par exemple, il est possible de compléter
*ctx.getGeom* mais pas *ctx.getGeomManager().newV*. Néanmoins si
*gm=ctx.getGeomManager()*, il est possible d’obtenir la complétion sur
*gm.newV*.

Pour charger un script entier, utiliser l’icône |image225|.

Les lignes non exécutées sont modifiables, mais une fois jouées, elles
ne sont plus éditables.

Il est possible de tout jouer jusqu’à la fin avec l’icône |image226|.

Mais s’il a été mis un point d’arrêt devant une commande avec l’icône
|image227|, l’exécution s’y arrête.

Pour se faire, se mettre en mode *debug* (icône |image228|) puis
utiliser l’icône |image229| pour exécuter la commande suivante ou
utiliser les icônes |image230| et |image226|.

L’icône |image230| permet d’arrêter le cours d’une exécution.

|image231|


Fonctions en Python
-------------------

Il est également possible de définir des fonctions. Par
exemple :

.. code-block:: python

    def pert(x,y,z):
        return [x,y,z+x/10.0+y/5.0]

Ou une fonction un peu plus compliquée :

.. code-block:: python

    from math import *
    def pert(x,y,z):
        if (y<=0.5) :
            salz = x+sin(x*pi)*(1-2*y)*(1.0-z)*0.2
        else :
            salz = x+sin(x*pi)*(1-2*y)*z*0.2
        return [salz,y,z]    

Cette fonction doit ensuite être appliquée à une ou plusieurs surfaces
(ou volumes), identifiées à l’aide des groupes. Par exemple :

.. code-block:: python

    # Ajoute des surfaces à un groupe
    ctx.getGeomManager().addToGroup (["Surf0005"], 2, "ZMAX")
    # Ajoute une perturbation à un groupe
    ctx.getGroupManager().addCartesianPerturbation("ZMAX", pert)

La perturbation est utilisée/appliquée lors de la création du maillage.

Actuellement les lissages ne sont accessibles qu’au travers des
modifications du maillage surfacique ou volumique. Il est donc possible
d’ajouter un lissage à un groupe ainsi :

.. code-block:: python

    # Ajoute une modification de maillage
    Lissage = Mgx3D.SurfacicSmoothing()
    ctx.getGroupManager().addSmoothing ("EXTERNE", Lissage)

Sorties Python
--------------

Les sorties des commandes Python apparaisent dans une vue dédiée.

Exemple de commande dont on attend une sortie ::

   help(ctx.getGeomManager().newVertex)

.. include:: substitution-images.rst
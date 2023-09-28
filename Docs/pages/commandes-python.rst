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
résultat d’une fonction.

Par exemple, il est possible de compléter *ctx.getGeom* mais pas *ctx.getGeomManager().newV*. 
Néanmoins si *gm=ctx.getGeomManager()*, il est possible d’obtenir la complétion sur *gm.newV*.

Pour charger un script entier, utiliser l’icône |load|

Les lignes non exécutées sont modifiables, mais une fois jouées, elles
ne sont plus éditables.

Il est possible de tout jouer jusqu’à la fin avec l’icône |play|.

Mais s’il a été mis un point d’arrêt devant une commande avec l’icône
|stop|, l’exécution s’y arrête.

Pour se faire, se mettre en mode *debug* (icône |debug|) puis
utiliser l’icône |step| pour exécuter la commande suivante ou
utiliser les icônes |pause| et |play|.

L’icône |pause| permet d’arrêter le cours d’une exécution.

.. |load| image:: ../images/image146.jpeg
   :scale: 80%

.. |play| image:: ../images/image147.jpeg
   :scale: 55%

.. |pause| image:: ../images/image151.jpeg
   :scale: 55%

.. |stop| image:: ../images/image148.jpeg
   :scale: 60%

.. |debug| image:: ../images/image149.jpeg
   :scale: 70%

.. |step| image:: ../images/image150.jpeg
   :scale: 70%

.. image:: ../images/image152.jpeg
   :width: 6in
   :height: 3.5in


Fonctions en Python
-------------------

Il est également possible de définir des fonctions. 

Par exemple :

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
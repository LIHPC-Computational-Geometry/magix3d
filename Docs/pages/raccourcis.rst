.. _raccourcis:

Raccourcis clavier
******************

Les raccourcis clavier de Magix3D sont les suivants :

-  **Annuler** la dernière commande : combinaison de touches **CTRL-Z**

-  **Refaire** la commande venant d’être annulée : combinaison de
   touches **CTRL-Y**

-  **Sauvegarder** une session (ensemble de commandes) : combinaison de
   touches **CTRL-S**

-  **Quitter** l’application : combinaison de touches **CRTL-Q**

-  **Imprimer** la vue 3D (imprimante/fichier image) : combinaison de
   touches **CTRL-P**

   NB : pour imprimer dans un fichier sélectionner *Print to file* et
   sélectionner le format du fichier via l’extension associée (ex :
   .bpm, .jpg, .pdf, .png, .ps).

-  Afficher l’\ **aide contextuelle** : touche **F1**

Les raccourcis clavier spécifiques à la console Python sont les
suivants :

-  Faire apparaitre les commandes précédemment jouées : **SHIFT** + les
   flèches de votre clavier.

-  Faire apparaitre la complétion possible pour une commande :
   **CTRL**-**Tab**, mais cette complétion n’est pas possible pour le
   résultat d’une fonction. Par exemple, il est possible de compléter

   .. code-block:: python

    ctx.getGeom

   mais pas
   
   .. code-block:: python

    ctx.getGeomManager().newV
    
   Néanmoins si

   .. code-block:: python

    gm = ctx.getGeomManager()

   il est possible d’obtenir la complétion sur

   .. code-block:: python

    gm.newV

Les raccourcis clavier spécifiques à la fenêtre graphique sont les
suivants :

-  **Recadrage** de la vue : touche **R**. La vue est recalculée de
   manière à ce que tout soit visible.

-  **Recadrage** sur le centre de la sélection : combinaison de touches
   **SHIFT-R.** La vue est recalculée de manière à ce que le point focal
   soit centré sur le centre de la sélection courante.

-  Affichage du plan **y0z** : touche **X**. Supprime également le
   roulis autour de l’axe de visée si la ressource
   *Magix3D.gui.theatre.vtk.xyzCancelRoll* vaut *true*.

-  Affichage du plan **x0z** : touche **Y**. Même remarque concernant le
   roulis.

-  Affichage du plan **x0y** : touche **Z**. Même remarque concernant le
   roulis.

-  Flèches **gauche**, **droite**, **haut**, **bas** : déplacement de la
   vue graphique conformément à la flèche utilisée,

-  **Focalisation** sur le centre d’une entité : touche **F**. La vue
   est recalculée de manière à ce que le point focal soit centré sur
   la dernière entité sélectionnée.

-  **Focalisation** sur un point particulier d’une entité : combinaison
   de touches **SHIFT-F**. La vue est recalculée de manière à ce que le
   point focal soit le point visé par la souris d’une entité (la plus
   proche en distance en cas de superposition).

-  **Sélection** d’une entité : touche **P** (ou **clic gauche**
   souris). Possibilité d’étendre la sélection en pressant simultanément
   la touche **CTRL**.

   Il est possible d'annuler la dernière opération de sélection effectuée : combinaison de touches **SHIFT-Z**

   Pour rejouer la dernière opération de sélection annulée : combinaison de touches **ALT-Z**.

Dans le cas où plusieurs entités sont pointées par le curseur de la
souris, possibilité de passer de l’une à l’autre en pressant la touche
**TAB** ou la touche **SPACE** selon la valeur de la ressource :ref:`Magix3D.gui.theatre.interactor.nextSelectionKey<nextSelectionKey>` (respectivement *tab*
ou *space*). Ce passage d’une entité pointée à la suivante peut
s’effectuer en ordre inverse en pressant simultanément la touche **SHIFT**.

La sensibilité de la sélection interactive peut être modifiée via les
ressources de la section :ref:`Magix3D.gui.theatre.picking<section-picking>`.

La combinaison de touches **CTRL-F** permet d'ouvrir une fenêtre de sélection d'entités par identifiants.

En mode de sélection d’entités dans le panneau de commandes, il est possible de valider la sélection et passer au champ suivant ou appliquer la commande avec
**CTRL-Bouton du milieu de la souris**.
.. _selection:

Sélection d’entités
********************

Les entités sont sélectionnées dans deux cas de figure :

-  En dehors d’un panneau de commande :

   -  Soit parce que l’on souhaite obtenir des informations sur
      celles-ci (elles sont alors coloriées en rouge),

   -  Soit parce que une commande nécessite une sélection avant de faire
      appel à cette dernière, c’est le cas pour l’éditeur de pointage
      laser, l’éditeur de diagnostique ou l’évaluation de qualité du
      maillage (les sous-ensembles du maillage ne changent pas de
      représentation lorsqu’ils sont sélectionnés)

-  Dans un panneau de commande qui attend une ou plusieurs entités d’un
   type donné, le fait de cliquer dans le champ de texte **met en
   violet** le fond du champ, rend visible le type d’entités
   sélectionnables et active le type de sélection adapté à cette
   sélection dans la fenêtre graphique.

Dans tous les cas, la sélection d’une ou plusieurs entités est notifiée
dans la :ref:`barre-notification` ainsi que dans l':ref:`onglet-historique`
et les informations associées sont disponibles
dans les :ref:`onglet-proprietes-individuelles` et :ref:`onglet-proprietes-communes`.

La sélection d’une entité peut se faire sans que celle-ci soit visible.
Si elle est visible et que ce n’est pas un groupe de mailles, alors sa
couleur passe au rouge ou à l’orange.

Il est possible de sélectionner une ou plusieurs entités à l’aide de
l’une des méthodes suivantes (la touche CTRL permet d’augmenter la
sélection, sinon il y a remplacement) :

-  Par « picking » dans la fenêtre de représentation à l’aide du bouton
   gauche de la souris ou de la touche « p », au-dessus de l’entité. La
   touche TAB permet de permuter avec une autre entité proche. Si c’est
   fait en dehors d’un panneau d’opération, il est nécessaire de
   sélectionner le type d’entités sélectionnable (voir :ref:`filtre-dimension-entites`).

-  Par recherche suivant le nom accessible avec le raccourci « CTRL+F »
   (voir :ref:`selectionner-entites`).

-  Par :ref:`selection-suivant-critere`.

-  Depuis l’un des menus contextuels qui apparaissent avec le bouton
   droit au-dessus d’un ou plusieurs types ou groupes sélectionnés dans
   l’:ref:`onglet-groupes`

-  Directement depuis l’:ref:`onglet-entites`. Il est
   possible d’utiliser la touche SHIFT pour étendre la sélection.

-  En saisissant le nom directement dans le champ d’un panneau
   opération.

-  A partir d’une commande terminée, depuis l’:ref:`onglet-moniteur`.

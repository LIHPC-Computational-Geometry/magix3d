.. _operations-topologiques:

Opérations topologiques
~~~~~~~~~~~~~~~~~~~~~~~

Opérations communes aux entités topologiques de différentes dimensions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _destruction-entites-topologiques:

Détruire des entités topologiques |image136|
'''''''''''''''''''''''''''''''''''''''''''''

Ce panneau est proposé pour les entités suivant les
différentes dimensions.

Il faut renseigner les noms des entités à détruire.

Une option vous permet de préciser si la destruction d’une entité
implique la destruction de celles de dimensions inférieures qui sont
liées.

.. _modification-affectation-groupe:

Ajouter/supprimer/affecter des entités topologiques à un groupe |image137|
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau est proposé pour les entités topologiques suivant
les différentes dimensions.

Il est possible soit d’ajouter, soit d’enlever ou soit d’affecter, un
nom de groupe (qui peut être vide) à un ensemble d’entités (de même
dimension) dont il faut renseigner les noms.

Il n’est pas prévu de mettre dans un même groupe des entités
géométriques et topologiques.

.. _association-topo-geo:

Associer des entités topologiques avec des entités géométriques |image144|
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau est proposé pour les entités suivant les
différentes dimensions.

Ce panneau comporte 4 méthodes que l’on peut voir comme autant de
panneaux :

1. *E. topologiques → E. géométrique,* permet d’associé des entités
   topologiques (de dimension pas forcément unique) à une entité
   géométrique.

2. *Sommets → E. géométriques,* permet d’associer des sommets
   topologiques à une entité géométrique, de dimension pas forcément
   unique, en choisissant la plus proche. (Il est donc par exemple
   possible de sélectionner un ensemble de sommets topologiques et un
   ensemble de sommets géométriques.)

3. *Arêtes → Courbes,* permet d’associer automatiquement un ensemble
   d’arêtes aux courbes ou surfaces adaptées en fonction des
   associations des sommets aux extrémités.

4. *Faces → Surfaces,* permet d’associer automatiquement un ensemble de
   faces aux surfaces adaptées en fonction des associations des arêtes
   aux bords.

Il est proposé par défaut de déplacer les sommets en fonction de
l’association.

.. _translation-entites-topologiques:

Réaliser une translation d’entités topologiques |image145|
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau est proposé pour les entités suivant les
différentes dimensions.

Il faut renseigner les noms des entités à translater, sachant que la
translation d’une entité implique la translation de celles de dimensions
inférieures qui sont liées.

Il faut également renseigner le vecteur pour la translation. Ce vecteur
peut être initialisé avec un vecteur topologique.

Il est proposé par défaut de faire subir à la géométrie cette même
opération.

.. _rotation-entites-topologiques:

Réaliser une rotation d’entités topologiques |image146|
''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau est proposé pour les entités suivant les
différentes dimensions.

Il faut renseigner les noms des entités à tourner, sachant que la
rotation d’une entité implique la rotation de celles de dimensions
inférieures qui sont liées.

Il faut également renseigner l’angle de rotation suivant des valeurs
soit prédéfinies (demi pour 180°) soit suivant un nombre de degrés
souhaité.

L’axe de rotation est par défaut l’axe des X mais il est possible d’en
sélectionner un autre (axe des Y ou Z, voir même un axe défini par une
courbe ou par deux points). Il est également possible de définir la
rotation à partir de 3 points.

Il est proposé par défaut de faire subir à la géométrie cette même
opération.

.. _homethetie-entites-topologiques:

Réaliser une homothétie d’entités topologiques |image147|
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau est proposé pour les entités suivant les
différentes dimensions.

Il faut renseigner les noms des entités à grossir ou diminuer, sachant
que l’homothétie d’une entité implique l’homothétie de celles de
dimensions inférieures qui sont liées.

Il faut également renseigner le rapport d’homothétie ainsi que le
centre. Ce centre peut être initialisé avec un sommet topologique.

Il est proposé par défaut de faire subir à la géométrie cette même
opération.

.. _symetrie-entites-topologiques:

Réaliser une symétrie d’entités topologiques |image148|
''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau est proposé pour les entités suivant les
différentes dimensions.

Il faut renseigner les noms des entités à symétriser, sachant que la
symétrie d’une entité implique la symétrie de celles de dimensions
inférieures qui sont liées.

Il faut également renseigner le plan de symétrie. Il est défini par un
centre et un vecteur normal au plan.

Il est proposé par défaut de faire subir à la géométrie cette même
opération.

.. _degenerescence-entites:

Fusionner des sommets, dégénérescence d'un bloc |image156|
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau est proposé pour les entités suivant les
différentes dimensions.

Il permet la dégénérescence de blocs par fusion de sommets.

Il faut renseigner les noms des entités qui seront fusionnées 2 à 2
suivant la dimension :

-  Un sommet topologique avec un autre,

-  Une arête topologique avec une autre (les 2 paires de sommets sont
   fusionnés),

-  Una face topologique avec une autre (les 2 quadruplets de sommets
   sont fusionnés).

Il est proposé par défaut de projeter le 2\ :sup:`ème` sommet sur le
1\ :sup:`er`, mais il est possible que le sommet conservé soit
positionné au milieu.

.. _operation-sommets:

Opérations associées aux sommets topologiques
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sélectionner la sous-famille d’opérations associés aux sommets
topologiques avec |image134|

.. _creation-sommet:

Créer un sommet topologique par association à un point |image135|
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de sélectionner le point géométrique et de
créer un sommet topologique qui lui soit associé.

.. _fusion-sommets:

Fusionner 2 sommets topologiques |image138|
'''''''''''''''''''''''''''''''''''''''''''

Fonction qui permet de fusionner 2 sommets pour qu’il n’en
reste plus qu’un. Les arêtes et faces associées sont fusionnées si
possible.

.. _positionnement-sommet:

Positionner un sommet topologique |image140|
'''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de renseigner à la main les coordonnées
d’un sommet, mais surtout lorsque l’on active l’aperçu, il permet de
déplacer graphiquement, à la souris, ce sommet. Par défaut le
déplacement du point se fait dans toutes les directions, mais il est
possible de le contraindre suivant un des 3 axes. Pour se faire il faut
appuyer sur la touche *Shift*, ensuite le segment de la boite de
contrôle du déplacement le plus proche du curseur sera celui suivant
lequel se fera le déplacement.

Il est également possible de contraindre le déplacement sur une entité
1D (courbe) ou 2D (surface). Il faut sélectionner en premier lieu
l’entité contraignante et en dernier le sommet à déplacer.

.. _modif-coord-sommets:

Modifier les coordonnées de sommets topologiques |image141|
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de modifier une ou plusieurs des
coordonnées d’un ensemble de sommets topologiques.

.. _alignement-sommets:

Aligner des sommets topologiques |image142|
'''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet d’aligner des sommets topologiques entre
les deux aux extrémités.

Pour se faire sélectionner, dans l’ordre les sommets en partant d’une
extrémité. Les sommets aux extrémités ne bougent pas. Les sommets
doivent être reliés par des arêtes.

.. _projection-sommets-geometrie:

Projeter des sommets sur leur géométrie associée |image143|
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de déplacer des sommets topologiques
suivant ce à quoi ils sont associés. Pour se faire, soit vous renseignez
les noms des sommets concernés, soit vous cochez *Tous les sommets* pour
le faire sur tous les sommets existants.

Opérations associées aux arêtes topologiques
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sélectionner la sous-famille d’opérations associés aux arêtes
topologiques avec |image149|

.. _creation-arete:

Créer une arête topologique par association à une courbe |image150|
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet la sélection d’une courbe et la création
d’une arête qui lui soit associé. Il est nécessaire d’avoir
préalablement fait les associations de sommets pour les points aux
extrémités de la courbe.

.. _decoupage-arete:

Découper une arête topologique |image153|
''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de découper une arête en 2. Il faut
renseigner le nom de l’arête concernée ainsi que le ratio (entre 0 et 1)
pour positionner la coupe. Ce ratio permet de positionner la coupe
suivant un des nœuds associés à cette arête et respectant ce ratio comme
proportion de bras avant la coupe (c’est donc un ratio en nombre de
bras). Il est également possible de sélectionner soit un sommet
topologique, soit une entité géométrique dont il sera pris le centre,
pour positionner la coupe.

.. _raboutage-aretes:

Fusionner des arêtes topologiques (en les raboutant) |image154|
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Fonction inverse de la précédente. Il faut renseigner les
noms des arêtes qui ne feront plus qu’une. Si les arêtes sont associées
à des courbes distinctes, il est fait une fusion des courbes. Les arêtes
ont un sommet en commun.

.. _fusion-aretes:

Fusionner 2 arêtes topologiques |image155|
''''''''''''''''''''''''''''''''''''''''''

Fonction qui permet de fusionner 2 arêtes (superposées) pour
qu’il n’en reste plus qu’une. Ou 2 listes d’arêtes. Les sommets des 2
arêtes doivent être proches.


.. _parametrage-discretisation-aretes:

Paramétrer la discrétisation des arêtes |image158|
''''''''''''''''''''''''''''''''''''''''''''''''''''''

Permet de modifier la discrétisation pour un ensemble
d’arêtes.

Trois méthodes sont disponibles :

1. *Discrétisation liste d’arêtes,* il faut renseigner les noms des
   arêtes auxquelles sera appliquée la nouvelle discrétisation.

2. *Discrétisation d’arêtes parallèle,* il faut renseigner les noms des
   arêtes de départ auxquelles sera appliquée la nouvelle
   discrétisation, discrétisation appliquée également aux arêtes
   parallèles strictement en correspondance. Il s’agit des arêtes
   topologiquements parallèles. Ne conviennent pas les arêtes, par
   exemple, les arêtes découpées face à une arête qui ne serait pas.

3. *Nombre de bras avec contrainte,* il faut renseigner le nom d’une
   arête de départ à laquelle on donne un nouveau nombre de bras. Il est
   possible de renseigner les noms des arêtes que l’on ne veut pas
   modifier. La modification est propagée de proche en proche, d’arête
   voisine vers une autre, pour maintenir la structuration.

Les algorithmes pour les discrétisations :

-  *Uniforme*, loi qui répartie les nœuds suivant un pas constant. Donc
   avec distance constante pour une droite ou pour une arête non
   projetée, avec un pas constant dans l’espace de paramétrisation si la
   projection se fait sur une courbe ou une surface (en utilisant une
   courbe issue de la projection de l’arête sur la surface).

-  *Avec progression géométrique*, comme pour *uniforme* mais avec des
   pas qui suivent une progression suivant une raison.

-  *De taille définie*, loi qui se base uniquement sur la longueur de
   bras désirée, adaptée aux méthodes de maillage non structurée.

-  *Par interpolation* basée soit sur une face qui permet d’en déduire
   la discrétisation à reporter en prenant en compte les arêtes qui font
   face à l’arête concernée, soit sur une ou plusieurs arêtes dont on
   interpole les points pour positionner ceux de l’arête à discrétiser.
   Si l’arête n’a pas d’association, alors sa forme est aussi
   interpolée.

-  *Par interpolation entre 2 ensembles d’arêtes* (à sélectionner de
   part et d’autre de l’arête), loi qui fonctionne comme la méthode de
   maillage transfinie avec comme discrétisation au bord chacune des
   listes de nœuds des 2 listes. La forme de l’arête est alors une
   interpolation entre les formes des autres arêtes.

-  *Avec progressions géométriques*, comme pour celle *avec progression
   géométrique*, mais dans ce cas avec une progression à chaque
   extrémité. Il est possible de demander une longueur nulle à l’une des
   extrémités, dans ce cas la discrétisation sera uniforme à cette
   extrémité. Tous les cas ne sont pas valides. Cette loi se veut
   équivalente à celle proposée par le mailleur d’ICEM-CFD. Il peut être
   utile de connaitre la taille des bras aux extrémités d’une arête,
   en utilisant la fonctionnalité :ref:`mesure-angle-3-points`.

-  *Avec progression hyperbolique*, Cette loi se voudrait équivalente à
   celle proposée par le mailleur d’ICEM-CFD, mais la documentation est
   insuffisante. Elle permet de renseigner les longueurs de bras aux
   extrémités.

-  *Avec loi beta de resserrement à la paroi*, Cette loi permet
   d’obtenir une progression à une extrémité et est uniforme à l’autre.
   Elle est comparable à celle dite *avec progressions géométriques*
   dans le cas uniforme à une extrémité.

Il est possible d’imposer un certain nombre de couches (donc de nœuds)
orthogonales à la surface à l’une des extrémités.

Il est possible d’utiliser un découpage polaire, c’est à dire un
découpage avec un contrôle sur les angles par rapport à un centre au
lieu des longueurs des bras. Cette option est disponible uniquement pour
les méthodes *Uniforme* et celles avec *progression[s] géométrique[s].*

Le bouton *Actualiser le panneau* permet d’initialiser l’algorithme et
le nombre de bras en fonction d’une ou plusieurs arêtes préalablement
sélectionnées.

.. _inversion-discretisation:

Inverser le sens de discrétisation
'''''''''''''''''''''''''''''''''''

|image159|\ Permet d’inverser le sens de parcours pour la discrétisation
d’un ensemble d’arêtes dont il faut renseigner les noms. Cette inversion
a une utilité pour les discrétisations telles que celles avec une
progression géométrique.

Opérations associées aux faces topologiques
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sélectionner la sous-famille d’opérations associés aux faces
topologiques avec |image164|

.. _creation-face-surface:

Créer une face par association à une surface |image165|
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Permet de créer une face topologique associée à une surface
géométrique.

Il faut renseigner le nom de la surface concernée et sélectionner un
type de construction :

-  Soit une *face structurée*, ce qui sera possible si la surface a 4
   côtés. Les arêtes seront alors associées aux courbes et les sommets
   topologiques aux sommets géométriques.

-  Soit une *face structurée libre* (sans association), il n’y aura pas
   d’association pour les sommets et les arêtes. La face sera
   optionnellement associée à une surface s’il y en a une de renseignée,
   soit simplement mise dans un groupe pour aider à sa manipulation,
   soit sans groupe (plus difficile de contrôler son affichage et donc
   sa sélection).

-  Soit une *face non structurée*. Les arêtes seront alors associées aux
   courbes et les sommets topologiques aux sommets géométriques.

.. _decoupage-faces-arete:

Découper des faces topologiques [2D] selon une arête |image168|
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de découper un ensemble de faces pour le
cas 2D, première étape pour la construction d’une modélisation avec
topologie 3D par révolution comme c’est le cas avec l’utilitaire
*revolMdlMesh* (basé sur Bibop3D).

Ce découpage est nécessaire pour placer le tracé de l’o-grid. Ce
placement correspond au positionnement de l’o-grid sur un plan de
symétrie.

Pour cela, sélectionner soit les faces concernées, soit cocher toutes
les faces, ensuite sélectionner une des arêtes que coupe le tracé que
vous souhaitez construire. Indiquer à l’aide du ratio où couper cette
arête (voir :ref:`decoupage-arete`). Indiquez à l’aide d’un ratio la
taille de l’o-grid pour les faces avec dégénérescence (0 pour dégénéré
en un point à un peu moins de 1 pour que l’o-grid soit proche du bord de
la surface).

Il est possible d’activer l’\ *Aperçu*, dans ce cas les entités créées
et celles modifiées seront mises en évidence dans la fenêtre de
représentation.

Continuer avec la construction de blocs par révolution, voir paragraphe
2.5.2.4.2.

.. _decoupage-face-arete:

Découper une face topologique selon une arête |image169|
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet le découpage d’une face.

Pour cela renseigner la face concernée ainsi que l’arête qui déterminera
le passage de la coupe (comme au paragraphe 2.5.2.2.4).

Les entités créées sur cette coupe sont projetées de manière identique
aux entités coupées. Par défaut les sommets sont projetés sur les
entités auxquelles ils sont associés.

Il est possible d’activer l’\ *Aperçu*, dans ce cas les entités créées
et celles modifiées seront mises en évidence dans la fenêtre de
représentation.

.. _decoupage-face-ogrid:

Découper des faces avec o-grid |image170|
'''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de découper une ou plusieurs faces.

Le fonctionnement est équivalent à celui proposé par ICEM-CFD.

Pour cela renseigner les noms des faces dans lesquels sera fait le
découpage.

Vous pouvez optionnellement renseigner les noms des arêtes dont il faut
considérer que les faces voisines seront à incorporer dans le découpage.
Ces faces peuvent ne pas exister, le découpage passera au travers de
l’arête.

.. _prolongation-face-sommet:

Prolonger le découpage d’une face topologique à partir d’un sommet |image171|
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de découper une face dont une arête est
déjà coupée.

Pour cela sélectionner la face concernée ainsi que le sommet sur le bord
de la face. La face est alors coupée en deux avec création d’une arête
qui passe par ce sommet.

Il est possible d’activer l’\ *Aperçu*, dans ce cas les entités créées
et celles modifiées seront mises en évidence dans la fenêtre de
représentation.

.. _insertion-trou:

Insérer un trou dans la topologie |image172|
''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de créer un trou dans la topologie, c’est
à dire duplique les arêtes et les faces entre les blocs pour faire
apparaître un trou.

Pour cela renseigner les noms des faces concernées (à dupliquer). Les
arêtes et les sommets seront également dupliqués, ou non, suivant que
les blocs sont reliés entre eux sans passer par les faces sélectionnées.

.. _fusion-faces:

Fusionner des faces topologiques |image173|
'''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de coller des blocs par fusion des
sommets, arêtes et faces, opération inverse de l’insertion d’un trou.

Pour cela sélectionner les paires de faces en vis à vis que vous
souhaitez fusionner. La première sera conservée, la deuxième détruite.

S’il y a des discrétisations différentes de part et d’autres mais qu’il
y a un rapport entier entre elles, alors à l’issue de la fusion le
raccord sera semi-conforme.

Le panneau de fusion de blocs est similaire, voir :ref:`collage-blocs`.

.. _parametrage-discretisation-faces:

Paramétrer le maillage des faces |image176|
'''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de modifier la méthode de maillage pour un
ensemble de faces.

Pour cela sélectionner la méthode de maillage désirée et les noms des
faces auxquelles l’appliquer. Les méthodes disponibles sont les
suivantes :

-  *Transfini*, méthode structurée générale, un peu lente et pas
   toujours adaptée à certain cas courbes avec extrémités dont la
   discrétisation n’est pas uniforme.

-  *Directionnel*, méthode structurée bien adaptée aux blocs dont les
   bords sont rectilignes.

-  *Directionnel orthogonal*, méthode structurée équivalente à la
   précédente avec un nombre de couches dont les nœuds sont alignés
   orthogonalement à l’arête de départ.

-  *Suivant une rotation*, méthode structurée bien adaptée aux blocs
   construits par révolution.

-  *Delaunay (GMSH)*, méthode non-structurée, construit des triangles.

-  *Delaunay (NetGen)*, méthode non-structurée, construit des triangles
   mais pas toujours robuste.

-  *Delaunay (MeshGems)*, méthode non-structurée, construit des
   triangles à l’aide de la bibliothèque commerciale GEMS.

Ce panneau a son équivalent pour les blocs, voir :ref:`parametrage-discretisation-blocs`.

Opérations associées aux blocs topologiques
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sélectionner la sous-famille d’opérations associés aux blocs
topologiques avec |image181|

.. _creation-bloc-volume:

Créer un bloc par association à un volume |image182|
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Permet de créer un ou plusieurs blocs topologiques associés
à un volume géométrique.

Il faut renseigner le nom du volume concerné et sélectionner un type de
construction :

-  Soit un *bloc structuré*, ce qui sera possible si le volume est d’un
   type connu et adapté (boîte par exemple). Les faces seront alors
   associées aux surfaces, les arêtes aux courbes et les sommets
   topologiques aux sommets géométriques.

-  Soit un ensemble de *blocs structurés en o-grid*, (similaire à *bloc
   structuré*), valable pour les sphères et les cylindres (ainsi que les
   portions de ces derniers).

-  Soit *bloc structuré libre* (sans association), il n’y aura pas
   d’association pour les sommets, les arêtes et les faces. Le bloc sera
   optionnellement associé à un volume s’il y en a un de renseigné, soit
   simplement mise dans un groupe pour aider à sa manipulation, soit
   sans groupe (plus difficile de contrôler son affichage et donc sa
   sélection).

-  Soit un *bloc non structuré*. Les faces seront alors associées aux
   surfaces, les arêtes aux courbes et les sommets topologiques aux
   sommets géométriques.

-  Soit un *bloc pour l’insertion*, permet d’identifier le volume auquel
   sera appliquée comme méthode de maillage l’insertion. Il s’agit d’une
   méthode de capture des mailles d’autres blocs à l’intérieur du volume
   concerné. Les maillages changent alors de groupe (de volume).

.. _creation-bloc-revolution:

Créer des blocs par révolution |image183|
'''''''''''''''''''''''''''''''''''''''''''

Il faut renseigner le nom d’une arête par lequel passe le
tracé de l’o-grid tel qu’il a été construit lors du découpage des faces
2D, voir :ref:`ajout-groupe`.

Il faut également renseigner l’angle de rotation suivant des valeurs
prédéfinies (plein, demi ou quart). L’axe de rotation est
obligatoirement celui des X.

Cette commande va construire des blocs à partir de la topologie 2D,
suivant une révolution autour de l’axe pour les faces au-dessus du tracé
de l’o-grid, et un unique bloc pour les faces sous le tracé.

Le paramètre « Ratio pour sommets de l’o-grid » permet de contrôler le
positionnement des sommets qui suivent le tracé de l’o-grid en dehors
des plans de symétrie. Avec une valeur de 1, les sommets seront placés
de manière à ce que les blocs soient parallélépipédiques, alors qu’avec
0 les sommets seront à égale distance de l’axe de révolution que ceux du
modèle 2D.

Exemple avec une unique face en 2D

.. image:: ../images/image119.jpeg
   :scale: 80%
   :align: left

.. list-table:: 
   :class: mytable

   * - .. image:: ../images/image120.jpeg
     - .. image:: ../images/image121.jpeg 
   * - Cas (par défaut) avec ratio à 1
     - Cas possible avec ratio à 0
   * - .. image:: ../images/image122.jpeg
     - .. image:: ../images/image123.jpeg
   * - Maillage des blocs, celui près de l’axe est régulier
     - Maillage des blocs, celui près de l’axe est déformé

.. _copie-blocs:

Copier des entités topologiques |image197|
'''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de dupliquer des blocs.

Il faut renseigner les noms des blocs concernés ainsi que le volume
auquel seront associés les nouveaux blocs

.. _extract-blocs:

Extraire des blocs topologiques |image198|
'''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de sélectionner un ensemble de blocs qui
sont désolidarisés du reste de la topologie (ce qui revient à faire une
copie des blocs sans conserver la relation avec les blocs qui ne sont
pas copiés, et une destruction des blocs originaux). Ces blocs nouveaux
sont mis dans un groupe à renseigner.

.. _decoupage-blocs-arete:

Découper des blocs selon une arête |image201|
'''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de découper un ou plusieurs blocs suivant
une position sur une arête. Il est possible de découper tous les blocs
(ceux dans le prolongement de la coupe de proche en proche).

Cette commande est similaire à celle de :ref:`decoupage-face-arete`.

.. _decoupage-blocs-ogrid:

Découper des blocs avec o-grid |image202|
'''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de découper un ou plusieurs blocs.

Pour cela renseigner les noms des blocs dans lesquels sera fait le
découpage.

Vous pouvez optionnellement renseigner les noms des faces sur lesquels
le découpage viendra s’appuyer avec optionnellement le découpage des
blocs voisins (s’ils existent).

.. _prolongation-decoupage-blocs:

Prolonger un découpage de blocs à partir d’une arête |image203|
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de découper un bloc en prolongeant le
découpage d’une face suivant une arête.

Pour cela renseigner le nom du bloc concerné ainsi que le nom de l’arête
qui va servir de base à la coupe. Fonction similaire à celle de :ref:`prolongation-face-sommet`.

.. _collage-blocs:

Coller des blocs topologiques |image204|
''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de coller des blocs par fusion des
sommets, arêtes et faces.

Soit vous sélectionnez une paire de blocs, soit une paire de volumes (si
vous avez sélectionné *Blocs associés à des volumes*).

Similaire à celui de :ref:`fusion-faces`

.. _fusion-blocs:

Fusionner des blocs topologiques |image205|
'''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de fusionner deux blocs (dans un même
volume) pour ne faire plus qu’un. Les faces entre les deux blocs
disparaissent. Les arêtes sur le bord sont conservées

.. _association-blocs-volumes:

Associer des blocs topologiques avec des volumes géométriques |image206|
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Ce panneau permet d'associer des blocs topologiques à un volume.

.. _parametrage-discretisation-blocs:

Paramétrer la discrétisation des blocs |image207|
'''''''''''''''''''''''''''''''''''''''''''''''''''''

Ce panneau permet de modifier la méthode de maillage pour un
ensemble de blocs.

Pour cela sélectionner la méthode de maillage désirée et les noms des
faces auxquelles l’appliquer. Les méthodes disponibles sont les
suivantes :

-  *Transfini*, méthode structurée générale, un peu lente et pas
   toujours adaptée à certain cas courbes avec extrémités dont la
   discrétisation n’est pas uniforme.

-  *Directionnel*, méthode structurée bien adaptée aux blocs dont les
   bords sont rectilignes.

-  *Directionnel orthogonal*, méthode structurée équivalente à la
   précédente avec un nombre de couches dont les nœuds sont alignés
   orthogonalement à la surface de départ.

-  *Suivant une rotation*, méthode structurée bien adaptée aux blocs
   construits par révolution.

-  *Delaunay (MeshGems)*, méthode non-structurée, construit des
   tétraèdres à l’aide de la bibliothèque commerciale GEMS.

-  *Delaunay (TetGen)*, méthode non-structurée, construit des
   tétraèdres.

-  *Delaunay (NetGen)*, méthode non-structurée, construit des tétraèdres
   mais pas toujours robuste.

-  *Insertion*, méthode de capture des mailles d’autres blocs à
   l’intérieur du volume concerné. Les maillages changent alors de
   groupe (de volume) [non finalisé].

Ce panneau a son équivalent pour les faces, voir :ref:`parametrage-discretisation-faces`.

.. _deraffinement-blocs:

Déraffiner des blocs |image208|
''''''''''''''''''''''''''''''''''

Ce panneau permet de déraffiner un bloc suivant une
direction. Cela implique en général que le maillage soit
« semi-conforme ».

Pour cela il faut renseigner le nom du bloc concerné, le nom d’une arête
qui servira de direction, ainsi que le facteur de déraffinement (2 en
général).

.. include:: substitution-images.rst
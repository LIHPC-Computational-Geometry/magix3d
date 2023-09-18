.. _menu-chambre-experimentale:

Menu Chambre expérimentale
==========================

Les commandes de ce menu permettent de mettre au point les jeux de
données pour un cas laser, aussi bien pour le code de calcul 3D (code T)
que pour le code diagnostic (code D).

Les fonctionnalités ci-dessous (en dehors de la sélection du contexte et
de la sélection pour la projection) sont également disponibles dans le
menu contextuel de :ref:`onglet-pointages-laser`.

L’ensemble des fonctionnalités de ce menu sont communes à celles
disponibles dans *Lem*.

Vous trouverez dans l’annexe :ref:`job34` un exemple d’utilisation des pointages
laser et des diagnostics.

Les pointages laser et diagnostics chargés sont accessibles depuis
l’onglet *Pointages laser* (Magix 3D) ou *Chambre d’expérience* (Lem),
qui comporte le panneau ci-dessous.

|image36|

Il y est possible de sélectionner des pointages laser et diagnostics qui
seront ensuite soumis à divers traitements (affichage, modification, ...) via le menu *Chambre expérimentale*.

Ce menu est accessible depuis le menu principal de l’application mais
également par clic droit dans ce panneau (menu contextuel).

Définition des pointages lasers et diagnostics
----------------------------------------------

Les pointages laser sont définis en coordonnées sphériques, dans le
repère chambre d’expérience (laboratoire) ou dans le repère maillage
(cible) selon les conventions du code T. Le pointage laser prend fin au
niveau d’un point appelé *foyer*, et à pour direction celle définie par
les angles φ et Θ selon la figure suivante.

|image37|

La source du pointage laser est considérée ici comme étant distante du
foyer de *distance focale*.

A noter que les pointages laser sont regroupés en anneaux, et que la
sélection d’un anneau provoque automatiquement celle des pointages laser
contenus (voir figure ci-dessus).

Le mécanisme utilisé pour les diagnostics suit exactement le même
principe avec, pour la version actuelle, le même vocabulaire.

Recommandations préalables
--------------------------

Les opérations d’affichage des pointages laser sont gourmandes en
mémoire et en temps de calcul. Il est vivement conseillé de trouver dans
un premier temps les bons paramètres (niveau de détail, opacité, ...)
sur un pointage laser avant de les modifier tous.

Ces opérations d’affichage passent par de très nombreux calculs
d’intersection entre les pointages laser et les surfaces et volumes de
projection. Il convient donc d’être vigilant lorsque l’on définit les
surfaces de projection à ne pas en mettre trop (voir :ref:`projeter-surfaces-ou-volumes`) et à ne pas
demander une définition trop fine des cylindres représentant les
pointages laser (voir :ref:`parametres-graphiques-laser`).

Point sujet à erreur : les unités. Les CAO et maillages peuvent ne pas
avoir d’unité de longueur associée, et, par défaut ils sont alors
considérés comme étant exprimés en mètres. Cela pose problème si les
valeurs associées (coordonnées, longueurs) ont été renseignées dans une
autre unité (ex : cm ou mm), les pointages laser sont alors éloignés de
la cible.

Lors du chargement de pointages laser l’application peut, si elle
constate une différence d’unité entre la cible et la position du foyer
des pointages laser, afficher un avertissement et éventuellement
proposer de remplacer l’unité de la cible par celle des positions foyer.
La question posée est du type suivant :

*Les unités de position du foyer et de longueur du maillage diffèrent
(cm/m). Souhaitez-vous appliquer au maillage l'unité de longueur des
pointages laser (cm) ?*

La réponse à fournir est bien sûr spécifique à chaque cas. Cette
question s’adresse avant tout aux cas où aucune attention particulière
n’a été fournie quant à l’unité de longueur de la cible (du maillage),
et concerne des maillages a priori antérieurs à 2016. Dans la plupart
des cas il faut y répondre par la négative.

.. _utiliser-contexte:

Utiliser ce contexte
--------------------

Cette action est nécessaire pour utiliser les fonctionnalités ci-dessous
de représentation des pointages laser ou des diagnostics. Elle nécessite
qu’au préalable soient définis l’unité (voir :ref:`unites-longueur`) et le type de
repère (voir :ref:`repere`).

Les surfaces et volumes sélectionnés (voir :ref:`projeter-surfaces-ou-volumes`) sont
également pris en compte.

Dans le cas du logiciel Lem l’unité de longueur utilisée est celle du
maillage sur les surfaces duquel sont effectuées les projections des
pointages laser.

.. _projeter-surfaces-ou-volumes:

Projeter sur les surfaces ou volumes sélectionnés
-------------------------------------------------

Les surfaces et volumes représentés de manière surfacique et
sélectionnées (via menu contextuel *Sélectionner.Volumes/Surfaces
géométriques* et/ou *Sélectionner.* de la zone Groupes) sont utilisés
comme surface de projection des pointages laser, c’est à dire qu’ils
stoppent (totalement ou partiellement) à leur rencontre la propagation
des pointages laser dans la chambre expérimentale. Il en est de même
avec les surfaces et volumes de maillage.

L’unité de longueur des surfaces et volumes de projection est utilisée
comme référence pour afficher les pointages lasers dans la fenêtre
graphique (application d’un éventuel coefficient multiplicateur lorsque
les unités de longueur diffèrent pour que tout soit représenté dans le
même repère).

Vue chambre d’expérience
------------------------

L’activation de cet item provoque l’affichage de la vue graphique telle
qu’on voit la cible dans la chambre d’expérience, à savoir l’axe Oz
vertical, et l’observateur sur la première bissectrice du plan xOy. Cela
revient à la vue « repère chambre » de la boite de dialogue de
transformation (voir :ref:`transformation-chambre-vers-cible`).

.. _transformation-chambre-vers-cible:

Transformation Chambre expérimentale vers cible
-----------------------------------------------

La boite de dialogue qui s’ouvre à l’aide de ce menu permet de définir
dans son quadrant supérieur gauche la transformation (rotations puis
translation) qui permet de passer du repère de la chambre expérimentale
au repère maillage (cible), conformément aux spécifications du bloc
LASER_MODULE_TAG du jeu de données du code T (voir balise <quel-repere>
dans la documentation du code T). Cette transformation est utile dans le
cas où le maillage est défini dans un repère et que l’on souhaite ne pas
bouger ce maillage (pour avoir par exemple un cas similaire à du 2D
axisymétrique, ou pour maintenir la géométrie dans un repère plus facile
à appréhender que celui de la chambre).

|image38|

Les rotations sont définies par un triplet d’angles (Phi, Thêta et
Oméga).

**Ces angles sont ceux qui permettent de passer du repère de la chambre
au repère de la cible**, avec comme particularité que s’ils sont tous
nuls, alors l’axe des X de la cible est confondu avec le Z de la chambre
(et cela pour faciliter les comparaisons 2D axisymétriques avec son
pendant en 3D). Ces permutations d’axes (Z -> X, X -> Y, Y -> Z) sont
systématiquement faites dès lors que le contenu de la balise
<quel-repere> est *chambre*, même en l’absence de transformation. Les
rotations sont appliquées dans cet ordre (phi, thêta, oméga) et avec
changement de repère après chacune d’elle.

La translation d’un repère par rapport à l’autre se fait depuis le
repère maillage.

Pour faciliter la mise au point de cette transformation il est possible
d’afficher les 2 trièdres et de jouer sur la longueur de leur
représentation. Il est permis avec l’option « toujours afficher les
trièdres » de les laisser visible après la fermeture de cette boite de
dialogue.

Par défaut cette transformation est appliquée aux pointages laser et aux
diagnostics.

Les modifications affectent la représentation des pointages et des
diagnostics, la vue des autres objets reste inchangée.

Il est possible de déterminer automatiquement la transformation de
passage du repère chambre au repère cible à l’aide de 3 ou 4 points non
alignés dont on connait les coordonnées dans les 2 repères. Le quadrant
inférieur gauche de la boite de dialogue permet de renseigner ces
coordonnées dans les 2 repères, puis, à l’aide du bouton *Déterminer*,
de calculer la transformation et remplir les différents champs du
quadrant supérieur gauche (phi, thêta, oméga, Dx, Dy, Dz). L’activation
du bouton *Appliquer* de ce même quadrant provoque la détermination,
puis, en cas de succès, l’applique.

Ce service de détermination automatique de la transformation vérifie
préalablement que les points sont disposés les uns par rapport aux de la
même façon dans les 2 repères. Cette vérification est effectuée par
comparaison des produits scalaires des différents vecteurs obtenus à
partir de ces 3 ou 4 points. Ce panneau permet de choisir la *précision*
utilisée dans ces tests. Cocher la case *Tolérer les erreurs* permet de
ne pas bloquer la détermination en cas d’échec de la vérification, seul
un avertissement sera affiché.

Pour utiliser un quatrième point il faut cocher la case *Pt 4*.

A noter que les éventuels points VISRAD_SPECIFIC_NODE_NAME1
VISRAD_SPECIFIC_NODE_NAME2, VISRAD_SPECIFIC_NODE_NAME3, et
VISRAD_SPECIFIC_NODE_NAME4 d’un fichier Visrad au format *vrw* seront
automatiquement proposés dans le repère Chambre d’expérience. Pour ce,
ces points doivent être définis comme suit sous Visrad :

-  Dans la rubrique *Target Components*,

-  Type *Sphere*,

-  Coordonnées absolues du centre dans le repère *Target Chamber*
   (coordonnées cartésiennes, cylindriques ou sphériques)

Il est proposé également une calculatrice (quadrant inférieur droit) qui
permet de trouver les coordonnées d’un point dans un repère depuis
l’autre.

.. _boite-englobante:

Boite englobante
----------------

|image39|\ 

Cette boite de dialogue est automatiquement renseignée à
l’aide des surfaces ou volumes de projection.

Par défaut les pointages laser et la représentation des lignes des
diagnostics sont restreints à cette boite englobante pour éviter de
représenter des entités qui paraissent infinis par rapport à la cible.

Par défaut les pointages laser continuent jusqu’à leur intersection avec
les surfaces ou volumes de projection. Il est possible d’arrêter leur
représentation au niveau du point focal (pour le cas où il n’y aurait
pas intersection par exemple).

On valide la saisie des coordonnées de la boite englobante par retour
chariot dans un champ de saisie ou par activation du bouton *Appliquer*.

Action projeter sur les surfaces ou volumes sélectionnés 
--------------------------------------------------------

//TODO Doublon ?
Cette action permet de définir les entités avec lesquels il est
recherché une intersection avec les pointages laser. Les pointages laser
s’arrêtent au niveau de cette sélection et les taches laser sont
représentées à l’intersection.

Il est conseillé de ne sélectionner que les surfaces et volumes
importants (cible, chambre), ayant une réelle chance d’intersection avec
les pointages laser, afin d’éviter autant que possible des calculs
d’intersection entre pointages laser et ces surfaces et volumes de
projection.

Charger des pointages laser
---------------------------

Ce menu ouvre une boite de dialogue qui vous permet de sélectionner **un de vos fichiers** 
au format du **code T** (T_INTERNAL_EXTENSION ou \*.xml)

Si c’est un fichier de jeu de données du *code T*, alors ne sont pris en
compte que les données correspondant à la partie laser (balise
LASER_MODULE_TAG). Les données peuvent être modifiées et sauvegardées,
voir :ref:`enregistrer-pointages-laser`.

S’il s’agit d’un fichier au format Visrad les données sont alors
systématiquement définies dans le repère chambre expérimentale. Dans le
cas d’un fichier au format *vrw*, il est possible de ne charger que les
pointages visibles (sous Visrad) en cochant la case *Uniquement les
pointages laser visibles*.

|image40|

Importer des pointages laser
----------------------------

Ce menu ouvre une boite de dialogue qui vous permet de sélectionner **un
fichier de la base de référence** au format du *code T*
(T_INTERNAL_EXTENSION ou \*.xml) ou au format *VisRAD* (\*.csv ou
\*.vrw).

Actuellement cette base ne contient que des fichiers au format xml avec
des pointages laser définis en repère chambre expérimentale. Les données
peuvent être modifiées et sauvegardées, voir :ref:`enregistrer-sous-pointages-laser`. Ce sera à vous de
les insérer ensuite dans votre jeu de données.

.. _enregistrer-pointages-laser:

Enregistrer les pointages laser
-------------------------------

Cet enregistrement n’est possible que pour vos fichiers au format du
*code T*.

Seule la partie concernant les lasers est modifiée, le reste est réécrit
tel quel.

Cette sauvegarde vous permet de sauvegarder les modifications affectant
les pointages laser (paramètres des pointages et transformation).

.. _enregistrer-sous-pointages-laser:

Enregistrer sous les pointages laser
------------------------------------

Comme pour l’enregistrement précédent, avec la possibilité de changer de
fichier destination et de choisir les pointages laser enregistrés. C’est
le moyen de sauvegarder les informations suite à une importation de
pointages laser.

Le seul format d’enregistrement proposé est celui du code T. Si la case
*Uniquement les pointages laser visibles* n’est pas cochée alors tous
les pointages laser seront enregistrés, alors que si elle est cochée
seuls ceux affichés le seront. Ce dispositif permet à partir d’un jeu de
données d’en générer plusieurs avec pour variable la configuration des
pointages laser.

Paramètres des pointages laser
------------------------------

Ce menu ouvre une boite de dialogue qui vous permet de modifier
certaines des caractéristiques des pointages sélectionnés.

La première caractéristique modifiable est la position du foyer. Il est
possible de renseigner un nombre variable de coordonnées et cela soit
dans le repère chambre, soit dans le repère maillage. Cette position
sera sauvegardée dans le repère d’origine. Il est possible d’afficher
une représentation de cette position et d’en faire varier la taille à
l’aide du curseur.

Dans le deuxième groupe de caractéristiques, on trouve l’angle Ω de
rotation autour de l’axe du faisceau, sa longueur, son rayon et son
Rapport d’Aspect.

.. _parametres-graphiques-laser:

Paramètres graphiques des pointages laser
-----------------------------------------

Ce menu ouvre une boite de dialogue qui permet de modifier les
caractéristiques de la représentation des pointages (épaisseur du trait,
opacité, nombre de secteurs, position du nom...).

Couleur : c’est la couleur du pointage laser et de la tache projetée

Epaisseur du trait : chaque secteur (facette) du cylindre est entourée
de 4 traits noirs d’épaisseur 0 par défaut. La modification de ce
paramètre permet de mettre en évidence la facettes.

Opacité : c’est l’opacité du pointage laser exprimée de 0 (transparent)
à 1 (opaque). Si l’on souhaite mettre en évidence une valeur de 0,2 est
adaptée.

Opacité du spot : c’est l’opacité de la tache faite par le pointage
laser sur la surface de projection, là aussi exprimée de 0 (transparent)
à 1 (opaque).

Améliorer l’affichage : option visant à limiter les effets de moirés dus
à de multiples facettes graphiques confondues (la surface de projection
et la tache). Pour ce, on rapproche très légèrement la tache de la
source. Le champ facteur correspond au coefficient multiplicateur
appliqué à la distance source du pointage laser/tache, et doit donc être
légèrement inférieur à 1. Le champ sensibilité correspond à un *epsilon*
dans les calculs d’intersection.

Eliminer les spots cachés : cette option vise à éliminer les taches
« sans objet » lorsqu’un pointage laser intersecte plusieurs surfaces ou
volumes, et à ne retenir que les éléments les plus proches de la source
d’émission. A cette fin des calculs de projection sont effectués dans
une fenêtre virtuelle dont les dimensions sont renseignées via les
champs Fenêtre.

Epaisseur du trait du spot : la tache formée par le pointage laser est
entourée de traits noirs d’épaisseur 0 par défaut, valeur modifiable.

Nombre de secteurs : c’est le nombre de facettes du cylindre dans le
sens de sa hauteur. Par défaut il est de 18, c’est à dire qu’une facette
couvre 20 degrés.

Position du nom : par défaut le nom d’un pointage laser est affiché à
l’extrémité côté source du pointage. Ce champ permet de modifier le
coefficient multiplicateur appliqué à la longueur affichée du pointage
pour définir un nouveau point d’ancrage à partir du foyer.

Décalage hor. et vert. : ces champs permettent de modifier les décalages
horizontal et vertical (en pixels) du texte du nom des pointages laser
du trait qui le relie au pointage.

Afficher/Masquer les pointages laser
------------------------------------

Chacune de ces actions permet d’afficher ou de masquer les pointages
laser sélectionnés.

Si rien n’apparait, il se pourrait que les pointages soient en dehors de
la :ref:`boite-englobante`. Il faut aussi prêter une attention
particulière aux unités. Les pointages lasers sont représentés jusqu’au
point focal ou jusqu’à l’intersection avec les surfaces ou volumes
sélectionnés pour cela. Il peut ne pas y avoir d’intersection.

Afficher/Masquer les taches laser
---------------------------------

Chacune de ces actions permet d’afficher ou de masquer les taches laser
faites par les pointages laser sélectionnés sur les surfaces et volumes
de projection.

Si rien n’apparait, il se pourrait que les pointages n’aient pas
d’intersection avec les surfaces ou volumes sélectionnés pour cela.

Afficher/Masquer les noms
-------------------------

Chacune de ces actions permet d’afficher ou de masquer les noms des
pointages sélectionnés.

Supprimer les pointages laser
-----------------------------

Cette action permet de supprimer les pointages laser sélectionnés. Ils
n’apparaitront plus dans le panneau ni dans le jeu de données.

Charger des diagnostics
-----------------------

Ce menu ouvre une boite de dialogue qui vous permet de sélectionner **un
de vos fichiers** au format du *code T* (T_INTERNAL_EXTENSION ou
\*.xml). Seules les données correspondant à l(balise LASER_MODULE_TAG)
sont prises en compte. Les données peuvent être modifiées et
sauvegardées, voir :ref:`enregister-diagnostics`.

Importer des diagnostics
------------------------

Ce menu ouvre une boite de dialogue qui vous permet de sélectionner **un
fichier de la base de référence** au format du *code T*
(T_INTERNAL_EXTENSION ou \*.xml). Les données peuvent être modifiées et
sauvegardées, voir :ref:`enregister-sous-diagnostics`.

.. _enregister-diagnostics:

Enregistrer des diagnostics
---------------------------

Cette sauvegarde vous permet de sauvegarder les modifications affectant
les diagnostics (paramètres des diagnostics et transformation).

.. _enregister-sous-diagnostics:

Enregistrer sous des diagnostics
--------------------------------

Comme pour l’enregistrement précédent, avec la possibilité de changer de
fichier destination.

C’est le moyen de sauvegarder les informations suite à une importation
de diagnostics.

Paramètres des diagnostics
--------------------------

Ce menu ouvre une boite de dialogue qui vous permet de modifier
certaines des caractéristiques des diagnostics. Le diagnostic est vu ici
comme une ligne de visée définie à partir d’un point, le *foyer*, et
orientée selon les angles φ et Θ en coordonnées sphériques. Pour les
informations code D Flux la position du détecteur est située sur la
ligne de visée à la distance *Longueur* (distance focale) du foyer.

La première caractéristique modifiable est la position du foyer. Il est
possible de renseigner un nombre variable de coordonnées et cela soit
dans le repère chambre, soit dans le repère maillage. Cette position
sera sauvegardée dans le repère d’origine. Il est possible d’afficher
une représentation de cette position et d’en faire varier la taille à
l’aide du curseur.

Dans le deuxième groupe de caractéristiques, on trouve l’angle Ω de
rotation autour de lui même du faisceau, sa longueur, son rayon et son
Rapport d’Aspect. Seule la longueur et le rayon ont actuellement un sens
et sont pris en compte.

Paramètres graphiques des diagnostics
-------------------------------------

Ce menu ouvre une boite de dialogue qui permet de modifier les
caractéristiques de la représentation des diagnostics (épaisseur du
trait, opacité, nombre de secteurs, position du nom...).

Afficher/Masquer les diagnostics
--------------------------------

Chacune de ces actions permet d’afficher ou de masquer les diagnostics
sélectionnés.

Si rien n’apparait, il se pourrait que les diagnostics soient en dehors
de la :ref:`boite-englobante`. Il faut aussi prêter une attention
particulière aux unités. Les diagnostics sont représentés depuis la
position foyer.

Afficher/Masquer les noms des diagnostics
-----------------------------------------

Chacune de ces actions permet d’afficher ou de masquer les noms des
diagnostics sélectionnés.

Vue du diagnostic
-----------------

Cette action bouge la vue (la caméra) de manière à observer les objets
depuis le diagnostic sélectionné.

Informations code diagnostic
----------------------------

Une boite de dialogue s’ouvre avec les informations à mettre dans le jeu
de données pour le code de diagnostic. Cette boite de dialogue comporte
2 onglets :

-  *Imagerie 2D axi* : ces informations sont à utiliser dans le cas
   d’une CAO/Maillage 2D axisymétrique (2D cylindrique),

-  *Imagerie 3D* : ces informations sont à utiliser dans le cas d’une
   CAO/Maillage 3D.

Ces informations prennent en compte les éventuelles transformations de
passage du repère chambre expérimentale (laboratoire) au repère cible
(géométrie).

L’item *Fichier.Enregistrer* du menu permet d’enregistrer dans un
fichier texte les informations de l’onglet courant.

Deux nouveaux onglets sont en cours de mise au point : *Flux 2D axi* et
*Flux 3D*.

Supprimer les diagnostics
-------------------------

Cette action permet de supprimer les diagnostics sélectionnés. Ils
n’apparaitront plus dans le panneau ni dans le jeu de données.

Aide sur les pointages laser
----------------------------

Ouvre cette aide au début de la rubrique concernant les lasers

.. note::
   **FAQ : Les pointages laser ne s’affichent pas**

   Les causes possibles d’un échec à l’affichage des pointages laser,
   souvent liées à un problème d’unité de longueur, sont les suivantes :

   #. Adéquation unité de longueur de la CAO/Maillage / unité de position
      du foyer des pointages laser : c’est généralement le cm ou le mm.
      Pour être affichés avec la cible à la bonne taille, un coefficient
      multiplicateur peut être appliqué aux coordonnées du point de
      focalisation des pointages laser car celles-ci sont potentiellement
      exprimées dans une autre unité. Or tout est finalement affiché dans
      un même repère (la vue graphique) qui utilise des grandeurs exprimées
      dans l’unité de longueur de la CAO/Maillage.

      Sous Magix3D l’unité de longueur utilisée est définie par l’item
      *Session.Unité de longueur*, avec pour choix possibles *m*, *cm*,
      *mm*, *um*.

      Sous Lem l’unité utilisée est celle du maillage chargé sur lequel
      sont effectuées les projections. Pour cela, sélectionner le maillage
      dans le panneau *Groupes de données*, puis activer l’item du menu
      *Maillage.Informations*. Dans la boite de dialogue un coefficient
      multiplicateur suit le libellé *Unité* : il permet de connaitre
      l’unité de longueur associée au maillage et utilisée dans la vue
      graphique : 1 pour *m*, -2 pour *cm*, -3 pour *mm*, ...

      En cas d’inadéquation entre les unités de longueur utilisées les
      pointages laser seront suffisamment éloignés de la cible pour ne pas
      être représentés (voir restriction d’affichage) ou visibles dans la
      fenêtre graphique.

   #. Absence de surface/volume de projection des pointages laser : il est
      préférable, surtout avec Lem, de définir une surface (ou volume) sur
      laquelle sont projetés les pointages lasers. Outre l’intérêt
      d’arrêter les pointages laser lorsqu’ils la rencontrent (voir :ref:`projeter-surfaces-ou-volumes`), 
      cette surface est utilisée pour fournir l’unité de longueur
      utilisée pour l’affichage (voir point précédent, elle est récupérée
      auprès du maillage de la surface).



   #. Adéquation unité de longueur dans la définition de la translation
      permettant de passer du repère chambre d’expérience au repère
      maillage, via la boite de dialogue accessible par le menu *Chambre
      d’expérience.Transformation Chambre expérimentale->cible*. La
      translation est définie dans le système métrique du maillage. Là
      encore le risque est que les pointages laser soient suffisamment
      éloignés de la cible pour ne pas être représentés ou visibles.


   #. Problèmes purement numériques : les intersections pointages laser /
      surfaces de projection sont calculées avant affichage de la partie
      visible desdits pointages laser. Ces calculs d’intersection peuvent
      échouer si un côté de la surface de projection a une taille
      supérieure à 500, ce qui correspond généralement à une cible exprimée
      en microns. Dans ce cas une boite de dialogue signale ce risque. Une
      solution est alors de réduire par homothétie les grandeurs d’un
      facteur 1000 et de modifier l’unité de longueur utilisée en passant
      au mm.


   #. Surface de projection plane : dans certains cas, par exemple une
      plaque orthogonale à un axe, une dimension de la boite englobante est
      nulle, ou presque, et de ce fait les pointages laser ne sont pas
      visibles.


   #. Problème de restriction d’affichage des pointages laser : en
      l’absence de restriction les pointages laser affichés font près de 8m
      de long pour une cible de quelques millimètres, ce qui parasite la
      gestion de la vue. Par défaut l’affichage des pointages laser est
      restreint à une boite englobante de taille environ moitié supérieure
      à celle des surfaces de projection, ce qui permet de les voir arriver
      sur la cible. L’utilisateur peut modifier cette boite englobante, via
      le menu *Chambre d’expérience.Boite englobante*, et même la
      désactiver. Une mauvaise définition de la boite englobante
      (positionnement, dimensions) et la partie intéressante des pointages
      laser n’est plus apparente.

.. include:: substitution-images.rst

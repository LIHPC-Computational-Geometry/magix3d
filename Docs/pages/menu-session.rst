.. _menu-session:

Menu Session
~~~~~~~~~~~~

.. _annuler:

Annuler
^^^^^^^

L’action d’annulation d’une commande permet de revenir en
arrière comme nous sommes en droit de nous y attendre, sauf pour les
commandes qui créent un fichier (exportation de modèle ou de maillage).
Le menu *Session/Annuler* du raccourci clavier CTRL+Z. Une icône est
disponible dans la barre des boutons pour faciliter son accès. Le nom de
la commande en question apparait dans le prolongement du menu ainsi que
dans l’aide sous le curseur au niveau du bouton.

Dans la pratique, les entités créées sont marquées comme étant détruites
et n’apparaissent plus. Leur destruction ne sera effective que si une
nouvelle commande prend la suite dans la liste des commandes depuis le
début de la session.

.. _rejouer:

Rejouer
^^^^^^^

Similaire à l’action d’annulation, permet de refaire la
dernière commande annulée, s’il y en a une.

Certaines commandes comme celles de maillage vont refaire l’opération,
alors que les opérations géométriques et topologiques vont simplement
inverser les marques de destruction des entités.

Réinitialiser
^^^^^^^^^^^^^

Permet de revenir à une session vide comme au lancement. Néanmoins, le
fichier d’archivage des commandes contient l’ensemble des commandes
ainsi que la réinitialisation.

.. _unites-longueur:

Unités de longueur
^^^^^^^^^^^^^^^^^^

Ce menu permet de consulter et de modifier l’unité utilisée. Cette unité
n’est pas définie par défaut, elle le devient si vous faites une action
dans ce sens ou si vous importez un fichier qui contient une unité.

.. _repere:

Repère
^^^^^^

Ce menu est utile pour la gestion des faisceaux laser et diagnostics. Il
permet de définir si le maillage que l’on construit est défini dans un
repère absolu lié à la chambre d’expérimentation ou dans un repère
indépendant (dit maillage) et dont on donnera la formule de passage avec
celui de la chambre. 

Voir :ref:`utiliser-contexte`.

Maillage en 2D
^^^^^^^^^^^^^^

Ce menu permet de spécifier que l’on souhaite générer un maillage 2D
lors de la sauvegarde.

.. _importer:

Importer
^^^^^^^^

Ce menu vous permet de sélectionner un fichier à importer. Une
importation peut se faire alors qu’il y a déjà une modélisation de
présente. Vous pouvez opter pour l’un des formats suivant :

-  MDL pour les fichiers de modélisation 2D en provenance de Magix ;

-  CATPart ou CATProduct pour les fichiers de modélisation en provenance
   de Catia ;

-  STP ou STEP pour les fichiers de modélisation dans ce format
   d’échange ;

-  IGS ou IGES pour les fichiers de modélisation dans cet autre format
   d’échange ;

-  MLI pour les fichiers de maillage ;

Si vous optez pour un fichier MDL, les options suivantes vous serons
proposées :

-  Importer la topologie : vous permet de n’importer que la géométrie ou
   d’importer également la topologie (issue de la décomposition en zones
   en 2D).

-  Importation de tous les contours : cette option permet d’importer les
   contours qui ne dépendent pas d’une zone (qui ne sont pas reliés).

-  Conservation du nom des zones : si cette option n’est pas activée, le
   nom des zones ne sont pas utilisées dès lors qu’il y a au moins un
   groupe pour cette zone.

-  Décomposer en opérations élémentaires : permet d’obtenir une
   décomposition en opérations élémentaires (avec ou sans topologie),
   vous obtiendrez alors dans le script de sauvegarde un ensemble de
   commandes Magix3D indépendantes du fichier importé.

-  Préfixe, permet de préfixer les noms des groupes importés.

-  Degrés pour les B-Splines, permet de sélectionner les degrés minimum
   (1 par défaut) et maximum (2 par défaut) lors de la création des
   B-Splines. Ces B-Splines se basent sur les supports et les lignes de
   la modélisation 2D, elles permettent la création des surfaces par
   révolution. Il apparait, à l’usage, que le degré maximum à 2 est le
   plus propres pour les constructions des contours relativement
   curvilignes, mais n’est pas adapté aux cas avec des lignes brisées
   qui oscillent. Dans ce dernier cas il est recommandé de mettre le
   degré maximum à 1. Il n’est pas encore permis de sélectionner un
   degré différent suivant le contour.

|image7|

.. _exporter:

Exporter tout
^^^^^^^^^^^^^

Ce menu est analogue à celui de l’importation. Si la
modélisation est 3D, alors l’exportation en MDL posera un problème
puisque ce format n’accepte que des entités 2D. C’est un moyen de
sauvegarder votre maillage. Le format recommandé pour les maillages est
le *MLI*.

.. _exporter-cao:

Exporter la CAO sélectionnée
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Comme pour l’exportation précédente, mais uniquement pour les
entités sélectionnées, et uniquement pour la géométrie.

.. _exporter-script:

Enregistrer le script Magix3D minimum
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Cette commande accessible avec le raccourci CTRL+S ouvre une
fenêtre pour sélectionner l’emplacement et le nom du fichier pour
sauvegarder les commandes python.

|image11|

Cette sauvegarde met dans un même fichier l’ensemble des commandes
python utiles pour arriver à l’état actuel de la session. Les commandes
annulées ou celles qui auraient échouées n’y apparaitrons pas.

Il existe 3 modes de sauvegarde :

1. Soit les commandes font référence aux noms des entités (mode par
   défaut) ;

2. Soit les commandes font référence à des entités créés dans des
   commandes précédentes ;

3. Soit les commandes utilisent autant que possible des coordonnés pour
   identifier les entités, leur nom sinon ;

Le 1\ :sup:`èr` mode a l’avantage d’être assez facile à suivre lorsque
l’on reprend un script, mais ne permet pas d’extraire un sous ensemble
du script pour le rejouer.

Le 2\ :sup:`ème` mode permet d’extraire un sous-ensemble à la condition
que les commandes extraites ne référencent pas une commande qui ne soit
pas extraite.

Le 3\ :sup:`ème` mode permet d’extraire un sous-ensemble à la condition
que les commandes référencent une et une unique entité dans la session
où cela sera importé.

Après une première sauvegarde, le même fichier sera utilisé (et écrasé)
pour les sauvegardes suivantes. Utiliser la commande qui suit pour ne
pas écraser le fichier précédent.

Enregistrer sous le script Magix3D minimum
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Comme la commande précédente, mais en sélectionnant systématiquement le
nom du fichier.

Exécuter un script Python
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Il est possible de rejouer l’intégralité d’un script mis en place « à la
main » ou sauvegardé lors d’une session Magix3D.

Les noms des entités dans un script importé sont automatiquement adaptés
si la session comporte déjà des entités. Aussi est-t-il facile
d’enchainer plusieurs scripts.

Il est également possible d’importer un script *Erebos*, et ainsi de
créer les objets qui sont donnés en entrée du code. Pour ce faire, il
suffit de lancer le code *Erebos* avec l’option -*visuM3D*.
L’utilisation des balises *MAGIX3D_BEGIN* et *MAGIX3D_END* est obsolète.
Pour le moment, cette fonctionnalité n’est pas complètement
opérationnelle. Certaines des entités créées par *Erebos* ne respectent
pas les contraintes liées à MAgix3D.

Scripts Magix3D récents
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Ce menu vous permet de retrouver facilement un des 10 derniers scripts
exécutés avec succès.

.. include:: substitution-images.rst

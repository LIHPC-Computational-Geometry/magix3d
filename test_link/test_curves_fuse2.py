###
### Reproducer avec un cylindre sans topo
### => pas de courbe composite
###

### Dans le cas composite paramVtx a une valeur négative alors qu'il doit
### être compris entre 0 et 1
### Idée : regarder les valeurs des attributs de la classe Curve : 
    # /// premier paramètre local à une des composantes
    # std::vector<double> paramLocFirst;
    # /// dernier paramètre local à une des composantes
    # std::vector<double> paramLocLast;
    # /// premier paramètre image pour une des composantes, dans [0,1]
    # std::vector<double> paramImgFirst;
    # /// dernier paramètre image pour une des composantes, dans [0,1]
    # std::vector<double> paramImgLast;



ctx.getGeomManager().newCylinder (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(1, 0, 0), 3.600000e+02)
tm.newFreeBoundedTopoInGroup ("aaa", 3, ["Crb0000","Crb0002"])
tm.setGeomAssociation (["Som0001","Som0003"], "Crb0000", True)
tm.setGeomAssociation (["Ar0005"], "Crb0000", True)


Commande Affectation d'une projection vers Crb0000 pour les entités topologiques Ar0005 exécutée avec succès (en 689 microsecondes).
Curve::getParametricsPoints pour 9 points avec la courbe Crb0000, qui possède 1 sommets.
paramPt0 = 2.35619
paramPt1 = 3.92699
first = 0
last = 6.28319
passeParSommet = faux
ratioSeuil = 0
paramVtx (pour i = 0) = 2.51327 => PtI  [ 1, -5.87785252292473e-1, -8.09016994374947e-1] 
paramVtx (pour i = 1) = 2.67035 => PtI  [ 1, -4.53990499739547e-1, -8.91006524188368e-1] 
paramVtx (pour i = 2) = 2.82743 => PtI  [ 1, -3.09016994374948e-1, -9.51056516295154e-1] 
paramVtx (pour i = 3) = 2.98451 => PtI  [ 1, -1.56434465040231e-1, -9.87688340595138e-1] 
paramVtx (pour i = 4) = 3.14159 => PtI  [ 1, -1.22464679914735e-16, -1] 
paramVtx (pour i = 5) = 3.29867 => PtI  [ 1, 1.5643446504023e-1, -9.87688340595138e-1] 
paramVtx (pour i = 6) = 3.45575 => PtI  [ 1, 3.09016994374947e-1, -9.51056516295154e-1] 
paramVtx (pour i = 7) = 3.61283 => PtI  [ 1, 4.53990499739547e-1, -8.91006524188368e-1] 
paramVtx (pour i = 8) = 3.76991 => PtI  [ 1, 5.87785252292473e-1, -8.09016994374948e-1] 
nb_iter = 0, points.size() = 10, init_size = 1
 lgi[0] = 0.156918
 lgi[1] = 0.313836
 lgi[2] = 0.470755
 lgi[3] = 0.627673
 lgi[4] = 0.784591
 lgi[5] = 0.941509
 lgi[6] = 1.09843
 lgi[7] = 1.25535
 lgi[8] = 1.41226
 lgi[9] = 1.56918
 ratios[0] = 0.1 ecarts[0] = 2.77556e-17
 ratios[1] = 0.2 ecarts[1] = -5.55112e-17
 ratios[2] = 0.3 ecarts[2] = -2.22045e-16
 ratios[3] = 0.4 ecarts[3] = -1.11022e-16
 ratios[4] = 0.5 ecarts[4] = -2.22045e-16
 ratios[5] = 0.6 ecarts[5] = 1.11022e-16
 ratios[6] = 0.7 ecarts[6] = 0
 ratios[7] = 0.8 ecarts[7] = -2.22045e-16
 ratios[8] = 0.9 ecarts[8] = 0
ecart_max = 2.22045e-16
ecart_tol = 0.00156918
Curve::getParametricsPoints pour 9 points avec la courbe Crb0000, qui possède 1 sommets.
paramPt0 = 2.35619
paramPt1 = 3.92699
first = 0
last = 6.28319
passeParSommet = faux
ratioSeuil = 0
paramVtx (pour i = 0) = 2.51327 => PtI  [ 1, -5.87785252292473e-1, -8.09016994374947e-1] 
paramVtx (pour i = 1) = 2.67035 => PtI  [ 1, -4.53990499739547e-1, -8.91006524188368e-1] 
paramVtx (pour i = 2) = 2.82743 => PtI  [ 1, -3.09016994374948e-1, -9.51056516295154e-1] 
paramVtx (pour i = 3) = 2.98451 => PtI  [ 1, -1.56434465040231e-1, -9.87688340595138e-1] 
paramVtx (pour i = 4) = 3.14159 => PtI  [ 1, -1.22464679914735e-16, -1] 
paramVtx (pour i = 5) = 3.29867 => PtI  [ 1, 1.5643446504023e-1, -9.87688340595138e-1] 
paramVtx (pour i = 6) = 3.45575 => PtI  [ 1, 3.09016994374947e-1, -9.51056516295154e-1] 
paramVtx (pour i = 7) = 3.61283 => PtI  [ 1, 4.53990499739547e-1, -8.91006524188368e-1] 
paramVtx (pour i = 8) = 3.76991 => PtI  [ 1, 5.87785252292473e-1, -8.09016994374948e-1] 
nb_iter = 0, points.size() = 10, init_size = 1
 lgi[0] = 0.156918
 lgi[1] = 0.313836
 lgi[2] = 0.470755
 lgi[3] = 0.627673
 lgi[4] = 0.784591
 lgi[5] = 0.941509
 lgi[6] = 1.09843
 lgi[7] = 1.25535
 lgi[8] = 1.41226
 lgi[9] = 1.56918
 ratios[0] = 0.1 ecarts[0] = 2.77556e-17
 ratios[1] = 0.2 ecarts[1] = -5.55112e-17
 ratios[2] = 0.3 ecarts[2] = -2.22045e-16
 ratios[3] = 0.4 ecarts[3] = -1.11022e-16
 ratios[4] = 0.5 ecarts[4] = -2.22045e-16
 ratios[5] = 0.6 ecarts[5] = 1.11022e-16
 ratios[6] = 0.7 ecarts[6] = 0
 ratios[7] = 0.8 ecarts[7] = -2.22045e-16
 ratios[8] = 0.9 ecarts[8] = 0
ecart_max = 2.22045e-16
ecart_tol = 0.00156918
 ligne 0 Le bloc Bl0000 n'est associé à aucun volume



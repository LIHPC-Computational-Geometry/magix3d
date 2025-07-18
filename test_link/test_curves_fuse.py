import os
import pyMagix3D as Mgx3D

step_file_name = "mambo/Simple/S39.step"

def test_curves_fuse_s45():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    magix3d_test_data_dir = os.environ['MAGIX3D_TEST_DATA_DIR']
    full_path = os.path.join(magix3d_test_data_dir, step_file_name)

    ctx.setLengthUnit(Mgx3D.Unit.meter)
    gm.importSTEP(full_path)
    assert gm.getNbVolumes()==1

    gm.joinSurfaces(["Surf0000","Surf0006"])
    gm.joinCurves (["Crb0000","Crb0001","Crb0023"])
    gm.joinCurves (["Crb0022","Crb0003"])
    tm.newFreeBoundedTopoInGroup ("aaa", 3, ["Crb0047","Crb0048"])
    tm.setGeomAssociation (["Som0003","Som0007"], "Crb0047", True)
    tm.setGeomAssociation (["Ar0011"], "Crb0047", True)

    # This command fails with
    # [Erreur interne] La courbe Crb0047 est une courbe composée ce qui ne permet pas de trouver les points en fonction d'une paramétrisation
    ctx.getMeshManager().newAllBlocksMesh()

-------------------------------------------------------------------

import os

step_file_name = "mambo/Simple/S39.step"
magix3d_test_data_dir = os.environ['MAGIX3D_TEST_DATA_DIR']
full_path = os.path.join(magix3d_test_data_dir, step_file_name)

ctx.setLengthUnit(Mgx3D.Unit.meter)
gm.importSTEP(full_path)

gm.joinSurfaces(["Surf0000","Surf0006"])
gm.joinCurves (["Crb0000","Crb0001","Crb0023"])
gm.joinCurves (["Crb0022","Crb0003"])
tm.newFreeBoundedTopoInGroup ("aaa", 3, ["Crb0047","Crb0048"])
tm.setGeomAssociation (["Som0003","Som0007"], "Crb0047", True)
tm.setGeomAssociation (["Ar0011"], "Crb0047", True)

ctx.getMeshManager().newAllBlocksMesh()

-----------------------------------

Commande Import STEP exécutée avec succès (en 44.3 millisecondes).
Commande Fusion de surfaces Surf0000 Surf0006 exécutée avec succès (en 2.26 millisecondes).
Commande Fusion de courbes Crb0000 Crb0001 Crb0023 exécutée avec succès (en 1.62 millisecondes).
Commande Fusion de courbes Crb0022 Crb0003 exécutée avec succès (en 3.36 millisecondes).
Commande Création d'un bloc unitaire mis dans le groupe aaa exécutée avec succès (en 2.76 millisecondes).
Commande Affectation d'une projection vers Crb0047 pour les entités topologiques Som0003 Som0007 exécutée avec succès (en 1.26 millisecondes).
Commande Affectation d'une projection vers Crb0047 pour les entités topologiques Ar0011 exécutée avec succès (en 655 microsecondes).
Curve::getParametricsPoints pour 9 points avec la courbe Crb0047, qui possède 1 sommets.
Curve::getParameter pour pt  [ 0.02, 4.77560969208601e-2, -6.31212805274300e-3] 
Projected point: 0.02, 0.04775609692086, -0.006312128052743
Parameter on curve: 1.2983336619674
First p 0.34830460804467 Last p 6.2831853071796
  => p = 1.2983336619674
Curve::getParameter pour pt  [ 0.02, 4.70710678118655e-2, 7.07106781186547e-3] 
Projected point: 0.02, 0.047071067811865, 0.0070710678118655
Parameter on curve: 2.4350056664577
First p 0.34830460804467 Last p 6.2831853071796
  => p = 2.4350056664577
paramPt0 = 1.2983336619674 pour  [ 0.02, 4.77560969208601e-2, -6.31212805274300e-3] 
paramPt1 = 2.4350056664577 pour  [ 0.02, 4.70710678118655e-2, 7.07106781186547e-3] 
first = 0
last = 3
passeParSommet = faux
ratioSeuil = 0
paramVtx (pour i = 0) = 1.4120008624165 => PtI  [ 0.02, 4.25270763529913e-2, -9.67542686945400e-3] 
paramVtx (pour i = 1) = 1.5256680628655 => PtI  [ 0.02, 4.32021991930792e-2, -9.47343234144010e-3] 
paramVtx (pour i = 2) = 1.6393352633145 => PtI  [ 0.02, 4.38614201477023e-2, -9.22439344580000e-3] 
paramVtx (pour i = 3) = 1.7530024637635 => PtI  [ 0.02, 4.45014655741283e-2, -8.92954689135670e-3] 
paramVtx (pour i = 4) = 1.8666696642126 => PtI  [ 0.02, 4.5119157053886e-2, -8.59035686439450e-3] 
paramVtx (pour i = 5) = 1.9803368646616 => PtI  [ 0.02, 4.5711427176558e-2, -8.208507757618e-3] 
paramVtx (pour i = 6) = 2.0940040651106 => PtI  [ 0.02, 4.75596151104979e-2, -6.54616065958760e-3] 
paramVtx (pour i = 7) = 2.2076712655596 => PtI  [ 0.02, 4.91018637992785e-2, -4.14198930218130e-3] 
paramVtx (pour i = 8) = 2.3213384660087 => PtI  [ 0.02, 4.99015304736954e-2, -1.39989080948530e-3] 
nb_iter = 0, points.size() = 10, init_size = 1
 lgi[0] = 0.0062172690974161
 lgi[1] = 0.0069219623913787
 lgi[2] = 0.0076266556853413
 lgi[3] = 0.0083313489793039
 lgi[4] = 0.0090360422732666
 lgi[5] = 0.0097407355672292
 lgi[6] = 0.01222653453607
 lgi[7] = 0.015082855759921
 lgi[8] = 0.017939176983771
 lgi[9] = 0.026870507167156
 ratios[0] = 0.1 ecarts[0] = -0.0035302183807004
 ratios[1] = 0.2 ecarts[1] = -0.0015478609579474
 ratios[2] = 0.3 ecarts[2] = 0.00043449646480562
 ratios[3] = 0.4 ecarts[3] = 0.0024168538875586
 ratios[4] = 0.5 ecarts[4] = 0.0043992113103117
 ratios[5] = 0.6 ecarts[5] = 0.0063815687330647
 ratios[6] = 0.7 ecarts[6] = 0.0065828204809393
 ratios[7] = 0.8 ecarts[7] = 0.0064135499738045
 ratios[8] = 0.9 ecarts[8] = 0.0062442794666697
ecart_max = 0.0065828204809393
ecart_tol = 2.6870507167156e-05
paramVtx (pour i = 0) = 0.84257577529596
paramVtx (pour i = 1) = 1.2759977122582
paramVtx (pour i = 2) = 1.7094196492205
paramVtx (pour i = 3) = 2.1428415861828
paramVtx (pour i = 4) = 2.5762635231451
paramVtx (pour i = 5) = 2.2721444731822
paramVtx (pour i = 6) = 2.3559671695678
paramVtx (pour i = 7) = 2.4628982562689
paramVtx (pour i = 8) = 2.4008081251999
nb_iter = 1, points.size() = 10, init_size = 1
 lgi[0] = 0.01274803634883
 lgi[1] = 0.019282854178473
 lgi[2] = 0.021962379657185
 lgi[3] = 0.027300491756213
 lgi[4] = 0.03769342324007
 lgi[5] = 0.045175281278057
 lgi[6] = 0.047284940650752
 lgi[7] = 0.049973050722729
 lgi[8] = 0.051537056433228
 lgi[9] = 0.058633667205309
 ratios[0] = -0.40095813468712 ecarts[0] = -0.0068846696282989
 ratios[1] = -0.01965030336013 ecarts[1] = -0.0075561207374109
 ratios[2] = 0.36165752796686 ecarts[2] = -0.0043722794955923
 ratios[3] = 0.74296535929386 ecarts[3] = -0.0038470248740898
 ratios[4] = 1.1242731906209 ecarts[4] = -0.0083765896374154
 ratios[5] = 0.85672103066484 ecarts[5] = -0.0099950809548713
 ratios[6] = 0.93046499203144 ecarts[6] = -0.0062413736070354
 ratios[7] = 1.0245388200827 ecarts[7] = -0.0030661169584816
 ratios[8] = 0.96991432785998 ecarts[8] = 0.0012332440515504
ecart_max = 0.0099950809548713
ecart_tol = 2.6870507167156e-05
paramVtx (pour i = 0) = 0.38594991005654
paramVtx (pour i = 1) = 0.053770675438541
paramVtx (pour i = 2) = 1.3544173913502
paramVtx (pour i = 3) = 1.9824070663305
paramVtx (pour i = 4) = 2.9167512027463
paramVtx (pour i = 5) = 1.8750118141977
paramVtx (pour i = 6) = 2.1076898131958
paramVtx (pour i = 7) = 2.5846213436893
paramVtx (pour i = 8) = 2.4067509499775
Curve::getParametricsPoints pour 9 points avec la courbe Crb0047, qui possède 1 sommets.
Curve::getParameter pour pt  [ 0.02, 4.77560969208601e-2, -6.31212805274300e-3] 
Projected point: 0.02, 0.04775609692086, -0.006312128052743
Parameter on curve: 1.2983336619674
First p 0.34830460804467 Last p 6.2831853071796
  => p = 1.2983336619674
Curve::getParameter pour pt  [ 0.02, 4.70710678118655e-2, 7.07106781186547e-3] 
Projected point: 0.02, 0.047071067811865, 0.0070710678118655
Parameter on curve: 2.4350056664577
First p 0.34830460804467 Last p 6.2831853071796
  => p = 2.4350056664577
paramPt0 = 1.2983336619674 pour  [ 0.02, 4.77560969208601e-2, -6.31212805274300e-3] 
paramPt1 = 2.4350056664577 pour  [ 0.02, 4.70710678118655e-2, 7.07106781186547e-3] 
first = 0
last = 3
passeParSommet = faux
ratioSeuil = 0
paramVtx (pour i = 0) = 1.4120008624165 => PtI  [ 0.02, 4.25270763529913e-2, -9.67542686945400e-3] 
paramVtx (pour i = 1) = 1.5256680628655 => PtI  [ 0.02, 4.32021991930792e-2, -9.47343234144010e-3] 
paramVtx (pour i = 2) = 1.6393352633145 => PtI  [ 0.02, 4.38614201477023e-2, -9.22439344580000e-3] 
paramVtx (pour i = 3) = 1.7530024637635 => PtI  [ 0.02, 4.45014655741283e-2, -8.92954689135670e-3] 
paramVtx (pour i = 4) = 1.8666696642126 => PtI  [ 0.02, 4.5119157053886e-2, -8.59035686439450e-3] 
paramVtx (pour i = 5) = 1.9803368646616 => PtI  [ 0.02, 4.5711427176558e-2, -8.208507757618e-3] 
paramVtx (pour i = 6) = 2.0940040651106 => PtI  [ 0.02, 4.75596151104979e-2, -6.54616065958760e-3] 
paramVtx (pour i = 7) = 2.2076712655596 => PtI  [ 0.02, 4.91018637992785e-2, -4.14198930218130e-3] 
paramVtx (pour i = 8) = 2.3213384660087 => PtI  [ 0.02, 4.99015304736954e-2, -1.39989080948530e-3] 
nb_iter = 0, points.size() = 10, init_size = 1
 lgi[0] = 0.0062172690974161
 lgi[1] = 0.0069219623913787
 lgi[2] = 0.0076266556853413
 lgi[3] = 0.0083313489793039
 lgi[4] = 0.0090360422732666
 lgi[5] = 0.0097407355672292
 lgi[6] = 0.01222653453607
 lgi[7] = 0.015082855759921
 lgi[8] = 0.017939176983771
 lgi[9] = 0.026870507167156
 ratios[0] = 0.1 ecarts[0] = -0.0035302183807004
 ratios[1] = 0.2 ecarts[1] = -0.0015478609579474
 ratios[2] = 0.3 ecarts[2] = 0.00043449646480562
 ratios[3] = 0.4 ecarts[3] = 0.0024168538875586
 ratios[4] = 0.5 ecarts[4] = 0.0043992113103117
 ratios[5] = 0.6 ecarts[5] = 0.0063815687330647
 ratios[6] = 0.7 ecarts[6] = 0.0065828204809393
 ratios[7] = 0.8 ecarts[7] = 0.0064135499738045
 ratios[8] = 0.9 ecarts[8] = 0.0062442794666697
ecart_max = 0.0065828204809393
ecart_tol = 2.6870507167156e-05
paramVtx (pour i = 0) = 0.84257577529596
paramVtx (pour i = 1) = 1.2759977122582
paramVtx (pour i = 2) = 1.7094196492205
paramVtx (pour i = 3) = 2.1428415861828
paramVtx (pour i = 4) = 2.5762635231451
paramVtx (pour i = 5) = 2.2721444731822
paramVtx (pour i = 6) = 2.3559671695678
paramVtx (pour i = 7) = 2.4628982562689
paramVtx (pour i = 8) = 2.4008081251999
nb_iter = 1, points.size() = 10, init_size = 1
 lgi[0] = 0.01274803634883
 lgi[1] = 0.019282854178473
 lgi[2] = 0.021962379657185
 lgi[3] = 0.027300491756213
 lgi[4] = 0.03769342324007
 lgi[5] = 0.045175281278057
 lgi[6] = 0.047284940650752
 lgi[7] = 0.049973050722729
 lgi[8] = 0.051537056433228
 lgi[9] = 0.058633667205309
 ratios[0] = -0.40095813468712 ecarts[0] = -0.0068846696282989
 ratios[1] = -0.01965030336013 ecarts[1] = -0.0075561207374109
 ratios[2] = 0.36165752796686 ecarts[2] = -0.0043722794955923
 ratios[3] = 0.74296535929386 ecarts[3] = -0.0038470248740898
 ratios[4] = 1.1242731906209 ecarts[4] = -0.0083765896374154
 ratios[5] = 0.85672103066484 ecarts[5] = -0.0099950809548713
 ratios[6] = 0.93046499203144 ecarts[6] = -0.0062413736070354
 ratios[7] = 1.0245388200827 ecarts[7] = -0.0030661169584816
 ratios[8] = 0.96991432785998 ecarts[8] = 0.0012332440515504
ecart_max = 0.0099950809548713
ecart_tol = 2.6870507167156e-05
paramVtx (pour i = 0) = 0.38594991005654
paramVtx (pour i = 1) = 0.053770675438541
paramVtx (pour i = 2) = 1.3544173913502
paramVtx (pour i = 3) = 1.9824070663305
paramVtx (pour i = 4) = 2.9167512027463
paramVtx (pour i = 5) = 1.8750118141977
paramVtx (pour i = 6) = 2.1076898131958
paramVtx (pour i = 7) = 2.5846213436893
paramVtx (pour i = 8) = 2.4067509499775
 ligne 0 Des polygones semblent retournés dans la face structurée Fa0001 (normales de directions opposées dans une même face, ce n'est pas forcément un pb)
 ligne 0 Des polygones semblent retournés dans la face structurée Fa0003 (normales de directions opposées dans une même face, ce n'est pas forcément un pb)
 ligne 0 Le bloc Bl0000 n'est associé à aucun volume
Commande Création du maillage pour tous les blocs exécutée avec succès (en 30.7 millisecondes).

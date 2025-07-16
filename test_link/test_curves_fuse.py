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
    #ctx.getMeshManager().newAllBlocksMesh()
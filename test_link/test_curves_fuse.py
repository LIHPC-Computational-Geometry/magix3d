import os
import pytest
import math
import pyMagix3D as Mgx3D

step_file_name = "mambo/Simple/S39.step"

def test_curves_fuse_s45(capfd):
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

    # On teste la création d'un point par abscisse curviligne
    create_curviline_points(gm, "Crb0047")

    ctx.getMeshManager().newAllBlocksMesh()
    out, err = capfd.readouterr()
    unexpected = "Des polygones semblent retournés dans la face structurée"
    assert unexpected not in out

def test_reference(capfd):
    # maillage d'un cylindre sans courbe composite
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    gm.newCylinder (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(1, 0, 0), 3.600000e+02)
    tm.newFreeBoundedTopoInGroup ("aaa", 3, ["Crb0000","Crb0002"])
    tm.setGeomAssociation (["Som0001","Som0003"], "Crb0000", True)
    tm.setGeomAssociation (["Ar0005"], "Crb0000", True)

    # On teste la création d'un point par abscisse curviligne
    create_curviline_points(gm, "Crb0000")

    ctx.getMeshManager().newAllBlocksMesh()
    out, err = capfd.readouterr()
    unexpected = "Des polygones semblent retournés dans la face structurée"
    assert unexpected not in out

def coord_are_equals(gm, pt1, pt2):
    c1 = gm.getCoord(pt1)
    c2 = gm.getCoord(pt2)
    assert math.isclose(c1.getX(), c2.getX(), abs_tol=1e-15)
    assert math.isclose(c1.getY(), c2.getY(), abs_tol=1e-15)
    assert math.isclose(c1.getZ(), c2.getZ(), abs_tol=1e-15)

def create_curviline_points(gm, curve):
    gm.newVertex (curve, 5.000000e-01)

    pt_start = gm.newVertex (curve, 0.000000e+00)
    coord_are_equals(gm, pt_start.getVertex(), gm.getInfos(curve, 1).vertices()[0])

    pt_end = gm.newVertex (curve, 1.000000e+00)
    coord_are_equals(gm, pt_end.getVertex(), pt_start.getVertex())

    with pytest.raises(RuntimeError) as excinfo:
      gm.newVertex (curve, 2.000000e+00)
    assert "Le paramètre doit être compris entre 0 et 1" in str(excinfo.value)

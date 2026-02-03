import os
import pyMagix3D as Mgx3D
import math

step_file_name = "cube_sphere.step"

def test_export_import_step():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    # Changement d'unité de longueur
    ctx.setLengthUnit(Mgx3D.Unit.meter)
    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création d'une sphère avec une topologie
    tm.newSphereWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.ENTIER, True, .5, 10, 10)
    # Fusion booléenne entre géométries avec topologies
    gm.fuse(["Vol0000", "Vol0001"])
    # Export STEP total (géométrie)
    gm.exportSTEP(step_file_name)
    assert os.path.exists(step_file_name)
    assert os.path.getsize(step_file_name) > 0
    # Destruction de tout, les commandes précédentes n'ont plus lieu d'être !
    ctx.clearSession()
    # Import STEP
    # Changement d'unité de longueur
    ctx.setLengthUnit(Mgx3D.Unit.meter)
    # Import STEP
    gm.importSTEP(step_file_name)
    assert gm.getNbVolumes()==1

def test_import_iges():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    # Changement d'unité de longueur
    ctx.setLengthUnit(Mgx3D.Unit.meter)
    # Import IGES
    gm.importIGES("ex1.iges")
    # pb de conversion d'unité lors du passage OCC 7.8.1
    c = gm.getCoord("Pt0000")
    assert math.isclose(c.getX(), -5e-6, abs_tol=1e-7)
    assert math.isclose(c.getY(), 2.5e-6, abs_tol=1e-7)
    assert math.isclose(c.getZ(), 0, abs_tol=1e-7)
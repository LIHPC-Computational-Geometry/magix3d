import sys
import pyMagix3D as Mgx3D


def test_export_import_step():
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()
    mm = ctx.getMeshManager ()
    # Changement d'unité de longueur
    ctx.setLengthUnit(Mgx3D.Unit.meter)
    # Création de la boite Vol0000
    ctx.getGeomManager().newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création d'une sphère avec une topologie
    ctx.getTopoManager().newSphereWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.ENTIER, True, .5, 10, 10)
    # Fusion booléenne entre géométries avec topologies
    ctx.getGeomManager().fuse(["Vol0000", "Vol0001"])
    # Export STEP total (géométrie)
    ctx.getGeomManager().exportSTEP("cube_sphere.step")
    # Destruction de tout, les commandes précédentes n'ont plus lieu d'être !
    ctx.clearSession()
    # Import STEP
    # Changement d'unité de longueur
    ctx.setLengthUnit(Mgx3D.Unit.meter)
    # Import STEP
    ctx.getGeomManager().importSTEP("cube_sphere.step")
    assert gm.getNbVolumes()==1
    #This last command is mandatory to clean the session for the next test
    ctx.clearSession()

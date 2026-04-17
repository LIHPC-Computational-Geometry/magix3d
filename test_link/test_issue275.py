import pyMagix3D as Mgx3D
import math

# Problème de fonction glue
def test_issue275_faces():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(1.2, 1, 1), 10, 10, 10)
    # Découpage des faces structurées Fa0001 suivant l'arête Ar0006 avec comme ratio 5.000000e-01
    tm.splitFaces (["Fa0001"], "Ar0006", .5, .5, True)
    # Découpage des faces structurées Fa0006 suivant l'arête Ar0014 avec comme ratio 5.000000e-01
    tm.splitFaces (["Fa0006"], "Ar0014", .5, .5, True)
    # Collage entre géométries avec topologies
    gm.glue (["Vol0000","Vol0001"])

    # avant correction = 14 faces
    assert tm.getNbFaces() == 12

def test_issue275_ogrid():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(1.2, 1, 1), 10, 10, 10)
    # Découpage en O-grid des blocs structurés Bl0000
    tm.splitBlocksWithOgridV2 (["Bl0000"], ["Fa0001","Fa0000"], .5, 10)
    # Découpage en O-grid des blocs structurés Bl0001
    tm.splitBlocksWithOgridV2 (["Bl0001"], ["Fa0007","Fa0006"], .5, 10)
    # Collage entre géométries avec topologies
    gm.glue (["Vol0000","Vol0001"])

    # avant correction = 44 faces
    assert tm.getNbFaces() == 39

import pyMagix3D as Mgx3D
import pytest

def test_needle(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    angle = 5
    r1 = 0.5
    r2 = 0.6
    r3 = 0.7
    r4 = 0.8

    # Création d'une aiguille pleine avec une topologie
    tm.newSpherePartWithTopo(r1, angle, angle, 5, 1, 1)
    # Création d'une aiguille creuse avec une topologie
    tm.newHollowSpherePartWithTopo(r1, r2, 5, 5, 1, 2, 2)
    # Création d'une aiguille creuse avec une topologie
    tm.newHollowSpherePartWithTopo(r2, r3, 5, 5, 1, 4, 4)
    # Création d'une aiguille creuse avec une topologie
    tm.newHollowSpherePartWithTopo(r3, r4, 5, 5, 1, 8, 8)
    # Collage
    gm.glue(gm.getVolumes())
    # Maillage
    mm.newAllBlocksMesh()
    assert mm.getNbRegions() == 89

    out, err = capfd.readouterr()
    assert len(err) == 0
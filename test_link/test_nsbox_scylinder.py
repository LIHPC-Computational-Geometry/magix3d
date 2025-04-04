import pyMagix3D as Mgx3D

def test_nsbox_scylinder(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    ctx.clearSession() # Clean the session after the previous test
    # Création d'un cylindre avec une topologie
    tm.newCylinderWithTopo(Mgx3D.Point(0, 0, 0), .2, Mgx3D.Vector(1, 0, 0), 90, True, .5, 10, 10, 10, "A")
    # Création d'une boite
    gm.newBox(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), "B")
    # Rotation d'une géométrie
    gm.rotate(["Vol0000"], Mgx3D.RotX(270))
    # Collage entre géométries
    gm.glue(["Vol0001", "Vol0000"])
    # Création d'un bloc topologique non structuré sur une géométrie
    tm.newUnstructuredTopoOnGeometry("Vol0002")

    out, err = capfd.readouterr()
    assert len(err) == 0


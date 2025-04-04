import pyMagix3D as Mgx3D

def test_glue_with_joined_surfaces(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()

    # Création du volume Vol0000
    gm.newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.DEMI, "A")
    # Création du volume Vol0001
    gm.newSphere (Mgx3D.Point(0, 0, 0), 2, Mgx3D.Portion.DEMI, "B")
    # Fusion de surfaces Surf0005 Surf0004
    gm.joinSurfaces (["Surf0005","Surf0004"])
    # Collage entre Vol0000 Vol0001
    gm.glue (["Vol0000","Vol0001"])

    assert gm.getNbSurfaces() == 6
    assert gm.getNbVolumes() == 2
    out, err = capfd.readouterr()
    assert len(err) == 0

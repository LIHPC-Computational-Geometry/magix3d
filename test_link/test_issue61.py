import pyMagix3D as Mgx3D

def test_issue61():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    gn = "test"
    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), gn)
    assert gm.getInfos("Vol0000", 3).groups() == [gn]

    # Création d'un bloc unitaire mis dans le groupe test
    tm.newFreeTopoInGroup (gn, 3)
    assert tm.getInfos("Bl0000", 3).groups() == [gn]

    # Affectation d'une projection vers Vol0000 pour les entités topologiques Bl0000
    tm.setGeomAssociation (["Bl0000"], "Vol0000", False)
    # lors de l'association, Bl0000 doit être retiré de gn car il en "hérite" par la géométrie
    assert gn not in tm.getInfos("Bl0000", 3).groups()

    # test du undo
    ctx.undo()
    assert tm.getInfos("Bl0000", 3).groups() == [gn]

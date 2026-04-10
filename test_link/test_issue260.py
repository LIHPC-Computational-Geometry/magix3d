import pyMagix3D as Mgx3D

def test_issue260(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    # Création de la boite Vol0000
    ctx.getGeomManager().newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), "aaa")
    # Création d'un bloc unitaire mis dans le groupe aaa
    tm.newFreeTopoInGroup ("aaa", 3)
    # Découpage des faces structurées Fa0005 suivant l'arête Ar0007 avec comme ratio 2.000000e-01
    tm.splitFaces (["Fa0005"], "Ar0007", .2, .5, True)
    # Découpage des faces structurées Fa0007 suivant l'arête Ar0013 avec comme ratio 6.000000e-01
    tm.splitFaces (["Fa0007"], "Ar0013", .6, .5, True)
    assert tm.getInfos("Ar0019", 1).edges() == ["Edge0006", "Edge0019"]

    # Découpage en O-grid des blocs structurés Bl0000
    ctx.getTopoManager().splitBlocksWithOgridV2 (["Bl0000"], ["Fa0000","Fa0009","Fa0008","Fa0006"], .5, 10)
    assert tm.getInfos("Ar0019", 1).edges() == ["Edge0019", "Edge0076"]
    assert tm.getInfos("Edge0076", 1) is not None

    ctx.undo()
    # Edge0076 ne devrait plus appartenir a Ar0019. Or c'est le cas et elle est détruite
    assert tm.getInfos("Ar0019", 1).edges() == ["Edge0006", "Edge0019"]

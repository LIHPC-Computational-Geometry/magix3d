import pyMagix3D as Mgx3D

def test_new_block_with_vertices():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()
    # Création de la boite Vol0000
    ctx.getGeomManager().newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création d'un sommet topologique (Pt0000)
    ctx.getTopoManager().newTopoOnGeometry ("Pt0000")
    # Création d'un sommet topologique (Pt0001)
    ctx.getTopoManager().newTopoOnGeometry ("Pt0001")
    # Création d'un sommet topologique (Pt0002)
    ctx.getTopoManager().newTopoOnGeometry ("Pt0002")
    # Création d'un sommet topologique (Pt0003)
    ctx.getTopoManager().newTopoOnGeometry ("Pt0003")
    # Création d'un sommet topologique (Pt0004)
    ctx.getTopoManager().newTopoOnGeometry ("Pt0004")
    # Création d'un sommet topologique (Pt0005)
    ctx.getTopoManager().newTopoOnGeometry ("Pt0005")
    # Création d'un sommet topologique (Pt0006)
    ctx.getTopoManager().newTopoOnGeometry ("Pt0006")
    # Création d'un sommet topologique (Pt0007)
    ctx.getTopoManager().newTopoOnGeometry ("Pt0007")

    tm.newTopoEntity(["Som0000", "Som0001", "Som0002", "Som0003", "Som0004", "Som0005", "Som0006", "Som0007"], 3, "GB")
    assert tm.getNbBlocks()==1
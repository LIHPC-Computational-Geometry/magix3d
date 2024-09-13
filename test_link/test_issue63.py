import pyMagix3D as Mgx3D

# Transfini par défaut à la place de directional
def test_issue63():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, 0, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(2, 0, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(2, 1, 0))
    # Création du sommet Pt0003
    gm.newVertex (Mgx3D.Point(0, 1, 0))
    # Création du segment Crb0000
    gm.newSegment("Pt0000", "Pt0001")
    # Création du segment Crb0001
    gm.newSegment("Pt0001", "Pt0002")
    # Création du segment Crb0002
    gm.newSegment("Pt0002", "Pt0003")
    # Création du segment Crb0003
    gm.newSegment("Pt0003", "Pt0000")
    # Création de la surface Surf0000
    gm.newPlanarSurface( ["Crb0000", "Crb0001", "Crb0002", "Crb0003"] , "")
    # Création d'une face topologique structurée sur une géométrie (Surf0000)
    tm.newStructuredTopoOnGeometry ("Surf0000")
    assert len(tm.getTransfiniteMeshLawFaces()) == 1
    assert('Fa0000' in tm.getTransfiniteMeshLawFaces())

    # ctx.clearSession() # Clean the session after the previous test
    # # Création du sommet Pt0000
    # gm.newVertex (Mgx3D.Point(0, 0, 0))
    # # Création du sommet Pt0001
    # gm.newVertex (Mgx3D.Point(2, 0, 0))
    # # Création du sommet Pt0002
    # gm.newVertex (Mgx3D.Point(2, 1, 0))
    # # Création du sommet Pt0003
    # gm.newVertex (Mgx3D.Point(0, 1, 0))
    # # Création du segment Crb0000
    # gm.newSegment("Pt0000", "Pt0001")
    # # Création du segment Crb0001
    # gm.newSegment("Pt0001", "Pt0002")
    # # Création du segment Crb0002
    # gm.newSegment("Pt0002", "Pt0003")
    # # Création du segment Crb0003
    # gm.newSegment("Pt0003", "Pt0000")
    # # Création de la surface Surf0000
    # gm.newPlanarSurface( ["Crb0000", "Crb0001", "Crb0002", "Crb0003"] , "")
    # # Création d'une face topologique structurée sur une géométrie (Surf0000)
    # tm.newStructuredTopoOnGeometry ("Surf0000")
    # # Découpage de la face Fa0000
    # tm.splitFace ("Fa0000", "Ar0001", .5, True)
    # # Construction Topo et Geom 3D avec o-grid par révolution
    # tm.makeBlocksByRevol (["Ar0008"], Mgx3D.Portion.DEMI)

    # tm.getTransfiniteMeshLawBlocks()
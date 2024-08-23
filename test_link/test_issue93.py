import pyMagix3D as Mgx3D

# 2 edges not associated with geometry => after makeBlocksByRevol
# SomXXXX has wrong association with the vertex
def test_issue93():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, 0, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(0, 0, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(1, 0, 0))
    # Création du sommet Pt0003
    gm.newVertex (Mgx3D.Point(1, .5, 0))
    # Création du sommet Pt0004
    gm.newVertex (Mgx3D.Point(0, .5, 0))
    # Création du sommet Pt0005
    gm.newVertex (Mgx3D.Point(1.5, 1, 0))
    # Création du sommet Pt0006
    gm.newVertex (Mgx3D.Point(.5, 1, 0))
    # Création du segment Crb0000
    gm.newSegment("Pt0001", "Pt0002")
    # Création du segment Crb0001
    gm.newSegment("Pt0002", "Pt0003")
    # Création du sommet Pt0007
    gm.newVertex (Mgx3D.Point(3, 1, 0))
    # Création du sommet Pt0008
    gm.newVertex (Mgx3D.Point(3, 1, 0))
    # Création du sommet Pt0009
    gm.newVertex (Mgx3D.Point(3, 2, 0))
    # Création du segment Crb0002
    gm.newSegment("Pt0003", "Pt0005")
    # Création du segment Crb0003
    gm.newSegment("Pt0005", "Pt0008")
    # Création du segment Crb0004
    gm.newSegment("Pt0008", "Pt0009")
    # Création du segment Crb0005
    gm.newSegment("Pt0009", "Pt0006")
    # Création du segment Crb0006
    gm.newSegment("Pt0006", "Pt0004")
    # Création du segment Crb0007
    gm.newSegment("Pt0004", "Pt0001")
    # Création de la surface Surf0000
    gm.newPlanarSurface( ["Crb0000", "Crb0001", "Crb0002", "Crb0003", "Crb0004", "Crb0005", "Crb0006", "Crb0007"] , "")
    # Création d'une face topologique structurée sans projection (Surf0000)
    tm.newFreeTopoOnGeometry ("Surf0000")
    # Découpage de toutes les faces 2D structurées
    tm.splitAllFaces ("Ar0002", .3, .5)
    # Découpage de toutes les faces 2D structurées
    tm.splitAllFaces ("Ar0005", .5, .5)
    # Affectation d'une projection vers Pt0001 pour les entités topologiques Som0001
    tm.setGeomAssociation (["Som0001"], "Pt0001", True)
    # Affectation d'une projection vers Pt0002 pour les entités topologiques Som0002
    tm.setGeomAssociation (["Som0002"], "Pt0002", True)
    # Affectation d'une projection vers Pt0003 pour les entités topologiques Som0004
    tm.setGeomAssociation (["Som0004"], "Pt0003", True)
    # Affectation d'une projection vers Pt0005 pour les entités topologiques Som0006
    tm.setGeomAssociation (["Som0006"], "Pt0005", True)
    # Affectation d'une projection vers Pt0004 pour les entités topologiques Som0005
    tm.setGeomAssociation (["Som0005"], "Pt0004", True)
    # Affectation d'une projection vers Pt0006 pour les entités topologiques Som0007
    tm.setGeomAssociation (["Som0007"], "Pt0006", True)
    # Affectation d'une projection vers Pt0008 pour les entités topologiques Som0003
    tm.setGeomAssociation (["Som0003"], "Pt0008", True)
    # Affectation d'une projection vers Pt0009 pour les entités topologiques Som0000
    tm.setGeomAssociation (["Som0000"], "Pt0009", True)
    # Affectation d'une projection vers Crb0000 pour les entités topologiques Ar0001
    tm.setGeomAssociation (["Ar0001"], "Crb0000", True)
    # Affectation d'une projection vers Crb0001 pour les entités topologiques Ar0004
    tm.setGeomAssociation (["Ar0004"], "Crb0001", True)
    # Suppression des projections pour les entités topologiques Ar0004
    tm.setGeomAssociation (["Ar0004"], "", True)
    # Affectation d'une projection vers Crb0002 pour les entités topologiques Ar0009
    tm.setGeomAssociation (["Ar0009"], "Crb0002", True)
    # Affectation d'une projection vers Crb0003 pour les entités topologiques Ar0010
    tm.setGeomAssociation (["Ar0010"], "Crb0003", True)
    # Affectation d'une projection vers Crb0004 pour les entités topologiques Ar0003
    tm.setGeomAssociation (["Ar0003"], "Crb0004", True)
    # Affectation d'une projection vers Crb0005 pour les entités topologiques Ar0011
    tm.setGeomAssociation (["Ar0011"], "Crb0005", True)
    # Affectation d'une projection vers Crb0006 pour les entités topologiques Ar0012
    tm.setGeomAssociation (["Ar0012"], "Crb0006", True)
    # Affectation d'une projection vers Crb0007 pour les entités topologiques Ar0007
    tm.setGeomAssociation (["Ar0007"], "Crb0007", True)
    # Affectation d'une projection vers Crb0001 pour les entités topologiques Ar0004
    tm.setGeomAssociation (["Ar0004"], "Crb0001", True)
    # Découpage de toutes les faces 2D structurées
    tm.splitAllFaces ("Ar0004", .5, .5)
    tm.setGeomAssociation (["Ar0003", "Ar0010"], "", True)
    # Construction Topo et Geom 3D avec o-grid par révolution
    tm.makeBlocksByRevol (["Ar0018"], Mgx3D.Portion.QUART)

    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

import pyMagix3D as Mgx3D

# Test que le découpage en o-grid n'est PAS fait quand la contrainte d'Euler n'est pas vérifiée 
def test_issue35():
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager(
    tm = ctx.getTopoManager()

    ######### 3D #########

    # [3D] Test 1 : 2 blocs ayant une arete commune
    # ---------------------------------------------
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo(Mgx3D.Point(1, 1, 0), Mgx3D.Point(2, 2, 1), 10, 10, 10)
    # Découpage en o-grid qui réussit car les blocs ne sont pas collés : pas d'arete commune
    tm.splitBlocksWithOgridV2(["Bl0000","Bl0001"],[],.5,10)
    assert(tm.getNbBlocks() == 14)
    # On annule pour coller les 2 blocs et refaire le o-grid
    ctx.undo()
    # Collage entre géométries avec topologies
    gm.glue(["Vol0000", "Vol0001"])
    tm.splitBlocksWithOgridV2(["Bl0000","Bl0001"],[],.5,10)
    # Split non réalisé => toujours que 2 blocs
    assert(tm.getNbBlocks() == 2)

    # [3D] Test 2 : 2 blocs ayant une face commune
    # --------------------------------------------
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), 10, 10, 10)
    # Découpage en o-grid qui réussit car les blocs ne sont pas collés : pas de face commune
    tm.splitBlocksWithOgridV2(["Bl0000","Bl0001"],[],.5,10)
    assert(tm.getNbBlocks() == 14)
    # On annule pour coller les 2 blocs et refaire le o-grid
    ctx.undo()
    # Collage entre géométries avec topologies
    gm.glue(["Vol0000", "Vol0001"])
    tm.splitBlocksWithOgridV2(["Bl0000","Bl0001"],[],.5,10)
    # Split réalisé pour l'ensemble
    assert(tm.getNbBlocks() == 12)

    # [3D] Test 3 : avec des splits
    # -----------------------------
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Découpage de tous les blocs suivant l'arête Ar0011
    tm.splitAllBlocks ("Ar0011",.5)
    # Découpage de tous les blocs suivant l'arête Ar0005
    tm.splitAllBlocks ("Ar0005",.5)
    # Découpage en O-grid des blocs structurés Bl0006 Bl0003
    tm.splitBlocksWithOgridV2 (["Bl0006", "Bl0003"], [], .5, 10)
    # Split non réalisé => toujours que 4 blocs
    assert(tm.getNbBlocks() == 4)

    ######### 2D #########
    
    # [2D] Test 1 : 2 faces totalement indépendantes accollées => 2 ogrids
    # --------------------------------------------------------------------
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une 1ere surface avec topologie
    gm.newVertex(Mgx3D.Point(0,0,0))
    gm.newVertex(Mgx3D.Point(1,0,0))
    gm.newVertex(Mgx3D.Point(1,1,0))
    gm.newVertex(Mgx3D.Point(0,1,0))
    gm.newSegment("Pt0000", "Pt0001")
    gm.newSegment("Pt0001", "Pt0002")
    gm.newSegment("Pt0002", "Pt0003")
    gm.newSegment("Pt0003", "Pt0000")
    gm.newPlanarSurface(["Crb0000","Crb0001","Crb0002","Crb0003"], "")
    tm.newStructuredTopoOnGeometry("Surf0000")
    # Création d'une 2eme surface avec topologie
    gm.newVertex(Mgx3D.Point(1,0,0))
    gm.newVertex(Mgx3D.Point(2,0,0))
    gm.newVertex(Mgx3D.Point(2,1,0))
    gm.newVertex(Mgx3D.Point(1,1,0))
    gm.newSegment("Pt0004", "Pt0005")
    gm.newSegment("Pt0005", "Pt0006")
    gm.newSegment("Pt0006", "Pt0007")
    gm.newSegment("Pt0007", "Pt0004")
    gm.newPlanarSurface(["Crb0004","Crb0005","Crb0006","Crb0007"], "")
    tm.newStructuredTopoOnGeometry("Surf0001")
    # Création des ogrids
    assert(tm.getNbFaces() == 2)
    tm.splitFacesWithOgrid(["Fa0000", "Fa0001"], [], .5, 10)
    assert(tm.getNbFaces() == 10)

    # [2D] Test 2 : 3 faces collées => 1 seul ogrid global
    # ----------------------------------------------------
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une surface avec topologie
    gm.newVertex(Mgx3D.Point(0,0,0))
    gm.newVertex(Mgx3D.Point(2,0,0))
    gm.newVertex(Mgx3D.Point(2,1,0))
    gm.newVertex(Mgx3D.Point(0,1,0))
    gm.newSegment("Pt0000", "Pt0001")
    gm.newSegment("Pt0001", "Pt0002")
    gm.newSegment("Pt0002", "Pt0003")
    gm.newSegment("Pt0003", "Pt0000")
    gm.newPlanarSurface(["Crb0000","Crb0001","Crb0002","Crb0003"], "")
    tm.newStructuredTopoOnGeometry("Surf0000")
    # Split de la face pour en créer 3
    tm.splitFace("Fa0000", "Ar0002", .3, True)
    tm.splitFace("Fa0002", "Ar0005", .5, True)
    # Création du ogrid
    assert(tm.getNbFaces() == 3)
    tm.splitFacesWithOgrid(["Fa0001", "Fa0003", "Fa0004"], [], .5, 10)
    assert(tm.getNbFaces() == 11)

    # [2D] Test 3 : 4 faces + ogrid sur 2 faces pas adjacentes => échec du ogrid
    # --------------------------------------------------------------------------
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une surface avec topologie
    gm.newVertex(Mgx3D.Point(0,0,0))
    gm.newVertex(Mgx3D.Point(2,0,0))
    gm.newVertex(Mgx3D.Point(2,1,0))
    gm.newVertex(Mgx3D.Point(0,1,0))
    gm.newSegment("Pt0000", "Pt0001")
    gm.newSegment("Pt0001", "Pt0002")
    gm.newSegment("Pt0002", "Pt0003")
    gm.newSegment("Pt0003", "Pt0000")
    gm.newPlanarSurface(["Crb0000","Crb0001","Crb0002","Crb0003"], "")
    tm.newStructuredTopoOnGeometry("Surf0000")
    # Split de la face pour en créer 4
    tm.splitFace("Fa0000", "Ar0001", .2, True)
    tm.splitAllFaces("Ar0000", .5, .5)
    # Création du ogrid non réalisé => toujours que 4 blocs
    assert(tm.getNbFaces() == 4)
    tm.splitFacesWithOgrid(["Fa0003", "Fa0006"], ["Ar0004", "Ar0010", "Ar0006", "Ar0015"], .5, 10)
    assert(tm.getNbFaces() == 4)

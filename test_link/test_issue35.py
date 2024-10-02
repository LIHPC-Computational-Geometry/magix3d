import pyMagix3D as Mgx3D

# Test que le découpage en o-grid n'est PAS fait quand la contrainte d'Euler n'est pas vérifiée 
def test_issue35():
    ctx = Mgx3D.getStdContext()
    tm = ctx.getTopoManager()

    # Test 1 : 2 blocs ayant une arete commune
    # ----------------------------------------
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
    ctx.getGeomManager().glue(["Vol0000", "Vol0001"])
    tm.splitBlocksWithOgridV2(["Bl0000","Bl0001"],[],.5,10)
    # Split non réalisé => toujours que 2 blocs
    assert(tm.getNbBlocks() == 2)

    # Test 2 : 2 blocs ayant une face commune
    # ---------------------------------------
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), 10, 10, 10)
    # Découpage en o-grid qui réussit car les blocs ne sont pas collés : pas de face commune
    tm.splitBlocksWithOgridV2(["Bl0000","Bl0001"],[],.5,10)
    assert(tm.getNbBlocks() == 14)
    # On annule pour coller les 2 blocs et refaire le o-grid
    ctx.undo()
    # Collage entre géométries avec topologies
    ctx.getGeomManager().glue(["Vol0000", "Vol0001"])
    tm.splitBlocksWithOgridV2(["Bl0000","Bl0001"],[],.5,10)
    # Split réalisé pour l'ensemble
    assert(tm.getNbBlocks() == 12)

    # Test 3 : avec des splits
    # ------------------------
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

import sys
import pyMagix3D as Mgx3D

# Test que le découpage en o-grid n'est PAS fait quand la contrainte d'Euler n'est pas vérifiée 
def test_issue35():
    ctx = Mgx3D.getStdContext()
    tm = ctx.getTopoManager()

    ctx.clearSession() # Clean the session after the previous test
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    tm.splitBlocks(["Bl0000"], "Ar0005", .3)
    tm.splitAllBlocks("Ar0016", .4)
    tm.splitBlocksWithOgridV2(["Bl0005","Bl0004"], [], .5, 10)
    # Split non réalisé => toujours que 4 blocs
    assert(tm.getNbBlocks() == 4)

    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo(Mgx3D.Point(1, 1, 0), Mgx3D.Point(2, 2, 1), 10, 10, 10)
    # Découpage en o-grid qui réussit car les blocs ne sont pas collés : pas de face commune
    tm.splitBlocksWithOgridV2(["Bl0000","Bl0001"],[],.5,10)
    assert(tm.getNbBlocks() == 14)

    # On annule pour coller les 2 blocs et refaire le o-grid
    ctx.undo()
    # Collage entre géométries avec topologies
    ctx.getGeomManager().glue(["Vol0000", "Vol0001"])
    tm.splitBlocksWithOgridV2(["Bl0000","Bl0001"],[],.5,10)
    # Split non réalisé => toujours que 2 blocs
    assert(tm.getNbBlocks() == 2)

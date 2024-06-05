import sys
import pyMagix3D as Mgx3D

def test_issue55():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    tm.splitBlock("Bl0000", "Ar0011",.5)
    tm.splitFace("Fa0014","Ar0023",.3,True)
    tm.splitFace("Fa0016","Ar0025",.5,True)
    tm.splitEdge("Ar0033",.3)
    tm.splitEdge("Ar0035",.5)
    tm.splitBlocksWithOgridV2(["Bl0001","Bl0002"],[],.5,10)
    # Erreur de la commande Découpage en O-grid des blocs structurés Bl0001 Bl0002 avec le message : 
    # Erreur interne dans CommandSplitBlocksWithOgrid::createEdge, pas d'arêtes interne face à 2 sommets (Som0016,Som0017)

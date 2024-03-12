import sys
import pyMagix3D as Mgx3D

def test_issue72():
    ctx = Mgx3D.getStdContext()
    #  Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    #  Découpage en O-grid des blocs structurés Bl0000
    ctx.getTopoManager().splitBlocksWithOgridV2 (["Bl0000"], [], .5, 10)
    # Fusion de sommets Som0014 avec Som006
    ctx.getTopoManager().snapVertices("Som0014", "Som0006", True)
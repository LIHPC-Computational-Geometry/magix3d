import pyMagix3D as Mgx3D

ctx = Mgx3D.getStdContext()
ctx.clearSession() # Clean the session after the previous test
tm = ctx.getTopoManager ()
tm.newSphereWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.ENTIER, True, .5, 10, 10)
tm.makeBlocksByExtrude (["Fa0010", "Fa0040"],Mgx3D.Vector(-1, 0, 1))

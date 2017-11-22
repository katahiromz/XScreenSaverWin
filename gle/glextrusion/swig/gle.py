# This file was created automatically by SWIG.
import glec


#-------------- FUNCTION WRAPPERS ------------------

def gleGetJoinStyle():
    """"""
    val = glec.gleGetJoinStyle()
    return val

def gleSetJoinStyle(arg0):
    """bitwise OR of flags   
"""
    val = glec.gleSetJoinStyle(arg0)
    return val

def glePolyCylinder(arg0,arg1,arg2):
    """num points in polyline   
"""
    val = glec.glePolyCylinder(arg0,arg1,arg2)
    return val

def glePolyCone(arg0,arg1,arg2):
    """numpoints in poly-line   
"""
    val = glec.glePolyCone(arg0,arg1,arg2)
    return val

def gleExtrusion(arg0,arg1,arg2,arg3,arg4):
    """number of contour points   
"""
    val = glec.gleExtrusion(arg0,arg1,arg2,arg3,arg4)
    return val

def gleTwistExtrusion(arg0,arg1,arg2,arg3,arg4,arg5):
    """number of contour points   
"""
    val = glec.gleTwistExtrusion(arg0,arg1,arg2,arg3,arg4,arg5)
    return val

def gleSuperExtrusion(arg0,arg1,arg2,arg3,arg4,arg5):
    """extrude 2D contour, specifying local affine tranformations   
"""
    val = glec.gleSuperExtrusion(arg0,arg1,arg2,arg3,arg4,arg5)
    return val

def gleSpiral(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10):
    """number of contour points   
"""
    val = glec.gleSpiral(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10)
    return val

def gleLathe(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10):
    """lathe moves contour along helical path by helically shearing 3D space   
"""
    val = glec.gleLathe(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10)
    return val

def gleHelicoid(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8):
    """circle contour (torus) radius   
"""
    val = glec.gleHelicoid(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8)
    return val

def gleToroid(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8):
    """circle contour (torus) radius   
"""
    val = glec.gleToroid(arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8)
    return val

def gleScrew(arg0,arg1,arg2,arg3,arg4,arg5):
    """number of contour points   
"""
    val = glec.gleScrew(arg0,arg1,arg2,arg3,arg4,arg5)
    return val

def gleTextureMode(arg0):
    """"""
    val = glec.gleTextureMode(arg0)
    return val

def rot_axis(arg0,arg1):
    """Rotation Utilities   
"""
    val = glec.rot_axis(arg0,arg1)
    return val

def rot_about_axis(arg0,arg1):
    """"""
    val = glec.rot_about_axis(arg0,arg1)
    return val

def rot_omega(arg0):
    """"""
    val = glec.rot_omega(arg0)
    return val

def rot_prince(arg0,arg1):
    """"""
    val = glec.rot_prince(arg0,arg1)
    return val

def urot_axis(arg0,arg1,arg2):
    """"""
    val = glec.urot_axis(arg0,arg1,arg2)
    return val

def urot_about_axis(arg0,arg1,arg2):
    """"""
    val = glec.urot_about_axis(arg0,arg1,arg2)
    return val

def urot_omega(arg0,arg1):
    """"""
    val = glec.urot_omega(arg0,arg1)
    return val

def urot_prince(arg0,arg1,arg2):
    """"""
    val = glec.urot_prince(arg0,arg1,arg2)
    return val

def uview_direction(arg0,arg1,arg2):
    """returned   
"""
    val = glec.uview_direction(arg0,arg1,arg2)
    return val

def uviewpoint(arg0,arg1,arg2,arg3):
    """returned   
"""
    val = glec.uviewpoint(arg0,arg1,arg2,arg3)
    return val

def gleFeedBack():
    """"""
    val = glec.gleFeedBack()
    return val

def gleFreeFeedbackBuffers():
    """"""
    val = glec.gleFreeFeedbackBuffers()
    return val

gleGetTriangleMesh = glec.gleGetTriangleMesh



#-------------- VARIABLE WRAPPERS ------------------

TUBE_JN_RAW = glec.TUBE_JN_RAW
TUBE_JN_ANGLE = glec.TUBE_JN_ANGLE
TUBE_JN_CUT = glec.TUBE_JN_CUT
TUBE_JN_ROUND = glec.TUBE_JN_ROUND
TUBE_JN_MASK = glec.TUBE_JN_MASK
TUBE_JN_CAP = glec.TUBE_JN_CAP
TUBE_NORM_FACET = glec.TUBE_NORM_FACET
TUBE_NORM_EDGE = glec.TUBE_NORM_EDGE
TUBE_NORM_PATH_EDGE = glec.TUBE_NORM_PATH_EDGE
TUBE_NORM_MASK = glec.TUBE_NORM_MASK
TUBE_CONTOUR_CLOSED = glec.TUBE_CONTOUR_CLOSED
GLE_TEXTURE_ENABLE = glec.GLE_TEXTURE_ENABLE
GLE_TEXTURE_STYLE_MASK = glec.GLE_TEXTURE_STYLE_MASK
GLE_TEXTURE_VERTEX_FLAT = glec.GLE_TEXTURE_VERTEX_FLAT
GLE_TEXTURE_NORMAL_FLAT = glec.GLE_TEXTURE_NORMAL_FLAT
GLE_TEXTURE_VERTEX_CYL = glec.GLE_TEXTURE_VERTEX_CYL
GLE_TEXTURE_NORMAL_CYL = glec.GLE_TEXTURE_NORMAL_CYL
GLE_TEXTURE_VERTEX_SPH = glec.GLE_TEXTURE_VERTEX_SPH
GLE_TEXTURE_NORMAL_SPH = glec.GLE_TEXTURE_NORMAL_SPH
GLE_TEXTURE_VERTEX_MODEL_FLAT = glec.GLE_TEXTURE_VERTEX_MODEL_FLAT
GLE_TEXTURE_NORMAL_MODEL_FLAT = glec.GLE_TEXTURE_NORMAL_MODEL_FLAT
GLE_TEXTURE_VERTEX_MODEL_CYL = glec.GLE_TEXTURE_VERTEX_MODEL_CYL
GLE_TEXTURE_NORMAL_MODEL_CYL = glec.GLE_TEXTURE_NORMAL_MODEL_CYL
GLE_TEXTURE_VERTEX_MODEL_SPH = glec.GLE_TEXTURE_VERTEX_MODEL_SPH
GLE_TEXTURE_NORMAL_MODEL_SPH = glec.GLE_TEXTURE_NORMAL_MODEL_SPH

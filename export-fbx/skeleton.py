import yaml
from vec import *

class Skeleton(object):
    # static/class variables
    scale = 1/1000.
    JointNames = "torso head neck left_shoulder right_shoulder left_elbow right_elbow left_hand right_hand left_hip right_hip left_knee right_knee left_foot right_foot".split()
    
    # parse and convert to relative coordinates
    def __init__(self, sk):
        self.sk = sk
        self.pos = {} # cache
        
        self.torso = self.GetPos('torso')
        self.head = self.GetPosRel('head', 'neck')
        self.neck = self.GetPosRel('neck', 'torso')

        self.left_shoulder = self.GetPosRel('left_shoulder', 'neck')
        self.right_shoulder = self.GetPosRel('right_shoulder', 'neck')

        self.left_elbow = self.GetPosRel('left_elbow', 'left_shoulder')
        self.right_elbow = self.GetPosRel('right_elbow', 'right_shoulder')

        self.left_hand = self.GetPosRel('left_hand', 'left_elbow')
        self.right_hand = self.GetPosRel('right_hand', 'right_elbow')

        self.left_hip = self.GetPosRel('left_hip', 'torso')
        self.right_hip = self.GetPosRel('right_hip', 'torso')

        self.left_knee = self.GetPosRel('left_knee', 'left_hip')
        self.right_knee = self.GetPosRel('right_knee', 'right_hip')
        self.left_foot = self.GetPosRel('left_foot', 'left_knee')
        self.right_foot = self.GetPosRel('right_foot', 'right_knee')

    def GetPos(self, jointName):
        pos = self.pos.get(jointName, None)
        if pos is not None: # cache the parse/transform
            return pos
        else:
            # maybe the _1 indicates that it tracks several people?
            #joint = self.sk.get('/'+jointName+'_1',None) # this is the old style
            try:
                tr = self.sk[jointName]['position']['translation']
                return ( # shuffle so that the skelton is upright, remember to do this transform on the mesh too
                    float(tr[1].replace('y -- ', ''))*self.scale,
                    float(tr[2].replace('z -- ', ''))*self.scale,
                    float(tr[0].replace('x -- ', ''))*self.scale
                )
            except KeyError:
                return None

    def GetPosRel(self, jointName, parentName):
        j = self.GetPos(jointName)
        p = self.GetPos(parentName)
        if j is None or p is None:
            return None
        else:
            return (j[0]-p[0], j[1]-p[1], j[2]-p[2])
    
    def GetBone(self, src, dst):
        return self.GetPos(src),self.GetPos(dst)
    
    def Bones(self):
        return {
            'T'    : self.GetBone('torso', 'neck'),
            'LS2E' : self.GetBone('left_shoulder', 'left_elbow'),
            'LE2H' : self.GetBone('left_elbow', 'left_hand'),
            'N2H'  : self.GetBone('neck', 'head'),
            'RS2E' : self.GetBone('right_shoulder', 'right_elbow'),
            'RE2H' : self.GetBone('right_elbow', 'right_hand'),
            'LH2K' : self.GetBone('left_hip', 'left_knee'),
            'RH2K' : self.GetBone('right_hip', 'right_knee'),
            'LK2F' : self.GetBone('left_knee', 'left_foot'),
            'RK2F' : self.GetBone('right_knee', 'right_foot')
        }  
    
    def Duplicate(self):
        return Skeleton(self.sk)
    
    def Canonicalize(self):
        bones = self.Bones()
        left_clav = self.GetBone('neck', 'left_shoulder')
        right_clav = self.GetBone('neck', 'right_shoulder')
        
        self.torso = (0,0,0)
        self.neck = add(self.torso, (0,boneLength(bones['T']),0))
        
        self.left_shoulder = add(self.neck, (0,0,-boneLength(left_clav)))
        self.right_shoulder = add(self.neck, (0,0,boneLength(right_clav)))
        self.left_elbow = add(self.left_shoulder, (0,0,-boneLength(bones['LS2E'])))
        self.right_elbow = add(self.right_shoulder, (0,0,boneLength(bones['RS2E'])))
        self.left_hand = add(self.left_elbow, (0,0,-boneLength(bones['LE2H'])))
        self.right_hand = add(self.right_elbow, (0,0,boneLength(bones['RE2H'])))

        # pelvis triangle
        pelvisBase = boneLength(self.GetBone('left_hip', 'right_hip'))
        pelvisTop = boneLength(self.GetBone('torso', 'left_hip'))+boneLength(self.GetBone('torso', 'right_hip'))
        hipDip = sqrt((pelvisTop/2)**2 - (pelvisBase/2)**2)
        
        self.left_hip = add(self.torso, (0, -hipDip, -pelvisBase/2))
        self.right_hip = add(self.torso, (0, -hipDip, pelvisBase/2))
        
        self.left_knee = add(self.left_hip, (0,-boneLength(bones['LH2K']),0))
        self.right_knee = add(self.right_hip, (0,-boneLength(bones['RH2K']),0))
        self.left_foot = add(self.left_knee, (0,-boneLength(bones['LK2F']),0))
        self.right_foot = add(self.right_knee, (0,-boneLength(bones['RK2F']),0))
        
    
    @staticmethod
    def fromFunnyYaml(fname):
        # apparently the yaml parser in python doesn't understand this syntax,
        # so we transform it!
        y = open(fname, 'r').read().replace('    - confidence --', '    confidence:')
        return Skeleton(yaml.load(y))
    
def boneLength(b):
    return -mag(subtract(b[0],b[1]))


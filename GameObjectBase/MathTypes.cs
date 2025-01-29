using GlmSharp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using SharpDX;
using GlmSharp;

namespace GameObjectBase
{
    internal class MathTypes
    {
        public static SharpDX.Vector3 toDX (vec3 vector){
            return new SharpDX.Vector3(vector.x, vector.y, vector.z);
        }
        public static vec3 toGLM(SharpDX.Vector3 vector)
        {
            return new vec3(vector.X, vector.Y, vector.Z);
        }
        public static quat toGLM(SharpDX.Quaternion quat)
        {
            return new quat(quat.X, quat.Y, quat.Z, quat.W);
        }
        public static SharpDX.Quaternion toDX(quat quat)
        {
            return new Quaternion(quat.x, quat.y, quat.z, quat.w);
        }
        public static quat toGLM(SharpDX.Vector4 quat)
        {
            return new quat(quat.X, quat.Y, quat.Z, quat.W);
        }

        public static quat LookAtLH(vec3 direction, vec3 up)
        {
            vec3 forward = direction.Normalized;
            vec3 right = GlmSharp.vec3.Cross(up, forward).Normalized;
            vec3 newUp = GlmSharp.vec3.Cross(forward, right);
            mat3 matrix = new mat3(right, newUp, forward);
            quat rotation = new GlmSharp.quat(matrix);
            return rotation;
        }

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase
{
    internal class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool GameObject_SetPosition(ulong entityID, GlmSharp.vec3 pos);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool GameObject_SetWorldMatrix(ulong entityID, GlmSharp.mat4 mat);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool GameObject_SetMaterialID(ulong entityID, ulong mat_id);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool GameObject_SetMeshID(ulong entityID, ulong mesh_id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool GameObject_AddGameObject();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool GameObject_RemoveGameObject(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Close();

    }
}

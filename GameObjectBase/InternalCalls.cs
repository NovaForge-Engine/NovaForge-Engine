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

    }
}

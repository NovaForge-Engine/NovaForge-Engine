using GameObjectsBase;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase
{
    public static class Root
    {
        private static ObjectsHolder _holder;

        public static void Initialize()
        {
            _holder = new ObjectsHolder();
        }

        public static void Update()
        {
            _holder.Update();
        }

        public static void FixedUpdate()
        {
            _holder.FixedUpdate();
        }
    }
}

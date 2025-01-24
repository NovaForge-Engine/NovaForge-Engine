using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase
{
    [AttributeUsage(AttributeTargets.Field)]
    public class SerializableField : Attribute
    {

    }

    [AttributeUsage(AttributeTargets.Field)]
    public class RangeField : Attribute
    {
        public readonly float Min, Max;
        public RangeField(float min, float max)
        {
            Min = min;
            Max = max;
        }
    }

   
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase
{
    internal class MaterialMap
    {
        public enum ObjectName
        {
            GG_idle1,
            GG_idle2,
            GG_attack1,
            GG_attack2,
            GG_dead,
            GG_Rest,
            Bush,
            Land,
            GG_walk1,
            GG_walk2,
            Monster_attack1,
            Monster_attack2,
            Monster_idle1,
            Monster_idle2,
            Monster_walk1,
            Monster_walk2
        }
        public enum MaterialName
        {
           Default,
           Bush,
           Land,
           GGSelected,
           GGIdle,
           GGDamaged,
           GGHealed,
           Monster,
           MonsterDamaged

        }
    }
}

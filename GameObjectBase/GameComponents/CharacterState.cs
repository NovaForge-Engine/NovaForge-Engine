using InteractableGroupsAi.Agents;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase.GameComponents
{

    public static class NovaMath
    {
        public static T Clamp<T>(this T val, T min, T max) where T : IComparable<T>
        {
            if (val.CompareTo(min) < 0) return min;
            else if (val.CompareTo(max) > 0) return max;
            else return val;
        }
    }
    


    public class CharacterState
    {
        public float MaxHealth { get; set; }
        public float CurrentHealth { get; set; }

        public int AgentID { get; private set; }

        public GroupId GroupId { get; private set; }

        public float CurrentRest { get; private set; }

        public System.Numerics.Vector3 CurrentPosition { get; private set; }

        public IAgentState CurrentTarget { get; private set; }

        public System.Numerics.Vector3 TargetPosition { get; private set; }

        public void SetAgentID(int id) => AgentID = id;

        public void SetGroupId(GroupId groupId) => GroupId = groupId;

        public void SetHealth(float health)
        {
            CurrentHealth = NovaMath.Clamp(health, 0, MaxHealth);
        }

        public void SetRest(float rest)
        {
            CurrentRest = NovaMath.Clamp(rest, 0, 1);
        }

        public void SetPosition(System.Numerics.Vector3 position) => TargetPosition = position;

        public void SetTargetPosition(System.Numerics.Vector3 position) => TargetPosition = position;

        public void SetTarget(IAgentState target) => CurrentTarget = target;

        public void SetPosition(GlmSharp.vec3 position)
        {
            CurrentPosition = new System.Numerics.Vector3(position.x, position.y, position.z);
        }


    }
}

using InteractableGroupsAi.Agents;
using InteractableGroupsAi.Memory;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.InteropServices;

namespace InteractableGroupsAi.Director.Groups
{
    /// <summary>
    /// Маркировочынй интерфейс для конкретных реализаций Consideration,
    /// Оценивающие какую-то характеристику группы\персонажа получаемую из класса который реализует IContext
    /// </summary>
    public interface IGroupContext
    {
        IGroupState GetState();
        Blackboard Memory { get; }
    }

    public interface IGroupState
    {
        float CurrentHealth { get; }
        float MaxHealth { get; }

        float CurrentRest { get; }
        float MaxRest { get; }

        IGroupContext CurrentTarget { get; }

        Vector3 CurrentPosition { get; }

        void SetTarget(IGroupContext target);
    }

    public class DesiredGroupState : IGroupState
    {
        public float CurrentHealth { get; set; }

        public float MaxHealth { get; set; }

        public float CurrentRest { get; set; }

        public float MaxRest { get; set; }

        public IGroupContext CurrentTarget { get; set; }

        public Vector3 CurrentPosition { get; set; }

        public void SetTarget(IGroupContext target)
        {
            CurrentTarget = target;
        }
    }

    public class GroupState : IGroupState
    {
        public float CurrentHealth 
        { 
            get
            {
                var health = 0f;
                foreach(var agent in _agents)
                {
                    health += agent.CurrentHealth;
                }
                return health;
            }
            set
            {
                CurrentHealth = value;
            }
        }
        public float MaxHealth { get; private set; }

        public float CurrentRest 
        {
            get
            {
                var rest = 0f;
                foreach (var agent in _agents)
                {
                    rest += agent.CurrentRest;
                }
                return rest;
            }

            set
            {
                CurrentRest = value;
            }
        }

        public float MaxRest { get; private set; }

        public IGroupContext CurrentTarget { get; private set; }

        public Vector3 CurrentPosition
        {
            get
            {
                var allX = 0f;
                var allY = 0f;
                var allZ = 0f;
                foreach (IAgentState agent in _agents)
                {
                    allX += agent.CurrentPosition.X;
                    allY += agent.CurrentPosition.Y;
                    allZ += agent.CurrentPosition.Z;
                }

                return new Vector3(allX / _agents.Count, allY / _agents.Count, allZ / _agents.Count);
            }

            private set
            {
                CurrentPosition = value;
            }
        }

        public IEnumerable<IAgentState> Agents => _agents;
        private List<IAgentState> _agents;

        public GroupState(List<IAgentState> agents)
        {
            _agents = agents;
        }

        public void SetTarget(IGroupContext target)
        {
            CurrentTarget = target;
        }
    }
}

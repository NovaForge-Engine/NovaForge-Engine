using System.Collections.Generic;
using System.Linq;

namespace InteractableGroupsAi.Agents.Conditions
{
    public class CompositeCondition<T> where T : Condition
    {
        public IEnumerable<T> ConditionsPublic => Conditions;

        protected List<T> Conditions = new List<T>();

        public void AddConditions(List<T> conditions)
        {
            conditions.ForEach(AddCondition);
        }

        public void AddCondition(T condition)
        {
            Conditions.Add(condition);
        }

        public bool IsSatisfied(out List<T> failedCondition)
        {
            failedCondition = new List<T>();
            foreach (var condition in Conditions)
            {
                if (condition.Check() == false)
                {
                    failedCondition.Add(condition);
                }
            }
            if (failedCondition.Any()) return false;

            failedCondition.Clear();
            return true;
        }
    }

    public class CompositeGroupCondition : CompositeCondition<GroupCondition>
    {
        public float GetDelta(AgentAction action)
        {
            var delta = 0f;
            foreach(var condition in Conditions)
            {
                delta += condition.GetConditionDelta(action);
            }
            
            return delta;
        }
    }

    public class ComppositeAgentCondition : CompositeCondition<AgentCondition>
    {
        public bool TrySatisfyConditions(AgentAction action)
        {
            foreach (var condition in Conditions)
            {
                if (condition.TrySatisfyCondition(action) == false) return false;
            }

            return true;
        }
    }
}

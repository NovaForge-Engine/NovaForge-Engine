using InteractableGroupsAi.Agents;
using InteractableGroupsAi.Agents.Conditions;
using InteractableGroupsAi.Director.Goals;
using System;
using System.Collections.Generic;

namespace InteractableGroupsAi
{
    public abstract class AgentAction
    {
        private ComppositeAgentCondition _condition;

        public Action OnCompleted { get; set; }
        public Action OnFailed { get; set; }

        public AgentAction(ComppositeAgentCondition condition)
        {
            _condition = condition;
        }

        public bool CanExecute(out List<AgentCondition> condition)
        {
            return _condition.IsSatisfied(out condition);
        }
        /// <summary>
        /// Для оценки акшн проходит по кондишенам Гоала, меняет их и дельту аггрегирует в выход.
        /// </summary>
        /// <param name="goal"></param>
        /// <returns>Если Акшн не может никак изменить цель, то возвращается 0, 
        /// в противном случае аггрегированные изменения кондишенов.</returns>
        public abstract float GetGoalChange(Goal goal);

        public abstract IAgentState GetNewState();

        public abstract void Update();

        public abstract void TryExecute();

        public abstract void OnBegin();
        public abstract void OnEnd();
        public abstract void ForceEnd();
    }
}

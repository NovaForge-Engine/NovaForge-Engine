using InteractableGroupsAi.Agents.Conditions;
using InteractableGroupsAi.Director.Groups;

namespace InteractableGroupsAi.Director.Goals
{
    public abstract class Goal
    {
        public IGroupContext Group { get; private set; }

        private CompositeGroupCondition _desiredCondition;

        public Goal(CompositeGroupCondition condition)
        {
            _desiredCondition = condition;
        }

        protected void SetGroupContext(IGroupContext group) => Group = group;

        public abstract void Accept();

        public float GetGoalDelta(AgentAction action) => _desiredCondition.GetDelta(action);
    }
}

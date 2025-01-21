using InteractableGroupsAi.Agents.Conditions;

namespace InteractableGroupsAi.Director.Goals
{
    public class NullGoal : Goal
    {
        public NullGoal() : base (new CompositeGroupCondition())
        {

        }

        public NullGoal(CompositeGroupCondition condition) : base(condition)
        {
        }

        public override void Accept()
        {
        }
    }
}

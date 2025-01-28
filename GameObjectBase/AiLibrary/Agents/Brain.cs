using AiLibrary.Other;
using InteractableGroupsAi.Agents.Conditions;
using InteractableGroupsAi.Director.Goals;

namespace InteractableGroupsAi.Agents
{
    public class NullBrain : Brain
    {
        public NullBrain() : base(null)
        {

        }

        public override void Reset()
        {
        }

        public override void Start()
        {

        }

        public override void Update()
        {
        }
    }

    public abstract class Brain : IUpdatable
    {
        private AiController<IAgentState> _controller;
        protected Goal CurrentGoal = new NullGoal(new CompositeGroupCondition());
        protected IAgentState AgentState => _controller.State;

        public Brain(AiController<IAgentState> controller)
        {
            _controller = controller;
        }

        public void SetGoal(Goal goal)
        {
            AiLogger.Log($"#Brain {_controller.State.AgentId} Set new goal from {CurrentGoal} to {goal}");

            if (goal == CurrentGoal)
            {
                AiLogger.Log($"Same action no reset");
                return;
            }
            CurrentGoal = goal;

            Reset();
        }

        public abstract void Reset();

        public abstract void Start();

        public abstract void Update();
    }
}

using InteractableGroupsAi.Agents;
using InteractableGroupsAi.Director.Groups;

namespace InteractableGroupsAi
{
    /// <summary>
    /// Каждый кондишн получает на вход необходимый ему контекст (GroupState, CharacterState) 
    /// И в чек проверяет то что нужно
    /// Например HealthGreaterCondition
    /// Получает CharacterState и проверяет хп
    /// </summary>
    public abstract class Condition
    {
        public abstract bool Check();
    }

    public class NullCondition : Condition
    {
        public override bool Check() => true;
    }

    public abstract class AgentCondition : Condition
    {
        public IAgentState AgentContext => AgentContextInternal;

        protected IAgentState AgentContextInternal { get; private set; }

        public AgentCondition(IAgentState agentContext)
        {
            AgentContextInternal = agentContext;
        }
        /// <summary>
        /// Get state from AgentAction and CallCheck state
        /// </summary>
        /// <param name="action"></param>
        /// <returns></returns>
        public bool TrySatisfyCondition(AgentAction action) => CheckState(action.GetNewState());

        public abstract bool CheckState(IAgentState context);

    }

    public abstract class GroupCondition : Condition
    {
        public IGroupState GroupState => GroupStateInternal;

        protected IGroupState GroupStateInternal { get; private set; }

        public GroupCondition(IGroupState groupContext)
        {
            GroupStateInternal = groupContext;
        }

        public abstract float GetConditionDelta(AgentAction action);
    }
}

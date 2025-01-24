using InteractableGroupsAi.Agents;
using InteractableGroupsAi.Director.Goals;
using InteractableGroupsAi.Memory;
using System;
using System.Collections.Generic;
using System.Linq;

namespace InteractableGroupsAi.Director.Groups
{
    public class Group : IGroupContext
    {
        public Action GoalReached;

        public Goal CurrentGoal => _currentGoal;
        public Blackboard Memory => _blackboard;
        public GroupId GroupId { get; private set; }

        private Goal _currentGoal = new NullGoal();
        private List<AiController<IAgentState>> _agents = new List<AiController<IAgentState>>();
        private Blackboard _blackboard;

        public int Id { get; }

        public Group(GroupId groupId)
        {
            GroupId = groupId;
            _blackboard = new Blackboard();
        }

        public void SetGroupGoal(Goal newGoal)
        {
            _currentGoal = newGoal;
            _currentGoal.Accept();
            _agents.ForEach(x => x.SetCurrentGoal(_currentGoal));
        }

        public IGroupState GetState() => new GroupState(_agents.Select(x => x.State).ToList());

        public void AddAgent(AiController<IAgentState> agent)
        {
            agent.SetGroupId(GroupId);
            _agents.Add(agent);
        }
    }
}

using AiLibrary.Other;
using InteractableGroupsAi.Agents;
using InteractableGroupsAi.Director.Buckets;
using InteractableGroupsAi.Director.Goals;
using InteractableGroupsAi.Memory;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;

namespace InteractableGroupsAi.Director.Groups
{
    public class Group : IGroupContext
    {
        public IEnumerable<Bucket> Buckets => _buckets;

        public Goal CurrentGoal => _currentGoal;
        public Blackboard Memory => _blackboard;
        public GroupId GroupId => _state.GroupId;
        public GroupState State => _state;
        public IEnumerable<AiController<IAgentState>> Agents => _agents;

        private Goal _currentGoal = new NullGoal();
        private List<AiController<IAgentState>> _agents = new List<AiController<IAgentState>>();
        private Blackboard _blackboard;

        private List<Bucket> _buckets = new List<Bucket>();

        private GroupState _state;
        private UtilityDirector _director;

        public int Id { get; }

        public Group(GroupId groupId)
        {
            AiLogger.Log($"Create new group with id = {groupId.Id}");
            _blackboard = new Blackboard();
            _state = new GroupState();
            _state.GroupId = groupId;
        }

        public void SetDirector(UtilityDirector director) => _director = director;

        public void SetGroupGoal(Goal newGoal)
        {
            AiLogger.Log($"{GroupId.Id} got new goal");
            _currentGoal = newGoal;
            _currentGoal.Accept();
            _agents.ForEach(x => x.SetCurrentGoal(_currentGoal));
        }

        public void SetTarget(IGroupState target) => _state.SetTarget(target);

        public IGroupState GetState() => _state;

        public void AddBucket(Bucket bucket)
        {
            _buckets.Add(bucket);
        }

        public void AddAgent(AiController<IAgentState> agent)
        {
            agent.SetGroupId(GroupId);
            agent.AgentDetected += SecureAgent;
            agent.AgentLost += ForgetAgent;

            AiLogger.Log($"Agent {agent.State.AgentId} set to {GroupId.Id}");

            _agents.Add(agent);
            _state.AddAgent(agent.State);
        }

        private void SecureAgent(IAgentState agent)
        {
            bool value = _blackboard.TryGet(new BlackboardKey($"{agent.GroupId}_count"), out int count);

            var newCount = value ? ++count : 1;

            _blackboard.AddValue($"{agent.GroupId}_count", newCount);

            var group = _director.Groups.Where(x => x.GroupId.Equals(agent.GroupId)).FirstOrDefault();
            if (group == null) return;

            _blackboard.AddValue($"{agent.GroupId}", group);

            if (_state.CurrentTarget == null)
            {
                SetTarget(group.State);
                return;
            }

            if (Vector3.Distance(group.State.CurrentPosition, _state.CurrentPosition) <
                    Vector3.Distance(_state.CurrentTarget.CurrentPosition, _state.CurrentPosition))
            {
                SetTarget(group.State);
            }
        }

        private void ForgetAgent(IAgentState agent)
        {
            bool value = _blackboard.TryGet(new BlackboardKey($"{agent.GroupId}_count"), out int count);
            if (value == false) return;

            _blackboard.AddValue($"{agent.GroupId}_count", --count);

            if (--count != 0)
            {
                return;
            }

            var group = _director.Groups.Where(x => x.GroupId.Equals(agent.GroupId)).FirstOrDefault();

            _blackboard.AddValue($"{agent.GroupId}", group);

            SetTarget(null);
        }
    }
}

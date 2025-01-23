using InteractableGroupsAi.Director.Goals;
using InteractableGroupsAi.Memory;
using System.Collections.Generic;
using System.Numerics;

namespace InteractableGroupsAi.Agents
{
    public class AiController<T> where T : IAgentState
    {
        private T _character;

        private Brain _brain = new NullBrain();
        private Blackboard _blackboard = new Blackboard();
        private List<IPerceptionSensor> _perceptionSensors = new List<IPerceptionSensor>();

        public T State => _character;
        public Blackboard Memory => _blackboard;

        public AiController(T character)
        {
            _character = character;
        }

        public void SetBrain(Brain brain)
        {
            _brain = brain;
        }

        public void SetState(T state)
        {
            _character = state;
        }

        public void AddSensor(IPerceptionSensor sensor)
        {
            _perceptionSensors.Add(sensor);
            sensor.OnAgentDetected += OnAgentDetected;
            sensor.OnAgentMoved += OnTargetMoved;
            sensor.OnAgentLost += OnAgentLost;
        }

        public void OnAgentDetected(IAgentState agentContext)
        {
            _blackboard.AddValue(new BlackboardKey($"{agentContext.AgentId}_position"), agentContext.CurrentPosition);
            _blackboard.AddValue(new BlackboardKey(agentContext.AgentId.ToString()), agentContext);
        }

        public void OnAgentLost(IAgentState agentContext)
        {
            _blackboard.AddValue(new BlackboardKey($"{agentContext.AgentId}_position"), agentContext.CurrentPosition);
            _blackboard.Remove(new BlackboardKey($"{agentContext.AgentId}"));
        }

        public void OnTargetMoved(IAgentState agentContext) 
            => _blackboard.AddValue(new BlackboardKey($"{agentContext.AgentId}_position"), agentContext.CurrentPosition);

        public void Start() => _brain.Start();

        public void Update()
        {
            _brain.Update();
            _perceptionSensors.ForEach(x => x.Update());
        }

        public IAgentState GetCharacterState()
        {
            return _character;
        }

        public void SetGroupId(GroupId groupId) => _character.SetGroupId(groupId);

        public void SetCurrentGoal(Goal newGoal) => _brain.SetGoal(newGoal);
    }
}

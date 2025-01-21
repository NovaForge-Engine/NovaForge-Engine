using InteractableGroupsAi.Director.Goals;
using System.Collections.Generic;
using System.Linq;

namespace InteractableGroupsAi.Agents
{
    public class GobBrain : Brain
    {
        private const int DeepBorder = 5;

        private AgentAction _currentAction = null;
        private List<AgentAction> _availableActions = new List<AgentAction>();

        private Queue<AgentAction> _plannedActions = new Queue<AgentAction>();
        private Queue<AgentAction> _tempQueue = new Queue<AgentAction>();

        private int _deepCounter = 0;

        public GobBrain(AiController<IAgentState> controller) : base(controller)
        {

        }

        public override void Reset()
        {
            _plannedActions.Clear();
            ChooseNewAction();
        }

        public override void Start()
        {
            if (_availableActions.Count <= 0) return;

            ChooseNewAction();
        }

        public override void Update() => _currentAction?.Update();

        public void SetAvailableActions(List<AgentAction> availableActions) 
        {
            _availableActions = availableActions;
        }

        private void ChooseNewAction()
        {
            var highestScore = 0f;
            AgentAction choosenAction = _availableActions.FirstOrDefault();
            foreach (var action in _availableActions)
            {
                _deepCounter = 0;
                var score = action.GetGoalChange(CurrentGoal);

                if (score <= highestScore)
                {
                    continue;
                }

                _tempQueue.Clear();

                if (action.CanExecute(out var failedCondition) == false)
                {
                    var resolvingAction = FindActionToSatisfyCondition(action, failedCondition);
                    if (resolvingAction == null)
                    {
                        continue;
                    }
                }
                
                highestScore = score;
                choosenAction = action;

                _tempQueue.Enqueue(choosenAction);
                foreach (var item in _tempQueue)
                {
                    _plannedActions.Enqueue(item);
                }
            }

            MoveToNextAction();
        }

        private AgentAction FindActionToSatisfyCondition(AgentAction actionToExecute, AgentCondition requiredCondition)
        {
            _deepCounter++;
            foreach (var action in _availableActions)
            {
                if (action == actionToExecute) continue;

                if (requiredCondition.TrySatisfyCondition(action) == false)
                    continue;
                    
                if (action.CanExecute(out var condition) == false)
                {
                    if (_deepCounter == DeepBorder)
                    {
                        return null;
                    }

                    var satisfyingAction = FindActionToSatisfyCondition(action, condition);

                    if (satisfyingAction == null)
                    {
                        continue;
                    }
                }

                _deepCounter--;
                _tempQueue.Enqueue(action);
                return action;
            }

            _deepCounter--;
            return null;
        }

        private void SetAction(AgentAction action)
        {
            if (_currentAction.OnFailed != null) _currentAction.OnFailed -= ChooseNewAction;
            if (_currentAction.OnCompleted != null) _currentAction.OnCompleted -= MoveToNextAction;

            _currentAction = action;
            _currentAction.OnCompleted += MoveToNextAction;
            _currentAction.OnFailed += ChooseNewAction;

            _currentAction.OnBegin();
        }

        private void MoveToNextAction()
        {
            _currentAction?.OnEnd();
            
            if (_plannedActions.Count <= 0)
            {
                ChooseNewAction();
                return;
            }

            _currentAction = _plannedActions.Dequeue();
            SetAction(_currentAction);
        }
    }
}

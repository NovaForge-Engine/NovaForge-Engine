using AiLibrary.Other;
using InteractableGroupsAi.Director.Goals;
using System;
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
            AiLogger.Log($"Brain reset");
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
            var highestDelta = 0f;
            AgentAction choosenAction = _availableActions.FirstOrDefault();
            foreach (var action in _availableActions)
            {
                _deepCounter = 0;
                var delta = action.GetGoalChange(CurrentGoal);
                AiLogger.Log($"#GobBrain: {action} has {delta} but {choosenAction} has {highestDelta} for {CurrentGoal}");
                if (delta <= highestDelta)
                {
                    continue;
                }

                _tempQueue.Clear();

                if (action.CanExecute(out var failedCondition) == false)
                {
                    var resolvingAction = FindActionToSatisfyCondition(action, failedCondition);
                    if (resolvingAction == false)
                    {
                        AiLogger.Warning($"#Satisfaction cant find action for {action} and {failedCondition} from try satisfy");
                        continue;
                    }
                }
                
                highestDelta = delta;
                choosenAction = action;
                _tempQueue.Enqueue(choosenAction);
                _plannedActions.Clear();
                foreach (var item in _tempQueue)
                {
                    AiLogger.Log($"#QUEUE From temp to actual {item}");
                    _plannedActions.Enqueue(item);
                }
            }

            MoveToNextAction();
        }

        private bool FindActionToSatisfyCondition(AgentAction actionToExecute, List<AgentCondition> requiredConditions)
        {
            _deepCounter++;
            foreach(var requiredCondition in requiredConditions)
            {
                AiLogger.Warning($"#Satisfaction {requiredCondition}");
                foreach (var action in _availableActions)
                {
                    if (action == actionToExecute) continue;

                    if (requiredCondition.TrySatisfyCondition(action) == false)
                    {
                        continue;
                    }

                    if (action.CanExecute(out var condition) == false)
                    {
                        if (_deepCounter == DeepBorder)
                        {
                            AiLogger.Warning($"#Satisfaction cant find action for {actionToExecute} and {requiredCondition} - too deep");
                            return false;
                        }

                        var satisfyingAction = FindActionToSatisfyCondition(action, condition);

                        if (satisfyingAction == false)
                        {
                            AiLogger.Warning($"#Satisfaction cant find action for {action} and {condition}");
                            continue;
                        }
                    }

                    AiLogger.Log($"#Satisfaction find action for {actionToExecute} and {requiredCondition}: {action}");
                    _deepCounter--;
                    _tempQueue.Enqueue(action);

                    if (_tempQueue.Count == requiredConditions.Count)
                    {
                        return true;
                    }
                }
            }

            _deepCounter--;
            AiLogger.Warning($"#Satisfaction cant find action for {actionToExecute} ran out of options");
            return false;
        }

        private void SetAction(AgentAction action)
        {
            AiLogger.Log($"#GobBrainSet {AgentState.AgentId} set {action}");
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

using AiLibrary.Other;
using InteractableGroupsAi.Agents.Conditions;
using InteractableGroupsAi.Director.Goals;
using InteractableGroupsAi.Director.Groups;
using System.Collections.Generic;
using System.Linq;

namespace InteractableGroupsAi.Director.Buckets
{
    public class Bucket
    {
        private float _weight;
        private List<ConsideredGoal> _availableGoals = new List<ConsideredGoal>();
        private GroupScorer _scorer;
        private float _minimunScore = 0f;

        public IEnumerable<ConsideredGoal> Goals => _availableGoals;
        public float Weight => _weight;

        public Bucket(float weight, GroupScorer scorer)
        {
            _weight = weight;
            _scorer = scorer;
        }

        public float EvaluateBucket(IGroupContext context)
        {
            return _scorer.GetScore() * _weight;
        }

        public void AddGoal(ConsideredGoal goal)
        {
            _availableGoals.Add(goal);
        }

        public ConsideredGoal EvaluateGoals(IGroupContext context)
        {
            ConsideredGoal bestGoal = _availableGoals.FirstOrDefault();
            float floor = _minimunScore;
            foreach (var goal in _availableGoals)
            {
                var score = goal.EvaluateGoal();

                AiLogger.Log($"Bucket: {goal.Goal}: {score} for {context.GetState().GroupId.Id}");
                if (score > floor)
                {
                    bestGoal = goal;
                    floor = score;
                }
            }

            return bestGoal;
        }
    }
}

namespace InteractableGroupsAi.Director.Goals
{
    public class ConsideredGoal
    {
        private Goal _goal;
        private GroupScorer _scorer;

        public Goal Goal => _goal;

        public ConsideredGoal(Goal goal, GroupScorer scorer)
        {
            _goal = goal;
            _scorer = scorer;
        }

        public float EvaluateGoal() => _scorer.GetScore();
    }
}

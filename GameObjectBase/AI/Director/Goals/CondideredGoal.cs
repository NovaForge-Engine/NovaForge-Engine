namespace InteractableGroupsAi.Director.Goals
{
    public class CondideredGoal
    {
        private Goal _goal;
        private GroupScorer _scorer;

        public Goal Goal => _goal;

        public CondideredGoal(Goal goal, GroupScorer scorer)
        {
            _goal = goal;
            _scorer = scorer;
        }

        public float EvaluateGoal() => _scorer.GetScore();
    }
}

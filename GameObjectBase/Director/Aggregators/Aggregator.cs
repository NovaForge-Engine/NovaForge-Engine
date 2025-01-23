using InteractableGroupsAi.Director.Goals;

namespace InteractableGroupsAi.Director.Aggregators
{
    public abstract class Aggregator
    {
        public abstract Goal Aggregate(Goal goalA, Goal goalB);
    }

    public class NullAggregator : Aggregator
    {
        public override Goal Aggregate(Goal goalA, Goal goalB)
        {
            return new NullGoal();
        }
    }
}

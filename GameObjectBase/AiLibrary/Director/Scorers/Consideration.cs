using InteractableGroupsAi.Director.Groups;

namespace InteractableGroupsAi.Director
{
    public abstract class Consideration
    {
        public abstract float GetScore(IGroupState context);
    }
}

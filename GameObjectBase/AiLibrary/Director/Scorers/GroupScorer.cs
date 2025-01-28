using AiLibrary.Other;
using InteractableGroupsAi.Director.Goals;
using InteractableGroupsAi.Director.Groups;
using System.Collections.Generic;
using System.Runtime.Remoting.Contexts;

namespace InteractableGroupsAi.Director
{
    public interface IScorer
    {
        float GetScore();
    }

    /// <summary>
    /// Для каждого бакета\гоала имеется этот класс с разным списком консидерейшенов, 
    /// В которых оценивается стат группы для выбора наилучшего для текущей ситуации
    /// </summary>
    public class GroupScorer : IScorer
    {
        private IGroupState _state;
        private List<Consideration> _considerations = new List<Consideration>();

        public GroupScorer(IGroupState state)
        {
            _state = state;
        }

        public void AddConsideration(Consideration consideration) => _considerations.Add(consideration);

        public virtual float GetScore()
        {
            var result = 1f;

            foreach(var consideration in _considerations)
            {
                float score = consideration.GetScore(_state);

                AiLogger.Log($"Scorer: {consideration}: {score} for {_state.GroupId.Id}");
                result *= score;
            }

            return result;
        }
    }
}

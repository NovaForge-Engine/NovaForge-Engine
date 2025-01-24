using InteractableGroupsAi.Other;
using System.Collections.Generic;
using System.Numerics;

namespace InteractableGroupsAi.Agents
{

    public interface IAgentState
    {
        float CurrentHealth { get; }
        float MaxHealth { get; }

        float CurrentRest { get; }
        float MaxRest { get; }

        GroupId GroupId { get; }
        int AgentId { get; }
        Vector3 CurrentPosition { get; }
        Vector3 TargetPosition { get; }

        IAgentState CurrentTarget { get; }
        Item CurrentHand { get; }
        List<Item> Items { get; }

        void SetGroupId(GroupId id);
        void SetAgentId(int id);
        void SetHealth(float health);
        void SetRest(float rest);
        void SetPosition(Vector3 position);
        void SetTargetPosition(Vector3 position);
        void SetTarget(IAgentState target);
        void SetItems(List<Item> items);
        void Equip(Item item);
    }
}

using System;
using System.Diagnostics.CodeAnalysis;

namespace InteractableGroupsAi.Agents
{
    public struct GroupId : IEquatable<GroupId>
    {
        public int Id { get; private set; }

        public GroupId(int id)
        {
            Id = id;
        }

        public bool Equals(GroupId other) => Id == other.Id;

        public GroupId Next()
        {
            var newId = new GroupId(Id);
            Id = ++Id;
            return newId;
        }
    }
}

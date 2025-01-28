
using System;

namespace InteractableGroupsAi.Other
{
    [Serializable]
    public class Item
    {
        private int _id;
        private string _name;

        public string Name => _name;
        public int Id => _id;

        public Item(int id, string name)
        {
            _id = id;
            _name = name;
        }
    }
}

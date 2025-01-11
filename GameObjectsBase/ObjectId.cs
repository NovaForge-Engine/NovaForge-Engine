namespace GameObjectsBase
{
    public struct ObjectId
    {
        private int _tag;

        public readonly int Value => _tag;
        
        public ObjectId(int tag)
        {
            _tag = tag;
        }

        public static ObjectId Zero => new(0);
        public static ObjectId First => new(1);

        public ObjectId Next() => new(++_tag);
    }
}

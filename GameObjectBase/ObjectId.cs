namespace GameObjectsBase
{
    public struct ObjectId
    {
        private int _tag;

        public int Value => _tag;
        
        public ObjectId(int tag)
        {
            _tag = tag;
        }

        public static ObjectId Zero => new ObjectId(0);
        public static ObjectId First => new ObjectId(1);

        public ObjectId Next() => new ObjectId(++_tag);
    }
}

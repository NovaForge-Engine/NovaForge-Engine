namespace GameObjectsBase
{
    public abstract class Component
    {
        public GameObject parent { get; private set; }

       public void Init(GameObject parent) => this.parent = parent;

        public abstract void OnEnable();
        public abstract void Start();
        public abstract void Update();
        public abstract void FixedUpdate();
        public abstract void OnDisable();
        public abstract void OnDestroy();
    }

    public interface IAddOnceComponent
    {
        
    }
}

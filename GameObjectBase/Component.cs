namespace GameObjectsBase
{
    public abstract class Component
    {
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

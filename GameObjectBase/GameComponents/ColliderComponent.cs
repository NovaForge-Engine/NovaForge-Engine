using GameObjectsBase;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase.GameComponents
{
    internal class ColliderComponent : Component
    {
        
        public Vector3 Center { get; private set; }
        public Vector3 Size { get; private set; }
        public Quaternion Rotation { get; private set; }

        public event Action<ColliderComponent> OnCollisionEvent;

        public ColliderComponent(Vector3 size)
        {
            Size = size / 2;
        }

        public override void FixedUpdate()
        {
            
        }

        public override void OnDestroy()
        {
            Root.CollisionManager.UnregisterCollider(this);
        }

        public override void OnDisable()
        {
            Root.CollisionManager.UnregisterCollider(this);
        }

        public override void OnEnable()
        {
            Root.CollisionManager.RegisterCollider(this);
            UpdateBounds();
        }

        public override void Start()
        {
            Console.WriteLine("Collider Start");
            Root.CollisionManager.RegisterCollider(this);
            UpdateBounds();
        }

        public override void Update()
        {
            Console.WriteLine("Collider Update");
            UpdateBounds();
        }

        private void UpdateBounds()
        {
            Console.WriteLine("Collider UpdateBounds");
            Center = MathTypes.ToCSharp(parent.GetComponent<TransformComponent>().Position);
            Rotation = MathTypes.ToCSharp(parent.GetComponent<TransformComponent>().Rotation);
        }

        public bool Intersects(ColliderComponent other)
        {
            Console.WriteLine("Collider Intersects");
            return OBBIntersection(this, other);
        }

        private bool OBBIntersection(ColliderComponent a, ColliderComponent b)
        {
            Vector3[] axes = new Vector3[15];

            Vector3[] aAxes = GetAxes(a.Rotation);
            Vector3[] bAxes = GetAxes(b.Rotation);

            axes[0] = aAxes[0]; // X
            axes[1] = aAxes[1]; // Y
            axes[2] = aAxes[2]; // Z
            axes[3] = bAxes[0]; // X
            axes[4] = bAxes[1]; // Y
            axes[5] = bAxes[2]; // Z

            int index = 6;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    axes[index++] = Vector3.Cross(aAxes[i], bAxes[j]);
                }
            }

            foreach (Vector3 axis in axes)
            {
                if (axis.LengthSquared() < 1e-6f)
                    continue;

                if (!OverlapOnAxis(a, b, axis))
                    return false;
            }

            return true;
        }

        private Vector3[] GetAxes(Quaternion rotation)
        {
            Vector3[] axes = new Vector3[3];
            Matrix4x4 rotMatrix = Matrix4x4.CreateFromQuaternion(rotation);
            axes[0] = new Vector3(rotMatrix.M11, rotMatrix.M12, rotMatrix.M13); // X
            axes[1] = new Vector3(rotMatrix.M21, rotMatrix.M22, rotMatrix.M23); // Y
            axes[2] = new Vector3(rotMatrix.M31, rotMatrix.M32, rotMatrix.M33); // Z
            return axes;
        }

        private bool OverlapOnAxis(ColliderComponent a, ColliderComponent b, Vector3 axis)
        {
            float aMin, aMax, bMin, bMax;
            ProjectOBB(a, axis, out aMin, out aMax);
            ProjectOBB(b, axis, out bMin, out bMax);

            return !(aMin > bMax || bMin > aMax);
        }

        private void ProjectOBB(ColliderComponent obb, Vector3 axis, out float min, out float max)
        {
            Vector3[] corners = GetOBBCorners(obb);
            min = float.MaxValue;
            max = float.MinValue;

            foreach (Vector3 corner in corners)
            {
                float projection = Vector3.Dot(corner, axis);
                if (projection < min) min = projection;
                if (projection > max) max = projection;
            }
        }

        private Vector3[] GetOBBCorners(ColliderComponent obb)
        {
            Vector3[] corners = new Vector3[8];
            Vector3[] baseAxes = GetAxes(obb.Rotation);
            Vector3 center = obb.Center;
            Vector3 size = obb.Size;

            corners[0] = center + baseAxes[0] * size.X + baseAxes[1] * size.Y + baseAxes[2] * size.Z;
            corners[1] = center - baseAxes[0] * size.X + baseAxes[1] * size.Y + baseAxes[2] * size.Z;
            corners[2] = center + baseAxes[0] * size.X - baseAxes[1] * size.Y + baseAxes[2] * size.Z;
            corners[3] = center + baseAxes[0] * size.X + baseAxes[1] * size.Y - baseAxes[2] * size.Z;
            corners[4] = center - baseAxes[0] * size.X - baseAxes[1] * size.Y - baseAxes[2] * size.Z;
            corners[5] = center + baseAxes[0] * size.X - baseAxes[1] * size.Y - baseAxes[2] * size.Z;
            corners[6] = center - baseAxes[0] * size.X + baseAxes[1] * size.Y - baseAxes[2] * size.Z;
            corners[7] = center - baseAxes[0] * size.X - baseAxes[1] * size.Y + baseAxes[2] * size.Z;

            return corners;
        }

        public void OnCollision(ColliderComponent col)
        {
            OnCollisionEvent?.Invoke(col);
        }
    }
}

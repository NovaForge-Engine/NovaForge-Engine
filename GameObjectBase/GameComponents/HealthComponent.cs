using GameObjectsBase;
using GlmSharp;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameObjectBase.GameComponents {
    
    internal class HealthComponent : Component {
        float maxHealth = 100.f;
        float health = maxHealth;
        public override void FixedUpdate() {
		// None
        }

        public override void OnDestroy() {
		// None
        }

        public override void OnDisable() {
		// None
        }

        public override void OnEnable() {
		// None
        }

        public override void Start() {
        // None
        }

        public override void Update() {
		// None
        }

        public void OverrideHealth(float health) {
            this.health = health;
            if (health < 0) { this.health = 0; Damage(0); };
            if (health > maxHealth) { this.health = maxHealth; };
        }

        public void Damage(float damage) {
            health -= damage;
            if (health <= 0) { 
                health = 0;
                GameManager.EndGame(); // TODO: link to the actual function
            }
        }

        public float GetHealth() {
            return health;
        }

        public float GetMaxHealth() {
            return maxHealth;
        }
    }
}

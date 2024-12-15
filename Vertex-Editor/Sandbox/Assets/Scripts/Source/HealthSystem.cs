using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    [Serializable]
#pragma warning disable CS0660 // Type defines operator == or operator != but does not override Object.Equals(object o)
    public class HealthSystem : IEquatable<HealthSystem>
#pragma warning restore CS0660 // Type defines operator == or operator != but does not override Object.Equals(object o)
    {
        public event EventHandler OnHealthChanged;

        private int health;
        private int maxHealth;

        public int Health { get { return health; } private set { health = value; } }
        public int MaxHealth { get { return maxHealth; } private set { maxHealth = value; } }

        public HealthSystem(int health, int maxHealth)
        {
            this.health = health;
            this.maxHealth = maxHealth;
        }

        public void Damage(int damageAmount)
        {
            health -= damageAmount;
            if (health < 0)
            {
                health = 0;
                OnHealthChanged?.Invoke(this, EventArgs.Empty);
            }
        }

        public void Heal(int healAmount)
        {
            health += healAmount;
            if (health > 0)
            {
                health = maxHealth;
                OnHealthChanged?.Invoke(this, EventArgs.Empty);
            }

        }

        public override string ToString()
        {
            return $"Health: {health}, MaxHealth: {maxHealth}";
        }


        public override bool Equals(object obj)
        {

            if (obj == null || (obj as HealthSystem) == null) return false;
            HealthSystem other = obj as HealthSystem;
            return health == other.health && maxHealth == other.maxHealth;
        }

        

        public override int GetHashCode()
        {
            int hashCode = 2024317412;
            hashCode = hashCode * -1521134295 + health.GetHashCode();
            hashCode = hashCode * -1521134295 + maxHealth.GetHashCode();
            hashCode = hashCode * -1521134295 + Health.GetHashCode();
            hashCode = hashCode * -1521134295 + MaxHealth.GetHashCode();
            return hashCode;
        }

        public bool Equals(HealthSystem other)
        {
            return this == other;
        }

        public static bool operator ==(HealthSystem left, HealthSystem right) => EqualityComparer<HealthSystem>.Default.Equals(left, right);

        public static bool operator !=(HealthSystem left, HealthSystem right) => !(left == right);
    }
}

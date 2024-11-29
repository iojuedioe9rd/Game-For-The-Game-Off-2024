using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vertex;

namespace Sandbox
{
    public class EnemySpawner : Entity
    {
        public Vector2 BoxSize = new Vector2(100, 100);

        public float speed = 1.5f;
        public float spawnTimer = 0.0f;

        public float gameTimer = 0.0f;

        public List<Enemy> enemies = new List<Enemy>();

        protected override void OnCreate()
        {
            enemies = new List<Enemy>();
            base.OnCreate();
        }

        protected override void OnUpdate(float ts)
        {
            spawnTimer += ts;
            gameTimer += ts;

            if (spawnTimer >= speed)
            {
                spawnTimer = 0.0f;
                SpawnEnemy();
            }


        }

        public int HashFloat(float number)
        {
            byte[] bytes = BitConverter.GetBytes(number); // Convert float to 4 bytes
            int bits = BitConverter.ToInt32(bytes, 0); // Convert bytes to int
            bits = (bits ^ (bits >> 16)) * 0x45d9f3b;
            bits = (bits ^ (bits >> 16)) * 0x45d9f3b;
            bits = bits ^ (bits >> 16);
            return bits;
        }


        public void SpawnEnemy()
        {
            Random rng = new Random(HashFloat(gameTimer));

            float posX = (float)(rng.NextDouble() * BoxSize.X);
            float posY = (float)(rng.NextDouble() * BoxSize.X);

            Vector3 pos = new Vector3(posX, posY, 0);

            Logger.Info(pos);

            Enemy enemy = Entity.NewEntity<Enemy>("Enemy", pos, Vector3.One, Vector3.Zero);
            if(enemy != null)
            {
                Logger.Info("Hi");
                enemies.Add(enemy);
                enemy.Time = gameTimer;
            }
            
        }



    }
}

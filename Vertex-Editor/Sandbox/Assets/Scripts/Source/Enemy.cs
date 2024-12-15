using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Vertex;

namespace Sandbox
{
    public class Enemy : ENTBaseBoxCollier2D
    {
        public float Time = 0.0f;

        Player player;

        public Enemy(string uuid) : base(uuid)
        {
        }

        public Enemy()
        {
        }

        protected override void OnCreate()
        {
            player = FindEntityByName("Player").As<Player>();

            Type = BodyType.Dynamic;
            base.OnCreate();

        }

        public static bool Intersects(Vector2 pos1, Vector2 size1, Vector2 pos2, Vector2 size2)
        {
            return pos1.X < pos2.X + size2.X &&
                   pos1.X + size1.X > pos2.X &&
                   pos1.Y < pos2.Y + size2.Y &&
                   pos1.Y + size1.Y > pos2.Y;
        }

        protected override void OnUpdate(float ts)
        {
            Time += ts;
            if (Pos.Y < -10)
            {
                RemoveEntity(this);
            }
            float Dis = Vector3.Dis(Pos, player.Pos);
            Logger.Info("Dis: ", Dis);
            if (Dis <= 55.5f)
            {
                Vector3 diff = Pos - player.Pos;

                Pos = Vector3.MoveTowards(Pos, player.Pos, 5 * ts);
            }

            foreach (Bullet bullet in FindEntitiesByName<Bullet>("Bullet")) 
            {
                if(Intersects(Pos, Size, bullet.Pos, bullet.Size))
                {
                    RemoveEntity(this);
                }
            }
        }

        private bool Intersects(Vector3 pos1, Vector3 size1, Vector3 pos2, Vector3 size2)
        {
            return Intersects(pos1.XY, size1.XY, pos2.XY, size2.XY);
        }

        protected override void OnDraw()
        {
            base.OnDraw();
            //Logger.Info("Run");
            float offset = (float)Math.Sin(Convert.ToDouble(Time)) / 10 + .1f;

            Renderer2D.DrawQuad(Pos + new Vector3(0, offset, 0), Size, "assets/textures/GameEnemy.png", 1.0f, new Colour(1, 1, 1, 1));
        }

        protected override void OnPhysUpdate(float ts)
        {
            base.OnPhysUpdate(ts);
        }
    }
}

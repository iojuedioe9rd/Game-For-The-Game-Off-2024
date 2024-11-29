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

        protected override void OnUpdate(float ts)
        {
            Time += ts;
            if (Pos.Y < -10)
            {
                RemoveEntity(this);
            }

            if(Vector3.Dis(Pos, player.Pos) <= 5.5f)
            {
                Vector3 diff = Pos - player.Pos;

                Pos += diff.Normalized * ts * 5.0f;
            }
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

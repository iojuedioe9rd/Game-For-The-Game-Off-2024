using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vertex;

namespace Sandbox
{
    public class Player : ENTBaseBoxCollier2D
    {
        public HealthSystem healthSystem;
        public int LOL;

        public float Speed = 10.0f;
        public float JumpTime = 0.0f;
        public float Time = 0.0f;
        public float GunDistance = 2;


        public List<Gun> Guns = new List<Gun>();
        Gun currentGun;
        int currentGunIndex = -1;

        public Player(string uuid) : base(uuid)
        {
        }

        public Player()
        {
        }

        protected override void OnCreate()
        {
            Logger.Info("Run 2");
            Type = BodyType.Dynamic;
            base.OnCreate();


            Guns = new List<Gun>();
            Guns.Add(new Gun(this));
            currentGun = Guns[0];
            currentGunIndex = 0;
        }

        float offset;
        protected override void OnDraw()
        {
            base.OnDraw();
            //Logger.Info("Run");
            offset = (float)Math.Sin(Convert.ToDouble(Time)) / 10 + .1f;

            Renderer2D.DrawQuad(Pos + new Vector3(0, offset, 0), Size, "assets/textures/GamePlayer.png", 1.0f, new Colour(1, 1, 1, 1));


            Vector2 mousePosWorld = (Input.GetMousePos(true).Normalized);
            Logger.Info(mousePosWorld);
            float radians = Mathf.Atan2(mousePosWorld.Y, mousePosWorld.X);


            float newX = GunDistance * Mathf.Cos(radians);
            float newY = GunDistance * Mathf.Sin(radians);

            Renderer2D.DrawQuad(Pos + new Vector3(0, offset, 0) + new Vector3(newX, newY, 0), Size, "assets/textures/Gun.png", Mathf.RadToDeg(radians), 1.0f, new Colour(1, 1, 1, 1));
        }

        protected override void OnPhysUpdate(float ts)
        {
            base.OnPhysUpdate(ts);
        }

        protected override void OnUpdate(float ts)
        {


            if(currentGunIndex != -1)
            {
                if(Guns.Contains(currentGun))
                {
                    currentGun.Update(ts);
                    if(currentGun.CanShoot())
                    {
                        Vector2 mousePosWorld = (Input.GetMousePos(true).Normalized);
                        Logger.Info(mousePosWorld);
                        float radians = Mathf.Atan2(mousePosWorld.Y, mousePosWorld.X);


                        float newX = GunDistance * Mathf.Cos(radians);
                        float newY = GunDistance * Mathf.Sin(radians);

                        Vector2 gunPos = new Vector2(0, offset) + new Vector2(newX, newY);

                        currentGun.Shoot(gunPos.Normalized, gunPos.Magnitude);
                    }
                }
            }
            
            Time += ts * 3;

            //Logger.Info("Pos");
            //Logger.Info(Pos);

            //Logger.Info("Size");
            //Logger.Info(Size);

            base.OnUpdate(ts);
            float speed = Speed * ts;
            Vector2 velocity = Vector2.Zero;

            if (Input.IsKeyDown(KeyCode.W))
                velocity.Y = 1.0f;
            else if (Input.IsKeyDown(KeyCode.S))
                velocity.Y = -1.0f;

            if (Input.IsKeyDown(KeyCode.A))
                velocity.X = -1.0f;
            else if (Input.IsKeyDown(KeyCode.D))
                velocity.X = 1.0f;

            if(Input.IsKeyDown(KeyCode.Space))
            {
                JumpTime += ts * 2.5f;
            }

            if (Input.IsKeyUp(KeyCode.Space))
            {
                if(JumpTime != 0.0f)
                {
                    
                }
                ApplyLinearImpulse(Vector2.Up * JumpTime * 2.5f, true);
                JumpTime = 0.0f;
            }

            if (Input.IsMouseButtonPressed(MouseCode.ButtonLeft))
            {
                float rad = Mathf.Atan2(Input.GetMousePos(true).Y, Input.GetMousePos(true).X);

                Logger.Info(rad);
            }

            velocity *= speed;

            ApplyLinearImpulse(velocity, true);
            ApplyLinearImpulse(Vector2.One / 1000, false);

            //throw new Exception(UUID);
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Vertex
{
    using UUID = String;

    public class Entity : Object
    {
        public Entity(UUID uuid) : base(uuid)
        {
            
        }

        public Entity() : base(UUID.Empty)
        {
            
        }

        protected virtual void OnCreate()             { }
        protected virtual void OnUpdate(float ts)     { }
        protected virtual void OnDraw()               { }
        protected virtual void OnPhysUpdate(float ts) { }

        

        public Vector3 Pos
        {
            get
            {
                
                InternalCalls.Entity_GetTranslation(UUID, out Vector3 result);
                return result;
            }
            set
            {
                InternalCalls.Entity_SetTranslation(UUID, ref value);
            }
        }


        public static Entity FindEntityByName(string name)
        {
            string entityID = InternalCalls.Entity_FindEntityByName(name);
            if (entityID == string.Empty)
                return null;
            return new Entity(entityID);
        }

        public static T NewEntity<T>(string name, Vector3 pos, Vector3 size, Vector3 rotation) where T : Entity, new()
        {
            string FullName = typeof(T).FullName;

            Entity entity = new Entity(InternalCalls.Entity_NewEntity(FullName, name, ref pos, ref size, ref rotation));

            return entity.As<T>();
        }

        public static T[] FindEntitiesByName<T>(string name) where T : Entity, new()
        {
            UUID[] entityIDS = InternalCalls.Entity_FindEntitiesByName(name);
            T[] entities = new T[entityIDS.Length];

            for (int i = 0; i < entityIDS.Length; i++)
            {
                entities[i] = new Entity(entityIDS[i]).As<T>();
            } 
            return entities;
        }

        public static bool RemoveEntity(Entity ent)
        {
            return InternalCalls.Entity_RemoveEntity(ent.UUID);
        }

        public Vector3 Size {
            get
            {

                InternalCalls.Entity_GetSize(UUID, out Vector3 result);
                return result;
            }
            set
            {
                InternalCalls.Entity_SetSize(UUID, ref value);
            }
        }

        public Vector3 Rotation
        {
            get
            {

                InternalCalls.Entity_GetRotation(UUID, out Vector3 result);
                return result;
            }
            set
            {
                InternalCalls.Entity_SetRotation(UUID, ref value);
            }
        }
    }
}

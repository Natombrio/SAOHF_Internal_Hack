void PrintInstructions(bool bHealth, bool bSpeedUncap, bool bDamage, bool bTeleportBall);
float CalculateDistance(Vector3 ent1, Vector3 ent2);
float CalculateAngleToEntityInRad(Vector3 ent1, Vector3 ent2);
ActorEntity* FindClosestMonster(int* numOfEntities, PlayerEntity* playerBase, EntityList* entList);
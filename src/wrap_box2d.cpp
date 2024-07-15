#include <bind.h>

extern lua_State* L;

#include <box2d/box2d.h>

REF_HANDLE_TYPE(b2WorldId);
REF_HANDLE_TYPE(b2BodyId);
REF_HANDLE_TYPE(b2ShapeId);
REF_HANDLE_TYPE(b2JointId);
REF_HANDLE_TYPE(b2ChainId);

REF_CONSTANT(b2_distanceJoint);
REF_CONSTANT(b2_motorJoint);
REF_CONSTANT(b2_mouseJoint);
REF_CONSTANT(b2_prismaticJoint);
REF_CONSTANT(b2_revoluteJoint);
REF_CONSTANT(b2_weldJoint);
REF_CONSTANT(b2_wheelJoint);

REF_CONSTANT(b2_nullWorldId);
REF_CONSTANT(b2_nullBodyId);
REF_CONSTANT(b2_nullShapeId);
REF_CONSTANT(b2_nullJointId);
REF_CONSTANT(b2_nullChainId);

REF_CONSTANT(b2_defaultCategoryBits);
REF_CONSTANT(b2_defaultMaskBits);

REF_CONSTANT(b2_maxPolygonVertices);

REF_FLATTEN_FLOATS(b2Vec2);
REF_FLATTEN_FLOATS(b2AABB);
REF_FLATTEN_FLOATS(b2Transform);
REF_FLATTEN_FLOATS(b2Circle);
REF_FLATTEN_FLOATS(b2Capsule);
REF_FLATTEN_FLOATS(b2Segment);
REF_FLATTEN_FLOATS(b2Sweep);
REF_FLATTEN_FLOATS(b2Rot);

REF_STRUCT(b2Polygon,
	REF_MEMBER_ARRAY(vertices, count),
	REF_MEMBER_ARRAY(normals, count),
	REF_MEMBER(centroid),
	REF_MEMBER(radius),
);

REF_STRUCT(b2SmoothSegment,
	REF_MEMBER(ghost1),
	REF_MEMBER(segment),
	REF_MEMBER(ghost2),
	REF_MEMBER(chainId),
);

REF_STRUCT(b2Hull,
	REF_MEMBER_ARRAY(points, count),
);

REF_STRUCT(b2RayCastInput,
	REF_MEMBER(origin),
	REF_MEMBER(translation),
	REF_MEMBER(maxFraction),
);

REF_STRUCT(b2CastOutput,
	REF_MEMBER(normal),
	REF_MEMBER(point),
	REF_MEMBER(fraction),
	REF_MEMBER(iterations),
	REF_MEMBER(hit),
);

REF_STRUCT(b2WorldDef,
	REF_MEMBER(gravity),
	REF_MEMBER(restitutionThreshold),
	REF_MEMBER(contactPushoutVelocity),
	REF_MEMBER(hitEventThreshold),
	REF_MEMBER(contactHertz),
	REF_MEMBER(contactDampingRatio),
	REF_MEMBER(jointHertz),
	REF_MEMBER(jointDampingRatio),
	REF_MEMBER(enableSleep),
	REF_MEMBER(enableContinous),
	REF_MEMBER(workerCount),
	REF_MEMBER(enqueueTask),
	REF_MEMBER(finishTask),
	REF_MEMBER(userTaskContext),
	REF_MEMBER(internalValue),
);

REF_CONSTANT(b2_staticBody);
REF_CONSTANT(b2_kinematicBody);
REF_CONSTANT(b2_dynamicBody);

REF_STRUCT(b2BodyDef,
	REF_MEMBER(type),
	REF_MEMBER(position),
	REF_MEMBER(angle),
	REF_MEMBER(linearVelocity),
	REF_MEMBER(angularVelocity),
	REF_MEMBER(linearDamping),
	REF_MEMBER(angularDamping),
	REF_MEMBER(gravityScale),
	REF_MEMBER(sleepThreshold),
	REF_MEMBER(enableSleep),
	REF_MEMBER(isAwake),
	REF_MEMBER(fixedRotation),
	REF_MEMBER(isBullet),
	REF_MEMBER(isEnabled),
	REF_MEMBER(automaticMass),
	REF_MEMBER(internalValue),
);

// @TODO b2ChainDef

REF_STRUCT(b2MassData,
	REF_MEMBER(mass),
	REF_MEMBER(center),
	REF_MEMBER(I),
);

REF_STRUCT(b2Filter,
	REF_MEMBER(categoryBits),
	REF_MEMBER(maskBits),
	REF_MEMBER(groupIndex),
);

REF_STRUCT(b2QueryFilter,
	REF_MEMBER(categoryBits),
	REF_MEMBER(maskBits),
);

REF_FUNCTION(b2DefaultFilter);
REF_FUNCTION(b2DefaultQueryFilter);

REF_STRUCT(b2ShapeDef,
	REF_MEMBER(userData),
	REF_MEMBER(friction),
	REF_MEMBER(restitution),
	REF_MEMBER(density),
	REF_MEMBER(filter),
	REF_MEMBER(isSensor),
	REF_MEMBER(enableSensorEvents),
	REF_MEMBER(enableContactEvents),
	REF_MEMBER(enableHitEvents),
	REF_MEMBER(enablePreSolveEvents),
	REF_MEMBER(forceContactCreation),
	REF_MEMBER(internalValue),
);

REF_STRUCT(b2DistanceProxy,
	REF_MEMBER_ARRAY(points, count),
	REF_MEMBER(radius),
);

REF_STRUCT(b2TOIInput,
	REF_MEMBER(proxyA),
	REF_MEMBER(proxyB),
	REF_MEMBER(sweepA),
	REF_MEMBER(sweepB),
	REF_MEMBER(tMax),
);

REF_STRUCT(b2TOIOutput,
	REF_MEMBER(state),
	REF_MEMBER(t),
);

REF_STRUCT(b2DistanceJointDef,
	REF_MEMBER(bodyIdA),
	REF_MEMBER(bodyIdB),
	REF_MEMBER(localAnchorA),
	REF_MEMBER(localAnchorB),
	REF_MEMBER(length),
	REF_MEMBER(enableSpring),
	REF_MEMBER(hertz),
	REF_MEMBER(dampingRatio),
	REF_MEMBER(enableLimit),
	REF_MEMBER(minLength),
	REF_MEMBER(maxLength),
	REF_MEMBER(enableMotor),
	REF_MEMBER(maxMotorForce),
	REF_MEMBER(motorSpeed),
	REF_MEMBER(collideConnected),
	REF_MEMBER(userData),
	REF_MEMBER(internalValue),
);

REF_STRUCT(b2MotorJointDef,
	REF_MEMBER(bodyIdA),
	REF_MEMBER(bodyIdB),
	REF_MEMBER(linearOffset),
	REF_MEMBER(angularOffset),
	REF_MEMBER(maxForce),
	REF_MEMBER(maxTorque),
	REF_MEMBER(correctionFactor),
	REF_MEMBER(collideConnected),
	REF_MEMBER(userData),
	REF_MEMBER(internalValue),
);

REF_STRUCT(b2MouseJointDef,
	REF_MEMBER(bodyIdA),
	REF_MEMBER(bodyIdB),
	REF_MEMBER(target),
	REF_MEMBER(hertz),
	REF_MEMBER(dampingRatio),
	REF_MEMBER(maxForce),
	REF_MEMBER(collideConnected),
	REF_MEMBER(userData),
	REF_MEMBER(internalValue),
);

REF_STRUCT(b2PrismaticJointDef,
	REF_MEMBER(bodyIdA),
	REF_MEMBER(bodyIdB),
	REF_MEMBER(localAnchorA),
	REF_MEMBER(localAnchorB),
	REF_MEMBER(localAxisA),
	REF_MEMBER(referenceAngle),
	REF_MEMBER(enableSpring),
	REF_MEMBER(hertz),
	REF_MEMBER(dampingRatio),
	REF_MEMBER(enableLimit),
	REF_MEMBER(lowerTranslation),
	REF_MEMBER(upperTranslation),
	REF_MEMBER(enableMotor),
	REF_MEMBER(maxMotorForce),
	REF_MEMBER(motorSpeed),
	REF_MEMBER(collideConnected),
	REF_MEMBER(userData),
	REF_MEMBER(internalValue),
);

REF_STRUCT(b2RevoluteJointDef,
	REF_MEMBER(bodyIdA),
	REF_MEMBER(bodyIdB),
	REF_MEMBER(localAnchorA),
	REF_MEMBER(localAnchorB),
	REF_MEMBER(referenceAngle),
	REF_MEMBER(enableSpring),
	REF_MEMBER(hertz),
	REF_MEMBER(dampingRatio),
	REF_MEMBER(enableLimit),
	REF_MEMBER(lowerAngle),
	REF_MEMBER(upperAngle),
	REF_MEMBER(enableMotor),
	REF_MEMBER(motorSpeed),
	REF_MEMBER(drawSize),
	REF_MEMBER(collideConnected),
	REF_MEMBER(userData),
	REF_MEMBER(internalValue),
);

REF_STRUCT(b2WeldJointDef,
	REF_MEMBER(bodyIdA),
	REF_MEMBER(bodyIdB),
	REF_MEMBER(localAnchorA),
	REF_MEMBER(localAnchorB),
	REF_MEMBER(referenceAngle),
	REF_MEMBER(linearHertz),
	REF_MEMBER(angularHertz),
	REF_MEMBER(linearDampingRatio),
	REF_MEMBER(angularDampingRatio),
	REF_MEMBER(collideConnected),
	REF_MEMBER(userData),
	REF_MEMBER(internalValue),
);

REF_STRUCT(b2WheelJointDef,
	REF_MEMBER(bodyIdA),
	REF_MEMBER(bodyIdB),
	REF_MEMBER(localAnchorA),
	REF_MEMBER(localAnchorB),
	REF_MEMBER(localAxisA),
	REF_MEMBER(enableSpring),
	REF_MEMBER(hertz),
	REF_MEMBER(dampingRatio),
	REF_MEMBER(enableLimit),
	REF_MEMBER(lowerTranslation),
	REF_MEMBER(upperTranslation),
	REF_MEMBER(enableMotor),
	REF_MEMBER(maxMotorTorque),
	REF_MEMBER(motorSpeed),
	REF_MEMBER(collideConnected),
	REF_MEMBER(userData),
	REF_MEMBER(internalValue),
);

REF_STRUCT(b2SensorBeginTouchEvent,
	REF_MEMBER(sensorShapeId),
	REF_MEMBER(visitorShapeId),
);

REF_STRUCT(b2SensorEndTouchEvent,
	REF_MEMBER(sensorShapeId),
	REF_MEMBER(visitorShapeId),
);

REF_STRUCT(b2ContactBeginTouchEvent,
	REF_MEMBER(shapeIdA),
	REF_MEMBER(shapeIdB),
);

REF_STRUCT(b2ContactEndTouchEvent,
	REF_MEMBER(shapeIdA),
	REF_MEMBER(shapeIdB),
);

REF_STRUCT(b2ContactHitEvent,
	REF_MEMBER(shapeIdA),
	REF_MEMBER(shapeIdB),
	REF_MEMBER(point),
	REF_MEMBER(normal),
	REF_MEMBER(approachSpeed),
);

REF_STRUCT(b2BodyMoveEvent,
	REF_MEMBER(transform),
	REF_MEMBER(bodyId),
	REF_MEMBER(userData),
	REF_MEMBER(fellAsleep),
);

REF_STRUCT(b2Manifold,
	REF_MEMBER_ARRAY(points, pointCount),
	REF_MEMBER(normal),
);

REF_STRUCT(b2ContactData,
	REF_MEMBER(shapeIdA),
	REF_MEMBER(shapeIdB),
	REF_MEMBER(manifold),
);

REF_STRUCT(b2ManifoldPoint,
	REF_MEMBER(point),
	REF_MEMBER(anchorA),
	REF_MEMBER(anchorB),
	REF_MEMBER(separation),
	REF_MEMBER(normalImpulse),
	REF_MEMBER(tangentImpulse),
	REF_MEMBER(maxNormalImpulse),
	REF_MEMBER(normalVelocity),
	REF_MEMBER(id),
	REF_MEMBER(persisted),
);

REF_STRUCT(b2Version,
	REF_MEMBER(major),
	REF_MEMBER(minor),
	REF_MEMBER(revision),
);

REF_STRUCT(b2ShapeCastInput,
	REF_MEMBER_ARRAY(points, count),
	REF_MEMBER(radius),
	REF_MEMBER(translation),
	REF_MEMBER(maxFraction),
);

REF_STRUCT(b2ShapeCastPairInput,
	REF_MEMBER(proxyA),
	REF_MEMBER(proxyB),
	REF_MEMBER(transformA),
	REF_MEMBER(transformB),
	REF_MEMBER(translationB),
	REF_MEMBER(maxFraction),
);

REF_CONSTANT(b2_toiStateUnknown);
REF_CONSTANT(b2_toiStateFailed);
REF_CONSTANT(b2_toiStateOverlapped);
REF_CONSTANT(b2_toiStateHit);
REF_CONSTANT(b2_toiStateSeparated);

REF_FUNCTION(b2GetByteCount);
REF_FUNCTION(b2GetVersion);
REF_FUNCTION(b2DefaultWorldDef);
REF_FUNCTION(b2DefaultBodyDef);
b2WorldId wrap_b2CreateWorld(b2WorldDef def) { return b2CreateWorld(&def); }
REF_FUNCTION_EX(b2CreateWorld, wrap_b2CreateWorld);
REF_FUNCTION(b2DestroyWorld);
REF_FUNCTION(b2World_IsValid);
REF_FUNCTION(b2World_Step);

int wrap_b2World_GetSensorEvents(lua_State* L)
{
	b2WorldId worldId = REF_Cast<b2WorldId>(lua_tointeger(L, -1));
	lua_pop(L, 1);
	b2SensorEvents events = b2World_GetSensorEvents(worldId);
	lua_newtable(L);
	lua_pushstring(L, "beginEvents");
	lua_newtable(L);
	for (int i = 0; i < events.beginCount; ++i) {
		REF_LuaSet(L, events.beginEvents + i);
		lua_rawseti(L, -2, i + 1);
	}
	lua_settable(L, -3);
	lua_pushstring(L, "endEvents");
	lua_newtable(L);
	for (int i = 0; i < events.endCount; ++i) {
		REF_LuaSet(L, events.endEvents + i);
		lua_rawseti(L, -2, i + 1);
	}
	lua_settable(L, -3);
	return 1;
}
REF_WRAP_MANUAL(wrap_b2World_GetSensorEvents);

int wrap_b2World_GetContactEvents(lua_State* L)
{
	b2WorldId worldId = REF_Cast<b2WorldId>(lua_tointeger(L, -1));
	lua_pop(L, 1);
	b2ContactEvents events = b2World_GetContactEvents(worldId);
	lua_newtable(L);
	lua_pushstring(L, "beginEvents");
	lua_newtable(L);
	for (int i = 0; i < events.beginCount; ++i) {
		REF_LuaSet(L, events.beginEvents + i);
		lua_rawseti(L, -2, i + 1);
	}
	lua_settable(L, -3);
	lua_pushstring(L, "endEvents");
	lua_newtable(L);
	for (int i = 0; i < events.endCount; ++i) {
		REF_LuaSet(L, events.endEvents + i);
		lua_rawseti(L, -2, i + 1);
	}
	lua_settable(L, -3);
	lua_pushstring(L, "hitEvents");
	lua_newtable(L);
	for (int i = 0; i < events.hitCount; ++i) {
		REF_LuaSet(L, events.hitEvents + i);
		lua_rawseti(L, -2, i + 1);
	}
	lua_settable(L, -3);
	return 1;
}
REF_WRAP_MANUAL(wrap_b2World_GetContactEvents);

bool wrap_b2OverlapResultFcn(b2ShapeId shapeId, void* context)
{
	const char* lua_fn_name = (const char*)context;
	bool result = true;
	REF_CallLuaFunction(L, lua_fn_name, { result }, shapeId);
	return result;
}
int wrap_b2World_OverlapAABB(lua_State* L)
{
	b2WorldId worldId = REF_Cast<b2WorldId>(lua_tointeger(L, -1));
	b2AABB shape;
	REF_LuaGet(L, -2, &shape);
	b2QueryFilter filter;
	REF_LuaGet(L, -3, &filter);
	const char* lua_fn_name = lua_tostring(L, -4);
	b2World_OverlapAABB(worldId, shape, filter, wrap_b2OverlapResultFcn, (void*)lua_fn_name);
	lua_pop(L, 4);
	return 0;
}
REF_WRAP_MANUAL(wrap_b2World_OverlapAABB);
#define WRAP_WORLD_OVERLAP(T) \
	int wrap_b2World_Overlap##T(lua_State* L) \
	{ \
		b2WorldId worldId = REF_Cast<b2WorldId>(lua_tointeger(L, -1)); \
		b2##T shape; \
		REF_LuaGet(L, -2, &shape); \
		b2QueryFilter filter; \
		REF_LuaGet(L, -3, &filter); \
		const char* lua_fn_name = lua_tostring(L, -4); \
		b2World_Overlap##T(worldId, &shape, b2Transform_identity, filter, wrap_b2OverlapResultFcn, (void*)lua_fn_name); \
		lua_pop(L, 4); \
		return 0; \
	} \
	REF_WRAP_MANUAL(wrap_b2World_Overlap##T)
WRAP_WORLD_OVERLAP(Circle);
WRAP_WORLD_OVERLAP(Capsule);
WRAP_WORLD_OVERLAP(Polygon);

float wrap_b2CastResultFcn(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
{
	const char* lua_fn_name = (const char*)context;
	float result = 1.0f;
	REF_CallLuaFunction(L, lua_fn_name, { result }, shapeId, point, normal, fraction);
	return result;
}
#define WRAP_WORLD_CAST(T) \
	int wrap_b2World_Cast##T(lua_State* L) \
	{ \
		b2WorldId worldId = REF_Cast<b2WorldId>(lua_tointeger(L, -1)); \
		b2##T shape; \
		REF_LuaGet(L, -2, &shape); \
		b2Vec2 translation; \
		REF_LuaGet(L, -3, &translation); \
		b2QueryFilter filter; \
		REF_LuaGet(L, -4, &filter); \
		const char* lua_fn_name = lua_tostring(L, -5); \
		b2World_Cast##T(worldId, &shape, b2Transform_identity, translation, filter, wrap_b2CastResultFcn, (void*)lua_fn_name); \
		lua_pop(L, 5); \
		return 0; \
	} \
	REF_WRAP_MANUAL(wrap_b2World_Cast##T)
WRAP_WORLD_CAST(Circle);
WRAP_WORLD_CAST(Capsule);
WRAP_WORLD_CAST(Polygon);

REF_STRUCT(b2Profile,
	REF_MEMBER(step),
	REF_MEMBER(pairs),
	REF_MEMBER(collide),
	REF_MEMBER(solve),
	REF_MEMBER(buildIslands),
	REF_MEMBER(solveConstraints),
	REF_MEMBER(prepareTasks),
	REF_MEMBER(solverTasks),
	REF_MEMBER(prepareConstraints),
	REF_MEMBER(integrateVelocities),
	REF_MEMBER(warmStart),
	REF_MEMBER(solveVelocities),
	REF_MEMBER(integratePositions),
	REF_MEMBER(relaxVelocities),
	REF_MEMBER(applyRestitution),
	REF_MEMBER(storeImpulses),
	REF_MEMBER(finalizeBodies),
	REF_MEMBER(splitIslands),
	REF_MEMBER(sleepIslands),
	REF_MEMBER(hitEvents),
	REF_MEMBER(broadphase),
	REF_MEMBER(continuous),
);

REF_STRUCT(b2Counters,
	REF_MEMBER(staticBodyCount),
	REF_MEMBER(bodyCount),
	REF_MEMBER(shapeCount),
	REF_MEMBER(contactCount),
	REF_MEMBER(jointCount),
	REF_MEMBER(islandCount),
	REF_MEMBER(stackUsed),
	REF_MEMBER(staticTreeHeight),
	REF_MEMBER(treeHeight),
	REF_MEMBER(byteCount),
	REF_MEMBER(taskCount),
	//REF_MEMBER(colorCounts), REF can't support arrays like this yet
);

REF_FUNCTION(b2World_EnableSleeping);
REF_FUNCTION(b2World_EnableContinuous);
REF_FUNCTION(b2World_SetRestitutionThreshold);
REF_FUNCTION(b2World_SetHitEventThreshold);
REF_FUNCTION(b2World_SetGravity);
REF_FUNCTION(b2World_Explode);
REF_FUNCTION(b2World_SetContactTuning);
REF_FUNCTION(b2World_EnableWarmStarting);
REF_FUNCTION(b2World_GetProfile);
REF_FUNCTION(b2World_GetCounters);
REF_FUNCTION(b2World_DumpMemoryStats);

b2BodyId wrap_b2CreateBody(b2WorldId worldId, b2BodyDef def) { return b2CreateBody(worldId, &def); }
REF_FUNCTION_EX(b2CreateBody, wrap_b2CreateBody);
REF_FUNCTION(b2DestroyBody);
REF_FUNCTION(b2Body_IsValid);
REF_FUNCTION(b2Body_GetType);
REF_FUNCTION(b2Body_SetType);
REF_FUNCTION(b2Body_GetPosition);
REF_FUNCTION(b2Body_GetRotation);
REF_FUNCTION(b2Body_GetAngle);
REF_FUNCTION(b2Body_GetTransform);
REF_FUNCTION(b2Body_SetTransform);
REF_FUNCTION(b2Body_GetLocalPoint);
REF_FUNCTION(b2Body_GetWorldPoint);
REF_FUNCTION(b2Body_GetLocalVector);
REF_FUNCTION(b2Body_GetLinearVelocity);
REF_FUNCTION(b2Body_GetAngularVelocity);
REF_FUNCTION(b2Body_SetLinearVelocity);
REF_FUNCTION(b2Body_SetAngularVelocity);
REF_FUNCTION(b2Body_ApplyForce);
REF_FUNCTION(b2Body_ApplyForceToCenter);
REF_FUNCTION(b2Body_ApplyTorque);
REF_FUNCTION(b2Body_ApplyLinearImpulse);
REF_FUNCTION(b2Body_ApplyLinearImpulseToCenter);
REF_FUNCTION(b2Body_ApplyAngularImpulse);
REF_FUNCTION(b2Body_GetMass);
REF_FUNCTION(b2Body_GetInertiaTensor);
REF_FUNCTION(b2Body_GetLocalCenterOfMass);
REF_FUNCTION(b2Body_GetWorldCenterOfMass);
REF_FUNCTION(b2Body_SetMassData);
REF_FUNCTION(b2Body_GetMassData);
REF_FUNCTION(b2Body_ApplyMassFromShapes);
//REF_FUNCTION(b2Body_SetAutomaticMass);
//REF_FUNCTION(b2Body_GetAutomaticMass);
REF_FUNCTION(b2Body_SetLinearDamping);
REF_FUNCTION(b2Body_GetLinearDamping);
REF_FUNCTION(b2Body_SetAngularDamping);
REF_FUNCTION(b2Body_GetAngularDamping);
REF_FUNCTION(b2Body_SetGravityScale);
REF_FUNCTION(b2Body_GetGravityScale);
REF_FUNCTION(b2Body_IsAwake);
REF_FUNCTION(b2Body_SetAwake);
REF_FUNCTION(b2Body_EnableSleep);
REF_FUNCTION(b2Body_IsSleepEnabled);
REF_FUNCTION(b2Body_SetSleepThreshold);
REF_FUNCTION(b2Body_GetSleepThreshold);
REF_FUNCTION(b2Body_IsEnabled);
REF_FUNCTION(b2Body_Disable);
REF_FUNCTION(b2Body_Enable);
REF_FUNCTION(b2Body_SetFixedRotation);
REF_FUNCTION(b2Body_IsFixedRotation);
REF_FUNCTION(b2Body_SetBullet);
REF_FUNCTION(b2Body_IsBullet);
REF_FUNCTION(b2Body_EnableHitEvents);
REF_FUNCTION(b2Body_GetShapeCount);
// @TODO b2Body_GetShapes
REF_FUNCTION(b2Body_GetJointCount);
// @TODO b2Body_GetJoints
REF_FUNCTION(b2Body_GetContactCapacity);
// @TODO b2Body_GetContactData
REF_FUNCTION(b2Body_ComputeAABB);

b2ShapeId wrap_b2CreateCircleShape(b2BodyId bodyId, b2ShapeDef def, b2Circle shape) { return b2CreateCircleShape(bodyId, &def, &shape); }
REF_FUNCTION_EX(b2CreateCircleShape, wrap_b2CreateCircleShape);
b2ShapeId wrap_b2CreateSegmentShape(b2BodyId bodyId, b2ShapeDef def, b2Segment shape) { return b2CreateSegmentShape(bodyId, &def, &shape); }
REF_FUNCTION_EX(b2CreateSegmentShape, wrap_b2CreateSegmentShape);
b2ShapeId wrap_b2CreateCapsuleShape(b2BodyId bodyId, b2ShapeDef def, b2Capsule shape) { return b2CreateCapsuleShape(bodyId, &def, &shape); }
REF_FUNCTION_EX(b2CreateCapsuleShape, wrap_b2CreateCapsuleShape);
b2ShapeId wrap_b2CreatePolygonShape(b2BodyId bodyId, b2ShapeDef def, b2Polygon shape) { return b2CreatePolygonShape(bodyId, &def, &shape); }
REF_FUNCTION_EX(b2CreatePolygonShape, wrap_b2CreatePolygonShape);

REF_FUNCTION(b2DestroyShape);
REF_FUNCTION(b2Shape_IsValid);
REF_FUNCTION(b2Shape_GetType);
REF_FUNCTION(b2Shape_GetBody);
REF_FUNCTION(b2Shape_IsSensor);
REF_FUNCTION(b2Shape_SetDensity);
REF_FUNCTION(b2Shape_GetDensity);
REF_FUNCTION(b2Shape_SetFriction);
REF_FUNCTION(b2Shape_GetFriction);
REF_FUNCTION(b2Shape_SetRestitution);
REF_FUNCTION(b2Shape_GetRestitution);
REF_FUNCTION(b2Shape_GetFilter);
REF_FUNCTION(b2Shape_SetFilter);
REF_FUNCTION(b2Shape_EnableSensorEvents);
REF_FUNCTION(b2Shape_AreSensorEventsEnabled);
REF_FUNCTION(b2Shape_EnableContactEvents);
REF_FUNCTION(b2Shape_AreContactEventsEnabled);
REF_FUNCTION(b2Shape_EnablePreSolveEvents);
REF_FUNCTION(b2Shape_ArePreSolveEventsEnabled);
REF_FUNCTION(b2Shape_EnableHitEvents);
REF_FUNCTION(b2Shape_AreHitEventsEnabled);
REF_FUNCTION(b2Shape_TestPoint);
REF_FUNCTION(b2Shape_RayCast);
REF_FUNCTION(b2Shape_GetCircle);
REF_FUNCTION(b2Shape_GetSegment);
REF_FUNCTION(b2Shape_GetSmoothSegment);
REF_FUNCTION(b2Shape_GetCapsule);
REF_FUNCTION(b2Shape_GetPolygon);

void wrap_b2Shape_SetCircle(b2ShapeId shapeId, b2Circle shape) { b2Shape_SetCircle(shapeId, &shape); }
REF_FUNCTION_EX(b2Shape_SetCircle, wrap_b2Shape_SetCircle);
void wrap_b2Shape_SetCapsule(b2ShapeId shapeId, b2Capsule shape) { b2Shape_SetCapsule(shapeId, &shape); }
REF_FUNCTION_EX(b2Shape_SetCapsule, wrap_b2Shape_SetCapsule);
void wrap_b2Shape_SetSegment(b2ShapeId shapeId, b2Segment shape) { b2Shape_SetSegment(shapeId, &shape); }
REF_FUNCTION_EX(b2Shape_SetSegment, wrap_b2Shape_SetSegment);
void wrap_b2Shape_SetPolygon(b2ShapeId shapeId, b2Polygon shape) { b2Shape_SetPolygon(shapeId, &shape); }
REF_FUNCTION_EX(b2Shape_SetPolygon, wrap_b2Shape_SetPolygon);

REF_FUNCTION(b2Shape_GetParentChain);
REF_FUNCTION(b2Shape_GetContactCapacity);
// @TODO b2Shape_GetContactData
REF_FUNCTION(b2Shape_GetAABB);
REF_FUNCTION(b2Shape_GetClosestPoint);

// @TODO chain functions

REF_FUNCTION(b2DestroyJoint);
REF_FUNCTION(b2Joint_IsValid);
REF_FUNCTION(b2Joint_GetType);
REF_FUNCTION(b2Joint_GetBodyA);
REF_FUNCTION(b2Joint_GetBodyB);
REF_FUNCTION(b2Joint_GetLocalAnchorA);
REF_FUNCTION(b2Joint_GetLocalAnchorB);
REF_FUNCTION(b2Joint_SetCollideConnected);
REF_FUNCTION(b2Joint_GetCollideConnected);
REF_FUNCTION(b2Joint_WakeBodies);
REF_FUNCTION(b2Joint_GetConstraintTorque);

REF_FUNCTION(b2DefaultDistanceJointDef);
REF_FUNCTION(b2DefaultMotorJointDef);
REF_FUNCTION(b2DefaultMouseJointDef);
REF_FUNCTION(b2DefaultPrismaticJointDef);
REF_FUNCTION(b2DefaultRevoluteJointDef);
REF_FUNCTION(b2DefaultWeldJointDef);
REF_FUNCTION(b2DefaultWheelJointDef);

b2JointId wrap_b2CreateDistanceJoint(b2WorldId worldId, b2DistanceJointDef def) { return b2CreateDistanceJoint(worldId, &def); }
REF_FUNCTION_EX(b2CreateDistanceJoint, wrap_b2CreateDistanceJoint);
REF_FUNCTION(b2DistanceJoint_SetLength);
REF_FUNCTION(b2DistanceJoint_GetLength);
REF_FUNCTION(b2DistanceJoint_EnableSpring);
REF_FUNCTION(b2DistanceJoint_IsSpringEnabled);
REF_FUNCTION(b2DistanceJoint_SetSpringHertz);
REF_FUNCTION(b2DistanceJoint_SetSpringDampingRatio);
REF_FUNCTION(b2DistanceJoint_GetHertz);
REF_FUNCTION(b2DistanceJoint_GetDampingRatio);
REF_FUNCTION(b2DistanceJoint_EnableLimit);
//REF_FUNCTION(b2DistanceJoint_IsLimitEnabled);
REF_FUNCTION(b2DistanceJoint_SetLengthRange);
REF_FUNCTION(b2DistanceJoint_GetMinLength);
REF_FUNCTION(b2DistanceJoint_GetMaxLength);
REF_FUNCTION(b2DistanceJoint_GetCurrentLength);
REF_FUNCTION(b2DistanceJoint_EnableMotor);
REF_FUNCTION(b2DistanceJoint_IsMotorEnabled);
REF_FUNCTION(b2DistanceJoint_SetMotorSpeed);
REF_FUNCTION(b2DistanceJoint_GetMotorSpeed);
REF_FUNCTION(b2DistanceJoint_SetMaxMotorForce);
REF_FUNCTION(b2DistanceJoint_GetMaxMotorForce);
REF_FUNCTION(b2DistanceJoint_GetMotorForce);

b2JointId wrap_b2CreateMotorJoint(b2WorldId worldId, b2MotorJointDef def) { return b2CreateMotorJoint(worldId, &def); }
REF_FUNCTION_EX(b2CreateMotorJoint, wrap_b2CreateMotorJoint);
REF_FUNCTION(b2MotorJoint_SetLinearOffset);
REF_FUNCTION(b2MotorJoint_GetLinearOffset);
REF_FUNCTION(b2MotorJoint_SetAngularOffset);
REF_FUNCTION(b2MotorJoint_GetAngularOffset);
REF_FUNCTION(b2MotorJoint_SetMaxForce);
REF_FUNCTION(b2MotorJoint_GetMaxForce);
REF_FUNCTION(b2MotorJoint_SetMaxTorque);
REF_FUNCTION(b2MotorJoint_GetMaxTorque);
REF_FUNCTION(b2MotorJoint_SetCorrectionFactor);
REF_FUNCTION(b2MotorJoint_GetCorrectionFactor);

b2JointId wrap_b2CreateMouseJoint(b2WorldId worldId, b2MouseJointDef def) { return b2CreateMouseJoint(worldId, &def); }
REF_FUNCTION_EX(b2CreateMouseJoint, wrap_b2CreateMouseJoint);
REF_FUNCTION(b2MouseJoint_SetTarget);
REF_FUNCTION(b2MouseJoint_GetTarget);
REF_FUNCTION(b2MouseJoint_SetSpringHertz);
REF_FUNCTION(b2MouseJoint_GetSpringHertz);
REF_FUNCTION(b2MouseJoint_SetSpringDampingRatio);
REF_FUNCTION(b2MouseJoint_GetSpringDampingRatio);
REF_FUNCTION(b2MouseJoint_SetMaxForce);
REF_FUNCTION(b2MouseJoint_GetMaxForce);

b2JointId wrap_b2CreatePrismaticJoint(b2WorldId worldId, b2PrismaticJointDef def) { return b2CreatePrismaticJoint(worldId, &def); }
REF_FUNCTION_EX(b2CreatePrismaticJoint, wrap_b2CreatePrismaticJoint);
REF_FUNCTION(b2PrismaticJoint_EnableSpring);
REF_FUNCTION(b2PrismaticJoint_IsSpringEnabled);
REF_FUNCTION(b2PrismaticJoint_SetSpringHertz);
REF_FUNCTION(b2PrismaticJoint_GetSpringHertz);
REF_FUNCTION(b2PrismaticJoint_SetSpringDampingRatio);
REF_FUNCTION(b2PrismaticJoint_GetSpringDampingRatio);
REF_FUNCTION(b2PrismaticJoint_EnableLimit);
REF_FUNCTION(b2PrismaticJoint_IsLimitEnabled);
REF_FUNCTION(b2PrismaticJoint_GetLowerLimit);
REF_FUNCTION(b2PrismaticJoint_GetUpperLimit);
REF_FUNCTION(b2PrismaticJoint_SetLimits);
REF_FUNCTION(b2PrismaticJoint_EnableMotor);
REF_FUNCTION(b2PrismaticJoint_IsMotorEnabled);
REF_FUNCTION(b2PrismaticJoint_SetMotorSpeed);
REF_FUNCTION(b2PrismaticJoint_GetMotorSpeed);
REF_FUNCTION(b2PrismaticJoint_SetMaxMotorForce);
REF_FUNCTION(b2PrismaticJoint_GetMaxMotorForce);
REF_FUNCTION(b2PrismaticJoint_GetMotorForce);

b2JointId wrap_b2CreateRevoluteJoint(b2WorldId worldId, b2RevoluteJointDef def) { return b2CreateRevoluteJoint(worldId, &def); }
REF_FUNCTION_EX(b2CreateRevoluteJoint, wrap_b2CreateRevoluteJoint);
REF_FUNCTION(b2RevoluteJoint_EnableSpring);
REF_FUNCTION(b2RevoluteJoint_IsLimitEnabled);
REF_FUNCTION(b2RevoluteJoint_SetSpringHertz);
REF_FUNCTION(b2RevoluteJoint_GetSpringHertz);
REF_FUNCTION(b2RevoluteJoint_SetSpringDampingRatio);
REF_FUNCTION(b2RevoluteJoint_GetSpringDampingRatio);
REF_FUNCTION(b2RevoluteJoint_GetAngle);
REF_FUNCTION(b2RevoluteJoint_GetLowerLimit);
REF_FUNCTION(b2RevoluteJoint_GetUpperLimit);
REF_FUNCTION(b2RevoluteJoint_SetLimits);
REF_FUNCTION(b2RevoluteJoint_EnableMotor);
REF_FUNCTION(b2RevoluteJoint_IsMotorEnabled);
REF_FUNCTION(b2RevoluteJoint_SetMotorSpeed);
REF_FUNCTION(b2RevoluteJoint_GetMotorSpeed);
REF_FUNCTION(b2RevoluteJoint_GetMotorTorque);
REF_FUNCTION(b2RevoluteJoint_SetMaxMotorTorque);
REF_FUNCTION(b2RevoluteJoint_GetMaxMotorTorque);

b2JointId wrap_b2CreateWeldJoint(b2WorldId worldId, b2WeldJointDef def) { return b2CreateWeldJoint(worldId, &def); }
REF_FUNCTION_EX(b2CreateWeldJoint, wrap_b2CreateWeldJoint);
REF_FUNCTION(b2WeldJoint_SetLinearHertz);
REF_FUNCTION(b2WeldJoint_GetLinearHertz);
REF_FUNCTION(b2WeldJoint_SetLinearDampingRatio);
REF_FUNCTION(b2WeldJoint_GetLinearDampingRatio);
REF_FUNCTION(b2WeldJoint_SetAngularHertz);
REF_FUNCTION(b2WeldJoint_GetAngularHertz);
REF_FUNCTION(b2WeldJoint_SetAngularDampingRatio);
REF_FUNCTION(b2WeldJoint_GetAngularDampingRatio);

b2JointId wrap_b2CreateWheelJoint(b2WorldId worldId, b2WheelJointDef def) { return b2CreateWheelJoint(worldId, &def); }
REF_FUNCTION_EX(b2CreateWheelJoint, wrap_b2CreateWheelJoint);
REF_FUNCTION(b2WheelJoint_EnableSpring);
REF_FUNCTION(b2WheelJoint_IsSpringEnabled);
REF_FUNCTION(b2WheelJoint_SetSpringHertz);
REF_FUNCTION(b2WheelJoint_GetSpringHertz);
REF_FUNCTION(b2WheelJoint_SetSpringDampingRatio);
REF_FUNCTION(b2WheelJoint_GetSpringDampingRatio);
REF_FUNCTION(b2WheelJoint_EnableLimit);
REF_FUNCTION(b2WheelJoint_IsLimitEnabled);
REF_FUNCTION(b2WheelJoint_GetLowerLimit);
REF_FUNCTION(b2WheelJoint_GetUpperLimit);
REF_FUNCTION(b2WheelJoint_SetLimits);
REF_FUNCTION(b2WheelJoint_EnableMotor);
REF_FUNCTION(b2WheelJoint_IsMotorEnabled);
REF_FUNCTION(b2WheelJoint_SetMotorSpeed);
REF_FUNCTION(b2WheelJoint_GetMotorSpeed);
REF_FUNCTION(b2WheelJoint_SetMaxMotorTorque);
REF_FUNCTION(b2WheelJoint_GetMaxMotorTorque);
REF_FUNCTION(b2WheelJoint_GetMotorTorque);

bool wrap_b2IsValidRay(b2RayCastInput input) { return b2IsValidRay(&input); }
REF_FUNCTION_EX(b2IsValidRay, wrap_b2IsValidRay);
b2Polygon wrap_b2MakePolygon(b2Hull hull, float radius) { return b2MakePolygon(&hull, radius); }
REF_FUNCTION_EX(b2MakePolygon, wrap_b2MakePolygon);
b2Polygon wrap_b2MakeOffsetPolygon(b2Hull hull, float radius, b2Transform transform) { return b2MakeOffsetPolygon(&hull, radius, transform); }
REF_FUNCTION_EX(b2MakeOffsetPolygon, wrap_b2MakeOffsetPolygon);
REF_FUNCTION(b2MakeSquare);
REF_FUNCTION(b2MakeBox);
REF_FUNCTION(b2MakeRoundedBox);
REF_FUNCTION(b2MakeOffsetBox);
b2Polygon wrap_b2TransformPolygon(b2Transform transform, b2Polygon polygon) { return b2TransformPolygon(transform, &polygon); }
REF_FUNCTION_EX(b2TransformPolygon, wrap_b2TransformPolygon);
b2MassData wrap_b2ComputeCircleMass(b2Circle shape, float density) { return b2ComputeCircleMass(&shape, density); }
REF_FUNCTION_EX(b2ComputeCircleMass, wrap_b2ComputeCircleMass);
b2MassData wrap_b2ComputeCapsuleMass(b2Capsule shape, float density) { return b2ComputeCapsuleMass(&shape, density); }
REF_FUNCTION_EX(b2ComputeCapsuleMass, wrap_b2ComputeCapsuleMass);
b2MassData wrap_b2ComputePolygonMass(b2Polygon shape, float density) { return b2ComputePolygonMass(&shape, density); }
REF_FUNCTION_EX(b2ComputePolygonMass, wrap_b2ComputePolygonMass);
b2AABB wrap_b2ComputeCircleAABB(b2Circle shape, b2Transform transform) { return b2ComputeCircleAABB(&shape, transform); }
REF_FUNCTION_EX(b2ComputeCircleAABB, wrap_b2ComputeCircleAABB);
b2AABB wrap_b2ComputeCapsuleAABB(b2Capsule shape, b2Transform transform) { return b2ComputeCapsuleAABB(&shape, transform); }
REF_FUNCTION_EX(b2ComputeCapsuleAABB, wrap_b2ComputeCapsuleAABB);
b2AABB wrap_b2ComputePolygonAABB(b2Polygon shape, b2Transform transform) { return b2ComputePolygonAABB(&shape, transform); }
REF_FUNCTION_EX(b2ComputePolygonAABB, wrap_b2ComputePolygonAABB);
b2AABB wrap_b2ComputeSegmentAABB(b2Segment shape, b2Transform transform) { return b2ComputeSegmentAABB(&shape, transform); }
REF_FUNCTION_EX(b2ComputeSegmentAABB, wrap_b2ComputeSegmentAABB);
bool wrap_b2PointInCircle(b2Vec2 point, b2Circle shape) { return b2PointInCircle(point, &shape); }
REF_FUNCTION_EX(b2PointInCircle, wrap_b2PointInCircle);
bool wrap_b2PointInCapsule(b2Vec2 point, b2Capsule shape) { return b2PointInCapsule(point, &shape); }
REF_FUNCTION_EX(b2PointInCapsule, wrap_b2PointInCapsule);
bool wrap_b2PointInPolygon(b2Vec2 point, b2Polygon shape) { return b2PointInPolygon(point, &shape); }
REF_FUNCTION_EX(b2PointInPolygon, wrap_b2PointInPolygon);
b2CastOutput wrap_b2RayCastCircle(b2RayCastInput input, b2Circle shape) { return b2RayCastCircle(&input, &shape); }
REF_FUNCTION_EX(b2RayCastCircle, wrap_b2RayCastCircle);
b2CastOutput wrap_b2RayCastCapsule(b2RayCastInput input, b2Capsule shape) { return b2RayCastCapsule(&input, &shape); }
REF_FUNCTION_EX(b2RayCastCapsule, wrap_b2RayCastCapsule);
b2CastOutput wrap_b2RayCastSegment(b2RayCastInput input, b2Segment shape, bool oneSided) { return b2RayCastSegment(&input, &shape, oneSided); }
REF_FUNCTION_EX(b2RayCastSegment, wrap_b2RayCastSegment);
b2CastOutput wrap_b2RayCastPolygon(b2RayCastInput input, b2Polygon shape) { return b2RayCastPolygon(&input, &shape); }
REF_FUNCTION_EX(b2RayCastPolygon, wrap_b2RayCastPolygon);
b2CastOutput wrap_b2ShapeCastCircle(b2ShapeCastInput input, b2Circle shape) { return b2ShapeCastCircle(&input, &shape); }
REF_FUNCTION_EX(b2ShapeCastCircle, wrap_b2ShapeCastCircle);
b2CastOutput wrap_b2ShapeCastCapsule(b2ShapeCastInput input, b2Capsule shape) { return b2ShapeCastCapsule(&input, &shape); }
REF_FUNCTION_EX(b2ShapeCastCapsule, wrap_b2ShapeCastCapsule);
b2CastOutput wrap_b2ShapeCastSegment(b2ShapeCastInput input, b2Segment shape) { return b2ShapeCastSegment(&input, &shape); }
REF_FUNCTION_EX(b2ShapeCastSegment, wrap_b2ShapeCastSegment);
b2CastOutput wrap_b2ShapeCastPolygon(b2ShapeCastInput input, b2Polygon shape) { return b2ShapeCastPolygon(&input, &shape); }
REF_FUNCTION_EX(b2ShapeCastPolygon, wrap_b2ShapeCastPolygon);

int wrap_b2ComputeHull(lua_State* L)
{
	b2Vec2* pts;
	int count = REF_LuaGetDynamicArray(L, lua_gettop(L), &pts);
	lua_pop(L, 1);
	b2Hull hull = b2ComputeHull(pts, count);
	cf_free(pts);
	REF_LuaSet(L, &hull);
	return 1;
}
REF_WRAP_MANUAL(wrap_b2ComputeHull);

bool wrap_b2ValidateHull(b2Hull hull) { return b2ValidateHull(&hull); }
REF_FUNCTION_EX(b2ValidateHull, wrap_b2ValidateHull);

b2CastOutput wrap_b2ShapeCast(b2ShapeCastPairInput input) { return b2ShapeCast(&input); }
REF_FUNCTION_EX(b2ShapeCast, wrap_b2ShapeCast);
b2DistanceProxy wrap_b2MakeProxy(b2Hull hull, float radius) { return b2MakeProxy(hull.points, hull.count, radius); }
REF_FUNCTION_EX(b2MakeProxy, wrap_b2MakeProxy);
b2Transform wrap_b2GetSweepTransform(b2Sweep sweep, float time) { return b2GetSweepTransform(&sweep, time); }
REF_FUNCTION_EX(b2GetSweepTransform, wrap_b2GetSweepTransform);
b2TOIOutput wrap_b2TimeOfImpact(b2TOIInput input) { return b2TimeOfImpact(&input); }
REF_FUNCTION_EX(b2TimeOfImpact, wrap_b2TimeOfImpact);

b2Manifold wrap_b2CollideCircles(b2Circle a, b2Transform xa, b2Circle b, b2Transform xb) { return b2CollideCircles(&a, xa, &b, xb); }
REF_FUNCTION_EX(b2CollideCircles, wrap_b2CollideCircles);
b2Manifold wrap_b2CollideCapsuleAndCircle(b2Capsule a, b2Transform xa, b2Circle b, b2Transform xb) { return b2CollideCapsuleAndCircle(&a, xa, &b, xb); }
REF_FUNCTION_EX(b2CollideCapsuleAndCircle, wrap_b2CollideCapsuleAndCircle);
b2Manifold wrap_b2CollideSegmentAndCircle(b2Segment a, b2Transform xa, b2Circle b, b2Transform xb) { return b2CollideSegmentAndCircle(&a, xa, &b, xb); }
REF_FUNCTION_EX(b2CollideSegmentAndCircle, wrap_b2CollideSegmentAndCircle);
b2Manifold wrap_b2CollidePolygonAndCircle(b2Polygon a, b2Transform xa, b2Circle b, b2Transform xb) { return b2CollidePolygonAndCircle(&a, xa, &b, xb); }
REF_FUNCTION_EX(b2CollidePolygonAndCircle, wrap_b2CollidePolygonAndCircle);
b2Manifold wrap_b2CollideCapsules(b2Capsule a, b2Transform xa, b2Capsule b, b2Transform xb) { b2DistanceCache c = { 0 }; return b2CollideCapsules(&a, xa, &b, xb, &c); }
REF_FUNCTION_EX(b2CollideCapsules, wrap_b2CollideCapsules);
b2Manifold wrap_b2CollideSegmentAndCapsule(b2Segment a, b2Transform xa, b2Capsule b, b2Transform xb) { b2DistanceCache c = { 0 }; return b2CollideSegmentAndCapsule(&a, xa, &b, xb, &c); }
REF_FUNCTION_EX(b2CollideSegmentAndCapsule, wrap_b2CollideSegmentAndCapsule);
b2Manifold wrap_b2CollidePolygonAndCapsule(b2Polygon a, b2Transform xa, b2Capsule b, b2Transform xb) { b2DistanceCache c = { 0 }; return b2CollidePolygonAndCapsule(&a, xa, &b, xb, &c); }
REF_FUNCTION_EX(b2CollidePolygonAndCapsule, wrap_b2CollidePolygonAndCapsule);
b2Manifold wrap_b2CollidePolygons(b2Polygon a, b2Transform xa, b2Polygon b, b2Transform xb) { b2DistanceCache c = { 0 }; return b2CollidePolygons(&a, xa, &b, xb, &c); }
REF_FUNCTION_EX(b2CollidePolygons, wrap_b2CollidePolygons);
b2Manifold wrap_b2CollideSegmentAndPolygon(b2Segment a, b2Transform xa, b2Polygon b, b2Transform xb) { b2DistanceCache c = { 0 }; return b2CollideSegmentAndPolygon(&a, xa, &b, xb, &c); }
REF_FUNCTION_EX(b2CollideSegmentAndPolygon, wrap_b2CollideSegmentAndPolygon);
b2Manifold wrap_b2CollideSmoothSegmentAndCircle(b2SmoothSegment a, b2Transform xa, b2Circle b, b2Transform xb) { return b2CollideSmoothSegmentAndCircle(&a, xa, &b, xb); }
REF_FUNCTION_EX(b2CollideSmoothSegmentAndCircle, wrap_b2CollideSmoothSegmentAndCircle);
b2Manifold wrap_b2CollideSmoothSegmentAndCapsule(b2SmoothSegment a, b2Transform xa, b2Capsule b, b2Transform xb) { b2DistanceCache c = { 0 }; return b2CollideSmoothSegmentAndCapsule(&a, xa, &b, xb, &c); }
REF_FUNCTION_EX(b2CollideSmoothSegmentAndCapsule, wrap_b2CollideSmoothSegmentAndCapsule);
b2Manifold wrap_b2CollideSmoothSegmentAndPolygon(b2SmoothSegment a, b2Transform xa, b2Polygon b, b2Transform xb) { b2DistanceCache c = { 0 }; return b2CollideSmoothSegmentAndPolygon(&a, xa, &b, xb, &c); }
REF_FUNCTION_EX(b2CollideSmoothSegmentAndPolygon, wrap_b2CollideSmoothSegmentAndPolygon);



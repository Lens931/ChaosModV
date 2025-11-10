/*
    Effect by Lucas7yoshi, modified
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

CHAOS_VAR Ped ms_JesusPed = 0;
CHAOS_VAR Hash ms_JesusRelationshipGroup = 0;
CHAOS_VAR Vehicle ms_LastJesusVehicle    = 0;

static constexpr Hash kJesusModelHash = -835930287;

static void AssignDrivingTask(Ped jesus, Vehicle veh)
{
        // Try to fetch waypoint coords (if waypoint exists)
        Vector3 coords;
        bool found = false;
        if (IS_WAYPOINT_ACTIVE())
        {
                coords = GET_BLIP_COORDS(GET_FIRST_BLIP_INFO_ID(8));
                found  = true;
        }
        else
        {
                for (int i = 0; i < 3; i++)
                {
                        Blip blip = GET_FIRST_BLIP_INFO_ID(i);
                        if (DOES_BLIP_EXIST(blip))
                        {
                                coords = GET_BLIP_COORDS(blip);
                                found  = true;

                                break;
                        }
                }
        }

        if (found)
                TASK_VEHICLE_DRIVE_TO_COORD_LONGRANGE(jesus, veh, coords.x, coords.y, coords.z, 9999.f, 262668, 0.f);
        else
                TASK_VEHICLE_DRIVE_WANDER(jesus, veh, 9999.f, 4176732);

        SET_PED_KEEP_TASK(jesus, true);
        SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(jesus, true);
}

static void SeatPlayerAsPassenger(Ped playerPed, Vehicle veh)
{
        SET_PED_INTO_VEHICLE(playerPed, veh, -2);

        if (GET_PED_IN_VEHICLE_SEAT(veh, -1) != playerPed)
                return;

        int maxPassengers = GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(veh);
        for (int seatIndex = 0; seatIndex <= maxPassengers; seatIndex++)
        {
                SET_PED_INTO_VEHICLE(playerPed, veh, seatIndex);

                if (GET_PED_IN_VEHICLE_SEAT(veh, -1) != playerPed)
                        return;
        }
}

static void EnsureJesusInVehicle(Ped playerPed, Vehicle veh)
{
        if (!DOES_ENTITY_EXIST(veh))
                return;

        if (ms_JesusRelationshipGroup == 0)
        {
                ADD_RELATIONSHIP_GROUP("_WHEEL_JESUS", &ms_JesusRelationshipGroup);
                SET_RELATIONSHIP_BETWEEN_GROUPS(0, ms_JesusRelationshipGroup, "PLAYER"_hash);
        }

        SeatPlayerAsPassenger(playerPed, veh);

        LoadModel(kJesusModelHash);

        if (!DOES_ENTITY_EXIST(ms_JesusPed) || IS_PED_DEAD_OR_DYING(ms_JesusPed, true))
        {
                if (DOES_ENTITY_EXIST(ms_JesusPed))
                {
                        Ped ped = ms_JesusPed;
                        DELETE_PED(&ped);
                }

                ms_JesusPed = CREATE_PED_INSIDE_VEHICLE(veh, 4, kJesusModelHash, -1, true, false);
                SET_PED_RELATIONSHIP_GROUP_HASH(ms_JesusPed, ms_JesusRelationshipGroup);
                SET_ENTITY_PROOFS(ms_JesusPed, true, false, false, false, false, false, false, false);
        }
        else
        {
                SET_PED_INTO_VEHICLE(ms_JesusPed, veh, -1);
        }

        CLEAR_PED_TASKS(ms_JesusPed);
        AssignDrivingTask(ms_JesusPed, veh);

        ms_LastJesusVehicle = veh;
}

static void OnStart()
{
        ms_JesusPed               = 0;
        ms_LastJesusVehicle       = 0;
        ms_JesusRelationshipGroup = 0;

        Ped playerPed = PLAYER_PED_ID();

        // If the player isn't in a vehicle, put him in a pink panto
        if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
        {
                Vector3 playerPos = GET_ENTITY_COORDS(playerPed, true);

                Vehicle veh =
                    CreatePoolVehicle("PANTO"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));
                SET_VEHICLE_COLOURS(veh, 135, 135);
                SET_PED_INTO_VEHICLE(playerPed, veh, -1);
        }

        Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);

        if (veh)
                EnsureJesusInVehicle(playerPed, veh);
}

static void OnStop()
{
        if (DOES_ENTITY_EXIST(ms_JesusPed))
        {
                SET_PED_KEEP_TASK(ms_JesusPed, false);
                CLEAR_PED_TASKS(ms_JesusPed);

                Ped ped = ms_JesusPed;
                DELETE_PED(&ped);
        }

        if (ms_JesusRelationshipGroup != 0)
        {
                REMOVE_RELATIONSHIP_GROUP(ms_JesusRelationshipGroup);
                ms_JesusRelationshipGroup = 0;
        }

        ms_JesusPed         = 0;
        ms_LastJesusVehicle = 0;
}

static void OnTick()
{
        Ped playerPed = PLAYER_PED_ID();

        if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
        {
                ms_LastJesusVehicle = 0;
                return;
        }

        Vehicle veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
        if (!veh)
                return;

        if (veh != ms_LastJesusVehicle || !DOES_ENTITY_EXIST(ms_JesusPed) || IS_PED_DEAD_OR_DYING(ms_JesusPed, true)
            || GET_PED_IN_VEHICLE_SEAT(veh, -1) != ms_JesusPed)
        {
                EnsureJesusInVehicle(playerPed, veh);
        }
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick,
        {
                .Name = "Jesus Take The Wheel",
                .Id = "veh_jesustakethewheel"
        }
);

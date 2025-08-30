// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PrimaryWeapons/Shotgun.h"
#include "Characters/BaseCharacter.h"

void AShotgun::ReloadFinished()
{
	bCanShoot = true;

	if(Character)
	{
		Character->SetIsNeutral();
	}
}

void AShotgun::PerformTraces()
{
	if(Target)
	{
		for(int32 i = 0; i < NumOfPellets; i++)
		{
			BulletTraceWithTarget();
		}
	}
	else
	{
		for(int32 i = 0; i < NumOfPellets; i++)
		{
			BulletTraceWithoutTarget(true);
		}
	}
}

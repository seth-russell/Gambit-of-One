/*
DataTables.cpp

Date Last Updated: August 16, 2015

This source file was made during the Spring 2015 SFML Game Development
Tutorial at New College of Florida.  This code follows the code from the
textbook "SFML Game Development" by Artur Moreira, Henrick Vogelius
Hansson, and Jan Haller.

This file uses the pugixml library (http://pugixml.org) to parse xml documents.
pugixml is Copyright (C) 2006-2015 Arseny Kapoulkine.
*/

#include "DataTables.hpp"
#include "Creature.hpp"
#include "Projectile.hpp"
#include "Pickup.hpp"
#include "Scenery.h"

#include "pugixml.hpp"


// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

std::vector<CreatureData> initializeCreatureData()
{
	std::vector<CreatureData> data(Creature::TypeCount);

	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file("xml/CreatureData.xml");

	pugi::xml_node table = doc.child("table");

	for (pugi::xml_node node = table.child("creature"); node; node = node.next_sibling("creature"))
	{
		int id = node.attribute("id").as_int();

		data[id].hitpoints = node.child("hitpoints").text().as_int();
		data[id].speed = node.child("speed").text().as_float();
		data[id].attackDamage = node.child("attackDamage").text().as_int();
		data[id].aggroDistance = node.child("aggroDistance").text().as_float();
		data[id].attackInterval = sf::seconds(node.child("attackInterval").text().as_float());
		int textureID = node.child("textureID").text().as_int();
		data[id].texture = static_cast<Textures::ID>(textureID);

		int width = node.child("textureRect").child("width").text().as_int();
		int height = node.child("textureRect").child("height").text().as_int();

		data[id].textureRect = sf::IntRect(0, 0, width, height);

		if (node.child("directions").first_child())
		{
			for (pugi::xml_node direction : node.child("directions").children("direction"))
			{
				float angle = direction.child("angle").text().as_float();
				float distance = direction.child("distance").text().as_float();
				float sign = direction.child("sign").text().as_float();

				data[id].directions.push_back(Direction(angle, distance, sign));
			}
		}

		if (node.child("teleports").first_child())
		{
			for (pugi::xml_node point : node.child("teleports").children("point"))
			{
				float x = point.child("x").text().as_float();
				float y = point.child("y").text().as_float();
				Compass comp = static_cast<Compass>(point.child("compass").text().as_int());
				data[id].teleportPoints.push_back(TeleportPoint(x, y, comp));
			}
		}
	}


	return data;
}

std::vector<ProjectileData> initializeProjectileData()
{
	std::vector<ProjectileData> data(Projectile::TypeCount);

	pugi::xml_document doc;
	doc.load_file("xml/ProjectileData.xml");

	pugi::xml_node table = doc.child("table");

	for (pugi::xml_node node = table.child("projectile"); node; node = node.next_sibling("projectile"))
	{
		int id = node.attribute("id").as_int();

		data[id].speed = node.child("speed").text().as_float();
		data[id].damage = node.child("damage").text().as_int();
		data[id].texture = static_cast<Textures::ID>(node.child("textureID").text().as_int());
	}

	return data;
}

std::vector<PickupData> initializePickupData()
{
	std::vector<PickupData> data(Pickup::TypeCount);

	data[Pickup::HealthRefill].texture = Textures::HealthRefill;
	data[Pickup::HealthRefill].action = [](Creature& a) { a.repair(25); };

	return data;
}

std::vector<SceneryData> initializeSceneryData()
{
	std::vector<SceneryData> data(Scenery::TypeCount);

	pugi::xml_document doc;
	doc.load_file("xml/SceneryData.xml");

	pugi::xml_node table = doc.child("table");

	for (pugi::xml_node node = table.child("scenery"); node; node = node.next_sibling("scenery"))
	{
		int id = node.attribute("id").as_int();

		data[id].texture = static_cast<Textures::ID>(node.child("textureID").text().as_int());
	}

	return data;
}

std::vector<std::vector<AreaData>> initializeAreaData()
{
	std::vector<std::vector<AreaData>> map{};
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("xml/AreaData.xml");

	int currentWidth = 0;
	int currentHeight = 0;

	pugi::xml_node data = doc.child("map");

	for (pugi::xml_node node = data.child("area"); node; node = node.next_sibling("area"))
	{
		int x = node.child("coordinates").child("x").text().as_int();
		int y = node.child("coordinates").child("y").text().as_int();

		if (x >= currentWidth && x != 0)
		{
			for (int i = 0; i < (x - currentWidth); i++)
			{
				std::vector<AreaData> newRow;
				for (int j = 0; j < currentHeight; j++)
				{
					newRow.push_back(AreaData());
				}
				map.push_back(newRow);
			}
			currentWidth = x + 1;
		}
		else
		{
			map.push_back(std::vector<AreaData>());
		}

		if (y >= currentHeight && y != 0)
		{
			for (int i = 0; i < (y - currentHeight); i++)
			{
				map[x].push_back(AreaData());
			}
			currentHeight = y + 1;
		}
		else
		{
			map[x].push_back(AreaData());
		}

		map[x][y].coordinates = sf::Vector2i(x, y);
		map[x][y].bgTexture = static_cast<Textures::ID>(node.child("bgTextureID").text().as_int());

		pugi::xml_node enemies = node.child("enemies");

		for (pugi::xml_node spawn = enemies.child("point"); spawn; spawn = spawn.next_sibling("point"))
		{
			int creatureID = spawn.child("creatureID").text().as_int();
			Creature::Type type = static_cast<Creature::Type>(creatureID);
			float spawnX = spawn.child("x").text().as_float();
			float spawnY = spawn.child("y").text().as_float();
			map[x][y].enemySpawns.push_back(EnemySpawn(type, spawnX, spawnY));
		}

		pugi::xml_node props = node.child("props");

		for (pugi::xml_node spawn = props.child("point"); spawn; spawn = spawn.next_sibling("point"))
		{
			int sceneryID = spawn.child("sceneryID").text().as_int();
			Scenery::Type type = static_cast<Scenery::Type>(sceneryID);
			float spawnX = spawn.child("x").text().as_float();
			float spawnY = spawn.child("y").text().as_float();
			map[x][y].scenerySpawns.push_back(ScenerySpawn(type, spawnX, spawnY));
		}
	}



	return map;
}
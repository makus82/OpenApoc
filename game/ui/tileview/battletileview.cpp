#include "game/ui/tileview/battletileview.h"
#include "forms/ui.h"
#include "framework/event.h"
#include "framework/framework.h"
#include "framework/includes.h"
#include "game/state/battle/battle.h"
#include "game/state/battle/battleunitmission.h"
#include "game/state/tileview/tileobject_battleunit.h"

namespace OpenApoc
{
BattleTileView::BattleTileView(TileMap &map, Vec3<int> isoTileSize, Vec2<int> stratTileSize,
                               TileViewMode initialMode, int currentZLevel,
                               Vec3<float> screenCenterTile, Battle::Mode battleMode)
    : TileView(map, isoTileSize, stratTileSize, initialMode), currentZLevel(currentZLevel),
	battleMode(battleMode)
{
	layerDrawingMode = LayerDrawingMode::UpToCurrentLevel;
	selectedTileEmptyImageBack =
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         182));
	selectedTileEmptyImageFront =
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         183));
	selectedTileFilledImageBack =
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         184));
	selectedTileFilledImageFront =
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         185));
	selectedTileFireImageBack =
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         186));
	selectedTileFireImageFront =
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         187));
	selectedTileBackgroundImageBack =
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         188));
	selectedTileBackgroundImageFront =
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         189));
	selectedTileImageOffset = {23, 42};
	pal = fw().data->loadPalette("xcom3/tacdata/tactical.pal");

	for (int i = 0; i < 6; i++)
	{
		activeUnitSelectionArrow.push_back(
		    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
		                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
		                                         167 + i)));
		inactiveUnitSelectionArrow.push_back(
		    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
		                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
		                                         173 + i)));
	}
	// Alexey Andronov (Istrebitel)
	// FIXME: For some reason, when drawing unit selection images, a wild pixel appears
	// in the bottom left (visible above soldier's shoulder) when moving
	// Using PNG instead solves the problem, but we should rather find a way to properly use
	// vanilla game resources?
	//	activeUnitSelectionArrow.front() = fw().data->loadImage("battle/167.png");

	behaviorUnitSelectionUnderlay[BattleUnit::BehaviorMode::Evasive] =
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         190));
	behaviorUnitSelectionUnderlay[BattleUnit::BehaviorMode::Normal] =
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         191));
	behaviorUnitSelectionUnderlay[BattleUnit::BehaviorMode::Aggressive] =
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         192));

	runningIcon = fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                                   "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                                   193));

	bleedingIcon = fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                                    "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                                    194));

	int healingIconsInARow = 4;
	for (int i = 0; i < healingIconsInARow; i++)
	{
		healingIcons.push_back(
		    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
		                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
		                                         195)));
	}
	for (int i = 0; i < healingIconsInARow; i++)
	{
		healingIcons.push_back(
		    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
		                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
		                                         196)));
	}
	healingIcon = *healingIcons.begin();

	targetLocationIcons.push_back(
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         68)));
	targetLocationIcons.push_back(
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         69)));
	targetLocationIcons.push_back(
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         70)));
	targetLocationIcons.push_back(
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         69)));
	targetLocationIcons.push_back(
	    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/icons.pck:xcom3/tacdata/"
	                                         "icons.tab:%d:xcom3/tacdata/tactical.pal",
	                                         68)));
	targetLocationOffset = {23, 42};

	waypointIcons.push_back(fw().data->loadImage("battle/battle-waypoint-1.png"));
	waypointIcons.push_back(fw().data->loadImage("battle/battle-waypoint-2.png"));
	waypointIcons.push_back(fw().data->loadImage("battle/battle-waypoint-3.png"));
	waypointIcons.push_back(fw().data->loadImage("battle/battle-waypoint-2.png"));
	waypointIcons.push_back(fw().data->loadImage("battle/battle-waypoint-1.png"));

	waypointDarkIcons.push_back(fw().data->loadImage("battle/battle-waypoint-1-dark.png"));
	waypointDarkIcons.push_back(fw().data->loadImage("battle/battle-waypoint-2-dark.png"));
	waypointDarkIcons.push_back(fw().data->loadImage("battle/battle-waypoint-3-dark.png"));
	waypointDarkIcons.push_back(fw().data->loadImage("battle/battle-waypoint-2-dark.png"));
	waypointDarkIcons.push_back(fw().data->loadImage("battle/battle-waypoint-1-dark.png"));

	auto font = ui().getFont("SMALLSET");

	for (int i = 0;i <= 99; i++)
	{
		tuIndicators.push_back(font->getString(UString::format("%d",i)));
	}
	pathPreviewTooFar = font->getString(tr("Too Far"));
	pathPreviewUnreachable = font->getString(tr("Blocked"));

	// FIXME: Load from save last screen location?
	setScreenCenterTile(screenCenterTile);
};

BattleTileView::~BattleTileView() = default;

void BattleTileView::eventOccurred(Event *e)
{
	if (e->type() == EVENT_KEY_DOWN &&
	    (e->keyboard().KeyCode == SDLK_F6 || e->keyboard().KeyCode == SDLK_F7))
	{
		switch (e->keyboard().KeyCode)
		{
			case SDLK_F6:
			{
				LogWarning("Writing voxel view to tileviewvoxels.png");
				auto imageOffset = -this->getScreenOffset();
				auto img = std::dynamic_pointer_cast<RGBImage>(this->map.dumpVoxelView(
				    {imageOffset, imageOffset + dpySize}, *this, currentZLevel));
				fw().data->writeImage("tileviewvoxels.png", img);
			}
			break;
			case SDLK_F7:
			{
				LogWarning("Writing voxel view (fast) to tileviewvoxels.png");
				auto imageOffset = -this->getScreenOffset();
				auto img = std::dynamic_pointer_cast<RGBImage>(this->map.dumpVoxelView(
				    {imageOffset, imageOffset + dpySize}, *this, currentZLevel, true));
				fw().data->writeImage("tileviewvoxels.png", img);
			}
			break;
		}
	}
	else
	{
		TileView::eventOccurred(e);
	}
}

void BattleTileView::render()
{
	TRACE_FN;
	Renderer &r = *fw().renderer;
	r.clear();
	r.setPalette(this->pal);

	// Rotate Icons
	{
		// FIXME: Rubbish, introduce delay like for waypoints
		auto pos = ++std::find(healingIcons.begin(), healingIcons.end(), healingIcon);
		if (pos == healingIcons.end())
			pos = healingIcons.begin();
		healingIcon = *pos;

		iconAnimationTicksAccumulated++;
		iconAnimationTicksAccumulated %= targetLocationIcons.size() * TARGET_ICONS_ANIMATION_DELAY;
	}

	// screenOffset.x/screenOffset.y is the 'amount added to the tile coords' - so we want
	// the inverse to tell which tiles are at the screen bounds
	auto topLeft = offsetScreenToTileCoords(Vec2<int>{-isoTileSize.x, -isoTileSize.y}, 0);
	auto topRight = offsetScreenToTileCoords(Vec2<int>{dpySize.x, -isoTileSize.y}, 0);
	auto bottomLeft = offsetScreenToTileCoords(Vec2<int>{-isoTileSize.x, dpySize.y}, map.size.z);
	auto bottomRight = offsetScreenToTileCoords(Vec2<int>{dpySize.x, dpySize.y}, map.size.z);

	int minX = std::max(0, topLeft.x);
	int maxX = std::min(map.size.x, bottomRight.x);

	int minY = std::max(0, topRight.y);
	int maxY = std::min(map.size.y, bottomLeft.y);

	int zFrom = 0;
	int zTo = maxZDraw;

	switch (layerDrawingMode)
	{
		case LayerDrawingMode::UpToCurrentLevel:
			zFrom = 0;
			zTo = currentZLevel;
			break;
		case LayerDrawingMode::AllLevels:
			zFrom = 0;
			zTo = maxZDraw;
			break;
		case LayerDrawingMode::OnlyCurrentLevel:
			zFrom = currentZLevel - 1;
			zTo = currentZLevel;
			break;
	}

	// FIXME: A different algorithm is required in order to properly display big units.
	/*
	1) Rendering must go in diagonal lines. Illustration:

	CURRENT		TARGET

	147			136
	258			258
	369			479

	2) Objects must be located in the bottom-most, right-most tile they intersect
	(already implemented)

	3) Object can either occupy 1, 2 or 3 tiles on the X axis (only X matters)

	- Tiny objects (items, projectiles) occupy 1 tile always
	- Small typical objects (walls, sceneries, small units) occupy 1 tile when static,
	2 when moving on X axis
	- Large objects (large units) occupy 2 tiles when static, 3 when moving on x axis

	How to determine this value is TBD.

	4) When rendering we must check 1 tile ahead for 2-tile object
	and 1 tile ahead and further on x axis for 3-tile object.

	If present we must draw 1 tile ahead for 2-tile object
	or 2 tiles ahead and one tile further on x-axis for 3 tile object
	then resume normal draw order without drawing already drawn tiles

	Illustration:

	SMALL MOVING	LARGE STATIC	LARGE MOVING		LEGEND

	xxxxx > xxxxx6.		x		= tile w/o  object drawn
	xxxx > xxxx48	xxxx > xxxx48	x+++  > x+++59		+		= tile with object drawn
	xxx  > xxx37	x++  > x++37	x++O  > x++28.		digit	= draw order
	x+O  > x+16	x+O  > x+16		x+OO  > x+13.		o		= object yet to draw
	x?   > x25		x?   > x25		x?	  > x47.		?		= current position

	So, if we encounter a 2-tile (on x axis) object in the next position (x-1, y+1)
	then we must first draw tile (x-1,y+1), and then draw our tile,
	and then skip drawing next tile (as we have already drawn it!)

	If we encounter a 3-tile (on x axis) object in the position (x-1,y+2)
	then we must first draw (x-1,y+1), then (x-2,y+2), then (x-1,y+2), then draw our tile,
	and then skip drawing next two tiles (as we have already drawn it) and skip drawing
	the tile (x-1, y+2) on the next row

	This is done best by having a set of Vec3<int>'s, and "skip next X tiles" variable.
	When encountering a 2-tile object, we inrement "skip next X tiles" by 1.
	When encountering a 3-tile object, we increment "skip next X tiles" by 2,
	and we add (x-1, y+2) to the set.
	When trying to draw a tile we first check the "skip next X tiles" variable,
	if > 0 we decrement and continue.
	Second, we check if our tile is in the set. If so, we remove from set and continue.
	Third, we draw normally
	*/

	// FIXME: A different drawing algorithm is required for battle's strategic view
	/*
	First, draw everything except units and items
	Then, draw items only on current z-level
	Then, draw agents, bottom to top, drawing hollow sprites for non-current levels
	*/

	// FIXME: Draw double selection bracket for big units?
	switch (this->viewMode)
	{
		case TileViewMode::Isometric:
		{
			// List of units that require drawing of an overhead icon
			std::list<std::pair<sp<BattleUnit>, bool>> unitsToDraw;

			// List of target icons to draw
			// std::map<Vec3<int>, std::list<Vec3<float>>> targetIconLocations;
			std::set<Vec3<int>> targetIconLocations;

			// List of waypointLocations to draw
			// std::map<Vec3<int>, std::list<Vec3<float>>> waypointLocations;
			std::set<Vec3<int>> waypointLocations;
			// FIXME: Actually read ingame option
			bool USER_OPTION_DRAW_WAYPOINTS = true;
			bool darkenWaypoints = false;
			for (auto u : selectedUnits)
			{
				for (auto &m : u->missions)
				{
					if (m->type == BattleUnitMission::MissionType::ReachGoal)
					{
						targetIconLocations.insert(m->targetLocation);
						break;
					}
					if (m->type == BattleUnitMission::MissionType::GotoLocation &&
					    !m->currentPlannedPath.empty())
					{
						targetIconLocations.insert(m->targetLocation);
						if (USER_OPTION_DRAW_WAYPOINTS)
						{
							for (auto w : m->currentPlannedPath)
							{
								if (w != m->targetLocation)
								{
									waypointLocations.insert(w);
								}
							}
						}
						break;
					}
				}
			}
			if (previewedPathCost != -1 && USER_OPTION_DRAW_WAYPOINTS && waypointLocations.empty())
			{
				darkenWaypoints = true;
				for (auto w : pathPreview)
				{
					waypointLocations.insert(w);
				}
			}
			auto &waypointImageSource = darkenWaypoints ? waypointDarkIcons : waypointIcons;

			for (int z = zFrom; z < zTo; z++)
			{
				int currentLevel = z - currentZLevel + 1;

				// Find out when to draw selection bracket parts (if ever)
				Tile *selTileOnCurLevel = nullptr;
				Vec3<int> selTilePosOnCurLevel;
				sp<Image> selectionImageBack;
				sp<Image> selectionImageFront;
				bool drawPathPreview = false;

				if (selectedTilePosition.z >= z && selectedTilePosition.x >= minX &&
				    selectedTilePosition.x < maxX && selectedTilePosition.y >= minY &&
				    selectedTilePosition.y < maxY)
				{
					selTilePosOnCurLevel = {selectedTilePosition.x, selectedTilePosition.y, z};
					selTileOnCurLevel = map.getTile(selTilePosOnCurLevel.x, selTilePosOnCurLevel.y,
					                                selTilePosOnCurLevel.z);

					// Find what kind of selection bracket to draw (yellow or green)
					// Yellow if this tile intersects with a unit
					if (selectedTilePosition.z == z)
					{
						drawPathPreview = previewedPathCost != -1;
						if (selTileOnCurLevel->getUnitIfPresent())
						{
							selectionImageBack = selectedTileFilledImageBack;
							selectionImageFront = selectedTileFilledImageFront;
						}
						else
						{
							selectionImageBack = selectedTileEmptyImageBack;
							selectionImageFront = selectedTileEmptyImageFront;
						}
					}
					else
					{
						selectionImageBack = selectedTileBackgroundImageBack;
						selectionImageFront = selectedTileBackgroundImageFront;
					}
				}

				// Actually draw stuff
				for (int layer = 0; layer < map.getLayerCount(); layer++)
				{
					for (int y = minY; y < maxY; y++)
					{
						for (int x = minX; x < maxX; x++)
						{
							auto tile = map.getTile(x, y, z);
							auto object_count = tile->drawnObjects[layer].size();
							size_t obj_id = 0;
							do
							{
								if (tile == selTileOnCurLevel && layer == 0 &&
								    selTileOnCurLevel->drawBattlescapeSelectionBackAt == obj_id)
								{
									r.draw(selectionImageBack,
									       tileToOffsetScreenCoords(selTilePosOnCurLevel) -
									           selectedTileImageOffset);
								}
								if (tile->drawTargetLocationIconAt == obj_id)
								{
									if (targetIconLocations.find({x, y, z}) !=
									    targetIconLocations.end())
									{
										r.draw(targetLocationIcons[iconAnimationTicksAccumulated /
										                           TARGET_ICONS_ANIMATION_DELAY],
										       tileToOffsetScreenCoords(Vec3<float>{
										           x, y, tile->getRestingPosition().z}) -
										           targetLocationOffset);
									}
									if (waypointLocations.find({x, y, z}) !=
									    waypointLocations.end())
									{
										r.draw(waypointImageSource[iconAnimationTicksAccumulated /
										                     TARGET_ICONS_ANIMATION_DELAY],
										       tileToOffsetScreenCoords(Vec3<float>{
										           x, y, tile->getRestingPosition().z}) -
										           targetLocationOffset);
									}
								}
								if (obj_id >= object_count)
								{
									break;
								}
								auto &obj = tile->drawnObjects[layer][obj_id];
								bool friendly = false;
								bool hostile = false;
								if (obj->getType() == TileObject::Type::Unit)
								{
									if (!selectedUnits.empty())
									{
										auto u = std::static_pointer_cast<TileObjectBattleUnit>(obj)
											->getUnit();
										auto selectedPos =
											std::find(selectedUnits.begin(), selectedUnits.end(), u);

										if (selectedPos == selectedUnits.begin())
										{
											unitsToDraw.push_back({ u, true });
										}
										else if (selectedPos != selectedUnits.end())
										{
											unitsToDraw.push_back({ u, false });
										}
									}
								}
								Vec2<float> pos = tileToOffsetScreenCoords(obj->getCenter());
								obj->draw(r, *this, pos, this->viewMode, currentLevel);
								// Loop ends when "break" is reached above
								obj_id++;
							} while (true);
							// When done with all objects, draw the front selection image
							if (tile == selTileOnCurLevel && layer == 0)
							{
								static const Vec2<int> offset = { 0, -50 };

								r.draw(selectionImageFront,
								       tileToOffsetScreenCoords(selTilePosOnCurLevel) -
								           selectedTileImageOffset);
								if (drawPathPreview)
								{
									sp<Image> img;
									switch (previewedPathCost)
									{ 
									case -3:
										img = pathPreviewUnreachable;
										break;
									case -2:
										img = pathPreviewTooFar;
										break;
									default:
										img = tuIndicators[previewedPathCost];
										break;
									}
									r.draw(img, tileToOffsetScreenCoords(selTilePosOnCurLevel)
										+ offset - Vec2<int>{img->size.x / 2, img->size.y / 2});
								}
							}
#ifdef PATHFINDING_DEBUG
							if (tile->pathfindingDebugFlag)
								r.draw(waypointIcons[0],
								       tileToOffsetScreenCoords(Vec3<int>{x, y, z}) -
								           selectedTileImageOffset);
#endif
						}
					}
				}
			}

			// Draw next level, units whose "legs" are below "zTo" only
			for (int z = zTo; z < maxZDraw && z < zTo + 1; z++)
			{
				int currentLevel = z - currentZLevel + 1;

				int layer1 = map.getLayer(TileObject::Type::Unit);
				int layer2 = map.getLayer(TileObject::Type::Shadow);
				int minLayer = std::min(layer1, layer2);
				int maxLayer = std::max(layer1, layer2);

				for (int layer = minLayer; layer <= maxLayer; layer++)
				{
					for (int y = minY; y < maxY; y++)
					{
						for (int x = minX; x < maxX; x++)
						{
							auto tile = map.getTile(x, y, z);
							auto object_count = tile->drawnObjects[layer].size();
							size_t obj_id = 0;
							do
							{
								if (obj_id >= object_count)
								{
									break;
								}
								auto &obj = tile->drawnObjects[layer][obj_id];
								if (obj->getType() == TileObject::Type::Unit &&
								    obj->getPosition().z < zTo)
								{
									auto u = std::static_pointer_cast<TileObjectBattleUnit>(obj)
									             ->getUnit();
									if (!selectedUnits.empty())
									{
										auto selectedPos =
											std::find(selectedUnits.begin(), selectedUnits.end(), u);

										if (selectedPos == selectedUnits.begin())
										{
											unitsToDraw.push_back({ u, true });
										}
										else if (selectedPos != selectedUnits.end())
										{
											unitsToDraw.push_back({ u, false });
										}
									}
									Vec2<float> pos = tileToOffsetScreenCoords(obj->getCenter());
									obj->draw(r, *this, pos, this->viewMode, currentLevel);
								}
								// Loop ends when "break" is reached above
								obj_id++;
							} while (true);
						}
					}
				}
			}

			// Draw selected unit arrows
			if (unitsToDraw.size() > 0)
			{
				for (auto obj : unitsToDraw)
				{
					static const Vec2<float> offset = {-13.0f, -19.0f};
					static const Vec2<float> offsetRunning = {0.0f, 0.0f};
					static const Vec2<float> offsetBehavior = {0.0f, 0.0f};
					static const Vec2<float> offsetBleed = {-5.0f, 0.0f};
					static const Vec2<float> offsetTU = { 13.0f, -5.0f };

					Vec2<float> pos =
					    tileToOffsetScreenCoords(
					        obj.first->getPosition() +
					        Vec3<float>{0.0f, 0.0f, obj.first->getCurrentHeight() * 1.5f / 40.0f}) +
					    offset;

					// FIXME: Draw hit points
					r.draw(obj.second ? activeUnitSelectionArrow[obj.first->squadNumber]
					                  : inactiveUnitSelectionArrow[obj.first->squadNumber],
					       pos);
					r.draw(behaviorUnitSelectionUnderlay[obj.first->behavior_mode],
					       pos + offsetBehavior);

					if (battleMode == Battle::Mode::TurnBased)
					{
						auto &img = tuIndicators[obj.first->agent->modified_stats.time_units];
						r.draw(img, pos + offsetTU - Vec2<float>{img->size.x / 2, img->size.y / 2});
					}

					if (obj.first->movement_mode == BattleUnit::MovementMode::Running)
					{
						r.draw(runningIcon, pos + offsetRunning);
					}
					if (obj.first->isFatallyWounded())
					{
						if (obj.first->isHealing)
						{
							r.draw(bleedingIcon, pos + offsetBleed);
						}
						else
						{
							r.draw(healingIcon, pos + offsetBleed);
						}
					}
				}
			}
		}
		break;
		case TileViewMode::Strategy:
		{
			std::list<sp<TileObject>> unitsToDraw;
			std::list<sp<TileObject>> itemsToDraw;

			for (int z = 0; z < zFrom; z++)
			{
				for (int layer = 0; layer < map.getLayerCount(); layer++)
				{
					for (int y = minY; y < maxY; y++)
					{
						for (int x = minX; x < maxX; x++)
						{
							auto tile = map.getTile(x, y, z);
							auto object_count = tile->drawnObjects[layer].size();

							for (size_t obj_id = 0; obj_id < object_count; obj_id++)
							{
								auto &obj = tile->drawnObjects[layer][obj_id];
								if (obj->getType() == TileObject::Type::Unit)
								{
									unitsToDraw.push_back(obj);
								}
							}
						}
					}
				}
			}

			for (int z = zFrom; z < zTo; z++)
			{
				// currentZLevel is an upper exclusive boundary, that's why we need to sub 1 here
				int currentLevel = z - (currentZLevel - 1);

				for (int layer = 0; layer < map.getLayerCount(); layer++)
				{
					for (int y = minY; y < maxY; y++)
					{
						for (int x = minX; x < maxX; x++)
						{
							auto tile = map.getTile(x, y, z);
							auto object_count = tile->drawnObjects[layer].size();

							for (size_t obj_id = 0; obj_id < object_count; obj_id++)
							{
								auto &obj = tile->drawnObjects[layer][obj_id];
								if (obj->getType() == TileObject::Type::Unit)
								{
									unitsToDraw.push_back(obj);
									continue;
								}
								else if (obj->getType() == TileObject::Type::Item)
								{
									if (currentLevel == 0)
									{
										itemsToDraw.push_back(obj);
									}
									continue;
								}

								Vec2<float> pos = tileToOffsetScreenCoords(obj->getCenter());
								obj->draw(r, *this, pos, this->viewMode, currentLevel);
							}
						}
					}
				}
			}

			for (int z = zTo; z < maxZDraw; z++)
			{
				for (int layer = 0; layer < map.getLayerCount(); layer++)
				{
					for (int y = minY; y < maxY; y++)
					{
						for (int x = minX; x < maxX; x++)
						{
							auto tile = map.getTile(x, y, z);
							auto object_count = tile->drawnObjects[layer].size();

							for (size_t obj_id = 0; obj_id < object_count; obj_id++)
							{
								auto &obj = tile->drawnObjects[layer][obj_id];
								if (obj->getType() == TileObject::Type::Unit)
								{
									unitsToDraw.push_back(obj);
								}
							}
						}
					}
				}
			}

			for (auto obj : unitsToDraw)
			{
				Vec2<float> pos = tileToOffsetScreenCoords(obj->getCenter());
				obj->draw(r, *this, pos, this->viewMode,
				          obj->getOwningTile()->position.z - (currentZLevel - 1));
			}

			for (auto obj : itemsToDraw)
			{
				Vec2<float> pos = tileToOffsetScreenCoords(obj->getCenter());
				obj->draw(r, *this, pos, this->viewMode,
				          obj->getOwningTile()->position.z - (currentZLevel - 1));
			}

			renderStrategyOverlay(r);
		}
		break;
		default:
			LogError("Unexpected tile view mode \"%d\"", (int)this->viewMode);
			break;
	}
}

void BattleTileView::setZLevel(int zLevel)
{
	currentZLevel = clamp(zLevel, 1, maxZDraw);
	setScreenCenterTile(Vec3<float>{centerPos.x, centerPos.y, currentZLevel - 1});
}

int BattleTileView::getZLevel() { return currentZLevel; }

void BattleTileView::setLayerDrawingMode(LayerDrawingMode mode) { layerDrawingMode = mode; }

void BattleTileView::setScreenCenterTile(Vec3<float> center)
{
	TileView::setScreenCenterTile(center);
	fw().soundBackend->setListenerPosition({center.x, center.y, center.z});
}

void BattleTileView::setScreenCenterTile(Vec2<float> center)
{
	this->setScreenCenterTile(Vec3<float>{center.x, center.y, 1});
}

void BattleTileView::setSelectedTilePosition(Vec3<int> newPosition)
{
	auto oldPosition = selectedTilePosition;
	TileView::setSelectedTilePosition(newPosition);
	if (oldPosition != selectedTilePosition)
	{
		resetPathPreview();
	}
}

void BattleTileView::resetPathPreview()
{
	pathPreviewTicksAccumulated = 0;
	previewedPathCost = -1;
	lastSelectedUnitPosition = { -1,-1,-1 };
	pathPreview.clear();
}

void BattleTileView::updatePathPreview()
{
	pathPreviewTicksAccumulated = 0;
	
	auto target = selectedTilePosition;
	if (!lastSelectedUnit)
	{
		LogError("Trying to update path preview with no unit selected!?");
		return;
	}
	if (!lastSelectedUnit->canMove())
		return;
	auto &map = lastSelectedUnit->tileObject->map;
	auto to = map.getTile(target);
	
	// Standart check for passability
	while (true)
	{
		if (!to->getPassable(lastSelectedUnit->isLarge(), lastSelectedUnit->agent->type->bodyType->maxHeight))
		{
			previewedPathCost = -3;
			return;
		}
		if (lastSelectedUnit->canFly() || to->getCanStand(lastSelectedUnit->isLarge()))
		{
			break;
		}
		target.z--;
		if (target.z == -1)
		{
			LogError("Solid ground missing on level 0? Reached %d %d %d", target.x, target.y,
				target.z);
			return;
		}
		to = map.getTile(target);
	}

	// Cost to move is 1.5x if prone and 0.5x if running, to keep things in integer 
	// we use a value that is then divided by 2
	float cost = 0.0f;
	int cost_multiplier_x_2 = 2;
	if (lastSelectedUnit->agent->canRun() && lastSelectedUnit->movement_mode == BattleUnit::MovementMode::Running)
	{
		cost_multiplier_x_2 = 1;
	}
	if (lastSelectedUnit->movement_mode == BattleUnit::MovementMode::Prone)
	{
		cost_multiplier_x_2 = 3;
	}
	
	// Get path
	float maxCost = (float)lastSelectedUnit->agent->modified_stats.time_units * 2 / cost_multiplier_x_2;
	pathPreview = map.findShortestPath(lastSelectedUnit->goalPosition,
		target, 500, BattleUnitTileHelper{ map,
		*lastSelectedUnit }, false, &cost, maxCost);
	if (pathPreview.empty())
	{
		LogError("Empty path returned for path preview!?");
		return;
	}
	// If we have not reached the target - then show "Too Far"
	// Otherwise, show amount of TUs remaining at arrival
	if (pathPreview.back() != target)
	{
		previewedPathCost = -2;
	}
	else
	{
		previewedPathCost = (int)roundf(cost * cost_multiplier_x_2 / 2);
		previewedPathCost = lastSelectedUnit->agent->modified_stats.time_units - previewedPathCost;
		if (previewedPathCost < 0)
		{
			// Sometimes it might happen that we barely miss goal after all calculations
			// In this case, properly display "Too far" and subtract cost
			previewedPathCost = -2;
			pathPreview.pop_back();
		}
	}
	if (pathPreview.front() == (Vec3<int>)lastSelectedUnit->position)
	{
		pathPreview.pop_front();
	}
}

}
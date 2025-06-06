//       _________ __                 __
//      /   _____//  |_____________ _/  |______     ____  __ __  ______
//      \_____  \\   __\_  __ \__  \\   __\__  \   / ___\|  |  \/  ___/
//      /        \|  |  |  | \// __ \|  |  / __ \_/ /_/  >  |  /\___ |
//     /_______  /|__|  |__|  (____  /__| (____  /\___  /|____//____  >
//             \/                  \/          \//_____/            \/
//  ______________________                           ______________________
//                        T H E   W A R   B E G I N S
//         Stratagus - A free fantasy real time strategy game engine
//
/**@name editor_brush.h - Assistant for brushes in the editor. */
//
//      (c) Copyright 2023-2025 by Alyokhin
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; only version 2 of the License.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//      02111-1307, USA.
//

#pragma once

//@{
#include <cstdint>
#include "tileset.h"
#include "vec2i.h"
#include "widgets.h"

/*----------------------------------------------------------------------------
--  Declarations
----------------------------------------------------------------------------*/
class CBrush
{
public:
	enum class EBrushTypes
	{
		NotSet,
		SingleTile,
		Decoration
	};

	enum class EBrushShapes
	{
		NotSet,
		Round,
		Rectangular
	};

	enum class EBrushAlign
	{
		NotSet,
		UpperLeft,
		Center
	};
 	
	// type alias
	using brushApplyFn = std::function<void(const TilePos&,
											tile_index,
											bool /*fixNeighbors*/,
											bool /*isDecorative*/)>;

	using TDecorationOptionName = std::string;
	using TDecorationOptionValue = std::string;
	using TDecorationOptions = std::map<TDecorationOptionName, TDecorationOptionValue, std::less<>>;

	using BrushSize = Vec2T<uint8_t>;

	struct Properties { // with default settings
		EBrushTypes type = EBrushTypes::SingleTile;
		EBrushShapes shape = EBrushShapes::Rectangular;
		EBrushAlign align = EBrushAlign::Center;
		bool symmetric = false;
		bool resizable = true;

		BrushSize resizeSteps{1, 1};
		BrushSize minSize{1, 1};
		BrushSize maxSize{20, 20};

		bool randomizeAllowed = true;
		bool fixNeighborsAllowed = true;
		bool tileIconsPaletteRequired = true;
		bool extendedTilesetRequired = false;

		struct Generator
		{
			std::string source;

			using TPossibleValues = std::vector<TDecorationOptionValue>;
			using TOptions = std::map<TDecorationOptionName, TPossibleValues>;
			TOptions options;
		} decorationGenerator;
	};

public:
	explicit CBrush(std::string name, CBrush::Properties properties);

	explicit CBrush(std::string name,
					CBrush::Properties properties,
					const std::vector<tile_index> &tilesSrc);

	~CBrush() = default;

	void applyAt(const TilePos &pos, brushApplyFn applyFn, bool forbidRandomization = false) const;

	uint8_t getWidth() const { return size.x; }
	uint8_t getHeight() const { return size.y; }

	EBrushTypes getType() const { return properties.type; }
	bool isRound() const { return properties.shape == EBrushShapes::Round; }

	graphic_index getGraphicTile(uint8_t col, uint8_t row) const;

	tile_index getTile(uint8_t col, uint8_t row) const;

	void setTile(tile_index tile, uint8_t col = 0, uint8_t row = 0);
	void setTiles(BrushSize srcSize, const std::vector<tile_index> &srcTiles);

	void fillWith(tile_index tile, bool init = false);
	void fillWith(const std::vector<tile_index> &tilesSrc);
	void randomize();

	void setAlign(EBrushAlign alignTo) { properties.align = alignTo; }
	TilePos getAlignOffset() const;
	EBrushAlign getAlign() const { return properties.align; }

	bool isCentered() const { return properties.align == EBrushAlign::Center; }

	void resizeW(uint8_t newWidth);
	void resizeH(uint8_t newHeight);
	void resize(uint8_t newWidth, uint8_t newHeight);

	BrushSize getResizeSteps() const { return properties.resizeSteps; }
	BrushSize getMaxSize() const { return properties.maxSize; }
	BrushSize getMinSize() const { return properties.minSize; }

	bool isSymmetric() const { return properties.symmetric; }
	bool isResizable() const { return properties.resizable; }
	bool isRandomizeAllowed() const { return properties.randomizeAllowed; }
	bool isNeighborsFixAllowed() const { return properties.fixNeighborsAllowed; }

	void enableRandomization(bool enable = true)
	{
		rndEnabled = properties.randomizeAllowed ? enable : false;
	}
	bool isRandomizationEnabled() const { return rndEnabled; }
	void enableFixNeighbors(bool enable = true)
	{
		fixNeighborsEnabled = properties.fixNeighborsAllowed ? enable : false;
	}
	bool isFixNeighborsEnabled() const { return fixNeighborsEnabled; }
	bool isTileIconsPaletteRequired() const { return properties.tileIconsPaletteRequired; };
	bool isExtendedTilesetRequired() const { return properties.extendedTilesetRequired; }

	const auto &getGeneratorOptions() const { return properties.decorationGenerator.options; }

	void updateDecorationOption(const TDecorationOptionName &option, 
								const TDecorationOptionValue &value);
	const TDecorationOptionValue& getDecorationOption(const TDecorationOptionName &option);

	void pushDecorationTiles(uint8_t srcWidth, uint8_t srcHeight, 
							 const std::vector<tile_index> &srcTiles);
	void loadDecoration();

	bool isDecorative() const { return decorative || getType() == EBrushTypes::Decoration; }
	void setDecorative(bool value) {
		decorative = properties.fixNeighborsAllowed ? value : true;
	}
	const std::string &getName() const { return name; }
	void setName(const std::string &name) { this->name = name; }

	static CBrush::EBrushTypes convertToEnumTypes(std::string_view type);
	static CBrush::EBrushShapes convertToEnumShapes(std::string_view shape);
	static CBrush::EBrushAlign convertToEnumAlign(std::string_view align);

protected:
	void setSize(BrushSize newSize);
	bool withinBounds(uint8_t col, uint8_t row) const { return col < size.x && row < size.y; }

	void drawCircle(int16_t xCenter,
					int16_t yCenter,
					int16_t diameter,
					tile_index tile,
					std::vector<tile_index> &canvas);

	tile_index randomizeTile(tile_index tileIdx) const;
	tile_index getCurrentTile() const;

	/// Move to protected
	auto& getDecoration(const TDecorationOptions &options);
	void generateDecoration();

protected:
	Properties properties;

	std::string name; /// brush name

	bool rndEnabled = false; /// Edit mode: place the selected tile or a random tile of the same type
	bool fixNeighborsEnabled = false; /// Edit mode: enabled fix up for neighbors with tile to be placed
	bool decorative = false;

	bool isInit = false;

	BrushSize size = {1, 1};
	std::vector<tile_index> tiles;

	/// for EBrushTypes::Decoration type
	TDecorationOptions decorationOptions;

	struct SDecoration
	{
		BrushSize size = {0, 0};
		std::vector<tile_index> tiles;
	};
	std::map<TDecorationOptions, SDecoration> decorationsPalette;
};

class CBrushesSet
{
public:
	CBrushesSet()
	{
		loadBrushes();
	}

	CBrushesSet(std::string_view brushesSrc)
	{
		loadBrushes(brushesSrc);
	}
	~CBrushesSet() = default;

public:
	void loadBrushes(std::string_view brushesSrc = {});
	bool isLoaded() const { return !brushes.empty(); }

	CBrush &getCurrentBrush() { return currentBrush; }

	bool setCurrentBrush(std::string_view name);
	void addBrush(CBrush brush) { brushes.push_back(std::move(brush)); }

	std::vector<std::string> getBrushesNames() const;

private:
	std::string brushesSrc;
	std::list<CBrush> brushes;
	CBrush currentBrush {std::string("Default"), CBrush::Properties()};
};
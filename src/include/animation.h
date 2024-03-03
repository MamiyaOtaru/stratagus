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
/**@name animation.h - The animations headerfile. */
//
//      (c) Copyright 2005-2007 by Jimmy Salmon
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

#ifndef __ANIMATIONS_H__
#define __ANIMATIONS_H__

//@{

#include <map>
#include <optional>
#include <string>
#include <string_view>

#include "upgrade_structs.h" // MaxCost
#define ANIMATIONS_DEATHTYPES 40

class CFile;
class CUnit;
class CUnitType;
struct lua_State;

/*----------------------------------------------------------------------------
--  Declarations
----------------------------------------------------------------------------*/

/**
**  Default names for the extra death types.
*/
extern std::string ExtraDeathTypes[ANIMATIONS_DEATHTYPES];

//Modify types
enum class SetVar_ModifyTypes {
	modSet = 0,      /// Set value to this
	modAdd,          /// Addition
	modSub,          /// Subtraction
	modMul,          /// Multiplication
	modDiv,          /// Division
	modMod,          /// Modulo
	modAnd,          /// Bitwise AND
	modOr,           /// Bitwise OR
	modXor,          /// Bitwise XOR
	modNot,          /// Bitwise NOT
};

SetVar_ModifyTypes toSetVar_ModifyTypes(std::string_view s);
void modifyValue(SetVar_ModifyTypes mod, int &value, int rop);

class CAnimation
{
public:
	CAnimation() = default;
	virtual ~CAnimation() {}

	virtual void Action(CUnit &unit, int &move, int scale) const = 0;
	virtual void Init(std::string_view s, lua_State *l = nullptr) {}
	virtual void MapSound() {}
	virtual std::optional<int> GetStillFrame(const CUnitType &type) { return std::nullopt; }
};

class CAnimations
{
public:
	CAnimations() = default;
	~CAnimations() = default;

	static void SaveUnitAnim(CFile &file, const CUnit &unit);
	static void LoadUnitAnim(lua_State *l, CUnit &unit, int luaIndex);
	static void LoadWaitUnitAnim(lua_State *l, CUnit &unit, int luaIndex);

public:
	std::vector<std::unique_ptr<CAnimation>> Attack;
	std::vector<std::unique_ptr<CAnimation>> RangedAttack;
	std::vector<std::unique_ptr<CAnimation>> Build;
	bool hasDeathAnimation = false;
	std::vector<std::unique_ptr<CAnimation>> Death[ANIMATIONS_DEATHTYPES + 1];
	std::vector<std::unique_ptr<CAnimation>> Harvest[MaxCosts];
	std::vector<std::unique_ptr<CAnimation>> Move;
	std::vector<std::unique_ptr<CAnimation>> Repair;
	std::vector<std::unique_ptr<CAnimation>> Research;
	std::vector<std::unique_ptr<CAnimation>> SpellCast;
	std::vector<std::unique_ptr<CAnimation>> Start;
	std::vector<std::unique_ptr<CAnimation>> Still;
	std::vector<std::unique_ptr<CAnimation>> Train;
	std::vector<std::unique_ptr<CAnimation>> Upgrade;
};

/*----------------------------------------------------------------------------
--  Functions
----------------------------------------------------------------------------*/

/// Get the animations structure by ident
extern CAnimations &AnimationsByIdent(std::string_view ident);

extern void AnimationCclRegister();

/// Handle the animation of a unit
extern int UnitShowAnimationScaled(CUnit &unit,
                                   const std::vector<std::unique_ptr<CAnimation>> *anims,
                                   int scale);
/// Handle the animation of a unit
extern int UnitShowAnimation(CUnit &unit, const std::vector<std::unique_ptr<CAnimation>> *anims);


extern int ParseAnimInt(const CUnit &unit, std::string_view parseint);

extern void FindLabelLater(std::size_t *labelIndex, std::string name);

extern void FreeAnimations();

//@}

#endif // !__ANIMATIONS_H__

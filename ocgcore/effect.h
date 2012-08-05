/*
 * effect.h
 *
 *  Created on: 2010-3-13
 *      Author: Argon
 */

#ifndef EFFECT_H_
#define EFFECT_H_

#include "common.h"
#include "field.h"
#include "effectset.h"
#include <stdlib.h>
#include <vector>
#include <map>

using namespace std;

class card;
class duel;
class group;
class effect;
struct tevent;
struct effect_set;
struct effect_set_v;

class effect {
public:
	int32 scrtype;
	int32 ref_handle;
	duel* pduel;
	card* owner;
	card* handler;
	uint8 effect_owner;
	uint32 description;
	uint32 code;
	uint32 flag;
	uint32 id;
	uint16 type;
	uint16 copy_id;
	uint16 range;
	uint16 s_range;
	uint16 o_range;
	uint16 reset_count;
	uint32 reset_flag;
	uint32 category;
	uint32 label;
	uint32 hint_timing[2];
	uint32 card_type;
	uint16 field_ref;
	uint16 status;
	void* label_object;
	int32 condition;
	int32 cost;
	int32 target;
	int32 value;
	int32 operation;

	effect();
	~effect();

	int32 is_disable_related();
	int32 is_available();
	int32 is_activateable(uint8 playerid, tevent& e, int32 neglect_cond = FALSE, int32 neglect_cost = FALSE, int32 neglect_target = FALSE);
	int32 is_action_check(uint8 playerid);
	int32 is_activate_ready(uint8 playerid, tevent& e, int32 neglect_cond = FALSE, int32 neglect_cost = FALSE, int32 neglect_target = FALSE);
	int32 is_condition_check(uint8 playerid, tevent& e);
	int32 is_activate_check(uint8 playerid, tevent& e, int32 neglect_cond = FALSE, int32 neglect_cost = FALSE, int32 neglect_target = FALSE);
	int32 is_target(card* pcard);
	int32 is_target_player(uint8 playerid);
	int32 is_player_effect_target(card* pcard);
	int32 is_immuned(effect_set_v* effects);
	int32 is_chainable(uint8 tp);
	int32 reset(uint32 reset_level, uint32 reset_type);
	void dec_count();
	void recharge();
	int32 get_value(uint32 extraargs = 0);
	int32 get_value(card* pcard, uint32 extraargs = 0);
	int32 get_value(effect* peffect, uint32 extraargs = 0);
	int32 check_value_condition(uint32 extraargs = 0);
	int32 get_speed();
	uint8 get_owner_player();
	uint8 get_handler_player();
};

//status
#define EFFECT_STATUS_AVAILABLE	0x0001

//========== Reset ==========
#define RESET_DRAW			PHASE_DRAW
#define RESET_STANDBY		PHASE_STANDBY
#define RESET_MAIN1			PHASE_MAIN1
#define RESET_BATTLE		PHASE_BATTLE
#define RESET_DAMAGE		PHASE_DAMAGE
#define RESET_DAMAGE_CAL	PHASE_DAMAGE_CAL
#define RESET_MAIN2			PHASE_MAIN2
#define RESET_END			PHASE_END
#define RESET_SELF_TURN		0x0100
#define RESET_OPPO_TURN		0x0200
#define RESET_PHASE			0x0400
#define RESET_CHAIN			0x0800
#define RESET_EVENT			0x1000
#define RESET_CODE			0x4000
#define RESET_COPY			0x8000

#define RESET_DISABLE		0x00010000
#define RESET_TURN_SET		0x00020000
#define RESET_TOGRAVE		0x00040000
#define RESET_REMOVE		0x00080000
#define RESET_TEMP_REMOVE	0x00100000
#define RESET_TOHAND		0x00200000
#define RESET_TODECK		0x00400000
#define RESET_LEAVE			0x00800000
#define RESET_TOFIELD		0x01000000
#define RESET_CONTROL		0x02000000
#define RESET_OVERLAY		0x04000000
#define RESET_MSCHANGE		0x08000000

//========== Types ==========
#define EFFECT_TYPE_SINGLE			0x0001	//
#define EFFECT_TYPE_FIELD			0x0002	//
#define EFFECT_TYPE_EQUIP			0x0004	//
#define EFFECT_TYPE_ACTIONS			0x0008	//
#define EFFECT_TYPE_ACTIVATE		0x0010	//
#define EFFECT_TYPE_FLIP			0x0020	//
#define EFFECT_TYPE_IGNITION		0x0040	//
#define EFFECT_TYPE_TRIGGER_O		0x0080	//
#define EFFECT_TYPE_QUICK_O			0x0100	//
#define EFFECT_TYPE_TRIGGER_F		0x0200	//
#define EFFECT_TYPE_QUICK_F			0x0400	//
#define EFFECT_TYPE_CONTINUOUS		0x0800	//

//========== Flags ==========
#define EFFECT_FLAG_INITIAL			0x0001	//
#define EFFECT_FLAG_FUNC_VALUE		0x0002	//
#define EFFECT_FLAG_COUNT_LIMIT		0x0004	//
#define EFFECT_FLAG_FIELD_ONLY		0x0008	//
#define EFFECT_FLAG_CARD_TARGET		0x0010	//
#define EFFECT_FLAG_IGNORE_RANGE	0x0020	//
#define EFFECT_FLAG_ABSOLUTE_TARGET	0x0040	//
#define EFFECT_FLAG_IGNORE_IMMUNE	0x0080	//
#define EFFECT_FLAG_SET_AVAILABLE	0x0100	//
#define EFFECT_FLAG_MULTIACT_HAND	0x0200	//
#define EFFECT_FLAG_CANNOT_DISABLE	0x0400	//
#define EFFECT_FLAG_PLAYER_TARGET	0x0800	//
#define EFFECT_FLAG_BOTH_SIDE		0x1000	//
#define EFFECT_FLAG_COPY_INHERIT	0x2000	//
#define EFFECT_FLAG_DAMAGE_STEP		0x4000	//
#define EFFECT_FLAG_DAMAGE_CAL		0x8000	//
#define EFFECT_FLAG_DELAY			0x10000	//
#define EFFECT_FLAG_SINGLE_RANGE	0x20000	//
#define EFFECT_FLAG_UNCOPYABLE		0x40000	//
#define EFFECT_FLAG_OATH			0x80000	//
#define EFFECT_FLAG_SPSUM_PARAM		0x100000	//
#define EFFECT_FLAG_REPEAT			0x200000	//
#define EFFECT_FLAG_NO_TURN_RESET	0x400000	//
#define EFFECT_FLAG_EVENT_PLAYER	0x800000	//
#define EFFECT_FLAG_OWNER_RELATE	0x1000000	//
#define EFFECT_FLAG_AVAILABLE_BD	0x2000000	//
#define EFFECT_FLAG_CLIENT_HINT		0x4000000	//
#define EFFECT_FLAG_CHAIN_UNIQUE	0x8000000	//
#define EFFECT_FLAG_NAGA			0x10000000	//
#define EFFECT_FLAG_COF				0x20000000	//
//========== Codes ==========
#define EFFECT_IMMUNE_EFFECT			1	//
#define EFFECT_DISABLE					2	//
#define EFFECT_CANNOT_DISABLE			3	//
#define EFFECT_SET_CONTROL				4	//
#define EFFECT_CANNOT_CHANGE_CONTROL	5	//
#define EFFECT_CANNOT_ACTIVATE			6	//
#define EFFECT_CANNOT_TRIGGER			7	//
#define EFFECT_DISABLE_EFFECT			8	//
#define EFFECT_DISABLE_CHAIN			9	//
#define EFFECT_DISABLE_TRAPMONSTER		10	//
#define EFFECT_CANNOT_INACTIVATE		12	//
#define EFFECT_CANNOT_DISEFFECT			13	//
#define EFFECT_CANNOT_CHANGE_POSITION	14	//
#define EFFECT_TRAP_ACT_IN_HAND			15	//
#define EFFECT_TRAP_ACT_IN_SET_TURN		16	//
#define EFFECT_REMAIN_FIELD				17	//
#define EFFECT_MONSTER_SSET				18	//
#define EFFECT_CANNOT_SUMMON			20	//
#define EFFECT_CANNOT_FLIP_SUMMON		21	//
#define EFFECT_CANNOT_SPECIAL_SUMMON	22	//
#define EFFECT_CANNOT_MSET				23	//
#define EFFECT_CANNOT_SSET				24	//
#define EFFECT_CANNOT_DRAW				25	//
#define EFFECT_CANNOT_DISABLE_SUMMON	26	//
#define EFFECT_CANNOT_DISABLE_SPSUMMON	27	//
#define EFFECT_SET_SUMMON_COUNT_LIMIT	28	//
#define EFFECT_EXTRA_SUMMON_COUNT		29	//
#define EFFECT_SPSUMMON_CONDITION		30	//
#define EFFECT_REVIVE_LIMIT				31	//
#define EFFECT_SUMMON_PROC				32	//
#define EFFECT_LIMIT_SUMMON_PROC		33	//
#define EFFECT_SPSUMMON_PROC			34	//
#define EFFECT_EXTRA_SET_COUNT			35	//
#define EFFECT_SET_PROC					36	//
#define EFFECT_LIMIT_SET_PROC			37	//
#define EFFECT_DEVINE_LIGHT				38	//

#define EFFECT_INDESTRUCTABLE			40	//
#define EFFECT_INDESTRUCTABLE_EFFECT	41	//
#define EFFECT_INDESTRUCTABLE_BATTLE	42	//
#define EFFECT_UNRELEASABLE_SUM			43	//
#define EFFECT_UNRELEASABLE_NONSUM		44	//
#define EFFECT_DESTROY_SUBSTITUTE		45	//
#define EFFECT_CANNOT_RELEASE			46	//
#define EFFECT_INDESTRUCTABLE_COUNT		47	//
#define EFFECT_UNRELEASABLE_EFFECT		48	//
#define EFFECT_DESTROY_REPLACE			50	//
#define EFFECT_RELEASE_REPLACE			51	//
#define EFFECT_SEND_REPLACE				52	//
#define EFFECT_CANNOT_DISCARD_HAND		55	//
#define EFFECT_CANNOT_DISCARD_DECK		56	//
#define EFFECT_CANNOT_USE_AS_COST		57	//

#define EFFECT_LEAVE_FIELD_REDIRECT		60	//
#define EFFECT_TO_HAND_REDIRECT			61	//
#define EFFECT_TO_DECK_REDIRECT			62	//
#define EFFECT_TO_GRAVE_REDIRECT		63	//
#define EFFECT_REMOVE_REDIRECT			64	//
#define EFFECT_CANNOT_TO_HAND			65	//
#define EFFECT_CANNOT_TO_DECK			66	//
#define EFFECT_CANNOT_REMOVE			67	//
#define EFFECT_CANNOT_TO_GRAVE			68	//
#define EFFECT_CANNOT_TURN_SET			69	//
#define EFFECT_CANNOT_BE_BATTLE_TARGET	70	//
#define EFFECT_CANNOT_BE_EFFECT_TARGET	71	//
#define EFFECT_IGNORE_BATTLE_TARGET		72	//
#define EFFECT_CANNOT_DIRECT_ATTACK		73	//
#define EFFECT_DIRECT_ATTACK			74	//
#define EFFECT_DUAL_STATUS				75	//
#define EFFECT_EQUIP_LIMIT				76	//
#define EFFECT_DUAL_SUMMONABLE			77	//
#define EFFECT_REVERSE_DAMAGE			80	//
#define EFFECT_REVERSE_RECOVER			81	//
#define EFFECT_CHANGE_DAMAGE			82	//
#define EFFECT_REFLECT_DAMAGE			83	//
#define EFFECT_CANNOT_ATTACK			85	//
#define EFFECT_CANNOT_ATTACK_ANNOUNCE	86	//
#define EFFECT_CANNOT_CHANGE_POS_E		87	//
#define EFFECT_ACTIVATE_COST			90	//
#define EFFECT_SUMMON_COST				91	//
#define EFFECT_SPSUMMON_COST			92	//
#define EFFECT_FLIPSUMMON_COST			93	//
#define EFFECT_MSET_COST				94	//
#define EFFECT_SSET_COST				95	//
#define EFFECT_ATTACK_COST				96	//

#define EFFECT_UPDATE_ATTACK			100	//
#define EFFECT_SET_ATTACK				101	//
#define EFFECT_SET_ATTACK_FINAL			102	//
#define EFFECT_SET_BASE_ATTACK			103	//
#define EFFECT_UPDATE_DEFENCE			104	//
#define EFFECT_SET_DEFENCE				105	//
#define EFFECT_SET_DEFENCE_FINAL		106	//
#define EFFECT_SET_BASE_DEFENCE			107	//
#define EFFECT_REVERSE_UPDATE			108	//
#define EFFECT_SWAP_AD					109	//
#define EFFECT_SWAP_BASE_AD				110	//
#define EFFECT_CHANGE_CODE				114	//
#define EFFECT_ADD_TYPE					115	//
#define EFFECT_REMOVE_TYPE				116	//
#define EFFECT_CHANGE_TYPE				117	//
#define EFFECT_ADD_RACE					120	//
#define EFFECT_REMOVE_RACE				121	//
#define EFFECT_CHANGE_RACE				122	//
#define EFFECT_ADD_ATTRIBUTE			125	//
#define EFFECT_REMOVE_ATTRIBUTE			126	//
#define EFFECT_CHANGE_ATTRIBUTE			127	//
#define EFFECT_UPDATE_LEVEL				130	//
#define EFFECT_CHANGE_LEVEL				131	//
#define EFFECT_SET_POSITION				140 //
#define EFFECT_SELF_DESTROY				141 //
#define EFFECT_DOUBLE_TRIBUTE			150
#define EFFECT_DECREASE_TRIBUTE			151
#define EFFECT_DECREASE_TRIBUTE_SET		152
#define EFFECT_EXTRA_RELEASE			153
#define EFFECT_TRIBUTE_LIMIT			154
#define EFFECT_PUBLIC					160
#define EFFECT_COUNTER_PERMIT			0x10000
#define EFFECT_COUNTER_LIMIT			0x20000
#define EFFECT_RCOUNTER_REPLACE			0x30000
#define EFFECT_LPCOST_CHANGE			170
#define EFFECT_LPCOST_REPLACE			171
#define EFFECT_SKIP_DP					180
#define EFFECT_SKIP_SP					181
#define EFFECT_SKIP_M1					182
#define EFFECT_SKIP_BP					183
#define EFFECT_SKIP_M2					184
#define EFFECT_CANNOT_BP				185
#define EFFECT_CANNOT_M2				186
#define EFFECT_CANNOT_EP				187
#define EFFECT_SKIP_TURN				188
#define EFFECT_DEFENCE_ATTACK			190
#define EFFECT_MUST_ATTACK				191
#define EFFECT_FIRST_ATTACK				192
#define EFFECT_ATTACK_ALL				193
#define EFFECT_EXTRA_ATTACK				194
#define EFFECT_MUST_BE_ATTACKED			195
#define EFFECT_AUTO_BE_ATTACKED			196
#define EFFECT_ATTACK_DISABLED			197
#define EFFECT_NO_BATTLE_DAMAGE			200
#define EFFECT_AVOID_BATTLE_DAMAGE		201
#define EFFECT_REFLECT_BATTLE_DAMAGE	202
#define EFFECT_PIERCE					203
#define EFFECT_BATTLE_DESTROY_REDIRECT	204
#define EFFECT_BATTLE_DAMAGE_TO_EFFECT	205
#define EFFECT_TOSS_COIN_REPLACE		220
#define EFFECT_TOSS_DICE_REPLACE		221
#define EFFECT_FUSION_MATERIAL			230
#define EFFECT_CHAIN_MATERIAL			231
#define EFFECT_SYNCHRO_MATERIAL			232
#define EFFECT_XYZ_MATERIAL				233
#define EFFECT_FUSION_SUBSTITUTE		234
#define EFFECT_CANNOT_BE_FUSION_MATERIAL	235
#define EFFECT_CANNOT_BE_SYNCHRO_MATERIAL	236
#define EFFECT_SYNCHRO_MATERIAL_CUSTOM		237
#define EFFECT_CANNOT_BE_XYZ_MATERIAL		238
#define EFFECT_SYNCHRO_LEVEL				240
#define EFFECT_RITUAL_LEVEL					241
#define EFFECT_EXTRA_RITUAL_MATERIAL		243
#define EFFECT_NONTUNER						244
#define EFFECT_OVERLAY_REMOVE_REPLACE		245
#define EFFECT_SPSUM_EFFECT_ACTIVATED	250
#define EFFECT_MATERIAL_CHECK			251
#define EFFECT_DISABLE_FIELD			260
#define EFFECT_USE_EXTRA_MZONE			261
#define EFFECT_USE_EXTRA_SZONE			262
#define EFFECT_MAX_MZONE				263
#define EFFECT_MAX_SZONE				264
#define EFFECT_HAND_LIMIT				270
#define EFFECT_DRAW_COUNT				271
#define EFFECT_SPIRIT_DONOT_RETURN		280
#define EFFECT_SPIRIT_MAYNOT_RETURN		281
#define EFFECT_CHANGE_ENVIRONMENT		290
#define EFFECT_NECRO_VALLEY				291
#define EFFECT_FORBIDDEN				292
#define EFFECT_NECRO_VALLEY_IM			293
#define EFFECT_REVERSE_DECK				294
#define EFFECT_REMOVE_BRAINWASHING		295
#define EFFECT_BP_TWICE					296

#define EVENT_STARTUP		1000
#define EVENT_FLIP			1001
#define EVENT_FREE_CHAIN	1002
#define EVENT_DESTROY		1010
#define EVENT_REMOVE		1011
#define EVENT_TO_HAND		1012
#define EVENT_TO_DECK		1013
#define EVENT_TO_GRAVE		1014
#define EVENT_LEAVE_FIELD	1015
#define EVENT_CHANGE_POS	1016
#define EVENT_RELEASE		1017
#define EVENT_DISCARD		1018
#define EVENT_LEAVE_FIELD_P	1019
#define EVENT_CHAIN_SOLVING		1020
#define EVENT_CHAIN_ACTIVATING	1021
#define EVENT_CHAIN_SOLVED		1022
#define EVENT_CHAIN_ACTIVATED	1023
#define EVENT_CHAIN_NEGATED		1024
#define EVENT_CHAIN_DISABLED	1025
#define EVENT_CHAIN_END			1026
#define EVENT_CHAINING			1027
#define EVENT_BECOME_TARGET		1028
#define EVENT_DESTROYED			1029
#define EVENT_ADJUST			1040
#define EVENT_SUMMON_SUCCESS		1100
#define EVENT_FLIP_SUMMON_SUCCESS	1101
#define EVENT_SPSUMMON_SUCCESS		1102
#define EVENT_SUMMON				1103
#define EVENT_FLIP_SUMMON			1104
#define EVENT_SPSUMMON				1105
#define EVENT_MSET					1106
#define EVENT_SSET					1107
#define EVENT_BE_MATERIAL			1108
#define EVENT_DRAW					1110
#define EVENT_DAMAGE				1111
#define EVENT_RECOVER				1112
#define EVENT_PREDRAW				1113
#define EVENT_CONTROL_CHANGED		1120
#define EVENT_EQUIP					1121
#define EVENT_ATTACK_ANNOUNCE		1130
#define EVENT_BE_BATTLE_TARGET		1131
#define EVENT_BATTLE_START			1132
#define EVENT_BATTLE_CONFIRM		1133
#define EVENT_PRE_DAMAGE_CALCULATE	1134
#define EVENT_DAMAGE_CALCULATING	1135
#define EVENT_PRE_BATTLE_DAMAGE		1136
#define EVENT_BATTLE_END			1137
#define EVENT_BATTLED				1138
#define EVENT_BATTLE_DESTROYING		1139
#define EVENT_BATTLE_DESTROYED		1140
#define EVENT_DAMAGE_STEP_END		1141
#define EVENT_ATTACK_DISABLED		1142
#define EVENT_BATTLE_DAMAGE			1143
#define EVENT_TOSS_DICE				1150
#define EVENT_TOSS_COIN				1151
#define EVENT_TOSS_COIN_NEGATE		1152
#define EVENT_TOSS_DICE_NEGATE		1153
#define EVENT_LEVEL_UP				1200
#define EVENT_PAY_LPCOST			1201
#define EVENT_TURN_END				1210
#define EVENT_PHASE					0x1000
#define EVENT_PHASE_START			0x2000
#define EVENT_ADD_COUNTER			0x10000
#define EVENT_REMOVE_COUNTER		0x20000

#endif /* EFFECT_H_ */

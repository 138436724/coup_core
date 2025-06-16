#pragma once

namespace COUP
{
	enum class ROLE_IDENTITY
	{
		DUKE,
		ASSASSIN,
		CONTESSA,
		CAPTAIN,
		AMBASSADOR,
	};

	enum class GAME_STATUS
	{
		JOIN,
		FULL,
		OVER
	};

	enum class ROLE_ACTION
	{
		/* 第一阶段有如下可能任务 */
		// 不可阻止不可质疑
		INCOME,
		COUP,

		// 可质疑不可阻止
		TAX,
		EXCHANGE,

		// 可阻止不可质疑
		FOREIGN_AID,

		// 可阻止可质疑
		ASSASSINATE,
		STEAL,

		/* 第二阶段可能被阻止也可能被质疑 */
		BLOCK, // and DOUBT

		/* 第三阶段只可能是被质疑 */
		DOUBT
	};
} // namespace COUP

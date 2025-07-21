#pragma once

// Packages
#include <boost/outcome.hpp>
#include <QString>

namespace VvvfSimulator::Outcome
{
	namespace BSLOutcomeV2 = BOOST_OUTCOME_V2_NAMESPACE;

	template <typename R, typename S>
	using Result = BSLOutcomeV2::result<R, S>;
	using BSLOutcomeV2::success;

	template <typename R>
	using ErrnoResult = Result<R, int>;
	template <typename R>
	using QStringResult = Result<R, QString>;
}

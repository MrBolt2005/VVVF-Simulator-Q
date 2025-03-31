#include "Struct.hpp"

// Packages
#include <QObject>

namespace NAMESPACE_VVVF::Struct
{
	QStringList VvvfValues::getPulseName() const
	{
		const auto &type = videoPulseMode.PulseType;
		const auto &pulseCount = videoPulseMode.PulseCount;

		switch (type)
		{
		case PulseTypeName::ASYNC:
			{
				QStringList names(3);
				auto it = names.begin();

				const CarrierFreq &carrier = videoCarrierFrequency;

				*(it++) = QObject::tr("Async - %1").arg(QString::number(carrier.baseFrequency));

				if (carrier.range != 0.0)
					*(it++) = QObject::tr("Random ± %1").arg(QString::number(carrier.range));

				const auto hasBipolar = videoPulseData.find(PulseDataKey::Bipolar);
				if (hasBipolar != videoPulseData.end())
					*it = QObject::tr("Bipolar - %1").arg(QString::number(hasBipolar->second));
				return names;
			}
			break;
		case PulseTypeName::SYNC:
			{
				const QString modeName = QObject::tr("%1-Pulse").arg(QString::number(pulseCount));
				const auto hasBipolar = videoPulseData.find(PulseDataKey::Bipolar);
				return hasBipolar == videoPulseData.end() ?
					QStringList(modeName) :
					QStringList{modeName, QObject::tr("Bipolar - %1").arg(QString::number(hasBipolar->second))};
			}
			break;
		case PulseTypeName::CHM:
			{
				return QStringList{QObject::tr("%1-Pulse").arg(QString::number(pulseCount)), QObject::tr("Current Harmonic Minimum")};
			}
			break;
		case PulseTypeName::SHE:
			{
				return QStringList{QObject::tr("%1-Pulse").arg(QString::number(pulseCount)), QObject::tr("Selective Harmonic Elimination")};
			}
			break;
		case PulseTypeName::HO:
			{
				return QStringList{QObject::tr("%1-Pulse").arg(QString::number(pulseCount)), QObject::tr("High Efficiency Over-modulation")};
			}
			break;
		default:
			{
				const auto unknown = QObject::tr("Unknown");
				return QStringList{unknown, unknown};
			}
		}
	}

	QVector<double> NAMESPACE_VVVF::Struct::PulseModeConfiguration::getAvailablePulseCount(PulseTypeName pulseType, int level)
	{
		if (level == 2)
		{
			switch (pulseType)
			{
			case PulseTypeName::SYNC: return {-1};
			case PulseTypeName::HO:   return {5,7,9,11,13,15,17};
			case PulseTypeName::SHE:  return {3,5,7,9,11,13,15};
			case PulseTypeName::CHM:  return {3,5,7,9,11,13,15,17,19,21,23,25};
			}
		}
		else if (level == 3)
		{
			switch (pulseType)
			{
				case PulseTypeName::SYNC: return {-1};
				case PulseTypeName::SHE:
				case PulseTypeName::CHM:  return {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21};
			}
		}
		return {};
	}

	QVector<PulseTypeName> PulseModeConfiguration::getAvailablePulseType(int level)
	{
		switch (level)
		{
			case 2:
				return {PulseTypeName::ASYNC, PulseTypeName::SYNC, PulseTypeName::SHE, PulseTypeName::CHM, PulseTypeName::HO};
			case 3:
				return {PulseTypeName::ASYNC, PulseTypeName::SYNC, PulseTypeName::SHE, PulseTypeName::CHM};
			default:
				return {};
		}
	}

	QVector<PulseAlternative> PulseModeConfiguration::getPulseAlternatives(PulseTypeName pulseType, double pulseCount, int level)
	{
		const auto AlternativesDefaultToX = [](qsizetype X, const QVector<PulseAlternative> *custom)
		{
			QVector<PulseAlternative> alternatives{PulseAlternative::Default};
			auto length = X + 1;
			if (custom) length += custom->size();
			alternatives.reserve(length);
			if (custom) for (const auto &customAlternative : *custom)
				alternatives.push_back(customAlternative);
			for (qsizetype i = 0; i < X; i++)
				alternatives.push_back(static_cast<PulseAlternative>(static_cast<int_fast8_t>(PulseAlternative::Alt1) + i));
			return alternatives;
		};

		if (level == 3)
		{
			if(pulseType == PulseTypeName::SYNC)
			{
				switch ((int64_t)pulseCount)
				{
				case 1:
				case 5:
					return {PulseAlternative::Default, PulseAlternative::Shifted, PulseAlternative::Alt1};
				default:
					return {PulseAlternative::Default, PulseAlternative::Shifted};
				};
			}

			if(pulseType == PulseTypeName::ASYNC)
			{
				return {PulseAlternative::Default, PulseAlternative::Shifted};
			}

			if (pulseType == PulseTypeName::CHM)
			{
				switch ((int64_t)pulseCount)
				{
					case 3:  return AlternativesDefaultToX(2, nullptr):
					case 5:  return AlternativesDefaultToX(4, nullptr);
					case 7:  return AlternativesDefaultToX(6, nullptr);
					case 9:  return AlternativesDefaultToX(7, nullptr);
					case 11: return AlternativesDefaultToX(10, nullptr);
					case 13: return AlternativesDefaultToX(14, nullptr);
					case 15: return AlternativesDefaultToX(17, nullptr);
					case 17: return AlternativesDefaultToX(19, nullptr);
					case 19: return AlternativesDefaultToX(25, nullptr);
					case 21: return AlternativesDefaultToX(22, nullptr);
					default: return {PulseAlternative::Default};
				};
			}

			if (pulseType == PulseTypeName::SHE)
			{
				return pulseCount == 3 ? AlternativesDefaultToX(1, nullptr) : QVector{PulseAlternative::Default};
			}

			return {PulseAlternative::Default};
		}
                
		if (level == 2)
		{
			if (pulseType == PulseTypeName::SYNC)
			{
				static const auto custom = QVector{PulseAlternative::Shifted, PulseAlternative::CP, PulseAlternative::Square,};
				switch ((int64_t)pulseCount)
				{
				case 3:
				case 5:
				case 6:
				case 8:
				case 9:
				case 13:
				case 17:
					return AlternativesDefaultToX(1, &custom);
				default:
					return {PulseAlternative::Default, PulseAlternative::Shifted, PulseAlternative::CP, PulseAlternative::Square,};
				};
			}

			if(pulseType == PulseTypeName::ASYNC)
			{
				return {PulseAlternative::Default, PulseAlternative::Shifted};
			}

			if (pulseType == PulseTypeName::CHM)
			{
				switch ((int64_t)pulseCount)
				{
				case 3:  return AlternativesDefaultToX(1, nullptr);
				case 5:  return AlternativesDefaultToX(3, nullptr);
				case 7:  return AlternativesDefaultToX(5, nullptr);
				case 9:  return AlternativesDefaultToX(8, nullptr);
				case 11: return AlternativesDefaultToX(12, nullptr);
				case 13:
				case 21: return AlternativesDefaultToX(13, nullptr);
				case 15: return AlternativesDefaultToX(23, nullptr);
				case 17:
				case 19: return AlternativesDefaultToX(11, nullptr);
				case 23: return AlternativesDefaultToX(14, nullptr);
				case 25: return AlternativesDefaultToX(20, nullptr);

				default: return {PulseAlternative::Default};
				};
			}

			if (pulseType == PulseTypeName::SHE)
			{
				switch ((int64_t)pulseCount)
				{
					case 3:
					case 7:
					case 11:
					case 13:
					case 15: return {PulseAlternative::Default, PulseAlternative::Alt1};
					case 5:
					case 9:  return {PulseAlternative::Default, PulseAlternative::Alt1, PulseAlternative::Alt2};
					default: return {PulseAlternative::Default};
				};
			}

			if (pulseType == PulseTypeName::HO)
			{
				switch ((int64_t)pulseCount)
				{
					case 5:  return AlternativesDefaultToX(7, nullptr);
					case 7:  return AlternativesDefaultToX(9, nullptr);
					case 9:  return AlternativesDefaultToX(6, nullptr);
					case 11: return AlternativesDefaultToX(5, nullptr);
					case 13: return AlternativesDefaultToX(3, nullptr);
					case 15: return AlternativesDefaultToX(2, nullptr);
					default: return {PulseAlternative::Default};
				};
			}

			return [PulseAlternative.Default];
		}

		return [PulseAlternative.Default];
	}
}

#include "Struct.hpp"

// Packages
#include <QObject>

QStringList NAMESPACE_VVVF::Struct::VvvfValues::getPulseName() const
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
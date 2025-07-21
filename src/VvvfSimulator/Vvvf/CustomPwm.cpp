#include "CustomPwm.hpp"

#include <algorithm>
#include <cmath>

#include "../Exception.hpp"
#include "InternalMath.hpp"

#include <rfl/json.hpp>
#include <rfl/yaml.hpp>

namespace NAMESPACE_VVVF
{
    using RflCppFormats = VvvfSimulator::Yaml::RflCppFormats;
    using namespace InternalMath;
    
    CustomPwm::CustomPwm() = default;

    CustomPwm::CustomPwm(const CustomPwm& other) :
        m_switchCount(other.m_switchCount),
        m_modulationIndexDivision(other.m_modulationIndexDivision),
        m_minimumModulationIndex(other.m_minimumModulationIndex),
        m_blockCount(other.m_blockCount),
        m_switchAngleTable(other.m_switchAngleTable),
        m_polarity(other.m_polarity)
    {}

    CustomPwm::CustomPwm(CustomPwm&& other)
    {
      QWriteLocker otherLocker(&(other.m_lock));
      m_switchCount = other.m_switchCount;
      m_modulationIndexDivision = other.m_modulationIndexDivision;
      m_minimumModulationIndex = other.m_minimumModulationIndex;
      m_blockCount = other.m_blockCount;
      m_switchAngleTable = std::move(other.m_switchAngleTable);
      m_polarity = std::move(other.m_polarity);
    }

    CustomPwm::CustomPwm(std::filesystem::path Path, RflCppFormats format)
    {
      using namespace VvvfSimulator::Exception;
			
			try { *this = std::move(load(Path, format).value()); }
      catch (const std::runtime_error& error)
      {
        const std::exception_ptr ePtr = std::current_exception();
        static const QMessageLogContext context(__FILE__, __LINE__, Q_FUNC_INFO, "VvvfSimulator::Exception::ConstructorError");
        const QString message = QObject::tr("Failed to load CustomPwm from file: %1\n\n%2").arg(
          QString::fromStdString(Path.string()),
          QString(error.what()));
        const QVariantMap details = {
          {"format", static_cast<char>(format)},
          {"longFormat", QObject::tr(VvvfSimulator::Yaml::toString(format))},
          {"path", QString::fromStdString(Path.string())},
          {"error", QByteArray(error.what())}	
        };
        throw VvvfSimulator::Exception::VvvfException(message.toUtf8().constData(), &context, &details, ePtr);
      }
    }

		CustomPwm &CustomPwm::operator=(const CustomPwm &other) noexcept
		{
			if (this != &other)
      {
        QWriteLocker thisLocker(&m_lock);
        //QReadLocker otherLocker(&(other.m_lock));
        m_switchCount = other.m_switchCount;
        m_modulationIndexDivision = other.m_modulationIndexDivision;
        m_minimumModulationIndex = other.m_minimumModulationIndex;
        m_blockCount = other.m_blockCount;
        m_switchAngleTable = other.m_switchAngleTable;
        m_polarity = other.m_polarity;
      }
      return *this;
		}

		CustomPwm &CustomPwm::operator=(CustomPwm &&other) noexcept
		{
			if (this != &other)
      {
        QWriteLocker thisLocker(&m_lock), otherLocker(&(other.m_lock));
        m_switchCount = other.m_switchCount;
        m_modulationIndexDivision = other.m_modulationIndexDivision;
        m_minimumModulationIndex = other.m_minimumModulationIndex;
        m_blockCount = other.m_blockCount;
        m_switchAngleTable = std::move(other.m_switchAngleTable);
        m_polarity = std::move(other.m_polarity);
      }
      return *this;
		}

		int_fast8_t CustomPwm::getPWM(double M, double X) const
    {
        int64_t index = static_cast<int64_t>((M - m_minimumModulationIndex) / m_modulationIndexDivision);
        index = std::clamp(index, 0LL, static_cast<int64_t>(m_blockCount) - 1);

        X = std::fmod(X, m_2PI);
        int64_t orthant = static_cast<int64_t>(X / m_PI_2);
        double angle = std::fmod(X, m_PI_2);

        if ((orthant & 0x01) == 1) angle = m_PI_2 - angle;

        int_fast8_t PWM = 0;
        bool inverted = m_polarity[index];

        auto it = m_switchAngleTable.lower_bound(angle);
        if (it != m_switchAngleTable.begin())
        {
          //--it;
          //if (it->first <= angle) PWM = it->second;
          if ((--it)->first <= angle) PWM = it->second;
        }

        if ((orthant > 1) ^ inverted) PWM = maxPwmLevel - PWM;

        return PWM;
    }

    rfl::Result<rfl::Nothing> CustomPwm::save(std::filesystem::path Path, RflCppFormats format) const
    {
      switch (format)
      {
      case RflCppFormats::JSON:
        return rfl::json::save(Path.string(), *this, rfl::json::pretty);
        break;
      default: // RflCppFormats::YAML
        return rfl::yaml::save(Path.string(), *this);
      }
    }

    rfl::Result<CustomPwm> CustomPwm::load(std::filesystem::path Path, RflCppFormats format)
    {
      switch (format)
      {
      case RflCppFormats::JSON:
        return rfl::json::load<CustomPwm>(Path.string());
        break;
      default: // RflCppFormats::YAML
        return rfl::yaml::load<CustomPwm>(Path.string());
      }
    }
}
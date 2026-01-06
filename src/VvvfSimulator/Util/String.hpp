#pragma once

// Standard Library
#include <memory>
#include <string>
#include <utility>
// Packages
#include <QByteArray>
#include <QList> // aka <QVector>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QString>
#include <QVariant>	
// Internal
#include "Defines.h"

#define qTr QObject::tr

namespace VvvfSimulator::Util::String
{
	class FmtStringArg;
	
	class TranslatableFmtString
	{
		Q_GADGET
		Q_PROPERTY(QList<FmtStringArg> args ATTRIBUTE args)

	public:
		QByteArray sourceText;
		QList<FmtStringArg> args;
		std::shared_ptr<std::string> disambiguation;
		int n = -1;

		QString makeTrString() const;
		QString makeUntrString() const;
	};

	#define TEMP sizeof(TranslatableFmtString)

	class FmtStringArg
	{
		Q_GADGET
		Q_PROPERTY(QString a ATTRIBUTE a)
		Q_PROPERTY(int fieldWidth ATTRIBUTE fieldWidth)
		Q_PROPERTY(QChar fillChar ATTRIBUTE fillChar)
		Q_PROPERTY(QVariant id ATTRIBUTE id)
	
	public:
		std::variant<QString, TranslatableFmtString> a;
		int fieldWidth = 0;
		QChar fillChar{};
		QVariant id;
	};

	enum class ChangeCases
	{
		NotChange, ToUpper, ToLower, ToTitleCase
	};

	/*
	@brief Converts a string from coding-oriented cases such as camel, snake or
	Pascal to regular uppercased words separated by spaces. May (and will) be
	useful to convert symbol names to more user-friendly phrases, though even
	this result may require additional editing after this for some words (like
	"and", "iPhone", "QoS" etc.)

	@param s Input string. Yes, it is passed by value, because we are going to
	return a modification of the argument under its same type.
	@param cases Recapitalization behavior for building the returned string.
	@param exceptions Exceptional recapitalization cases. The pair's first member
	is the case to ignore, and the second one, what to do or to replace instead.
	@return The resulting string.
	*/
	QString filterCases(
		QString s,
		ChangeCases cases = ChangeCases::ToUpper,
		const QMap<QString, std::variant<ChangeCases, QString>> &exceptions = {}
	);
}

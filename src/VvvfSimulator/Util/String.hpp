#pragma once

// Standard Library
#include <span>
#include <string>
#include <utility>
// Packages
#include <QByteArray>
#include <QList> // aka <QVector>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QSharedPointer>
#include <QString>
#include <QTranslator>
#include <QVariant>
// Internal
#include "Defines.h"

#define qTr QObject::tr

namespace VvvfSimulator::Util::String {
class FmtStringArg;

class TranslatableFmtString {
    Q_GADGET
    // Consider replacing sourceText from QByteArray to QString?
    Q_PROPERTY(QString sourceTextStr READ sourceTextStr WRITE setSourceTextStr)
    Q_PROPERTY(QString contextStr READ contextStr WRITE setContextStr)
    Q_PROPERTY(QList<FmtStringArg> args ATTRIBUTE args)
    Q_PROPERTY(QString disambiguationStr READ disambiguationStr WRITE
                   setDisambiguationStr)

  public:
    QByteArray sourceText;
    QSharedPointer<QTranslator> translator;
    QByteArray context;
    QList<FmtStringArg> args;
    std::optional<QByteArray> disambiguation;
    int n = -1;

    constexpr TranslatableFmtString() = default;
    TranslatableFmtString(const TranslatableFmtString &other);
    TranslatableFmtString(TranslatableFmtString &&other) noexcept;
    TranslatableFmtString(const QByteArray &source,
                          QSharedPointer<QTranslator> trnsltr = nullptr,
                          const QByteArray *ctx = nullptr,
                          const std::span<FmtStringArg> &args = {},
                          const QByteArray *disamb = nullptr, int n = -1);
    ~TranslatableFmtString();

    TranslatableFmtString &operator=(const TranslatableFmtString &other);
    TranslatableFmtString &operator=(TranslatableFmtString &&other) noexcept;

    QString makeTrString() const;
    QString makeUntrString() const;

    QString contextStr() const {
        return QString(context);
    }
    void setContextStr(const QStringView &str) {
        context = str.toUtf8();
    }
    QString disambiguationStr(bool *hasValue = nullptr) const {
        if (hasValue)
            *hasValue = disambiguation.has_value();
        return disambiguation.has_value() ? QString{*disambiguation}
                                          : QString{};
    }
    void setDisambiguationStr(const QStringView &str) {
        disambiguation.emplace(str.toUtf8());
    }
    void reset() noexcept;
    QString sourceTextStr() const {
        return QString(sourceText);
    }
    void setSourceTextStr(const QStringView &str) {
        sourceText = str.toUtf8();
    }
};

#define TEMP sizeof(TranslatableFmtString)

class FmtStringArg {
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

    FmtStringArg();
    FmtStringArg(const QString &str);
    FmtStringArg(QString &&str);
};

enum class ChangeCases { NotChange, ToUpper, ToLower, ToTitleCase };

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
    QString s, ChangeCases cases = ChangeCases::ToUpper,
    const QMap<QString, std::variant<ChangeCases, QString>> &exceptions = {});
} // namespace VvvfSimulator::Util::String

#include "String.hpp"

// Packages
#include <QDebug>
#include <QRegularExpression>
#include <QStringView>

namespace VvvfSimulator::Util::String {
TranslatableFmtString::TranslatableFmtString(const TranslatableFmtString &other)
    : sourceText(other.sourceText)
    , translator(other.translator)
    , context(other.context)
    , args(other.args)
    , disambiguation(other.disambiguation)
    , n(other.n) {}

TranslatableFmtString::TranslatableFmtString(
    TranslatableFmtString &&other) noexcept
    : sourceText(std::move(other.sourceText))
    , translator(std::move(other.translator))
    , context(std::move(other.context))
    , args(std::move(other.args))
    , disambiguation(std::move(other.disambiguation))
    , n(std::move(other.n)) {}

TranslatableFmtString::TranslatableFmtString(
    const QByteArray &source, QSharedPointer<QTranslator> trnsltr,
    const QByteArray *ctx, const std::span<FmtStringArg> &args,
    const QByteArray *disamb, int n)
    : sourceText(source)
    , translator(std::move(trnsltr))
    , context()
    , args(args.begin(), args.end())
    , disambiguation(/* std::nullopt */)
    , n(n) {
    if (ctx)
        context = *ctx;
    if (disamb)
        disambiguation.emplace(*disamb);
}

TranslatableFmtString::~TranslatableFmtString() = default;

TranslatableFmtString &
TranslatableFmtString::operator=(const TranslatableFmtString &other) {
    sourceText = other.sourceText;
    translator = other.translator;
    context = other.context;
    args = other.args;
    disambiguation = other.disambiguation;
    n = other.n;

    return *this;
}

TranslatableFmtString &
TranslatableFmtString::operator=(TranslatableFmtString &&other) noexcept {
    sourceText = std::move(other.sourceText);
    translator = std::move(other.translator);
    context = std::move(other.context);
    args = std::move(other.args);
    disambiguation = std::move(other.disambiguation);
    n = std::move(other.n);

    return *this;
}

QString TranslatableFmtString::makeTrString() const {
    auto &disamb = disambiguation;
    const auto maybeDisambig = disamb ? disamb->constData() : nullptr;
    QString trStr = translator
                        ? translator->translate(context.constData(), sourceText,
                                                maybeDisambig, n)
                        : QObject::tr(sourceText, maybeDisambig, n);
    for (const auto &arg : args) {
        if (const auto x = std::get_if<QString>(&arg.a); x)
            trStr = trStr.arg(*x, arg.fieldWidth, arg.fillChar);
        else // TranslatableFmtString
            trStr =
                trStr.arg(std::get<TranslatableFmtString>(arg.a).makeTrString(),
                          arg.fieldWidth, arg.fillChar);
    }

    return trStr;
}

QString TranslatableFmtString::makeUntrString() const {
    QString str(sourceText);
    for (const auto &arg : args) {
        if (const auto x = std::get_if<QString>(&arg.a); x)
            str = str.arg(*x, arg.fieldWidth, arg.fillChar);
        else // TranslatableFmtString
            str =
                str.arg(std::get<TranslatableFmtString>(arg.a).makeUntrString(),
                        arg.fieldWidth, arg.fillChar);
    }

    return str;
}

void TranslatableFmtString::reset() noexcept {
    static_assert(std::is_nothrow_default_constructible_v<QByteArray>);
    static_assert(std::is_nothrow_default_constructible_v<decltype(args)>);

    sourceText = QByteArray();
    translator.clear();
    args = decltype(args)();
    disambiguation.reset();
    n = -1;
}

QString filterCases(
    QString s, ChangeCases cases,
    const QMap<QString, std::variant<ChangeCases, QString>> &exceptions) {
    static const QStringView wordRegEx(u"\\s+");
    static const auto handleExceptCase = [&exceptions](QString &word) {
        Q_ASSERT(!word.isEmpty());

        const auto it = exceptions.constFind(word);
        if (it == exceptions.cend()) // Not an exceptional case
            return false;

        if (const auto replace = std::get_if<QString>(&it.value()))
            word = *replace;
        else
            try {
                switch (std::get<ChangeCases>(it.value())) {
                case ChangeCases::ToLower:
                    word[0] = word[0].toLower();
                    break;
                case ChangeCases::ToUpper:
                    word[0] = word[0].toUpper();
                    break;
                case ChangeCases::ToTitleCase:
                    word[0] = word[0].toTitleCase();
                    // break;
                default: // (ChangeCases::NotChange -- do nothing)
                    ;
                }
            } catch (const std::bad_variant_access &ex) {
                qWarning() << qTr("Exception thrown:") << ex.what();
            }

        return true;
    };

    // Filter out snake_case
    s.replace('_', ' ');

    // Filter out camel and Pascal cases
    QRegularExpression camelCase(QStringLiteral("([a-z0-9])([A-Z])"));
    s.replace(camelCase, QStringLiteral("\\1 \\2"));

    const bool doExcepts = !exceptions.isEmpty();
    if (cases == ChangeCases::NotChange) {
        if (!doExcepts)
            return s;
        QStringList words = s.split(QRegularExpression(QString::fromRawData(
                                        wordRegEx.data(), wordRegEx.size())),
                                    Qt::SkipEmptyParts);
        for (auto &word : words)
            handleExceptCase(word);

        return words.join(' ');
    } else {
        QStringList words = s.split(QRegularExpression(QString::fromRawData(
                                        wordRegEx.data(), wordRegEx.size())),
                                    Qt::SkipEmptyParts);
        for (auto &word : words) {
            if (!doExcepts ||
                !handleExceptCase(
                    word)) // Not an exception, follow regular flow
            {
                switch (cases) {
                case ChangeCases::ToLower:
                    word[0] = word[0].toLower();
                    break;
                case ChangeCases::ToUpper:
                    word[0] = word[0].toUpper();
                    break;
                case ChangeCases::ToTitleCase:
                    word[0] = word[0].toTitleCase();
                    // break;
                default:;
                }
            }
        }

        return words.join(' ');
    }
}

FmtStringArg::FmtStringArg() = default;

FmtStringArg::FmtStringArg(const QString &str)
    : a(str)
    , fieldWidth{0}
    , fillChar{}
    , id() {}

FmtStringArg::FmtStringArg(QString &&str)
    : a(std::move(str))
    , fieldWidth{0}
    , fillChar{}
    , id() {}
} // namespace VvvfSimulator::Util::String
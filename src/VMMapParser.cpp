#include "VMMapParser.h"
#include <QProcess>
#include <QDebug>

VMMapParser::Result VMMapParser::parseForPid(const int64_t pid)
{

    Result result;
    result.pid = pid;

    QProcess process;
    process.start("vmmap", {QString("%1").arg(pid)}, QIODevice::OpenModeFlag::ReadOnly);

    if (!process.waitForFinished(10000))
    {
        qCritical() << "timeout wait for finished";
        process.close();
        return result;
    }

    const auto output = process.readAllStandardOutput();
    process.close();

    result.hasAccess = true;

    enum class Category
    {
        Unknown,
        Initial,

        NonWritibleRegionsTableHeader,
        NonWritibleRegionsTable,

        WritibleRegionsTableHeader,
        WritibleRegionsTable,
    };

    int regionDetailPos = -1;
    auto category = Category::Initial;
    const auto lines = output.split('\n');
    result.regions.reserve(lines.count());

    bool end = false;

    for (auto line_ : lines)
    {
        if (end)
        {
            break;
        }

        const auto line = line_.replace('\r', "");

        switch(category)
        {
        case Category::Unknown:
            qCritical() << "Unknown category";
            return result;

        case Category::Initial:
        {
            if (line.startsWith("=") &&
                line.toLower().replace(' ', "").replace('=', "").startsWith("non-writableregions"))
            {
                category = Category::NonWritibleRegionsTableHeader;
            }
        }
        break;

        case Category::NonWritibleRegionsTableHeader:
        {
            regionDetailPos = line.toLower().indexOf("region detail");
            if (regionDetailPos < 0)
            {
                qCritical() << "region detail not found";
                return result;
            }

            category = Category::NonWritibleRegionsTable;
        }
        break;

        case Category::NonWritibleRegionsTable:
        {
            if (line.startsWith("=") &&
                line.toLower().replace(' ', "").replace('=', "").startsWith("writableregions"))
            {
                category = Category::WritibleRegionsTableHeader;
            }
            else
            {
                result.regions.append(line.mid(regionDetailPos));
            }
        }
        break;

        case Category::WritibleRegionsTableHeader:
        {
            regionDetailPos = line.toLower().indexOf("region detail");
            if (regionDetailPos < 0)
            {
                qCritical() << "region detail not found";
                return result;
            }

            category = Category::WritibleRegionsTable;
        }
        break;

        case Category::WritibleRegionsTable:
        {
            if (line.startsWith("="))
            {
                end = true;
            }
            else
            {
                result.regions.append(line.mid(regionDetailPos));
            }
        }
        break;
        }
    }

    return result;
}

#ifndef Logger_H
#define Logger_H

#include <fstream>
#include <string>
#include <tuple>

namespace Util
{
    /**
     * A utility class to log output to a csv file
     */
    template <typename ... Types>
    class Logger
    {
    public:
        /**
         * Constructor
         */
        Logger(const std::string& filename) 
            : m_filename(filename)
        {
        }

        ~Logger()
        {
            if (!m_entries.empty())
            {
                Write();
            }
        }

        /**
         * Set a new filename
         */
        void SetFilename(const std::string& filename) 
        { 
            m_filename = filename; 
        }

        /**
         * Adds a new row of data
         */
        void AddLine(const Types& ... params)
        {
            m_entries.push_back(std::make_tuple(params ...));
        }

        /**
         * Writes the stored values to file
         */
        void Write()
        {
            std::ofstream f(m_filename);
            for (auto& entry : m_entries)
            {
                std::apply([&f](const Types& ... values) 
                {
                    size_t n = 0;
                    ((f << values << (++n != sizeof...(Types) ? "," : "\n")), ...);
                }, entry);
            }
            m_entries.clear();
        }

    private:
        std::vector<std::tuple<Types...>> m_entries;
        std::string m_filename;
    };
}

#endif 

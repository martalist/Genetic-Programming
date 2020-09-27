#ifndef Logger_H
#define Logger_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <tuple>

namespace Util
{
    namespace
    {
        const std::string OutputDir = "output";
        const std::string Extension = ".csv";
    }

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
            namespace fs = std::filesystem;

            // Get the current time, to use in the file output
            auto now = std::chrono::system_clock::now();
            auto t_c = std::chrono::system_clock::to_time_t(now);
            std::stringstream str;
            str << std::put_time(std::localtime(&t_c), "%Y-%m-%d_%H-%M-%S");
            m_prefix = str.str();

            // Create the output directory, if it doesn't exist
            auto outputDir = std::filesystem::current_path().append(OutputDir);
            if (!fs::exists(outputDir))
            {
                fs::create_directory(outputDir);
            }
            outputDir = outputDir.append(m_prefix);
            if (!fs::exists(outputDir))
            {
                fs::create_directory(outputDir);
            }
            m_outputDir = outputDir;
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

        template <typename ... H>
        void WriteHeader(const H& ... h)
        {
            std::ofstream f(GetFilename(), std::ios_base::app);
            // std::apply([&f](const H& ... header) 
            // {
                // size_t n = 0;
                // ((f << header << (++n != sizeof...(H) ? "," : "\n")), ...);
            // }, std::make_tuple(h ...));
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
            std::ofstream f(GetFilename(), std::ios_base::app);
            f << std::fixed;
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

        /**
         * @return the directory that logged results are written to
         */
        std::string GetOutputDir() const { return m_outputDir; }

    private:

        std::string GetFilename() const
        {
            std::stringstream file;
            file << m_outputDir << "/" << m_prefix << "_" << m_filename << Extension;
            return file.str();
        }

        std::vector<std::tuple<Types...>> m_entries; ///< rows of data to be written to file
        std::string m_filename; ///< The user-specified filename
        std::string m_prefix; ///< The prefix, used for sub-dir and file output
        std::string m_outputDir; ///< The directory the files will be written to
    };
}

#endif 

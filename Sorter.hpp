#ifndef SORTER_HPP
#define SORTER_HPP

#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <unordered_map>

namespace fs = std::filesystem;

class Sorter {
    private:
        fs :: path sourceDir;
        fs :: path logPath;

        std :: unordered_map<std :: string, std :: string> rules = {
            {".jpg", "Images"},
            {".png", "Images"},
            {".gif", "Images"},
            {".mp4", "Videos"},
            {".mkv", "Videos"},
            {".mp3", "Music"},
            {".pdf", "Documents"},
            {".docx", "Documents"},
            {".zip", "Archives"},
            {".rar", "Archives"},
            {".jpeg", "Images"},
            {".txt", "Documents"},
            {".dmg", "Archives"},
            {".mov", "Videos"}
        };

        void log(const std :: string &message) {
            std :: ofstream logFile(logPath, std::ios_base::app);
            auto now = std :: chrono :: system_clock :: to_time_t(std :: chrono :: system_clock :: now());
            logFile << "[" << std :: strtok(std :: ctime(&now), "\n") << "]" << message << std :: endl;
        }

    public:
        Sorter(std :: string src, std :: string logFile) : sourceDir(src) , logPath(logFile) {

            for(const auto &rule : rules) {
                fs :: path target = sourceDir / rule.second;
                if(!fs::exists(target)) {
                    fs::create_directory(target);
                }
            }
        }

        void run_once() {
            for(const auto &entry : fs :: directory_iterator(sourceDir)) {
                if(fs :: is_regular_file(entry)) {
                    std :: string ext = entry.path().extension().string();

                    if(rules.find(ext) != rules.end()) {
                        std :: string folderName = rules[ext];
                        fs :: path targetPath = sourceDir / folderName / entry.path().filename();

                        try{
                            fs::rename(entry.path(), targetPath);
                            log("Moved file " + entry.path().filename().string() + " to " + folderName);
                        }catch(const fs :: filesystem_error & e) {
                            log("Error moving file " + entry.path().string() + " to " + targetPath.string() + ": " + e.what());
                        }
                    }
                }
            }
        }
};

#endif
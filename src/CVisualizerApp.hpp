#ifndef CVISUALIZERAPP_HPP
#define CVISUALIZERAPP_HPP

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>

#include "ISearchable.hpp"
#include "IFrontier.hpp"
#include "Algorithm.hpp"

class CVisualizerApp
{
    public:
        CVisualizerApp();
        ~CVisualizerApp();
        
        bool init(int width, int height, const std::string &title);
        bool loadGraph(const std::string &filePath);

        void startSearch(IFrontier *frontier);
        void stopSearch();

        void run();
    private:
        std::unique_ptr<ISearchable> m_graph;
    
        std::unordered_set<NodeID> m_visited;
        std::unordered_map<NodeID, NodeID> m_parents;
        std::vector<NodeID> m_path;
        std::mutex m_stateMutex;

        std::thread m_worker;
        std::atomic<bool> m_isRunning{false};
        std::atomic<bool> m_shouldStop{false};
        std::atomic<int> m_sleepMs{50};

        NodeID m_startNode = 0;
        NodeID m_endNode = 0;

        void executeSearch(IFrontier *frontier);

        void renderUI();
        void renderContent();
};

#endif
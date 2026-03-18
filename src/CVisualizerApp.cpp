#include <algorithm>
#include <iostream>

#include "CVisualizerApp.hpp"
#include "CMaze.hpp"
#include "CGraph.hpp"
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "CQueueFrontier.hpp"
#include "CStackFrontier.hpp"
#include "CPriorityFrontier.hpp"

CVisualizerApp::CVisualizerApp()
    : m_isRunning(false), m_shouldStop(false), m_sleepMs(500) {}

CVisualizerApp::~CVisualizerApp()
{
    stopSearch();
    rlImGuiShutdown();
    CloseWindow();
}

bool CVisualizerApp::init(int width, int height, const std::string &title)
{
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);
    rlImGuiSetup(true);

    return true;
}

bool CVisualizerApp::loadGraph(const std::string &filePath)
{
    stopSearch();
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_visited.clear();
        m_parents.clear();
        m_path.clear();
        m_visitedEdges.clear();
    }

    if(filePath.find(".graph") != std::string::npos) {
        m_graph = std::make_unique<CGraph>(filePath);
    } else {
        m_graph = std::make_unique<CMaze>(filePath);
    }
    
    if(m_graph) {
        m_startNode = m_graph->getStart();
        m_endNode = m_graph->getEnd();
    }

    return true;
}

void CVisualizerApp::startSearch(IFrontier *frontier)
{
    if(m_isRunning) return;

    if(m_worker.joinable()) {
        m_worker.join();
    }

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_visited.clear();
        m_parents.clear();
        m_path.clear();
        m_visitedEdges.clear();
    }

    m_worker = std::thread([this, frontier]() { this->executeSearch(frontier); });
}

void CVisualizerApp::stopSearch()
{
    m_shouldStop = true;
    if(m_worker.joinable()) m_worker.join();
    m_isRunning = false;
    m_shouldStop = false;
}

void CVisualizerApp::run()
{
    while(!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        
        if(m_graph) m_graph->update(deltaTime);

        BeginDrawing();
        ClearBackground(DARKGRAY);

        renderContent();
        renderUI();

        EndDrawing();
    }
}

void CVisualizerApp::executeSearch(IFrontier *frontier)
{
    if(!m_graph) return;

    m_isRunning = true;

    onVisit hook = [this](NodeID parent, NodeID current) {
        if(m_shouldStop) return;

        if(parent == -1)
        {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            m_visited.insert(current);
            return;
        }

        m_activeParent = parent;
        m_activeCurrent = current;
        m_edgeProgress = 0.0f;

        int steps = 20;
        int sleepTime = m_sleepMs.load();
        int stepTime = (sleepTime > 0) ? sleepTime / steps : 0;
        for(int i = 1; i <= steps; ++i)
        {
            if(m_shouldStop) return;
            
            m_edgeProgress = (float)i / (float)steps;
            if(stepTime > 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(stepTime));
            }
        }

        {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            m_visitedEdges.push_back({parent, current});
            m_visited.insert(current);
        }

        m_activeParent = -1;
        m_activeCurrent = -1;
    };

    auto parents = genericSearch(*m_graph, m_startNode, m_endNode, *frontier, hook);

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_parents = parents;
        m_path = reconstructPath(m_parents, m_startNode, m_endNode);
    }

    m_isRunning = false;
}

void CVisualizerApp::renderUI()
{
    rlImGuiBegin();
    ImGui::Begin("Controls");

    static int currentAlgo = 0;
    ImGui::Combo("Algorithm", &currentAlgo, "BFS\0DFS\0A*\0");

    int speed = m_sleepMs.load();
    if(ImGui::SliderInt("Speed", &speed, 1, 1000))
    {
        m_sleepMs.store(speed);
    }

    if(ImGui::Button("Start search") && !m_isRunning)
    {
        static CQueueFrontier bfs;
        static CStackFrontier dfs;
        static CPriorityFrontier aStar;

        IFrontier *selected = nullptr;
        if(currentAlgo == 0) selected = &bfs;
        else if(currentAlgo == 1) selected = &dfs;
        else selected = &aStar;

        startSearch(selected);
    }

    if(ImGui::Button("Reset"))
    {
        loadGraph("demo-graphs/test2.graph");
    }

    ImGui::End();
    rlImGuiEnd();
}

void CVisualizerApp::renderContent()
{
    if(!m_graph) return;

    auto getColor = [this](NodeID id) -> Color
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);

        if(id == m_startNode) return GREEN;
        if(id == m_endNode) return RED;
        if(std::find(m_path.begin(), m_path.end(), id) != m_path.end()) return BLUE;
        if(m_visited.count(id)) return YELLOW;

        return WHITE;
    };

    auto getEdge = [this](NodeID u, NodeID v) -> TEdgeState
    {
        {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            bool uOnPath = (u == m_startNode || u == m_endNode || std::find(m_path.begin(), m_path.end(), u) != m_path.end());
            bool vOnPath = (v == m_startNode || v == m_endNode || std::find(m_path.begin(), m_path.end(), v) != m_path.end());

            if(uOnPath && vOnPath)
            {
                if (m_parents.count(u) && m_parents.at(u) == v) return { 1.0f, BLUE };
                if (m_parents.count(v) && m_parents.at(v) == u) return { 1.0f, BLUE };
            }
        }

        if(u == m_activeParent && v == m_activeCurrent)
        {
            return { m_edgeProgress.load(), YELLOW };
        }

        {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            for(const auto& edge : m_visitedEdges)
            {
                if((edge.u == u && edge.v == v) || (edge.u == v && edge.v == u))
                {
                    return { 1.0f, YELLOW };
                }
            }
        }

        return { 0.0f, BLACK };
    };

    m_graph->draw(0, 0, (float)GetScreenWidth(), (float)GetScreenHeight(), getColor, getEdge);
}

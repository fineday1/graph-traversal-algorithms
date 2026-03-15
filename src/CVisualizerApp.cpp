#include <algorithm>
#include <iostream>

#include "CVisualizerApp.hpp"
#include "CMaze.hpp"
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "CQueueFrontier.hpp"
#include "CStackFrontier.hpp"
#include "CPriorityFrontier.hpp"

CVisualizerApp::CVisualizerApp()
    : m_isRunning(false), m_shouldStop(false), m_sleepMs(50) {}

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
    }

    m_graph = std::make_unique<CMaze>(filePath);
    
    // Capture start/end for the visualizer state
    if (auto* maze = dynamic_cast<CMaze*>(m_graph.get())) {
        m_startNode = maze->getStart();
        m_endNode = maze->getEnd();
    }

    return true;
}

void CVisualizerApp::startSearch(IFrontier *frontier)
{
    if(m_isRunning) return;

    if (m_worker.joinable()) {
        m_worker.join();
    }

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_visited.clear();
        m_parents.clear();
        m_path.clear();
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
        BeginDrawing();
        ClearBackground(DARKGRAY);

        renderContent();
        renderUI();

        EndDrawing();
    }
}

void CVisualizerApp::executeSearch(IFrontier *frontier)
{
    if (!m_graph) return;

    m_isRunning = true;

    onVisit hook = [this](NodeID node) {
        if (m_shouldStop) return;

        {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            m_visited.insert(node);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepMs.load()));
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
    if(ImGui::SliderInt("Speed", &speed, 1, 500))
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
        loadGraph("maze.txt");
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

    m_graph->draw(0, 0, (float)GetScreenWidth(), (float)GetScreenHeight(), getColor);
}

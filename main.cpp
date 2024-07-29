/**
 * @file main.cpp
 * @brief 2024华为嵌入式软件大赛·追光者(算法组)
 */

#include <cstdint>
#include <cstdio>
#include <cassert>
#include <vector>
#include <list>
#include <queue>
#include <algorithm>
using namespace std;

/* LOG */
#ifdef DEBUG // DEBUG在本地g++编译时定义，上传至官网后不会被定义
    #define LOG_INIT(LOG_FILE_PATH) {freopen(LOG_FILE_PATH, "w+", stderr);}
    #define LOG_INFO(arg...) {fprintf(stderr, ##arg); fflush(stderr);}
    #define LOG_LINE() LOG_INFO("####################################################################################\n");
    #define LOG(arg...) {fprintf(stderr, "%s:%d > ", __FILE__, __LINE__); fprintf(stderr, ##arg);}
    #define ASSERT(state) assert(state)
#else
    #define LOG_INIT(...)
    #define LOG_INFO(...)
    #define LOG_LINE(...)
    #define LOG(...)
    #define ASSERT(...)
#endif // DEBUG

#define ASSERT_ID(id, vec) ASSERT(id > 0 && (id - 1) < vec.size())

static constexpr uint16_t INVALID_ID = 0; // 无效ID

/**
 * @brief 有编号的类型
 */
class IdObject {
public:
    explicit IdObject(uint16_t id) : id(id) {}

public:
    uint16_t GetId() const {return id;}

protected:
    uint16_t id;
};

/**
 * @brief 有存活状态的类型
 */
class LiveObject {
public:
    explicit LiveObject(bool isAlive = true) : isAlive(isAlive) {}

public:
    bool IsAlive() const {return isAlive;}
    void SetAlive(bool isAlive = true) {this->isAlive = isAlive;}
    void Kill() {SetAlive(false);}

protected:
    bool isAlive;
};

/**
 * @brief 边
 */
class Edge : public IdObject, public LiveObject {
public:
    static const uint8_t CHANNELS_NUM = 40;

public:
    Edge(uint16_t id, uint16_t node1, uint16_t node2) :
        IdObject(id), node1(node1), node2(node2), channels(CHANNELS_NUM, INVALID_ID) {}

public:
    /**
     * @brief 获取另一个端点
     * @param node 端点编号
     * @return uint16_t 另一个端点的编号
     */
    uint16_t GetAnotherNode(uint16_t node) const {
        ASSERT(node == node1 || node == node2);
        return node == node1 ? node2 : node1;
    }
    uint16_t GetChannel(uint8_t id) const {ASSERT_ID(id, channels); return channels[id - 1];}
    void SetChannel(uint8_t id, uint16_t service) {
        ASSERT_ID(id, channels); 
#ifdef DEBUG
        if(GetChannel(id) != INVALID_ID && service != INVALID_ID && GetChannel(id) != service) {
            LOG("[Warning] Edge[%02d](%02d): [%02d]->[%02d] Channel is not released before use!\n",
                GetId(), id, GetChannel(id), service);
        }
#endif // DEBUG
        channels[id - 1] = service;
    }
    void SetChannels(uint8_t startChannel, uint8_t useChannelsNum, uint16_t service) {
        for (uint8_t i = 0; i < useChannelsNum; ++i) {
            SetChannel(startChannel++, service);
        }
    }
    bool CheckChannelFree(uint8_t channel) const {return GetChannel(channel) == INVALID_ID;}
    bool CheckChannelsFree(uint8_t startChannel, uint8_t useChannelsNum) const {
        ASSERT(startChannel + useChannelsNum - 1 <= Edge::CHANNELS_NUM);
        for (uint8_t i = 0; i < useChannelsNum; ++i) {
            if (!CheckChannelFree(startChannel + i)) {
                return false;
            }
        }
        return true;
    }
<<<<<<< HEAD
    
    /**
     * @brief 查找空闲空间
     * @param size 宽度
     * @param busissnessId 业务Id 可以使用业务自身的资源
     * @return 空区间的起点
     */
    uint16_t findEmptyChannel(int size){
        uint16_t emptyBegin;
        for(int i = 0; i < CHANNELS_NUM; i++){
            if(channels[i] == 0){
                int flag = 1;
                int j;
                for(int j = 1; j < size; j++){
                    if(i+j < CHANNELS_NUM && channels[i+j] != 0){
                        flag = 0;
                        break;
                    }
                }
                if(flag){
                    emptyBegin = i;
                    break;
                }else{
                    i = i+j;
                }
            }
        }
        // 通道编号是从1开始的 数组编号从0开始的
        emptyBegin++;
        return emptyBegin;
=======
    /**
     * @brief 分配通道
     * @param useChannelsNum 需要占用的通道数
     * @return uint8_t 为0时代表无可用通道
     */
    uint8_t AllocateChannel(uint8_t useChannelsNum) {
        ASSERT(useChannelsNum <= CHANNELS_NUM);
        // !TODO:需要性能更佳的分配策略
        // 采用首次适配法
        uint8_t len = 0;
        uint8_t endI = CHANNELS_NUM - useChannelsNum + 1; // 允许从len=0开始寻找的最晚开始位置
        for (uint8_t i = 1; i <= CHANNELS_NUM; ++i) {
            if (CheckChannelFree(i)) {
                ++len;
                if (len == useChannelsNum) {
                    return i - (useChannelsNum - 1);
                }
            } else {
                len = 0;
                if (i > endI) { // 提前剪枝
                    break;
                }
            }
        }
        return 0;
>>>>>>> b6a6b03c7906fe98c01e28dc2016254dd369cff1
    }

private:
    uint16_t node1; // 编号数值较低的端点
    uint16_t node2; // 编号数值较高的端点
    vector<uint16_t> channels; // 占用各个通道的业务编号
};

/**
 * @brief 结点
 */
class Node : public IdObject {
public:
    Node(uint16_t id, uint8_t changeChannelCntMax) :
        IdObject(id), changeChannelCntMax(changeChannelCntMax), changeChannelCnt(0) {}

public:
    const vector<uint16_t>& GetConnectedEdges() const {return connectedEdges;}
    uint8_t GetRemainChangeChannelCnt() const {return changeChannelCntMax - changeChannelCnt;}
<<<<<<< HEAD
    void addEdge(uint16_t id) {connectedEdges.push_back(id);}
=======
    void AddEdge(uint16_t id) {connectedEdges.push_back(id);}
    bool IsAllowChangeChannel() const {return GetRemainChangeChannelCnt() > 0;}
    void UseChangeChannelCnt() {
        if (changeChannelCnt < changeChannelCntMax) {
            ++changeChannelCnt;
        }
    }
    void ReleaseChangeChannelCnt() {
        if (changeChannelCnt > 0) {
            --changeChannelCnt;
        }
    }

>>>>>>> b6a6b03c7906fe98c01e28dc2016254dd369cff1
private:
    vector<uint16_t> connectedEdges;
    uint8_t changeChannelCntMax; // 最多可变通道数
    uint8_t changeChannelCnt; // 可变通道数
};

/**
 * @brief 路径上的一步
 */
class Step {
public:
    Step(uint16_t edge, uint16_t startNode, uint8_t startChannel, uint8_t useChannelsNum,
         bool channelChanged = false) :
        edge(edge), startNode(startNode), startChannel(startChannel),
        useChannelsNum(useChannelsNum), channelChanged(channelChanged)
    {
        ASSERT(GetEndChannel() <= Edge::CHANNELS_NUM);
    }

public:
    uint16_t GetEdge() const {return edge;}
    uint16_t GetStartNode() const {return startNode;}
    uint8_t GetStartChannel() const {return startChannel;}
    uint8_t GetUseChannelsNum() const {return useChannelsNum;}
    uint8_t GetEndChannel() const {return GetStartChannel() + GetUseChannelsNum() - 1;}
    bool IsChannelChanged() const {return channelChanged;}

private:
    uint16_t edge;
    uint16_t startNode;
    uint8_t startChannel;
    uint8_t useChannelsNum;
    bool channelChanged; // 从起点处变更了通道
};

/**
 * @brief 业务
 */
class Service : public IdObject, public LiveObject {
public:
    Service(uint16_t id, uint16_t start, uint16_t end, double value,
            uint8_t defaultChannelStart, uint8_t useChannelsNum) :
        IdObject(id), start(start), end(end), pathEnd(start), value(value),
        defaultChannelStart(defaultChannelStart), useChannelsNum(useChannelsNum),
        lastChannelStart(defaultChannelStart) {}
    void AddStep(uint16_t edge, uint16_t endNode, uint8_t startChannel) {
        path.push_back(Step(edge, pathEnd, startChannel, GetUseChannelsNum(), lastChannelStart != startChannel));
        pathEnd = endNode;
        lastChannelStart = startChannel;
    }
    void AddStep(uint16_t edge, uint16_t endNode) {
        AddStep(edge, endNode, GetLastChannelStart());
    }
    double GetValue() const {return value;}
    uint16_t GetStart() const {return start;}
    uint16_t GetEnd() const {return end;}
    uint8_t GetDefaultChannelStart() const {return defaultChannelStart;}
    uint8_t GetLastChannelStart() const {return lastChannelStart;}
    uint8_t GetUseChannelsNum() const {return useChannelsNum;}
    uint16_t GetDefualtChannelEnd() const {return defaultChannelStart + useChannelsNum - 1;}
    const vector<Step>& GetPath() const {return path;}
    vector<Step> ClearPath() {
        auto ret = path;
        path.clear();
        pathEnd = start;
        lastChannelStart = defaultChannelStart;
        return ret;
    }
    /**
     * @brief 重设路径（默认为完整有效路径，不进行检查）
     * @param path 新路径
     */
    void ResetPath(const vector<Step>& path) {
        this->path = path;
        pathEnd = end;
        lastChannelStart = path.back().GetStartChannel();
    }
    bool IsPathComplete() const {return pathEnd == end;}
    /**
     * @brief 获取路径当前已规划到的点
     * @return uint16_t 路径当前已规划到的点
     */
    uint16_t GetPathEnd() const {return pathEnd;}

private:
    uint16_t start;
    uint16_t end;
    vector<Step> path;
    uint16_t pathEnd; // 路径已经规划到的点
    double value;
    uint8_t defaultChannelStart;
    uint8_t useChannelsNum;
    uint8_t lastChannelStart; // 上次添加的路径中通道的起点
};

/**
 * @brief 场景
 */
class Scene {
public:
    void AddNode(uint8_t changeChannelCntMax) {nodes.push_back(Node(nodes.size() + 1, changeChannelCntMax));}
    void CreateNodeDistanceTable(uint16_t nodesNum) {
        ASSERT(nodeDistance == nullptr); // 只允许创建一次
        nodeDistance = new vector<vector<uint16_t>>(nodesNum, vector<uint16_t>(nodesNum, UINT16_MAX));
        for (uint16_t i = 0; i < nodesNum; ++i) {
            (*nodeDistance)[i][i] = 0;
        }
    }
    void DeleteNodeDistanceTable() {
        ASSERT(nodeDistance); // 只允许在创建后调用
        delete nodeDistance;
    }
    uint16_t GetNodeDistance(uint16_t node1, uint16_t node2) const {
        ASSERT(nodeDistance);
        ASSERT_ID(node1, nodes);
        ASSERT_ID(node2, nodes);
        return (*nodeDistance)[node1 - 1][node2 - 1];
    }
    void Floyd() {
        for (uint16_t relay = 1; relay <= GetNodesNum(); ++relay) { // 中继点
            for (uint16_t start = 1; start <= GetNodesNum(); ++start) { // 起点
                for (uint16_t end = start + 1; end <= GetNodesNum(); ++end) { // 终点
                    uint16_t dis1 = GetNodeDistance(start, relay), dis2 = GetNodeDistance(relay, end);
                    if (dis1 == UINT16_MAX || dis2 == UINT16_MAX) {continue;}
                    RefreshNodeDistance(start, end, dis1 + dis2);
                }
            }
        }
    }
    void AddEdge(uint16_t node1, uint16_t node2) {
        uint16_t id = edges.size() + 1;
        edges.push_back(Edge(id, node1, node2));
        GetNode(node1).AddEdge(id);
        GetNode(node2).AddEdge(id);
        RefreshNodeDistance(node1, node2, 1);
    }
    void AddService(uint16_t start, uint16_t end, double value, uint8_t startChannel, uint8_t useChannelsNum) {
        services.push_back(Service(services.size() + 1, start, end, value, startChannel, useChannelsNum));
    }
    Node& GetNode(uint16_t id)                        {ASSERT_ID(id, nodes);    return nodes[id - 1];}
    const Node& GetNodeConst(uint16_t id) const       {ASSERT_ID(id, nodes);    return nodes[id - 1];}
    Edge& GetEdge(uint16_t id)                        {ASSERT_ID(id, edges);    return edges[id - 1];}
    const Edge& GetEdgeConst(uint16_t id) const       {ASSERT_ID(id, edges);    return edges[id - 1];}
    Service& GetService(uint16_t id)                  {ASSERT_ID(id, services); return services[id - 1];}
    const Service& GetServiceConst(uint16_t id) const {ASSERT_ID(id, services); return services[id - 1];}
    void AddServiceStep(uint16_t id, uint16_t edge, uint8_t startChannel) {
        Service& s = GetService(id);
        Edge& e = GetEdge(edge);
        if (startChannel != s.GetLastChannelStart()) {
            Node& node = GetNode(s.GetPathEnd());
            ASSERT(node.IsAllowChangeChannel());
            node.UseChangeChannelCnt();
        }
        s.AddStep(edge, e.GetAnotherNode(s.GetPathEnd()), startChannel);
        e.SetChannels(startChannel, s.GetUseChannelsNum(), id); // 记录业务路径的同时要在边的通道中标记业务
    }
    void AddServiceStep(uint16_t id, uint16_t edge) {
        Service& s = GetService(id);
        Edge& e = GetEdge(edge);
        s.AddStep(edge, e.GetAnotherNode(s.GetPathEnd()));
        e.SetChannels(s.GetLastChannelStart(), s.GetUseChannelsNum(), id); // 记录业务路径的同时要在边的通道中标记业务
    }
    size_t GetNodesNum() const {return nodes.size();}
    size_t GetEdgesNum() const {return edges.size();}
    size_t GetServicesNum() const {return services.size();}
    double GetValue() const {
        double value = 0.0;
        for (auto s : services) {
            if (s.IsAlive()) {
                value += s.GetValue();
            }
        }
        return value;
    }
    /**
     * 
     */
    void ResetChannelByPath(vector<Step> path, uint16_t id){
        AddPath(path, id);
    }
    /**
     * @brief 杀死指定边和路过的所有业务
     * @param edge 指定边
     * @return vector<uint16_t>&& 被杀死的业务
     */
    vector<uint16_t> Kill(uint16_t edge) {
        Edge& e = GetEdge(edge);
        e.Kill();
        vector<uint16_t> ret;
        for (uint8_t id = 1; id <= Edge::CHANNELS_NUM; ++id) {
            uint16_t s = e.GetChannel(id);
            if (s != INVALID_ID && GetService(s).IsAlive()) {
                ret.push_back(s);
                GetService(s).Kill();
            }
        }
        return ret;
    }
    /**
     * @brief 将业务路径添加到场景中
     * @param id 业务编号
     */
    void AddServicePath(uint16_t id) {
        AddPath(GetService(id).GetPath(), id);
    }
    /**
     * @brief 重设业务路径
     * @param id 业务编号
     * @param path 新路径
     */
    void ResetServicePath(uint16_t id, const vector<Step>& path) {
        GetService(id).ResetPath(path);
        AddServicePath(id);
    }
    /**
     * @brief 重设业务路径
     * @param id 业务编号
     * @param path 新路径
     * @param isReverse 是否需要翻转路径
     */
    void ResetServicePath(uint16_t id, vector<Step> path, bool isReverse) {
        if (isReverse) {
            reverse(path.begin(), path.end());
        }
        ResetServicePath(id, path);
    }
    /**
     * @brief 隐藏业务路径
     * @param id 需要隐藏的业务
     */
    void HideServicePath(uint16_t id) {
        auto path = GetService(id).ClearPath();
        DeletePath(path);
        servicesHided.push_back(make_pair(id, path));
    }
    /**
     * @brief 恢复被隐藏的路径，如果没有被隐藏过则不做任何操作
     * @param id 被隐藏的业务
     */
    void RecoverServicePath(uint16_t id) {
        for (auto it = servicesHided.begin(); it != servicesHided.end(); ++it) {
            if (it->first == id) {
                ResetServicePath(id, it->second);
                break;
            }
        }
    }
    /**
     * @brief 恢复被隐藏的路径，如果没有被隐藏过则不做任何操作
     * @param id 被隐藏的业务
     * @param resetServicePath 恢复时是否需要覆盖业务中已存储的路径
     */
    void RecoverServicePath(uint16_t id, bool resetServicePath) {
        for (auto it = servicesHided.begin(); it != servicesHided.end(); ++it) {
            if (it->first == id) {
                if (resetServicePath) {
                    ResetServicePath(id, it->second);
                } else {
                    AddPath(it->second, id);
                }
                break;
            }
        }
    }
    /**
     * @brief 删除路径隐藏记录
     * @param id 被隐藏的业务
     * @param reDelete 是否需要再执行一次删除操作
     */
    void DeleteHidedPath(uint16_t id, bool reDelete = false) {
        for (auto it = servicesHided.begin(); it != servicesHided.end(); ++it) {
            if (it->first == id) {
                if (reDelete) {
                    DeletePath(it->second);
                }
                servicesHided.erase(it);
                break;
            }
        }
    }
    /**
     * @brief 清空被隐藏的路径
     * @param reDelete 是否需要再执行一次删除操作
     */
    void ClearHidedPath(bool reDelete = false) {
        if (reDelete) {
            for (auto it : servicesHided) {
                DeletePath(it.second);
            }
        }
        servicesHided.clear();
    }

private:
    void AddPath(const vector<Step>& path, uint16_t id) {
        for (auto step : path) {
            GetEdge(step.GetEdge()).SetChannels(step.GetStartChannel(), step.GetUseChannelsNum(), id);
            if (step.IsChannelChanged()) {
                GetNode(step.GetStartNode()).UseChangeChannelCnt();
            }
        }
    }
    void DeletePath(const vector<Step>& path) {
        for (auto step : path) {
            GetEdge(step.GetEdge()).SetChannels(step.GetStartChannel(), step.GetUseChannelsNum(), INVALID_ID);
            if (step.IsChannelChanged()) {
                GetNode(step.GetStartNode()).ReleaseChangeChannelCnt();
            }
        }
    }
    void RefreshNodeDistance(uint16_t node1, uint16_t node2, uint16_t distance) {
        if (distance < GetNodeDistance(node1, node2)) {
            (*nodeDistance)[node1 - 1][node2 - 1] = distance;
            (*nodeDistance)[node2 - 1][node1 - 1] = distance;
        }
    }

private:
    vector<Node> nodes; // 结点表
    vector<Edge> edges; // 边表
    vector<Service> services; // 业务表
    vector<pair<uint16_t, vector<Step>>> servicesHided; // 隐藏的业务表路径
    vector<vector<uint16_t>>* nodeDistance = nullptr; // 结点距离表
};

/**
 * @brief 解决方案
 */
class Solution {
public:
    Solution(const Scene& s) : s(s) {}

public:
    void Handle(uint16_t edge) {
        vector<uint16_t> services = s.Kill(edge);
        vector<uint16_t> output;
        sort(services.begin(), services.end(), [=] (uint16_t sid1, uint16_t sid2) {
            return ServiceCompare(s.GetServiceConst(sid1), s.GetServiceConst(sid2));
        });
        Planning(services, output);
        for (auto sid : output) {
            s.GetService(sid).SetAlive();
        }
        PrintAns(output);
    }
    double GetValue() const {
        return s.GetValue();
    }

protected:
    Scene s;

private:
    /**
     * @brief 业务比较函数
     * @param s1 业务1
     * @param s2 业务2
     * @return true 在排序时将业务1放在前面
     * @return false 在排序时将业务2放在前面
     */
    virtual bool ServiceCompare(const Service& s1, const Service& s2) const  = 0;
    /**
     * @brief 规划器
     * @param services 受到影响的业务编号 
     * @param output 成功重新规划的业务编号（放入output的业务会自动被设置为复活）
     */
    virtual void Planning(const vector<uint16_t>& services, vector<uint16_t>& output) = 0;
    void PrintAns(const vector<uint16_t>& service) const {
        printf("%lu\n", service.size());
        for (auto bid : service) {
            const Service& ser = s.GetServiceConst(bid);
            const vector<Step>& path = ser.GetPath();
            printf("%u %lu\n", ser.GetId(), path.size());
            for (auto step : path) {
                printf("%u %u %u ", step.GetEdge(), step.GetStartChannel(), step.GetEndChannel());
            }
            printf("\n");
        }
        fflush(stdout);
    }
};

/**
 * @brief 徐天泽的算法
 */
class SolutionXTZ final : public Solution {
public:
    explicit SolutionXTZ(const Scene& s) : Solution(s) {}

private:
    /**
     * @brief 业务比较函数
     * @param s1 业务1
     * @param s2 业务2
     * @return true 在排序时将业务1放在前面
     * @return false 在排序时将业务2放在前面
     */
    bool ServiceCompare(const Service& s1, const Service& s2) const override {
        if (s1.GetValue() == s2.GetValue()) {
            return s1.GetUseChannelsNum() < s2.GetUseChannelsNum();
        }
        return s1.GetValue() > s2.GetValue();
    }
    /**
     * @brief 规划器
     * @param services 受到影响的业务编号 
     * @param output 成功重新规划的业务编号（放入output的业务会自动被设置为复活）
     */
    void PrintEdgeByService(Service service){
        vector<Step> path = service.GetPath();
        for(auto step : path){
            Edge& e = s.GetEdge(step.GetEdge());
            // printf("边%d的第%d个通道占用情况%d\n", e.GetId(), step.GetStartChannel(), e.GetChannel(step.GetStartChannel()));
        }
    }
    /**
     * @brief 规划器
     * @param services 受到影响的业务编号 
     * @param output 成功重新规划的业务编号（放入output的业务会自动被设置为复活）
     */
    void Planning(const vector<uint16_t>& services, vector<uint16_t>& output) override {
<<<<<<< HEAD
        for (auto b : services) {
            s.HideServicePath(b);
            const auto& service = s.GetService(b);
            uint16_t start = service.GetStart();
            uint16_t end = service.GetEnd();
            // 这里编号是对的吗？ 编号都是从1开始的 数组会不会小了一个？
            vector<bool> visited(s.GetNodesNum()+5, false);
=======
        // BFS(services, output);
        AStar(services, output);
    }
    void BFS(const vector<uint16_t>& services, vector<uint16_t>& output) {
        for (auto sid : services) {
            s.HideServicePath(sid);
            const auto& service = s.GetService(sid);
            const uint16_t start = service.GetStart();
            const uint16_t end = service.GetEnd();
>>>>>>> b6a6b03c7906fe98c01e28dc2016254dd369cff1
            struct BfsNode {
                uint16_t n; // 终点
                uint8_t c; // 使用的通道
                uint16_t e; // 抵达终点的边
                BfsNode* f; // 父结点
            };
            queue<BfsNode*> openSet; // 开集，待遍历
            queue<BfsNode*> closeSet; // 闭集，已完成遍历
            BfsNode* endNode = nullptr;
            for (uint8_t round = 0; round <= 1; ++round) {
                vector<bool> visited(s.GetNodesNum(), false);
                bool allowChangeChannel = bool(round); // 先不允许换通道，没有方案再换通道
                // 推入搜索起点
                openSet.push(new BfsNode{start, service.GetDefaultChannelStart(), INVALID_ID, nullptr});
                while (!openSet.empty()) { // BFS
                    auto node = openSet.front();
                    openSet.pop();
                    if (node->n == end) {
                        endNode = node;
                        closeSet.push(node);
                        break;
                    }
                    visited[node->n - 1] = true;
                    vector<uint16_t> edges = s.GetNodeConst(node->n).GetConnectedEdges();
                    sort(edges.begin(), edges.end(), [=](uint16_t e1, uint16_t e2) {
                        uint16_t n1 = s.GetEdge(e1).GetAnotherNode(node->n);
                        uint16_t n2 = s.GetEdge(e2).GetAnotherNode(node->n);
                        if (s.GetNodeDistance(n1, end) < s.GetNodeDistance(n2, end)) {
                            return true;
                        }
                        return s.GetNodeConst(n1).GetRemainChangeChannelCnt() > s.GetNodeConst(n2).GetRemainChangeChannelCnt();
                    });
                    for (auto e : edges) { // 从连接的边中找另一端结点
                        Edge& edge = s.GetEdge(e);
                        if (!edge.IsAlive()) { // 断边不考虑
                            continue;
                        }
                        uint16_t nextNode = edge.GetAnotherNode(node->n);
                        if (visited[nextNode - 1]) { // 遍历过的点不考虑
                            continue;
                        }
                        uint8_t nextChannel = node->c;
                        // 如果通道被占用，特殊处理
                        // !TODO:查询时间复杂度过高，需要优化
                        if (!edge.CheckChannelsFree(nextChannel, service.GetUseChannelsNum())) {
                            if (!allowChangeChannel || !s.GetNode(node->n).IsAllowChangeChannel()) {
                                continue;
                            }
                            nextChannel = edge.AllocateChannel(service.GetUseChannelsNum());
                            if (nextChannel == 0) { // 没找到能分配的通道
                                continue;
                            }
                        }
                        openSet.push(new BfsNode{nextNode, nextChannel, e, node});
                    }
                    closeSet.push(node);
                } 
                if (endNode) { // 找到了方案就不再尝试
                    break;
                }
            }
            if (endNode) {
                vector<Step> path;
                while (endNode->f != nullptr) {
                    path.push_back(Step(endNode->e, endNode->f->n, endNode->c, service.GetUseChannelsNum(), endNode->f->f ? endNode->c != endNode->f->c : false));
                    endNode = endNode->f;
                }
                s.ResetServicePath(sid, path, true);
                output.push_back(sid);
            }
            while (!openSet.empty()) {
                delete openSet.front();
                openSet.pop();
            }
            while (!closeSet.empty()) {
                delete closeSet.front();
                closeSet.pop();
            }
            s.RecoverServicePath(sid, endNode == nullptr); // 恢复老路径，但不覆盖掉新路径
        }
        // 所有新路径至此已经生成完毕
        s.ClearHidedPath(true); // 先不考虑新路径是否包含老路径，将老路径全部再删除一遍
        for (auto sid : services) { // 将新路径更新到场景中
            s.AddServicePath(sid);
        }
    }
    void AStar(const vector<uint16_t>& services, vector<uint16_t>& output) {
        constexpr double CHANGE_CHANNEL_COST = 0.1; // 换通道的成本
        for (auto sid : services) {
            s.HideServicePath(sid);
            const auto& service = s.GetService(sid);
            const uint16_t startNode = service.GetStart();
            const uint16_t endNode = service.GetEnd();
            const uint16_t useChannelsNum = service.GetUseChannelsNum();
            struct AStarNode {
                uint16_t n; // 终点结点
                uint8_t c; // 使用的通道
                uint16_t e; // 抵达终点的边
                AStarNode* f; // 父结点
                double passed; // 已走过的路的总成本
                double remain; // 剩余距离
                double Cost() const {return passed + remain;}
                bool operator<(const AStarNode& rhs) const {return Cost() < rhs.Cost();}
            };
            uint8_t searchChannelsNum = Edge::CHANNELS_NUM - useChannelsNum + 1; // 可以作为起始通道的通道总数
            // 已访问的起始通道 第一维为边的编号，第二维为起始通道编号（末尾的几个通道因为宽度不够，一定不会作为起始通道）
            vector<vector<bool>> visited(s.GetEdgesNum(), vector<bool>(searchChannelsNum, false));
            vector<bool> visitedEdge(s.GetEdgesNum(), false); // 边是否访问
            // 根据已占用的通道信息，将不可能作为起始通道的通道标记为已访问
            for (uint16_t eid = 1; eid <= s.GetEdgesNum(); ++eid) {
                const Edge& edge = s.GetEdgeConst(eid);
                if (!edge.IsAlive()) { // 将断边设置为已访问
                    visitedEdge[eid - 1] = true;
                    continue;
                }
                // 倒序遍历，当通道被占用，向前推 useChannelsNum 个通道都不可能作为起始通道
                bool isLastChannelFree = searchChannelsNum == Edge::CHANNELS_NUM ?
                    false : edge.CheckChannelsFree(searchChannelsNum + 1, useChannelsNum - 1); // 上一个通道是否未被占用
                for (int8_t cid = searchChannelsNum; cid >= 1; --cid) { // 向下循环不能用无符号数，使用有符号的int8_t
                    // 如果通道被占用，设置为已访问
                    if (!edge.CheckChannelFree(cid)) {
                        isLastChannelFree = false;
                        visited[eid - 1][cid - 1] = true;
                    // 通道没被占用，而上一个通道被占用，说明到了占用与未占用的分界处，向下推 useChannelsNum - 1 个通道都设置为已访问
                    } else if (!isLastChannelFree) {
                        isLastChannelFree = true;
                        uint8_t banId = cid; // 需要禁用的通道
                        for (uint8_t i = 0; i < useChannelsNum - 1; ++i) {
                            // 遇到了下一个占用通道的边界
                            if (!edge.CheckChannelFree(banId)) {
                                break;
                            }
                            visited[eid - 1][banId-- - 1] = true;
                            if (banId == 0) { // 防止在推的过程中推到边界
                                break;
                            }
                        }
                        cid = banId + 1;
                    }
                }
            }
            AStarNode* root = new AStarNode{startNode, service.GetDefaultChannelStart(), INVALID_ID, nullptr,
                                            0.0, double(s.GetNodeDistance(startNode, endNode))};
            AStarNode* current = root; // 正在遍历的结点
            list<AStarNode*> openSet; // 开集，待遍历
            queue<AStarNode*> closeSet; // 闭集，已完成遍历
            bool isSuccess = false; // 已完成寻路
            while (!isSuccess) {
                const vector<uint16_t>& edges = s.GetNodeConst(current->n).GetConnectedEdges();
                for (auto eid : edges) {
                    if (!visitedEdge[eid - 1]) {
                        visitedEdge[eid - 1] = true;
                        const Edge& edge = s.GetEdgeConst(eid);
                        const uint16_t nid = edge.GetAnotherNode(current->n);
                        const Node& node = s.GetNodeConst(nid);
                        uint16_t remainDis = s.GetNodeDistance(nid, endNode);
                        if (remainDis == UINT16_MAX) {continue;}
                        for (uint8_t cid = 1; cid < searchChannelsNum; ++cid) {
                            if (!visited[eid - 1][cid - 1]) {
                                visited[eid - 1][cid - 1] = true;
                                double stepCost = 1.0; // 这一步的成本
                                if (cid != current->c && node.IsAllowChangeChannel()) { // 增加换通道成本
                                    stepCost += CHANGE_CHANNEL_COST;
                                }
                                auto newNode = new AStarNode{nid, cid, eid, current, current->passed + stepCost, double(remainDis)};
                                // 将新结点插入有序的 openSet
                                bool insterted = false;
                                for (auto it = openSet.begin(); it != openSet.end(); ++it) { // 插入有序表
                                    if (*newNode < **it) {
                                        openSet.insert(it, newNode);
                                        insterted = true;
                                        break;
                                    }
                                }
                                if (!insterted) {
                                    openSet.push_back(newNode);
                                }
                            }
                        }
                    }
                }
                closeSet.push(current); // 将当前结点加入闭集
                if (openSet.empty()) { // 开集为空，说明寻路失败
                    break;
                }
                current = openSet.front(); // 取出代价最小的结点
                openSet.pop_front();
                if (current->n == endNode) { // 寻路结束
                    vector<Step> path;
                    for (; current->f != nullptr; current = current->f) {
                        path.push_back(Step(current->e, current->f->n, current->c, useChannelsNum, current->f->f ? current->c != current->f->c : false));
                    }
                    s.ResetServicePath(sid, path, true);
                    output.push_back(sid);
                    isSuccess = true;
                }
            }
            while (!openSet.empty()) {
                delete openSet.front();
                openSet.pop_front();
            }
            while (!closeSet.empty()) {
                delete closeSet.front();
                closeSet.pop();
            }
            s.RecoverServicePath(sid, !isSuccess); // 恢复老路径
        }
        // 所有新路径至此已经生成完毕
        s.ClearHidedPath(true); // 先不考虑新路径是否包含老路径，将老路径全部再删除一遍
        for (auto sid : services) { // 将新路径更新到场景中
            s.AddServicePath(sid);
        }
    }
};

/**
 * @brief 唐鑫的算法
 */
class SolutionTX final : public Solution {
public:
    explicit SolutionTX(const Scene& s) : Solution(s) {}

private:
<<<<<<< HEAD
    class ProgramPlan{
        private:
            uint16_t businessId; // 业务Id
            vector<Step> path; // 目前最优解
            uint16_t score; // 分数
        public:
            ProgramPlan(uint16_t businessId) : businessId(businessId){}
            ProgramPlan(uint16_t businessId, vector<Step> path, uint16_t score) : businessId(businessId), path(path), score(score){};
            uint16_t GetScore(){return score;}
            uint16_t GetBusinessId(){return businessId;}
            vector<Step> GetPath(){return path;}
            void SetScore(uint16_t score){this->score = score;}
            void SetPath(vector<Step> path){this->path = path;}
    };
    queue<vector<Step>> q;
    uint16_t n1,n2,n3; // 超参数


    static bool scoreCmp(ProgramPlan a, ProgramPlan b){
        return a.GetScore() > b.GetScore();
=======
    /**
     * @brief 业务比较函数
     * @param s1 业务1
     * @param s2 业务2
     * @return true 在排序时将业务1放在前面
     * @return false 在排序时将业务2放在前面
     */
    bool ServiceCompare(const Service& s1, const Service& s2) const override {
        return true;
    }
    /**
     * @brief 规划器
     * @param services 受到影响的业务编号 
     * @param output 成功重新规划的业务编号（放入output的业务会自动被设置为复活）
     */
    void Planning(const vector<uint16_t>& services, vector<uint16_t>& output) override {
        
>>>>>>> b6a6b03c7906fe98c01e28dc2016254dd369cff1
    }
        /**
     * @brief 徐哥BFS的方法
     * @param b 受到影响的业务编号 
     * @param vector<Step> 成功重新规划的路径
     */
    vector<Step> BFSNew(uint16_t b){
        
        const auto& service = s.GetService(b);
        uint16_t start = service.GetStart();
        uint16_t end = service.GetEnd();
        // 这里编号是对的吗？ 编号都是从1开始的 数组会不会小了一个？
        vector<bool> visited(s.GetNodesNum()+5, false);
        struct BfsNode {
            uint16_t n; // 终点
            uint16_t e; // 抵达终点的边
            uint16_t startChannel; // 通道起点
            bool isChange; // 是否变道
            BfsNode* f; // 父结点
        };
        queue<BfsNode*> openSet; // 开集，待遍历
        queue<BfsNode*> closeSet; // 闭集，已完成遍历
        openSet.push(new BfsNode{start, INVALID_ID, INVALID_ID, false, nullptr});
        BfsNode* endNode = nullptr;
        vector<Step> path;
        while (!openSet.empty()) { // BFS
            auto node = openSet.front();
            openSet.pop();
            visited[node->n - 1] = true;
            if (node->n == end) {
                endNode = node;
                closeSet.push(node);
                break;
            }
            const vector<uint16_t>& edges = s.GetNodeConst(node->n).GetConnectedEdges();
            for (auto e : edges) { // 从连接的边中找另一端结点
                Edge& edge = s.GetEdge(e);
                bool isChange = false;
                uint16_t startChannel=-1;
                bool isOk=false;
                // 对变通道的考虑
                if(edge.IsAlive()){
                    //TODO 查找算法的优化 时间复杂度太大了
                    if(edge.CheckChannelsFree(service.GetDefaultChannelStart(), service.GetDefualtChannelEnd())){
                        startChannel = service.GetDefaultChannelStart();
                        isOk = true;
                    }else if(s.GetNode(node->n).GetRemainChangeChannelCnt()>0){
                        //通过变道来解决问题
                        // TODO 优化 找尽量少会影响到后面业务路径通道的区间
                        isChange = true;
                        isOk = true;
                        uint16_t emptyBegin = edge.findEmptyChannel(service.GetUseChannelsNum());
                        startChannel = emptyBegin;
                    }
                }
                uint16_t nextNode = edge.GetAnotherNode(node->n);
                if (!visited[nextNode - 1] && isOk) {
                    openSet.push(new BfsNode{nextNode, e, startChannel, isChange, node});
                }
            }
            closeSet.push(node);
        }
        if (endNode) {
            while (endNode->f != nullptr) {
                path.push_back(Step(endNode->e, endNode->f->n, endNode->startChannel, service.GetUseChannelsNum(), endNode->isChange));
                endNode = endNode->f;
            }
        }
        while (!openSet.empty()) {
            delete openSet.front();
            openSet.pop();
        }
        while (!closeSet.empty()) {
            delete closeSet.front();
            closeSet.pop();
        }
        if (!path.empty()) {
            reverse(path.begin(), path.end());
        }
        return path;
    }
    // 判断业务能否塞入
    bool PlanCheck(ProgramPlan plan){
        // 有可能新规划的业务占用了已规划的通道
    }
    void Program(vector<ProgramPlan> &programPlans){
        sort(programPlans.begin(), programPlans.end(), scoreCmp);
        for(auto plan : programPlans){
            // 判断能不能放进去 如果能放进去 有点冗余 因为已经判断过了 很冗余啊
            uint16_t businessId = plan.GetBusinessId();
            s.HideServicePath(businessId);
            if(PlanCheck(plan)){
                
            }
        }
    }

    // 对考虑变道情况的BFS进行测试
    // 本算法为对service根据价值进行排序，考虑使用变道次数
    void Planning(const vector<uint16_t>& services, vector<uint16_t>& output) override {
        vector<ProgramPlan> unfinishPlans;
        for(auto serviceId : services){
            // 初始分数计算公式 = value
            ProgramPlan plan(serviceId, vector<Step>(), s.GetService(serviceId).GetValue());
            unfinishPlans.push_back(plan);
        }
        sort(unfinishPlans.begin(), unfinishPlans.end(), scoreCmp);
        for(auto plan : unfinishPlans){
            s.HideServicePath(plan.GetBusinessId());
            vector<Step> path = BFSNew(plan.GetBusinessId());
            if(!path.empty()){
                s.ResetServicePath(plan.GetBusinessId(), path);
                output.push_back(plan.GetBusinessId());
                s.DeleteHidedPath(plan.GetBusinessId());
            } else {
                s.RecoverServicePath(plan.GetBusinessId());
            }
        }
    }
    // 循环优化（暂时未完成)
    void CyclePlanningTest(const vector<uint16_t>& services, vector<uint16_t>& output) {
        // 修改这里的结构 感觉可以直接直接用 programPlans 在这里初始化就行了 不要用uint16_t 太奇怪了
        vector<ProgramPlan> unfinishPlans;
        for(auto serviceId : services){
            // 初始分数计算公式 = value
            ProgramPlan plan(serviceId, vector<Step>(), s.GetService(serviceId).GetValue());
            unfinishPlans.push_back(plan);
        }
        // 循环次数参数
        int i = 0;
        while(!unfinishPlans.empty() && i<20){
            i++;
            // 先查找路径 得到目前的最优解
            sort(unfinishPlans.begin(), unfinishPlans.end(), scoreCmp);
            for(auto it = unfinishPlans.begin(); it != unfinishPlans.end(); it++){
                ProgramPlan plan = *it;
                s.HideServicePath(plan.GetBusinessId());
                Service& service = s.GetService(plan.GetBusinessId());
                vector<Step> ProgramPath = BFSNew(service.GetId());
                // TODO 根据上轮BFS的解来找次优解
                if(!ProgramPath.empty()||plan.GetPath().empty()){
                    // 规划成功 直接把业务塞进去
                    s.ResetServicePath(plan.GetBusinessId(), ProgramPath);
                    output.push_back(plan.GetBusinessId());
                    s.DeleteHidedPath(plan.GetBusinessId());
                    unfinishPlans.erase(it);
                    continue;
                    // 删除这个plan
                }else{
                    // 计算变道次数
                    // 重新赋值 到下一次循环再说
                    s.RecoverServicePath(plan.GetBusinessId());
                    uint16_t changeNum = 0;
                    for(auto step : ProgramPath){
                        if(step.GetUseChannelsNum()){
                            changeNum++;
                    }
                    }
                    // 计算分数
                    int score = n1 * service.GetValue() - n2 * ProgramPath.size() - n3 * changeNum;
                    plan.SetScore(score);
                    plan.SetPath(ProgramPath);
                }
            }
        }
    }

};

/**
 * @brief 陈嘉的算法
 */
class SolutionCJ final : public Solution {
public:
    explicit SolutionCJ(const Scene& s) : Solution(s) {}

private:
    /**
     * @brief 业务比较函数
     * @param s1 业务1
     * @param s2 业务2
     * @return true 在排序时将业务1放在前面
     * @return false 在排序时将业务2放在前面
     */
    bool ServiceCompare(const Service& s1, const Service& s2) const override {
        return true;
    }
    /**
     * @brief 规划器
     * @param services 受到影响的业务编号 
     * @param output 成功重新规划的业务编号（放入output的业务会自动被设置为复活）
     */
    void Planning(const vector<uint16_t>& services, vector<uint16_t>& output) override {
        
    }
};

/**
 * @brief 主函数
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char **argv) {
    /* LOG初始化 */////////////////////////////////////////////////////////////////////////////////////////////////
    LOG_INIT(argc > 1 ? argv[1] : "log/log.txt"); // 程序运行时第一个参数传递log输出位置
    LOG_LINE();
    LOG_INFO("编译时间[%s %s]\n", __DATE__, __TIME__)
    LOG_LINE();

    /* 变量定义 *//////////////////////////////////////////////////////////////////////////////////////////////////
    Scene original; // 初始场景

    /* 初始环境输入 *///////////////////////////////////////////////////////////////////////////////////////////////
    int N, M;
    scanf("%d %d", &N, &M);
    for (int i = 0; i < N; ++i) {
        int Pi;
        scanf("%d", &Pi);
        original.AddNode(Pi);
    }
    original.CreateNodeDistanceTable(N);
    for (int i = 0; i < M; ++i) {
        int ui, vi;
        scanf("%d %d", &ui, &vi);
        if (ui < vi) {
            original.AddEdge(ui, vi);
        } else {
            original.AddEdge(vi, ui);
        }
    }
    original.Floyd();
    int J;
    scanf("%d", &J);
    for (int i = 0; i < J; ++i) {
        int Src, Snk, S, L, R;
        long V;
        scanf("%d %d %d %d %d %ld", &Src, &Snk, &S, &L, &R, &V);
        uint8_t useChannelsNum = R - L + 1;
        original.AddService(Src, Snk, double(V), L, useChannelsNum);
        for (int j = 0; j < S; ++j) {
            int ei;
            scanf("%d", &ei);
            original.AddServiceStep(i + 1, ei);
        }
    }
#ifdef DEBUG
    double initValue = original.GetValue(); // 初始状态业务总价值
    double finalScore = 0.0; // 最终得分
#endif // DEBUG

    /* 交互部分 *//////////////////////////////////////////////////////////////////////////////////////////////////
    int T;
    scanf("%d", &T);
    for (int i = 0; i < T; ++i) {
        LOG_INFO("场景[%3d]###########################################################################\n", i + 1);
        //auto s{new SolutionXTZ(original)};
        auto s{new SolutionTX(original)};
        // auto s{new SolutionCJ(original)};
        int e_failed;
        while (true) {
            scanf("%d", &e_failed);
            if (e_failed == -1) {
                break;
            }
            s->Handle(e_failed);
        }
#ifdef DEBUG
        double score = s->GetValue() * 10000.0 / initValue; // 本场景得分
        LOG_INFO("得分：%.0f\n", score);
        finalScore += score;
#endif // DEBUG
        delete s;
    }
    original.DeleteNodeDistanceTable(); // 释放内存

    /* LOG输出 *//////////////////////////////////////////////////////////////////////////////////////////////////
    LOG_LINE();
#ifdef DEBUG
    LOG_INFO("总分：%.0f\n", finalScore);
#endif // DEBUG

    return 0;
}
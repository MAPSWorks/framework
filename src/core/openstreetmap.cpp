#include "openstreetmap.h"

#include "shader.h"
#include "renderable_object.h"
#include "latlon_converter.h"

#include <pcl/common/centroid.h>
#include <pcl/search/impl/flann_search.hpp>

// Osmium
// Osmium for openstreetmap
#include <osmium/io/xml_input.hpp>
#include <osmium/handler.hpp>
#include <osmium/visitor.hpp>

struct OsmWay {
    bool is_oneway = false;
    WayType type;
    vector<size_t> node_idxs;
};

glm::vec4 getWayColor(int type) {
    switch (type) {
        case WayType::MOTORWAY:
        case WayType::MOTORWAY_LINK:
            return glm::vec4(0.0f, 0.4f, 0.8f, 1.0f);
        case WayType::TRUNK:
        case WayType::TRUNK_LINK:
            return glm::vec4(0.0f, 0.8f, 0.4f, 1.0f);
        case WayType::PRIMARY:
        case WayType::PRIMARY_LINK:
            return glm::vec4(1.0f, 0.4f, 0.4f, 1.0f);
        case WayType::SECONDARY:
        case WayType::SECONDARY_LINK:
            return glm::vec4(1.0f, 0.7f, 0.4f, 1.0f);
        case WayType::TERTIARY:
        case WayType::TERTIARY_LINK:
            return glm::vec4(1.0f, 0.7f, 0.4f, 1.0f);
        default:
            return glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
    }
}

class MyHandlerClass : public osmium::handler::Handler {
public:
    void node(osmium::Node& node) {
        if (node_table_.find(node.id()) == node_table_.end()) {
            // Add a new node
            node_table_[node.id()] = nodes_.size();
            nodes_.emplace_back(pair<double, double>(node.location().lat(),
                                                     node.location().lon()));
        }
    }

    void way(osmium::Way& way) {
        if (way.nodes().size() == 0) return;

        // Process way tags
        const char* highway_type = way.tags().get_value_by_key("highway");
        OsmWay a_way;
        if (highway_type) {
            if (strcmp(highway_type, "motorway") == 0) {
                a_way.is_oneway = true;
                a_way.type = WayType::MOTORWAY;
            } else if (strcmp(highway_type, "motorway_link") == 0) {
                a_way.is_oneway = true;
                a_way.type = WayType::MOTORWAY_LINK;
            } else if (strcmp(highway_type, "trunk") == 0) {
                a_way.type = WayType::TRUNK;
            } else if (strcmp(highway_type, "trunk_link") == 0) {
                a_way.type = WayType::TRUNK_LINK;
            } else if (strcmp(highway_type, "primary") == 0) {
                a_way.type = WayType::PRIMARY;
            } else if (strcmp(highway_type, "primary_link") == 0) {
                a_way.type = WayType::PRIMARY_LINK;
            } else if (strcmp(highway_type, "secondary") == 0) {
                a_way.type = WayType::SECONDARY;
            } else if (strcmp(highway_type, "secondary_link") == 0) {
                a_way.type = WayType::SECONDARY_LINK;
            } else if (strcmp(highway_type, "tertiary") == 0) {
                a_way.type = WayType::TERTIARY;
            } else if (strcmp(highway_type, "tertiary_link") == 0) {
                a_way.type = WayType::TERTIARY_LINK;
            } else {
                if (strcmp(highway_type, "unclassified") == 0 ||
                    strcmp(highway_type, "residential") == 0 ||
                    strcmp(highway_type, "road") == 0)
                    a_way.type = WayType::OTHER;
                else {
                    return;
                }
            }
        } else {
            return;
        }

        const char* oneway_tag = way.tags().get_value_by_key("oneway");
        if (oneway_tag && strcmp(oneway_tag, "yes") == 0) {
            a_way.is_oneway = true;
        }

        // Process way nodes
        for (auto it = way.nodes().begin(); it != way.nodes().end(); ++it) {
            size_t node_idx = node_table_[it->ref()];
            a_way.node_idxs.emplace_back(node_idx);
        }
        ways_.emplace_back(a_way);
    }

    vector<pair<double, double>>& getNodes() { return nodes_; }
    vector<OsmWay>& getWays() { return ways_; }

private:
    vector<pair<double, double>> nodes_;
    vector<OsmWay> ways_;
    map<int64_t, size_t> node_table_;
};

OpenStreetMap::OpenStreetMap()
    : m_mapPoints(new pcl::PointCloud<MapPointType>),
      m_searchTree(new pcl::search::FlannSearch<MapPointType>(false)),
      m_interpolation(10.0f),
      m_dataUpdated(false),
      m_vboPoints(new RenderableObject),
      m_vboLines(new RenderableObject) {}

OpenStreetMap::~OpenStreetMap() {}

bool OpenStreetMap::load(const string& filename) {
    clear();

    osmium::io::File input_file(filename.c_str());
    osmium::io::Reader reader(input_file);

    MyHandlerClass handler;
    osmium::apply(reader, handler);
    reader.close();

    // Build the graph
    vector<OsmWay>& raw_ways = handler.getWays();
    vector<pair<double, double>>& raw_nodes = handler.getNodes();
    map<size_t, graph_vertex_descriptor> vertex_table;
    Converter& latlon_converter = Converter::getInstance();
    for (const auto& a_way : raw_ways) {
        graph_vertex_descriptor prev_v;
        vector<graph_vertex_descriptor> indexed_road;
        for (size_t i = 0; i < a_way.node_idxs.size(); ++i) {
            graph_vertex_descriptor v;
            pair<double, double> node_loc = raw_nodes[a_way.node_idxs[i]];
            if (vertex_table.find(a_way.node_idxs[i]) != vertex_table.end()) {
                v = vertex_table[a_way.node_idxs[i]];
            } else {
                // Add a new node
                v = boost::add_vertex(m_graph);
                vertex_table[a_way.node_idxs[i]] = v;
                float easting, northing;
                latlon_converter.convertLatLonToXY(
                    node_loc.first, node_loc.second, easting, northing);
                m_graph[v].easting = easting;
                m_graph[v].northing = northing;
                if (easting < m_boundBox[0]) {
                    m_boundBox[0] = easting;
                }
                if (easting > m_boundBox[1]) {
                    m_boundBox[1] = easting;
                }
                if (northing < m_boundBox[2]) {
                    m_boundBox[2] = northing;
                }
                if (northing > m_boundBox[3]) {
                    m_boundBox[3] = northing;
                }
            }

            indexed_road.push_back(v);
            if (i > 0) {
                // Add edge
                float edge_length =
                    distance(m_graph[prev_v].easting, m_graph[prev_v].northing,
                             m_graph[v].easting, m_graph[v].northing);

                auto e = boost::add_edge(prev_v, v, m_graph);
                if (e.second) {
                    m_graph[e.first].length = edge_length;
                    m_graph[e.first].type = a_way.type;
                }

                // If is not oneway, add opposite edge
                if (!a_way.is_oneway) {
                    auto e = boost::add_edge(v, prev_v, m_graph);
                    if (e.second) {
                        m_graph[e.first].length = edge_length;
                        m_graph[e.first].type = a_way.type;
                    }
                }
            }
            prev_v = v;
        }
        m_indexedRoads.push_back(indexed_road);
    }

    updateBBOX(m_boundBox[0], m_boundBox[1], m_boundBox[2], m_boundBox[3]);
    printf("OpenStreetMap Loaded, there are %lu ways, %lu nodes\n",
           handler.getWays().size(), handler.getNodes().size());
    printf("\tThe graph has %lu nodes, %lu edges\n",
           boost::num_vertices(m_graph), boost::num_edges(m_graph));
    printf("\tupdated bbox: %.2f, %.2f, %.2f, %.2f\n", m_boundBox[0],
           m_boundBox[1], m_boundBox[2], m_boundBox[3]);

    return true;
}

void OpenStreetMap::computeMapPointCloud() {
    m_mapPoints->clear();
    m_pointHeading.clear();
    m_graphVertices.clear();
    m_pointEdgeIds.clear();
    printf("Start interpolating OpenStreetMap with %.1f meter accuracy......",
           m_interpolation);
    // Iterate over the edges
    auto es = boost::edges(m_graph);
    for (auto eit = es.first; eit != es.second; ++eit) {
        if (m_graph[*eit].length < 0.1f) {
            // Ignore very short edges
            continue;
        }

        auto source_v = source(*eit, m_graph);
        auto target_v = target(*eit, m_graph);

        Eigen::Vector2f start_pt(m_graph[source_v].easting,
                                 m_graph[source_v].northing);
        Eigen::Vector2f end_pt(m_graph[target_v].easting,
                               m_graph[target_v].northing);

        Eigen::Vector2f dir = end_pt - start_pt;

        int heading = vector2fToHeading(dir);

        MapPointType point;
        point.setCoordinate(start_pt[0], start_pt[1], 0.0f);
        m_mapPoints->push_back(point);
        m_pointHeading.push_back(heading);
        m_graphVertices.push_back(source_v);
        m_pointEdgeIds.push_back(*eit);

        if (m_graph[*eit].length > m_interpolation) {
            // Insert point
            int n_pt_to_insert = floor(m_graph[*eit].length / m_interpolation);
            // Normalize direction
            dir /= m_graph[*eit].length;
            float delta_length = m_graph[*eit].length / (n_pt_to_insert + 1);
            for (size_t i = 0; i < n_pt_to_insert; ++i) {
                Eigen::Vector2f pt = start_pt + dir * (i + 1) * delta_length;
                point.setCoordinate(pt[0], pt[1], 0.0f);
                m_mapPoints->push_back(point);
                m_pointHeading.push_back(heading);
                m_pointEdgeIds.push_back(*eit);
                if (i < n_pt_to_insert / 2) {
                    m_graphVertices.push_back(source_v);
                } else {
                    m_graphVertices.push_back(target_v);
                }
            }
        }

        // Insert the end point
        point.setCoordinate(end_pt[0], end_pt[1], 0.0f);
        m_mapPoints->push_back(point);
        m_pointHeading.push_back(heading);
        m_graphVertices.push_back(target_v);
        m_pointEdgeIds.push_back(*eit);
    }

    m_searchTree->setInputCloud(m_mapPoints);
    printf("done.\n");
}

// Rendering
void OpenStreetMap::render(unique_ptr<Shader>& shader) {
    if (params::inst().boundBox.updated) {
        updateVBO();
    }
    glm::mat4 model(1.0f);
    shader->setMatrix("matModel", model);

    // Render Points
    params::inst().glFuncs->glPointSize(10.0f);
    m_vboPoints->render();
    params::inst().glFuncs->glPointSize(1.0f);

    // Render Lines
    m_vboLines->render();
}

void OpenStreetMap::updateVBO() {
    float scale = params::inst().scale;
    vector<RenderableObject::Vertex> lineData;
    vector<RenderableObject::Vertex> pointData;

    glm::vec4 vertex_color(1.0f, 1.0f, 0.0f, 0.5f);
    auto vs = boost::vertices(m_graph);
    for (auto vit = vs.first; vit != vs.second; ++vit) {
        auto out_es = boost::out_edges(*vit, m_graph);
        auto in_es = boost::in_edges(*vit, m_graph);
        set<graph_vertex_descriptor> neighbors;
        for (auto eit = in_es.first; eit != in_es.second; ++eit) {
            neighbors.emplace(boost::source(*eit, m_graph));
        }
        for (auto eit = out_es.first; eit != out_es.second; ++eit) {
            neighbors.emplace(boost::target(*eit, m_graph));
        }
        int n_neighbors = neighbors.size();
        m_graph[*vit].nNeighbors = n_neighbors;
        if (n_neighbors == 1 || n_neighbors > 2) {
            RenderableObject::Vertex pt;
            pt.Position = convertToDisplayCoord(m_graph[*vit].easting,
                                                m_graph[*vit].northing, 1.01f);
            pt.Color = vertex_color;
            pointData.push_back(pt);
        }
    }

    auto es = boost::edges(m_graph);
    for (auto eit = es.first; eit != es.second; ++eit) {
        glm::vec4 edge_color = getWayColor(m_graph[*eit].type);

        auto source_v = source(*eit, m_graph);
        auto target_v = target(*eit, m_graph);

        RenderableObject::Vertex source_pt, target_pt;
        source_pt.Position = convertToDisplayCoord(
            m_graph[source_v].easting, m_graph[source_v].northing, 1.0f);
        source_pt.Color = edge_color;

        target_pt.Position = convertToDisplayCoord(
            m_graph[target_v].easting, m_graph[target_v].northing, 1.0f);
        target_pt.Color = edge_color;

        lineData.push_back(source_pt);
        lineData.push_back(target_pt);
    }
    m_vboPoints->setData(pointData, GL_POINTS);

    m_vboLines->setData(lineData, GL_LINES);
    m_dataUpdated = true;
}

void OpenStreetMap::clear() {
    m_dataUpdated = false;

    m_mapPoints->clear();
    m_pointHeading.clear();
    m_graphVertices.clear();
    m_pointEdgeIds.clear();

    m_indexedRoads.clear();
    m_boundBox = Eigen::Vector4f(POSITIVE_INFINITY, -POSITIVE_INFINITY,
                                 POSITIVE_INFINITY, -POSITIVE_INFINITY);
    m_graph.clear();
    updateVBO();
}

bool OpenStreetMap::isEmpty() {
    if (m_indexedRoads.empty()) {
        return true;
    }
    return false;
}
